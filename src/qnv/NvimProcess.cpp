#include "qnv/NvimProcess.hpp"

#include <msgpack.hpp>
#include <iostream> // TODO: for printing out msgpack. make a QDebug overload instead
#include <string_view>
#include <QProcess>
#include "msgpack_view/msgpack_view.hpp"
#include "qnv/Unicode.hpp"
#include "qnv/Config.hpp"

// TODO: refactor this mess

using namespace std::string_literals;
using namespace std::string_view_literals;

struct NvimProcess::Private
{
    QProcess mProcess;
    msgpack::unpacker mUnpacker;

    void parseResponse(Integer id, msgpack_view::object_view error, msgpack_view::object_view result);
    void parseNotification(NvimProcess& self, msgpack_view::array_view events);
    void parseEvent(NvimProcess& self, msgpack_view::array_view data);
};

NvimProcess::NvimProcess(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    connect(&d->mProcess, &QProcess::started, this, &NvimProcess::onStarted);
    connect(&d->mProcess, &QProcess::readyRead, this, &NvimProcess::onReadyRead);
}

NvimProcess::~NvimProcess()
{
    delete d;
}

void NvimProcess::start()
{
    if (d->mProcess.state() == QProcess::NotRunning)
        d->mProcess.start("nvim", { "--embed", });
}

void NvimProcess::onStarted()
{
    std::cerr << "nvim started\n";

    {
        msgpack::sbuffer buf;
        msgpack::packer<msgpack::sbuffer> pk { &buf };
        pk.pack_array(4);
        pk.pack_int(0);
        pk.pack_uint32(1);
        pk.pack("nvim_exec_lua"sv);
        pk.pack_array(2);
        pk.pack(QStringLiteral("dofile([[%1/nvim/init.lua]])").arg(Config::runtimePath()).toStdString());
        pk.pack_array(0);
        d->mProcess.write(buf.data(), buf.size());
    }

    {
        msgpack::sbuffer buf;
        msgpack::packer<msgpack::sbuffer> pk { &buf };
        pk.pack_array(4);
        pk.pack_int(0);
        pk.pack_uint32(1);
        pk.pack("nvim_ui_attach"sv);
        pk.pack_array(3);
        pk.pack_int64(60);
        pk.pack_int64(20);
        pk.pack_map(3);
        pk.pack("rgb"sv);
        pk.pack(true);
        pk.pack("ext_hlstate"sv);
        pk.pack(true);
        pk.pack("ext_multigrid"sv);
        pk.pack(true);
        // pk.pack("ext_cmdline"sv);
        // pk.pack(true);
        // pk.pack("ext_messages"sv);
        // pk.pack(true);
        // pk.pack("ext_tabline"sv);
        // pk.pack(true);
        // pk.pack("ext_popupmenu"sv);
        // pk.pack(true);
        d->mProcess.write(buf.data(), buf.size());
    }
}

void NvimProcess::sendInput(const QString& key)
{
    msgpack::sbuffer buf;
    msgpack::packer<msgpack::sbuffer> pk { &buf };
    pk.pack_array(4);
    pk.pack_int(0);
    pk.pack_uint32(1);
    pk.pack("nvim_input"sv);
    pk.pack_array(1);
    pk.pack(key.toStdString());
    d->mProcess.write(buf.data(), buf.size());
}

void NvimProcess::tryResize(uint32_t width, uint32_t height)
{
    msgpack::sbuffer buf;
    msgpack::packer<msgpack::sbuffer> pk { &buf };
    pk.pack_array(4);
    pk.pack_int(0);
    pk.pack_uint32(1);
    pk.pack("nvim_ui_try_resize"sv);
    pk.pack_array(2);
    pk.pack_int64(width);
    pk.pack_int64(height);
    d->mProcess.write(buf.data(), buf.size());
}

