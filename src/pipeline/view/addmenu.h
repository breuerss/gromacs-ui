#ifndef PIPELINE_ADD_MENU_H
#define PIPELINE_ADD_MENU_H

#include "../step.h"
#include "../../projectmanager.h"
#include "../../model/project.h"
#include "../../pipeline/simulation/step.h"
#include "src/pipeline/simulation/configuration.h"
#include <QWidget>
#include <QMap>
#include <memory>

class QParallelAnimationGroup;

namespace Pipeline { namespace View {

class AddNodeMenu;
class ActionButton;

class AddMenu : public QWidget {
  Q_OBJECT
public:
  AddMenu(ActionButton* trigger);
  ~AddMenu();
  QSize sizeHint() const override;

public slots:
  void show();
  void hide();
  void toggle();

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  void createShowAnimation();
  void addMoveAnimation(ActionButton* button, const QPoint& start, const QPoint& end);
  void refreshShowAnimation();
  QPoint oldTriggerCenter;

  struct ButtonDefinition {
    QString label;
    Step::Category category;
  };

  QList<ActionButton*> buttons;
  ActionButton* trigger;
  std::shared_ptr<QParallelAnimationGroup> showAnimation;
  QMap<Step::Category, AddNodeMenu*> menus;
  template<typename T>
  static std::shared_ptr<T> addStepToProject()
  {
    return ProjectManager::getInstance()->getCurrentProject()->addStep<T>();
  }

  std::shared_ptr<Pipeline::Simulation::Step>
  addSimulationToProject(Pipeline::Simulation::Configuration::Type type);
};

} }

#endif
