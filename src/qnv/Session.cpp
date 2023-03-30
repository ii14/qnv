#include "qnv/Session.hpp"

#include <QRawFont>

Session::Session(QObject* parent) : QObject(parent)
{
  // suckless configuration, just modify the source code(tm)
  // QFont font { "tewi" };
  QFont font { "JetBrains Mono NL" };
  font.setPixelSize(13);
  setFont(font);

  auto fg = QColor::fromRgb(0xABB2BF);
  auto bg = QColor::fromRgb(0x282C34);
  setDefaultColors(fg, bg, fg);
}

void Session::setFont(const QFont& font)
{
  mFont = font;
  const auto rawFont = QRawFont::fromFont(font);
  mCellWidth = static_cast<int>(std::min(rawFont.averageCharWidth(), rawFont.maxCharWidth()));
  mCellBaseline = static_cast<int>(rawFont.ascent());
  mCellHeight = mCellBaseline + static_cast<int>(rawFont.descent());
  emit fontChanged();
}

void Session::setDefaultColors(const QColor& fg, const QColor& bg, const QColor& sp)
{
  mFgColor = fg;
  mBgColor = bg;
  mSpColor = sp;
  emit defaultColorsChanged();
}

void Session::onHlAttrDefine(const std::vector<std::pair<uint32_t, HlGroup>>& data)
{
  for (const auto& hl : data)
    mHls.insert(hl.first, hl.second);
  emit hlChanged();
}

void Session::onGridCursorGoto(int64_t grid, int64_t row, int64_t col)
{
  mCursorGrid = grid;
  mCursorRow = row;
  mCursorCol = col;
  emit cursorMoved();
}

// vim: tw=100 sw=2 sts=2 et
