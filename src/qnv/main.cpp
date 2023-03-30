#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "qnv/Config.hpp"
#include "qnv/Grid.hpp"
#include "qnv/GridRenderer.hpp"
#include "qnv/GridList.hpp"
#include "qnv/NvimProcess.hpp"
#include "qnv/UserInput.hpp"
#include "qnv/Session.hpp"

int main(int argc, char** argv)
{
  QGuiApplication app { argc, argv };

  QQmlApplicationEngine engine;
  QQmlContext* ctx = engine.rootContext();
  qmlRegisterType<GridRenderer>("QNV", 1, 0, "GridRenderer");
  qmlRegisterUncreatableMetaObject(Qnv::staticMetaObject, "Qnv", 1, 0, "Qnv", "Enums");

  // TODO: Use stdint.h types for msgpack only, and Qt's types everywhere
  // else, so these don't have to be registered in the type system.
  qRegisterMetaType<int8_t>("int8_t");
  qRegisterMetaType<uint8_t>("uint8_t");
  qRegisterMetaType<int16_t>("int16_t");
  qRegisterMetaType<uint16_t>("uint16_t");
  qRegisterMetaType<int32_t>("int32_t");
  qRegisterMetaType<uint32_t>("uint32_t");
  qRegisterMetaType<int64_t>("int64_t");
  qRegisterMetaType<uint64_t>("uint64_t");

  // Created with new + using a parent, because clang-tidy's NewDelete
  // checks like to give false positives for Qt5's shared/guarded
  // pointers, and this usually seems to calm it down.
  auto* session = new Session(&app);
  auto* gridList = new GridList(&app);
  auto* nvim = new NvimProcess(&app);
  auto* input = new UserInput(&app);

  // Installing a global event filter for handling keyboard
  // input might not work once custom QML is introduced.
  app.installEventFilter(input);
  QObject::connect(input, &UserInput::keyEvent, nvim, &NvimProcess::sendInput);

  ctx->setContextProperty("gGridList", gridList);
  ctx->setContextProperty("gSession", session);
  ctx->setContextProperty("gNeovim", nvim);

  // TODO: Build option to compile QRC or load QML at runtime
  const QUrl url { Config::runtimePath() + "/qml/main.qml" };
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
  engine.load(url);

  // TODO: Move all these connections into some class
  // that wraps nvim process, grid list and session.
  QObject::connect(nvim, &NvimProcess::gridLine, [&](const std::vector<GridLine>& data) {
    int64_t id = -1;
    Grid* grid = nullptr;
    for (const auto& d : data) {
      if (id != d.mGrid) {
        id = d.mGrid;
        grid = gridList->get(d.mGrid);
      }
      if (grid != nullptr)
        grid->onGridLine(d);
    }
  });

  QObject::connect(nvim, &NvimProcess::gridClear, [&](Integer id) {
    if (Grid* grid = gridList->get(id))
      grid->clear();
  });

  QObject::connect(nvim, &NvimProcess::gridDestroy, [&](Integer id) { gridList->remove(id); });

  QObject::connect(nvim, &NvimProcess::gridResize, [&](Integer id, Integer width, Integer height) {
    gridList->add(id).first->setGridSize(width, height);
  });

  QObject::connect(nvim, &NvimProcess::gridScroll,
                   [&](Integer id, Integer top, Integer bot, Integer left, Integer right,
                       Integer rows, Integer cols) {
                     if (Grid* grid = gridList->get(id))
                       grid->onGridScroll(top, bot, left, right, rows, cols);
                   });

  QObject::connect(nvim, &NvimProcess::winPos,
                   [&](Integer id, Window win, Integer startRow, Integer startCol, Integer width,
                       Integer height) {
                     Q_UNUSED(win); // is window handle going to be useful for anything?
                     if (Grid* grid = gridList->get(id))
                       grid->setWinPos(startCol, startRow, width, height);
                   });

  QObject::connect(nvim, &NvimProcess::winHide, [&](Integer id) {
    if (Grid* grid = gridList->get(id))
      grid->setHidden(true);
  });

  QObject::connect(nvim, &NvimProcess::winClose, [&](Integer id) {
    if (Grid* grid = gridList->get(id))
      grid->setHidden(true);
  });

  // TODO: msg_set_pos should set zindex=200 on the target grid
  QObject::connect(nvim, &NvimProcess::msgSetPos,
                   [&](Integer id, Integer row, Boolean scrolled, const String& sepChar) {
                     Q_UNUSED(scrolled); // TODO: handle whatever this stuff is
                     Q_UNUSED(sepChar);
                     gridList->add(id).first->setMsgPos(row);
                   });

  QObject::connect(nvim, &NvimProcess::winFloatPos,
                   [&](Integer id, Window win, Anchor anchor, Integer anchorGrid, Float anchorRow,
                       Float anchorCol, Boolean focusable, Integer zindex) {
                     Q_UNUSED(win);
                     Q_UNUSED(anchor);
                     if (Grid* grid = gridList->get(id)) {
                       grid->mAnchor = anchor;
                       grid->mAnchorGrid = anchorGrid;
                       grid->mAnchorRow = anchorRow;
                       grid->mAnchorCol = anchorCol;
                       grid->mFocusable = focusable;
                       emit grid->anchorChanged();
                       grid->setZIndex(zindex);
                     }
                   });

  QObject::connect(nvim, &NvimProcess::gridCursorGoto, session, &Session::onGridCursorGoto);
  QObject::connect(nvim, &NvimProcess::hlAttrDefine, session, &Session::onHlAttrDefine);
  QObject::connect(nvim, &NvimProcess::flush, session, &Session::flush);

  nvim->start();

  return app.exec();
}

extern "C" const char* __ubsan_default_options()
{
  return "print_stacktrace=1";
}

extern "C" const char* __asan_default_options()
{
  return "detect_leaks=0,halt_on_error=1";
}

// vim: tw=100 sw=2 sts=2 et
