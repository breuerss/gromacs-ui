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
  void addMoveAnimation(std::shared_ptr<ActionButton> button, const QPoint& start, const QPoint& end);
  std::shared_ptr<ActionButton> addDataProviderButton;
  std::shared_ptr<ActionButton> addPreprocessButton;
  std::shared_ptr<ActionButton> addSimulationButton;
  std::shared_ptr<ActionButton> addViewerButton;
  ActionButton* trigger;
  std::shared_ptr<QParallelAnimationGroup> showAnimation;
  void createShowAnimation();

};

} }

#endif
