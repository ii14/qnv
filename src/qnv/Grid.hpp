#pragma once

#include <QColor>
#include <QFont>
#include <QHash>
#include <QObject>
#include <vector>

#include "qnv/Defs.hpp"

class GridRenderer;

class Grid : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int gridWidth READ gridWidth NOTIFY gridSizeChanged)
    Q_PROPERTY(int gridHeight READ gridHeight NOTIFY gridSizeChanged)
    Q_PROPERTY(int winX READ winX NOTIFY winPosChanged)
    Q_PROPERTY(int winY READ winY NOTIFY winPosChanged)
    Q_PROPERTY(int winY READ winY NOTIFY winPosChanged)
    Q_PROPERTY(qint64 id READ id CONSTANT)
    Q_PROPERTY(qint64 zIndex READ zIndex NOTIFY zIndexChanged)

public:
    explicit Grid(QObject* parent = nullptr);

    void setGridSize(size_t width, size_t height);
    void setWinPos(int x, int y, int width, int height);
    void setMsgPos(int y);

    void onGridLine(const GridLine&);
    void onGridScroll(int64_t top, int64_t bot, int64_t left, int64_t right, int64_t rows, int64_t cols);

    int gridWidth() const { return mGridWidth; }
    int gridHeight() const { return mGridHeight; }
    int winX() const { return mWinX; }
    int winY() const { return mWinY; }

    void clear();
    void setHidden(bool hidden);

    qint64 id() const { return mId; }
    qint64 zIndex() const { return mZIndex; }
    void setZIndex(qint64 zindex);

signals:
    void gridSizeChanged();
    void winPosChanged();
    void hiddenChanged();
    void zIndexChanged();

public:
    // TODO: z index
    qint64 mId { -1 }; // TODO: implement
    qint64 mZIndex { -1 };
    qint64 mAnchorGrid { -1 };

    std::vector<GridCell> mGridCells;
    size_t mGridWidth { 0 };
    size_t mGridHeight { 0 };
    bool mHidden { false };

    int mWinX { 0 };
    int mWinY { 0 };

    friend class GridRenderer;
};
