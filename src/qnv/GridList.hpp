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

  [[nodiscard]] Q_INVOKABLE Grid* get(qint64 id);
  std::pair<Grid*, bool> add(qint64 id);
  bool remove(qint64 id);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

private:
  std::vector<Grid*> mItems;
};

// vim: tw=100 sw=2 sts=2 et
