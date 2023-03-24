#include "qnv/Grid.hpp"

#include <QRawFont>
#include <QDebug>

#include "qnv/Unicode.hpp"

Grid::Grid(QObject* parent)
    : QObject(parent)
{
}

void Grid::clear()
{
    std::fill(mGridCells.begin(), mGridCells.end(), GridCell {});
}

void Grid::setHidden(bool hidden) {
    if (mHidden != hidden) {
        mHidden = hidden;
        emit hiddenChanged();
    }
}

void Grid::setGridSize(size_t width, size_t height)
{
    // TODO: Reuse previous vector
    std::vector<GridCell> ncells;
    ncells.resize(width * height);
    const size_t h = std::min(height, mGridHeight);
    const size_t w = std::min(width, mGridWidth);
    for (size_t y = 0; y < h; ++y) {
        const auto start = mGridCells.begin() + y * mGridWidth;
        std::copy(start, start + w, ncells.begin() + y * width);
    }

    mGridWidth = width;
    mGridHeight = height;
    mGridCells = ncells;
    emit gridSizeChanged();
}

void Grid::setWinPos(int x, int y, int width, int height)
{
    setHidden(false);
    setGridSize(width, height);
    mWinX = x;
    mWinY = y;
    emit winPosChanged();
}

void Grid::setMsgPos(int y)
{
    setHidden(false);
    mWinY = y;
    emit winPosChanged();
}

void Grid::onGridLine(const GridLine& d)
{
    if (d.mRow < 0 || size_t(d.mRow) >= mGridHeight)
        return;
    if (d.mColStart < 0 || size_t(d.mColStart) >= mGridWidth)
        return;
    size_t col = d.mColStart;
    for (const auto& c : d.mCells) {
        auto hl = c.mHlId;
        QChar ch[2] = { 0, 0 };
        ucsToUtf16(c.mChar, ch);
        for (int64_t i = 0; i < c.mRepeat && col < mGridWidth; ++i) {
            auto& cell = mGridCells[d.mRow * mGridWidth + col++];
            cell.mHl = hl;
            cell.mCh[0] = ch[0];
            cell.mCh[1] = ch[1];
        }
    }
}

void Grid::onGridScroll(
        int64_t top, int64_t bot,
        int64_t left, int64_t right,
        int64_t rows, int64_t cols)
{
    if (Q_UNLIKELY(cols != 0)) {
        qWarning() << "grid_scroll cols not implemented";
        return;
    }

    if (mGridWidth == 0 || mGridHeight == 0)
        return;
    if (top < 0 || top >= bot || size_t(bot) > mGridHeight)
        return;
    if (left < 0 || left >= right || size_t(right) > mGridWidth)
        return;
    if (size_t(std::abs(rows)) > mGridHeight)
        return;

    auto iter = [&](int64_t i) {
        auto src = mGridCells.begin() + (top + i) * mGridWidth;
        auto offset = top + i - rows;
        if (offset >= 0 && offset < int64_t(mGridHeight)) {
            auto dst = mGridCells.begin() + offset * mGridWidth;
            std::move(src + left, src + right, dst + left);
        } else {
            std::fill(src + left, src + right, GridCell {});
        }
    };

    if (rows > 0) {
        for (int64_t i = top; i < bot; ++i)
            iter(i);
    } else if (rows < 0) {
        for (int64_t i = bot - 1; i >= top; --i)
            iter(i);
    }
}