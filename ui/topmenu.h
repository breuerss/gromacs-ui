#ifndef TOPMENU_H
#define TOPMENU_H

#include <QWidget>

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

signals:
  void alignLeftClicked();
  void alignRightClicked();
  void alignHCenterClicked();
  void alignTopClicked();
  void alignBottomClicked();
  void alignVCenterClicked();
  void distributeVerticallyClicked();
  void distributeHorizontallyClicked();

private:
  Ui::TopMenu *ui;
};

#endif // TOPMENU_H
