#pragma once

#include <QQuickPaintedItem>
#include <QPointer>

#include "qnv/Grid.hpp"
#include "qnv/Session.hpp"

class GridRenderer : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Grid* grid READ grid WRITE setGrid NOTIFY gridChanged)
    Q_PROPERTY(Session* session READ session WRITE setSession NOTIFY sessionChanged)

public:
    explicit GridRenderer(QQuickItem* parent = nullptr);

    Grid* grid() const { return mGrid; }
    void setGrid(Grid* grid);
    Session* session() const { return mSession; }
    void setSession(Session* session);

    void paint(QPainter* p) override;

public slots:
    void flush() { update(); }

signals:
    void gridChanged();
    void sessionChanged();

private:
    QPointer<Grid> mGrid;
    QPointer<Session> mSession;
};
