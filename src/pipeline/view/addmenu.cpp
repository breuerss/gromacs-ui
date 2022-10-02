#include "addmenu.h"
#include "addnodebutton.h"
#include "addnodemenu.h"
#include "colors.h"
#include <memory>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QIcon>
#include "../../command/fileobjectprovider.h"
#include "../../command/fileobjectconsumer.h"
#include "../steps.h"

template <typename> struct tag {};
namespace Pipeline { namespace View {

AddMenu::AddMenu(ActionButton* trigger)
  : QWidget(trigger->parentWidget())
  , trigger(trigger)
  , showAnimation(std::make_shared<QParallelAnimationGroup>())
{
  using Category = Step::Category;
  QList<ButtonDefinition> definitions = {
    { "D", Category::DataProvider },
    { "Pre", Category::PreProcess },
    { "S", Category::Simulation },
    { "P", Category::PostProcess },
    { "A", Category::Analysis },
  };

  using namespace Pipeline;
  QMap<Category, QList<AddNodeMenu::ButtonDefinition>> nodeMenuDefinitions;
  auto addToDefinition = [&nodeMenuDefinitions]<typename Type> (tag<Type>)
  {
    nodeMenuDefinitions[Type::config.category] << AddNodeMenu::ButtonDefinition({ Type::config.name, addStepToProject<Type> });
  };

  addToDefinition(tag<Gyrate::Step>{});
  addToDefinition(tag<PdbDownload::Step>{});

  nodeMenuDefinitions[Category::PreProcess] = {
    { "Preparation Pipeline",
      [] () {
        ProjectManager::getInstance()->getCurrentProject()->addPreparationPipeline();

      // TODO
      // Filter
    }},
  };
  addToDefinition(tag<PdbFixer::Step>{});
  addToDefinition(tag<CreateGromacsModel::Step>{});
  addToDefinition(tag<CreateBox::Step>{});
  addToDefinition(tag<Solvate::Step>{});
  addToDefinition(tag<Neutralise::Step>{});

  using SimulationType = Pipeline::Simulation::Configuration::Type;
  nodeMenuDefinitions[Category::Simulation] = {
    { "Simulation Pipeline",
      [] () {
        ProjectManager::getInstance()->getCurrentProject()->addSimulationPipeline();
      } },
    { "Minimisation",
      [this] () {
        addSimulationToProject(SimulationType::Minimisation);
      } },
    { "NVT Simulation",
      [this] () {
        addSimulationToProject(SimulationType::NVT);
      } },
    { "NPT Simulation",
      [this] () {
        addSimulationToProject(SimulationType::NPT);
      } },
  };

  addToDefinition(tag<SmoothTrajectory::Step>{});
  addToDefinition(tag<CenterProtein::Step>{});
  addToDefinition(tag<FitToInput::Step>{});
  addToDefinition(tag<TimeStepControl::Step>{});

  for (const auto& definition: definitions)
  {
    const auto color = Colors::getColorFor(definition.category);
    auto button = new ActionButton(50, color, this);
    button->setText(definition.label);
    buttons << button;
    menus[definition.category] = new AddNodeMenu(
      nodeMenuDefinitions[definition.category], color, button, this);
    connect(
      button, &QPushButton::clicked,
      [
        this,
        category = definition.category
      ] () {
        for (const auto& currentCategory: menus.keys())
        {
          auto menu = menus[currentCategory];
          if (currentCategory == category)
          {
            menu->toggle();
          }
          else 
          {
            menu->hide();
          }
        }

    });
  }

  createShowAnimation();
  stackUnder(trigger);
  QWidget::hide();
}

AddMenu::~AddMenu()
{
  for (auto button : buttons)
  {
    delete button;
  }

  for (auto menu : menus.values())
  {
    delete menu;
  }
}

std::shared_ptr<Pipeline::Simulation::Step>
AddMenu::addSimulationToProject(Pipeline::Simulation::Configuration::Type type)
{
  return ProjectManager::getInstance()->getCurrentProject()->addSimulation(type);
}

QSize AddMenu::sizeHint() const
{
  return QSize(trigger->parentWidget()->width(), trigger->parentWidget()->height());
}

void AddMenu::show()
{
  setFixedSize(sizeHint());
  QWidget::show();
  refreshShowAnimation();
  showAnimation->setDirection(QAbstractAnimation::Forward);
  showAnimation->start();
}

void AddMenu::hide()
{
  for (auto menu : menus)
  {
    menu->hide();
  }
  showAnimation->setDirection(QAbstractAnimation::Backward);
  showAnimation->start();
}

void AddMenu::refreshShowAnimation()
{
  auto triggerCenter = trigger->rect().center();
  auto triggerCenterInViewport = trigger->mapToParent(triggerCenter);
  if (oldTriggerCenter != triggerCenterInViewport && buttons.size() > 0)
  {
    oldTriggerCenter = triggerCenterInViewport;
    showAnimation->clear();

    static const unsigned spacing = 15;

    const auto firstButtonRect = buttons[0]->rect();
    const auto position = trigger->rect().center() - firstButtonRect.center();
    const auto start = trigger->mapToParent(position);

    auto end = start;
    for (const auto& button: buttons)
    {
      end.ry() -= button->height() + spacing;
      addMoveAnimation(button, start, end);
    }
  }
}

void AddMenu::createShowAnimation()
{
  refreshShowAnimation();
  connect(
    showAnimation.get(), &QAbstractAnimation::finished,
    [this] () {
      if (showAnimation->direction() == QAbstractAnimation::Backward)
      {
        QWidget::hide();
      }
    });
}

void AddMenu::toggle()
{
  if (isVisible())
  {
    hide();
  }
  else
  {
    show();
  }
}

void AddMenu::mousePressEvent(QMouseEvent*)
{
  hide();
}

void AddMenu::addMoveAnimation(
  ActionButton* button, const QPoint& start, const QPoint& end)
{
  auto animation = new QPropertyAnimation(button, "pos");
  animation->setDuration(200);
  animation->setStartValue(start);
  animation->setEndValue(end);
  animation->setEasingCurve(QEasingCurve::OutQuad);
  showAnimation->addAnimation(animation);
}
} }
