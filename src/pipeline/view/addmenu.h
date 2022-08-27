#ifndef PIPELINE_VIEW_MENU_H
#define PIPELINE_VIEW_MENU_H

#include "actionbutton.h"
#include <QWidget>
#include <QParallelAnimationGroup>
#include <memory>

namespace Pipeline { namespace View {

class AddMenu : public QWidget {
  Q_OBJECT
public:
  AddMenu(ActionButton* trigger);
  QSize sizeHint() const override;

public slots:
  void show();
  void hide();
  void toggle();

private:
  void addMoveAnimation(ActionButton* button, const QPoint& start, const QPoint& end);

  struct ButtonDefinition {
    QColor color;
    QString label;
    QString buttonType;
  };

  typedef QPair<QString, ActionButton*> ButtonPair;
  QList<ButtonPair> buttons;
  ActionButton* trigger;
  std::shared_ptr<QParallelAnimationGroup> showAnimation;
  void createShowAnimation();

};

} }

#endif
