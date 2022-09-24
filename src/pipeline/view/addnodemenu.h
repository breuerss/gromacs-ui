#ifndef PIPELINE_VIEW_ADDNODEMENU_H
#define PIPELINE_VIEW_ADDNODEMENU_H

#include <functional>
#include <QWidget>

class QParallelAnimationGroup;

namespace Pipeline { namespace View {

class AddNodeButton;

class AddNodeMenu : public QWidget {
  Q_OBJECT
public:
  struct ButtonDefinition {
    QString label;
    std::function<void()> callback;
  };

  AddNodeMenu(
    const QList<ButtonDefinition>& definitions,
    const QColor& buttonColor,
    QWidget* trigger,
    QWidget* parent
    );
  ~AddNodeMenu();

protected:
  void hideEvent(QHideEvent *event) override;

public slots:
  void show();
  void hide();
  void toggle();

private:
  void addMoveAnimation(AddNodeButton* button, const QPoint& start, const QPoint& end);
  QList<AddNodeButton*> buttons;
  std::shared_ptr<QParallelAnimationGroup> showAnimation;
  void createShowAnimation();
  QColor buttonColor;
  QWidget* trigger;
};

} }

#endif
