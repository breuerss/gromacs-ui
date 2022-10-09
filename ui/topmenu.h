#ifndef TOPMENU_H
#define TOPMENU_H

#include <QWidget>
#include <QMetaObject>

namespace Ui {
class TopMenu;
}

class TopMenu : public QWidget
{
  Q_OBJECT

public:
  explicit TopMenu(QWidget *parent = nullptr);
  ~TopMenu();

  void setAlignmentButtonsEnabled(bool enabled);
  void setDistributionButtonsEnabled(bool enabled);
  void setDeleteButtonEnabled(bool enabled);
  void resizeEvent(QResizeEvent* event);

signals:
  void alignLeftClicked();
  void alignRightClicked();
  void alignHCenterClicked();
  void alignTopClicked();
  void alignBottomClicked();
  void alignVCenterClicked();
  void distributeVerticallyClicked();
  void distributeHorizontallyClicked();
  void deleteSelectedClicked();

private:
  Ui::TopMenu *ui;
  QMetaObject::Connection projectConn;
  QList<QMetaObject::Connection> conns;
};

#endif // TOPMENU_H