void NvimProcess::tryResizeGrid(int64_t grid, uint32_t width, uint32_t height)
{
    msgpack::sbuffer buf;
    msgpack::packer<msgpack::sbuffer> pk { &buf };
    pk.pack_array(4);
    pk.pack_int(0);
    pk.pack_uint32(1);
    pk.pack("nvim_ui_try_resize"sv);
    pk.pack_array(3);
    pk.pack_int64(grid);
    pk.pack_int64(width);
    pk.pack_int64(height);
    d->mProcess.write(buf.data(), buf.size());
}

void NvimProcess::onReadyRead()
{
    const auto buf = d->mProcess.readAllStandardOutput();
    d->mUnpacker.reserve_buffer(buf.size());
    std::memcpy(d->mUnpacker.buffer(), buf.data(), buf.size());
    d->mUnpacker.buffer_consumed(buf.size());

    msgpack::object_handle res;
    while (d->mUnpacker.next(res)) {
        const auto& obj = res.get();
        try {
            auto args = msgpack_view::object_view { obj }.as<msgpack_view::array_view>();
            if (args.empty())
                continue;
            auto type = args[0].as<Integer>();
            if (type == 0) {
                // request
                if (args.size() != 4)
                    continue;

                auto id = args[1].as<int64_t>();
                if (id < 0 || id > std::numeric_limits<uint32_t>::max())
                    continue;

                auto method = args[2].as<std::string_view>();
                // auto params = args[3].as<msgpack_view::array_view>();
                std::cerr << "received request: " << method << ' ' << args[3] << '\n';

                msgpack::sbuffer buf;
                msgpack::packer<msgpack::sbuffer> pk { &buf };
                pk.pack_array(4);
                pk.pack_int(1);
                pk.pack_uint32(id);
                pk.pack_nil();
                pk.pack_nil();
                d->mProcess.write(buf.data(), buf.size());
            } else if (type == 1) {
                // response
                if (args.size() != 4)
                    continue;
                d->parseResponse(args[1].as<Integer>(), args[2], args[3]);
            } else if (type == 2) {
                // notification
                if (args.size() != 3)
                    continue;
                auto method = args[1].as<std::string_view>();
                if (method == "redraw"sv) {
                    d->parseNotification(*this, args[2].as<msgpack_view::array_view>());
                } else if (method == "qnv"sv) {
                    std::cerr << "received notification: " << obj << '\n';
                }
            } else {
                std::cerr << "invalid message type\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "uncaught exception while parsing message: " << e.what() << '\n';
            std::cerr << "  message: " << obj << '\n';
        }
    }
}

void NvimProcess::Private::parseResponse(
        Integer id,
        msgpack_view::object_view error,
        msgpack_view::object_view result)
{
    std::cerr << "response: " << id << ", " << error << ", " << result << '\n';
}

void NvimProcess::Private::parseNotification(NvimProcess& self, msgpack_view::array_view events)
{
    for (auto event : events) {
        try {
            // std::cerr << ">> " << event << '\n';
            parseEvent(self, event.as<msgpack_view::array_view>());
        } catch (const std::exception& e) {
            std::cerr << "uncaught exception while parsing event: " << e.what() << '\n';
            std::cerr << "  event: " << event << '\n';
        }
    }
}

void NvimProcess::Private::parseEvent(NvimProcess& self, msgpack_view::array_view args)
{
    if (args.empty())
        return;
    auto eventName = args[0].as<std::string_view>();
    if (eventName == "grid_line"sv) {
        std::vector<GridLine> res;
        res.reserve(args.size() - 1);
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 4)
                continue;
            GridLine rLine;
            rLine.mGrid = ev[0].as<Integer>();
            rLine.mRow = ev[1].as<Integer>();
            rLine.mColStart = ev[2].as<Integer>();
            auto cells = ev[3].as<msgpack_view::array_view>();
            rLine.mCells.reserve(cells.size());
            uint32_t rHl = 0;
            for (auto it : cells) {
                auto cell = it.as<msgpack_view::array_view>();
                if (cell.size() < 1)
                    continue;
                GridLine::Cell rCell;
                rCell.mChar = utf8toUcs(cell[0].as<std::string_view>());
                if (cell.size() >= 2)
                    rHl = cell[1].as<Integer>();
                rCell.mHlId = rHl;
                if (cell.size() >= 3)
                    rCell.mRepeat = cell[2].as<Integer>();
                rLine.mCells.emplace_back(std::move(rCell));
            }
            res.emplace_back(std::move(rLine));
        }
        emit self.gridLine(res);
    } else if (eventName == "grid_cursor_goto"sv) {
        auto ev = args[args.size() - 1].as<msgpack_view::array_view>();
        if (ev.size() < 3)
            return;
        emit self.gridCursorGoto(
                ev[0].as<Integer>(),
                ev[1].as<Integer>(),
                ev[2].as<Integer>());
    } else if (eventName == "default_colors_set"sv) {
        auto ev = args[args.size() - 1].as<msgpack_view::array_view>();
        if (ev.size() < 5)
            return;
        emit self.defaultColorsSet(
                uint32_t(ev[0].as<Integer>() & 0xFFFFFF),
                uint32_t(ev[1].as<Integer>() & 0xFFFFFF),
                uint32_t(ev[2].as<Integer>() & 0xFFFFFF),
                ev[3].as<Integer>(),
                ev[4].as<Integer>());
    } else if (eventName == "grid_scroll"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 7)
                continue;
            emit self.gridScroll(
                    ev[0].as<Integer>(),
                    ev[1].as<Integer>(),
                    ev[2].as<Integer>(),
                    ev[3].as<Integer>(),
                    ev[4].as<Integer>(),
                    ev[5].as<Integer>(),
                    ev[6].as<Integer>());
        }
    } else if (eventName == "grid_resize"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 3)
                continue;
            emit self.gridResize(
                    ev[0].as<Integer>(),
                    ev[1].as<Integer>(),
                    ev[2].as<Integer>());
        }
    } else if (eventName == "grid_clear"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.gridClear(ev[0].as<Integer>());
        }
    } else if (eventName == "grid_destroy"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.gridDestroy(ev[0].as<Integer>());
        }
    } else if (eventName == "win_pos"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 6)
                continue;
            emit self.winPos(
                    ev[0].as<Integer>(),
                    ev[1].as<Window>(),
                    ev[2].as<Integer>(),
                    ev[3].as<Integer>(),
                    ev[4].as<Integer>(),
                    ev[5].as<Integer>());
        }
    } else if (eventName == "win_float_pos"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 7)
                continue;
            emit self.winFloatPos(
                    ev[0].as<Integer>(),
                    ev[1].as<Window>(),
                    ev[2].as<String>(),
                    ev[3].as<Integer>(),
                    ev[4].as<Integer>(),
                    ev[5].as<Integer>(),
                    ev[6].as<Boolean>());
        }
    } else if (eventName == "win_viewport"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 7)
                continue;
            emit self.winViewport(
                    ev[0].as<Integer>(),
                    ev[1].as<Window>(),
                    ev[2].as<Integer>(),
                    ev[3].as<Integer>(),
                    ev[4].as<Integer>(),
                    ev[5].as<Integer>(),
                    ev[6].as<Integer>());
        }
    } else if (eventName == "win_hide"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.winHide(ev[0].as<Integer>());
        }
    } else if (eventName == "win_close"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.winClose(ev[0].as<Integer>());
        }
    } else if (eventName == "msg_set_pos"sv) {
        auto ev = args[args.size() - 1].as<msgpack_view::array_view>();
        if (ev.size() < 4)
            return;
        emit self.msgSetPos(
                ev[0].as<Integer>(),
                ev[1].as<Integer>(),
                ev[2].as<Boolean>(),
                ev[3].as<String>());
    } else if (eventName == "hl_attr_define"sv) {
        std::vector<std::pair<uint32_t, HlGroup>> res;
        res.reserve(args.size() - 1);
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 4)
                continue;
            auto id = ev[0].as<Integer>();
            auto rgb_attr = ev[1].as<msgpack_view::map_view>();
            // auto cterm_attr = item[2].as<msgpack_view::map_view>();
            // auto info = item[3].as<msgpack_view::array_view>();
            HlGroup hl;
            for (auto it : rgb_attr) {
                auto key = it.first.as<std::string_view>();
                auto val = it.second;
                if (key == "foreground"sv) {
                    hl.mFgColor = val.as<Integer>() & 0xFFFFFF;
                } else if (key == "background"sv) {
                    hl.mBgColor = val.as<Integer>() & 0xFFFFFF;
                } else if (key == "special"sv) {
                    hl.mSpColor = val.as<Integer>() & 0xFFFFFF;
                } else if (key == "reverse"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Inverse;
                } else if (key == "italic"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Italic;
                } else if (key == "bold"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Bold;
                } else if (key == "strikethrough"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Strikethrough;
                } else if (key == "underline"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Underline;
                } else if (key == "undercurl"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Undercurl;
                } else if (key == "underdouble"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Underdouble;
                } else if (key == "underdotted"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Underdotted;
                } else if (key == "underdashed"sv) {
                    if (val.as<Boolean>()) hl.mAttrs |= HlGroup::Underdashed;
                } else if (key == "blend"sv) {
                    hl.mHlBlend = val.as<Integer>();
                }
                res.emplace_back(id, hl);
            }
            emit self.hlAttrDefine(res);
        }
    } else if (eventName == "hl_group_set"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 2)
                continue;
            emit self.hlGroupSet(
                    ev[0].as<String>(),
                    ev[1].as<Integer>());
        }
    } else if (eventName == "tabline_update"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 4)
                continue;

            auto rTab = ev[0].as<Tabpage>();
            auto rBuf = ev[2].as<Buffer>();

            std::vector<std::pair<Tabpage, String>> rTabs;
            {
                auto tabs = ev[1].as<msgpack_view::array_view>();
                rTabs.reserve(tabs.size());
                for (auto tab : tabs) {
                    std::pair<Tabpage, String> r;
                    uint8_t m = 0;
                    for (auto it2 : tab.as<msgpack_view::map_view>()) {
                        auto k = it2.first.as<std::string_view>();
                        if (k == "tab"sv) {
                            m |= 0b01;
                            r.first = it2.second.as<Tabpage>();
                        } else if (k == "name"sv) {
                            m |= 0b10;
                            r.second = it2.second.as<std::string>();
                        }
                    }
                    if (m == 0b11)
                        rTabs.emplace_back(std::move(r));
                }
            }

            std::vector<std::pair<Buffer, String>> rBufs;
            {
                auto bufs = ev[3].as<msgpack_view::array_view>();
                rBufs.reserve(bufs.size());
                for (auto buf : bufs) {
                    std::pair<Buffer, String> r;
                    uint8_t m = 0;
                    for (auto it2 : buf.as<msgpack_view::map_view>()) {
                        auto k = it2.first.as<std::string_view>();
                        if (k == "buffer"sv) {
                            m |= 0b01;
                            r.first = it2.second.as<Buffer>();
                        } else if (k == "name"sv) {
                            m |= 0b10;
                            r.second = it2.second.as<std::string>();
                        }
                    }
                    if (m == 0b11)
                        rBufs.emplace_back(std::move(r));
                }
            }

            emit self.tablineUpdate(rTab, rTabs, rBuf, rBufs);
        }
    } else if (eventName == "cmdline_show"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 6)
                continue;
            auto firstc = ev[2].as<std::string_view>();
            char rFirstc = firstc.empty() ? '\0' : firstc.front();
            emit self.cmdlineShow(
                    ev[0].as<std::vector<std::pair<Integer, String>>>(),
                    ev[1].as<Integer>(),
                    rFirstc,
                    ev[3].as<String>(),
                    ev[4].as<Integer>(),
                    ev[5].as<Integer>());
        }
    } else if (eventName == "cmdline_pos"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 2)
                continue;
            emit self.cmdlinePos(
                    ev[0].as<Integer>(),
                    ev[1].as<Integer>());
        }
    } else if (eventName == "cmdline_special_char"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 3)
                continue;
            emit self.cmdlineSpecialChar(
                    ev[0].as<String>(),
                    ev[1].as<Boolean>(),
                    ev[2].as<Integer>());
        }
    } else if (eventName == "cmdline_hide"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.cmdlineHide(ev[0].as<Integer>());
        }
    } else if (eventName == "cmdline_block_show"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.cmdlineBlockShow(ev[0].as<std::vector<std::pair<Integer, String>>>());
        }
    } else if (eventName == "cmdline_block_append"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.cmdlineBlockAppend(ev[0].as<std::vector<std::pair<Integer, String>>>());
        }
    } else if (eventName == "cmdline_block_hide"sv) {
        emit self.cmdlineBlockHide();
    } else if (eventName == "msg_show"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 3)
                continue;
            emit self.msgShow(
                    ev[0].as<String>(),
                    ev[1].as<std::vector<std::pair<Integer, String>>>(),
                    ev[2].as<Boolean>());
        }
    } else if (eventName == "msg_clear"sv) {
        emit self.msgClear();
    } else if (eventName == "msg_showcmd"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.msgShowcmd(ev[0].as<std::vector<std::pair<Integer, String>>>());
        }
    } else if (eventName == "msg_showmode"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.msgShowmode(ev[0].as<std::vector<std::pair<Integer, String>>>());
        }
    } else if (eventName == "msg_ruler"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.msgRuler(ev[0].as<std::vector<std::pair<Integer, String>>>());
        }
    } else if (eventName == "msg_history_show"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 1)
                continue;
            emit self.msgHistoryShow(ev[0].as<std::vector<std::pair<String, String>>>());
        }
    } else if (eventName == "msg_history_clear"sv) {
        emit self.msgHistoryClear();
    } else if (eventName == "mode_change"sv) {
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto ev = it->as<msgpack_view::array_view>();
            if (ev.size() < 2)
                continue;
            emit self.modeChange(ev[0].as<String>(), ev[1].as<Integer>());
        }
    } else if (eventName == "update_menu"sv) {
        std::cerr << eventName << '\n'; // no args
    } else if (eventName == "busy_start"sv) {
        emit self.busy(true);
    } else if (eventName == "busy_stop"sv) {
        emit self.busy(false);
    } else if (eventName == "mouse_on"sv) {
        emit self.mouse(true);
    } else if (eventName == "mouse_off"sv) {
        emit self.mouse(false);
    } else if (eventName == "bell"sv) {
        emit self.bell();
    } else if (eventName == "visual_bell"sv) {
        emit self.visualBell();
    } else if (eventName == "flush"sv) {
        emit self.flush();
    } else if (eventName == "suspend"sv) {
        emit self.suspend();
    } else if (eventName == "clear"sv) {
        std::cerr << eventName << '\n'; // no args
    } else if (eventName == "eol_clear"sv) {
        std::cerr << eventName << '\n'; // no args
    } else if (eventName == "popupmenu_hide"sv) {
        std::cerr << eventName << '\n'; // no args
    } else if (eventName == "wildmenu_hide"sv) {
        std::cerr << eventName << '\n'; // no args
    } else if (eventName == "set_title"sv) {
        auto ev = args[args.size() - 1].as<msgpack_view::array_view>();
        if (ev.size() < 1)
            return;
        emit self.setTitle(ev[0].as<String>());
    } else if (eventName == "set_icon"sv) {
        auto ev = args[args.size() - 1].as<msgpack_view::array_view>();
        if (ev.size() < 1)
            return;
        emit self.setIcon(ev[0].as<String>());
    } else {
        std::cerr << eventName << '\n';
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            std::cerr << "  " << *it << '\n';
        }
    }
}
