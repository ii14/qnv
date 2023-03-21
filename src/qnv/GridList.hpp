#pragma once

#include <QAbstractListModel>
#include <vector>

class Grid;

class GridList : public QAbstractListModel
{
    Q_OBJECT

    enum Role {
        RoleId = Qt::UserRole + 1,
        RoleGrid,
    };

public:
    explicit GridList(QObject* parent = nullptr);

    [[nodiscard]] Grid* get(int64_t id);
    std::pair<Grid*, bool> add(int64_t id);
    bool remove(int64_t id);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<Grid*> mItems;
};
