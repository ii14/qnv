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
    Q_PROPERTY(qint64 id READ id CONSTANT)
    Q_PROPERTY(bool hidden READ hidden NOTIFY hiddenChanged)
    Q_PROPERTY(qint64 zIndex READ zIndex NOTIFY zIndexChanged)
    Q_PROPERTY(Qnv::Anchor anchor READ anchor NOTIFY anchorChanged)
    Q_PROPERTY(qint64 anchorGrid READ anchorGrid NOTIFY anchorChanged)
    Q_PROPERTY(double anchorRow READ anchorRow NOTIFY anchorChanged)
    Q_PROPERTY(double anchorCol READ anchorCol NOTIFY anchorChanged)
    Q_PROPERTY(double focusable READ focusable NOTIFY anchorChanged)

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
    bool hidden() const { return mHidden; }

    qint64 id() const { return mId; }
    qint64 zIndex() const { return mZIndex; }
    void setZIndex(qint64 zindex);

    Qnv::Anchor anchor() const { return mAnchor; }
    qint64 anchorGrid() const { return mAnchorGrid; }
    double anchorRow() const { return mAnchorRow; }
    double anchorCol() const { return mAnchorCol; }
    double focusable() const { return mFocusable; }

signals:
    void gridSizeChanged();
    void winPosChanged();
    void hiddenChanged();
    void zIndexChanged();
    void anchorChanged();

public:
    // TODO: z index
    qint64 mId { -1 }; // TODO: implement
    qint64 mZIndex { -1 };

    Anchor mAnchor { Anchor::NE };
    qint64 mAnchorGrid { -1 };
    double mAnchorRow { 0.0 };
    double mAnchorCol { 0.0 };
    bool mFocusable { true };

    std::vector<GridCell> mGridCells;
    size_t mGridWidth { 0 };
    size_t mGridHeight { 0 };
    bool mHidden { false };

    int mWinX { 0 };
    int mWinY { 0 };

    friend class GridRenderer;
};
