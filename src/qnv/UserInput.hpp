#pragma once

#include <QObject>
#include <QKeyEvent>

class UserInput : public QObject
{
  Q_OBJECT

public:
  explicit UserInput(QObject* parent = nullptr) : QObject(parent) { }

  static QString translateKeyCode(const QKeyEvent& ev);

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

signals:
  void keyEvent(const QString& key);
};

// vim: tw=100 sw=2 sts=2 et
