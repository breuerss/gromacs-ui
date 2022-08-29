#ifndef PIPELINE_VIEW_MENU_H
#define PIPELINE_VIEW_MENU_H

#include "actionbutton.h"
#include "addnodemenu.h"
#include "../step.h"
#include <QWidget>
#include <QMap>
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
    QString label;
    Step::Category category;
  };

  QList<ActionButton*> buttons;
  ActionButton* trigger;
  std::shared_ptr<QParallelAnimationGroup> showAnimation;
  QMap<Step::Category, AddNodeMenu*> menus;
  void createShowAnimation();

};

} }

#endif
