#include "qnv/GridRenderer.hpp"

#include <QPainter>
#include <QGlyphRun>

#include "qnv/Grid.hpp"

GridRenderer::GridRenderer(QQuickItem* parent) : QQuickPaintedItem(parent)
{
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
  setAntialiasing(true);
}

void GridRenderer::setGrid(Grid* grid)
{
  if (mGrid == grid)
    return;
  if (not mGrid.isNull())
    disconnect(mGrid, nullptr, this, nullptr);
  mGrid = grid;
  if (grid != nullptr) {
    connect(grid, &Grid::hiddenChanged, this, [this]() { setVisible(not mGrid->mHidden); });
  }
  emit gridChanged();
}

void GridRenderer::setSession(Session* session)
{
  if (mSession == session)
    return;
  if (not mSession.isNull())
    disconnect(mSession, nullptr, this, nullptr);
  mSession = session;
  if (session != nullptr) {
    connect(session, &Session::flush, this, &GridRenderer::flush);
    connect(session, &Session::defaultColorsChanged, this, [this]() {
      setFillColor(mSession->mBgColor);
      flush();
    });

    setFillColor(mSession->mBgColor);
    flush();
  }
  emit sessionChanged();
}

void GridRenderer::paint(QPainter* p)
{
  // TODO: This method runs on a rendering thread, but data is not protected by a mutex.
  // Docs hint that this is fine, but I'm not sure if I understood it correctly. Confirm
  // with TSAN or something

  const Grid* grid = mGrid.data();
  const Session* session = mSession.data();
  if (grid == nullptr || session == nullptr)
    return;

  // QRawFont and QGlyphRun are implicitly shared classes aka copy-on-write, and they
  // always allocate memory in the constructor. Keep them outside of the loop
  auto font = session->mFont;
  const auto rawFont = QRawFont::fromFont(font);

  QGlyphRun glyphRun;
  glyphRun.setRawFont(rawFont);

  // JetBrains Mono reports a large max width and correct average width, but tewi
  // reports a very large average width and correct max width. Use whatever is the
  // lowest. Maybe calculate the width from some glyph instead?
  const auto width = static_cast<int>(std::min(rawFont.averageCharWidth(), rawFont.maxCharWidth()));
  const auto baseline = rawFont.ascent();
  const auto height = static_cast<int>(baseline + rawFont.descent());

  uint32_t hl = 0; ///< Selected highlight group
  QColor fg = session->mFgColor; ///< Selected foreground color
  QColor bg = session->mBgColor; ///< Selected background color

  QPen pen;
  pen.setWidth(1);
  pen.setCapStyle(Qt::SquareCap);
  pen.setJoinStyle(Qt::MiterJoin);
  pen.setColor(fg);
  p->setPen(pen);

  auto setHl = [&](uint32_t id) {
    if (hl == id)
      return;
    hl = id;
    auto it = session->mHls.constFind(hl);
    bg = it != session->mHls.constEnd() && it->mBgColor != -1 ? QColor::fromRgb(it->mBgColor)
                                                              : session->mBgColor;
    auto nfg = it != session->mHls.constEnd() && it->mFgColor != -1 ? QColor::fromRgb(it->mFgColor)
                                                                    : session->mFgColor;
    if (fg != nfg) {
      fg = nfg;
      pen.setColor(fg);
      p->setPen(pen);
    }
  };

  for (size_t i = 0; i < grid->mGridCells.size(); ++i) {
    const auto& cell = grid->mGridCells[i];
    const int x = i % grid->mGridWidth;
    const int y = i / grid->mGridWidth;

    // TODO: Maybe do more characters at the time whenever possible?
    // TODO: There are already slight performance problems with this. Profile this,
    // and find some way of redrawing just certain parts of the screen. I think it's
    // possible to tell QQuickPaintedItem to redraw just some parts, but is that going
    // to be enough? Maybe create QQuickPaintedItem per line.
    // TODO: See what even happens when font.supportsCharacter(ch) fails
    // TODO: Figure out multibyte and composed characters like emojis, and ligatures
    // TODO: Render box drawing and block element characters (2500-259F) + powerline
    // characters (E0B0-E0BF) manually, so they are properly aligned with the grid.
    // TODO: Allow rendering multiple fonts, including non-monospace ones?

    uint32_t glyphIndex = 0;
    int numGlyphs = 1;
    bool res = rawFont.glyphIndexesForChars(cell.mCh, 1, &glyphIndex, &numGlyphs);
    Q_ASSERT(res);
    Q_ASSERT(numGlyphs == 1);
    const QPointF glyphPos { 0, baseline };
    // NOTE: setRawData DOES NOT COPY the data
    glyphRun.setRawData(&glyphIndex, &glyphPos, numGlyphs);
    // docs say this is supposed to be useful for something, ligatures or whatever
    // glyphRun.setBoundingRect(font.boundingRect(glyphIndex));

    setHl(cell.mHl);

    const int px = x * width;
    const int py = y * height;

    // Draw cell background
    // TODO: Doing this cell by cell can get slow
    if (bg != session->mBgColor)
      p->fillRect(px, py, width, height, bg);

    // Draw a glyph
    p->drawGlyphRun({ static_cast<double>(px), static_cast<double>(py) }, glyphRun);

    // Draw the cursor
    // TODO: Cursor shapes
    if (session->mCursorGrid == grid->mId && x == session->mCursorCol && y == session->mCursorRow) {
      // To draw 1 pixel wide lines with antialiasing enabled,
      // the position has to point to the middle of the pixel.
      QRectF rect(px + 0.5, py + 0.5, width - 1, height - 1);
      p->drawRoundedRect(rect, 1.0, 1.0);
      // p->drawRect(rect);
    }
  }
}

// vim: tw=100 sw=2 sts=2 et
