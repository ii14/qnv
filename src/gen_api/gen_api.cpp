#include <iostream>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <QProcess>
#include <msgpack.hpp>
#include "msgpack_view/msgpack_view.hpp"

// not really generating anything atm, just prints out api info

using namespace std::string_literals;
using namespace std::string_view_literals;


struct Parameter
{
    std::string type;
    std::string name;
};

struct Function
{
    std::vector<Parameter> parameters;
    std::string return_type;
    std::string name;
    bool method;
    int64_t since { -1 };
    int64_t deprecated_since { -1 };
};

struct UiEvent
{
    std::vector<Parameter> parameters;
    std::string name;
    int64_t since { -1 };
    int64_t deprecated_since { -1 };
};


inline std::ostream& operator<<(std::ostream& os, const Parameter& v)
{
    return os << v.type << ' ' << v.name;
}

inline std::ostream& operator<<(std::ostream& os, const std::vector<Parameter>& v)
{
    if (v.empty())
        return os;
    os << v.at(0);
    for (size_t i = 1; i < v.size(); ++i)
        os << ", " << v.at(i);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Function& v)
{
    return os << v.return_type << ' ' << v.name << '(' << v.parameters << ')';
}

inline std::ostream& operator<<(std::ostream& os, const UiEvent& v)
{
    return os << v.name << '(' << v.parameters << ')';
}


static const std::unordered_set<std::string_view> kValidTypes = {
    "Array"sv,
    "Boolean"sv,
    "Buffer"sv,
    "Dictionary"sv,
    "Float"sv,
    "Integer"sv,
    "LuaRef"sv,
    "Object"sv,
    "String"sv,
    "Tabpage"sv,
    "Window"sv,
    "void"sv,
};

static std::string parseType(std::string_view sv)
{
    if (sv.empty())
        return {};
    auto it = kValidTypes.find(sv);
    if (it != kValidTypes.end())
        return std::string { *it };
    if (sv.substr(0, 8) == "ArrayOf(" && sv.back() == ')') {
        std::string s { sv };
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '(') {
                s[i] = '<';
            } else if (s[i] == ')') {
                s[i] = '>';
            }
        }
        return s;
    }
    return {};
}


static void parseVersion(msgpack_view::object_view obj)
{
    std::cerr << "\nversion\n";
    for (auto [k, v] : obj.as<msgpack_view::map_view>()) {
        std::cerr << "  " << k << ": " << v << '\n';
    }
}

static void parseFunctions(msgpack_view::object_view obj)
{
    std::cerr << "\nfunctions\n";
    for (auto it : obj.as<msgpack_view::array_view>()) {
        Function fn;
        for (auto it2 : it.as<msgpack_view::map_view>()) {
            auto key = it2.first.as<std::string_view>();
            auto val = it2.second;
            if (key == "parameters"sv) {
                auto params = val.as<msgpack_view::array_view>();
                fn.parameters.reserve(params.size());
                for (auto it3 : params) {
                    auto param = it3.as<msgpack_view::array_view>();
                    if (param.size() < 1) {
                        std::cerr << "invalid parameters size\n";
                        continue;
                    }
                    Parameter p;
                    p.type = parseType(param[0].as<std::string_view>());
                    if (param.size() >= 2)
                        p.name = param[1].as<std::string>();
                    fn.parameters.emplace_back(std::move(p));
                }
            } else if (key == "return_type"sv) {
                fn.return_type = parseType(val.as<std::string_view>());
            } else if (key == "name"sv) {
                fn.name = val.as<std::string>();
            } else if (key == "method"sv) {
                fn.method = val.as<bool>();
            } else if (key == "since"sv) {
                fn.since = val.as<int64_t>();
            } else if (key == "deprecated_since"sv) {
                fn.deprecated_since = val.as<int64_t>();
            }
        }
        if (std::string_view { fn.name }.substr(0, 5) != "nvim_"sv)
            continue;
        std::cerr << "  " << fn << '\n';
    }
}

static void parseUiEvents(msgpack_view::object_view obj)
{
    std::cerr << "\nui events\n";
    for (auto it : obj.as<msgpack_view::array_view>()) {
        UiEvent fn;
        for (auto it2 : it.as<msgpack_view::map_view>()) {
            auto key = it2.first.as<std::string_view>();
            auto val = it2.second;
            if (key == "parameters"sv) {
                auto params = val.as<msgpack_view::array_view>();
                fn.parameters.reserve(params.size());
                for (auto it3 : params) {
                    auto param = it3.as<msgpack_view::array_view>();
                    if (param.size() < 1) {
                        std::cerr << "invalid parameters size\n";
                        continue;
                    }
                    Parameter p;
                    p.type = param[0].as<std::string>();
                    if (param.size() >= 2)
                        p.name = param[1].as<std::string>();
                    fn.parameters.emplace_back(std::move(p));
                }
            } else if (key == "name"sv) {
                fn.name = val.as<std::string>();
            } else if (key == "since"sv) {
                fn.since = val.as<int64_t>();
            } else if (key == "deprecated_since"sv) {
                fn.deprecated_since = val.as<int64_t>();
            }
        }
        std::cerr << "  " << fn << '\n';
    }
}

static void parseUiOptions(msgpack_view::object_view obj)
{
    std::cerr << "\nui options\n";
    for (auto it : obj.as<msgpack_view::array_view>()) {
        std::cerr << "  " << it << '\n';
    }
}

static void parseErrorTypes(msgpack_view::object_view obj)
{
    std::cerr << "\nerror types\n";
    for (auto [k, v] : obj.as<msgpack_view::map_view>()) {
        std::cerr << "  " << k << ": " << v << '\n';
    }
}

static void parseTypes(msgpack_view::object_view obj)
{
    std::cerr << "\ntypes\n";
    for (auto [k, v] : obj.as<msgpack_view::map_view>()) {
        std::cerr << "  " << k << ": " << v << '\n';
    }
}

static void parseApi(msgpack_view::object_view obj)
{
    for (auto [k, v] : obj.as<msgpack_view::map_view>()) {
        auto key = k.as<std::string_view>();
        if (key == "version"sv) {
            parseVersion(v);
        } else if (key == "functions"sv) {
            parseFunctions(v);
        } else if (key == "ui_events"sv) {
            parseUiEvents(v);
        } else if (key == "ui_options"sv) {
            parseUiOptions(v);
        } else if (key == "error_types"sv) {
            parseErrorTypes(v);
        } else if (key == "types"sv) {
            parseTypes(v);
        }
    }
}


int main(int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QProcess p;
    p.start("nvim", QStringList { "--api-info" });
    p.waitForFinished();
    const auto output = p.readAllStandardOutput();
    auto res = msgpack::unpack(output.data(), output.size());
    parseApi(res.get());
}
