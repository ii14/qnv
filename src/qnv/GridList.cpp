#include "qnv/GridList.hpp"

#include "qnv/Grid.hpp"

GridList::GridList(QObject* parent)
    : QAbstractListModel(parent)
{
}

Grid* GridList::get(int64_t id)
{
    // TODO: use a map, or binary search or something
    for (Grid* grid : mItems)
        if (grid->mId == id)
            return grid;
    return nullptr;
}

std::pair<Grid*, bool> GridList::add(int64_t id)
{
    for (Grid* grid : mItems)
        if (grid->mId == id)
            return { grid, false };
    auto* grid = new Grid(this);
    grid->mId = id;
    grid->setZIndex(id == 1 ? -1 : 0); // global grid id=1 is always at the bottom
    beginInsertRows({}, mItems.size(), mItems.size());
    mItems.push_back(grid);
    endInsertRows();
    return { grid, true };
}

bool GridList::remove(int64_t id)
{
    for (size_t i = 0; i < mItems.size(); ++i) {
        Grid* grid = mItems[i];
        if (grid->mId != id)
            continue;
        mItems.erase(mItems.begin() + i);
        delete grid;
        return true;
    }
    return false;
}

int GridList::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return mItems.size();
}

QVariant GridList::data(const QModelIndex& index, int role) const
{
    const int idx = index.row();
    if (idx < 0 || size_t(idx) >= mItems.size())
        return {};
    Grid* dev = mItems[idx];
    if (role == Role::RoleGrid)
        return QVariant::fromValue(dev);
    if (role == Role::RoleId)
        return quint64(dev->mId);
    return {};
}

QHash<int, QByteArray> GridList::roleNames() const
{
    static const QHash<int, QByteArray> kRoleNames = {
        { Role::RoleGrid, "grid" },
        { Role::RoleId, "id" },
    };
    return kRoleNames;
}
