#pragma once

#include <QObject>
#include <vector>
#include "qnv/Defs.hpp"
#include "qnv/ApiTypes.hpp"

class NvimProcess : public QObject
{
  Q_OBJECT

public:
  explicit NvimProcess(QObject* parent = nullptr);
  ~NvimProcess() override;

  NvimProcess(const NvimProcess&) = delete;
  NvimProcess(NvimProcess&&) = delete;
  NvimProcess& operator=(const NvimProcess&) = delete;
  NvimProcess& operator=(NvimProcess&&) = delete;

  void start();

signals:
  void gridLine(const std::vector<GridLine>&);
  void gridCursorGoto(Integer grid, Integer row, Integer col);
  void gridResize(Integer grid, Integer width, Integer height);
  void gridClear(Integer grid);
  void gridDestroy(Integer grid);
  void gridScroll(Integer grid,
                  Integer top,
                  Integer bot,
                  Integer left,
                  Integer right,
                  Integer rows,
                  Integer cols);

  void hlAttrDefine(const std::vector<std::pair<uint32_t, HlGroup>>&);
  void hlGroupSet(const String& name, Integer id);

  void winPos(
      Integer grid, Window win, Integer startRow, Integer startCol, Integer width, Integer height);
  void winFloatPos(Integer grid,
                   Window win,
                   Anchor anchor,
                   Integer anchorGrid,
                   Float anchorRow,
                   Float anchorCol,
                   Boolean focusable,
                   Integer zindex);
  void winViewport(Integer grid,
                   Window win,
                   Integer topline,
                   Integer botline,
                   Integer curline,
                   Integer curcol,
                   Integer lineCount);
  void winHide(Integer grid);
  void winClose(Integer grid);
  void defaultColorsSet(uint32_t fg, uint32_t bg, uint32_t sp, int ctermFg, int ctermBg);

  void flush();
  void mouse(Boolean on);
  void busy(Boolean on);
  void suspend();
  void bell();
  void visualBell();
  void setTitle(const String& title);
  void setIcon(const String& icon);

  void modeChange(const String& mode, Integer modeIdx);
  void msgSetPos(Integer grid, Integer row, Boolean scrolled, const String& sepChar);

  void tablineUpdate(Tabpage tab,
                     const std::vector<std::pair<Tabpage, String>>& tabs,
                     Buffer buf,
                     const std::vector<std::pair<Buffer, String>>& bufs);

  void cmdlineShow(const std::vector<std::pair<Integer, String>>& content,
                   Integer pos,
                   char firstc,
                   String prompt,
                   Integer indent,
                   Integer level);
  void cmdlinePos(Integer pos, Integer level);
  void cmdlineSpecialChar(const String& c, Boolean shift, Integer level);
  void cmdlineHide(Integer level);
  void cmdlineBlockShow(const std::vector<std::pair<Integer, String>>& lines);
  void cmdlineBlockAppend(const std::vector<std::pair<Integer, String>>& lines);
  void cmdlineBlockHide();

  void msgShow(const String& kind,
               const std::vector<std::pair<Integer, String>>& content,
               Boolean replace_last);
  void msgClear();
  void msgShowcmd(const std::vector<std::pair<Integer, String>>& content);
  void msgShowmode(const std::vector<std::pair<Integer, String>>& content);
  void msgRuler(const std::vector<std::pair<Integer, String>>& content);
  void msgHistoryShow(const std::vector<std::pair<String, String>>& entries);
  void msgHistoryClear();

public slots:
  void sendInput(const QString& key);
  void tryResize(uint32_t width, uint32_t height);
  void tryResizeGrid(int64_t grid, uint32_t width, uint32_t height);

private slots:
  void onStarted();
  void onReadyRead();

private:
  struct Private;
  friend struct Private;
  Private* d;
};

// vim: tw=100 sw=2 sts=2 et
