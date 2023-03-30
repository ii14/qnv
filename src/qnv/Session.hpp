#pragma once

#include <QColor>
#include <QFont>
#include <QHash>
#include <QObject>

#include "qnv/Defs.hpp"

class Session : public QObject
{
  Q_OBJECT

  Q_PROPERTY(int cellWidth READ cellWidth NOTIFY fontChanged)
  Q_PROPERTY(int cellHeight READ cellHeight NOTIFY fontChanged)

public:
  explicit Session(QObject* parent = nullptr);

  void setFont(const QFont& font);
  void setDefaultColors(const QColor& fg, const QColor& bg, const QColor& sp);

  int cellWidth() const { return mCellWidth; }
  int cellHeight() const { return mCellHeight; }

public slots:
  void onHlAttrDefine(const std::vector<std::pair<uint32_t, HlGroup>>&);
  void onGridCursorGoto(int64_t grid, int64_t row, int64_t col);

signals:
  void fontChanged();
  void defaultColorsChanged();
  void hlChanged();
  void cursorMoved();
  void flush();

public:
  int64_t mCursorGrid { -1 };
  int64_t mCursorRow { -1 };
  int64_t mCursorCol { -1 };

  QHash<int64_t, HlGroup> mHls;
  QColor mBgColor { QColor::fromRgb(0x000000) };
  QColor mFgColor { QColor::fromRgb(0xFFFFFF) };
  QColor mSpColor { QColor::fromRgb(0xFFFFFF) };

  QFont mFont;
  int mCellWidth { -1 };
  int mCellBaseline { -1 };
  int mCellHeight { -1 };
};

// vim: tw=100 sw=2 sts=2 et
