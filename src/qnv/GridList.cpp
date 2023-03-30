#include "qnv/GridList.hpp"

#include "qnv/Grid.hpp"

GridList::GridList(QObject* parent) : QAbstractListModel(parent) { }

Grid* GridList::get(qint64 id)
{
  // TODO: use a map, or binary search or something
  for (Grid* grid : mItems)
    if (grid->mId == id)
      return grid;
  return nullptr;
}

std::pair<Grid*, bool> GridList::add(qint64 id)
{
  for (Grid* grid : mItems)
    if (grid->mId == id)
      return { grid, false };
  auto* grid = new Grid(this);
  grid->mId = id;
  grid->setZIndex(id == 1 ? -1 : 0); // global grid id=1 is always at the bottom
  const auto idx = static_cast<int>(mItems.size());
  beginInsertRows({}, idx, idx);
  mItems.push_back(grid);
  endInsertRows();
  return { grid, true };
}

bool GridList::remove(qint64 id)
{
  for (size_t i = 0; i < mItems.size(); ++i) {
    Grid* grid = mItems[i];
    if (grid->mId != id)
      continue;
    const auto idx = static_cast<int>(i);
    beginRemoveRows({}, idx, idx);
    mItems.erase(mItems.begin() + static_cast<ptrdiff_t>(i));
    endRemoveRows();
    delete grid;
    return true;
  }
  return false;
}

int GridList::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return static_cast<int>(mItems.size());
}

QVariant GridList::data(const QModelIndex& index, int role) const
{
  const int idx = index.row();
  if (idx < 0 || static_cast<size_t>(idx) >= mItems.size())
    return {};
  Grid* dev = mItems[idx];
  if (role == Role::RoleGrid)
    return QVariant::fromValue(dev);
  if (role == Role::RoleId)
    return static_cast<quint64>(dev->mId);
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

// vim: tw=100 sw=2 sts=2 et
