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
  auto addToDefinition = [&nodeMenuDefinitions]<typename Type> (Category category, tag<Type>)
  {
    nodeMenuDefinitions[category] << AddNodeMenu::ButtonDefinition({ Type::name, addStepToProject<Type> });
  };

  addToDefinition(Category::Analysis, tag<Gyrate::Step>{});
  addToDefinition(Category::DataProvider, tag<PdbDownload::Step>{});

  nodeMenuDefinitions[Category::PreProcess] = {
    { "Preparation Pipeline",
      [] () {
        ProjectManager::getInstance()->getCurrentProject()->addPreparationPipeline();

      // TODO
      // Filter
    }},
  };
  addToDefinition(Category::PreProcess, tag<PdbFixer::Step>{});
  addToDefinition(Category::PreProcess, tag<CreateGromacsModel::Step>{});
  addToDefinition(Category::PreProcess, tag<CreateBox::Step>{});
  addToDefinition(Category::PreProcess, tag<Solvate::Step>{});
  addToDefinition(Category::PreProcess, tag<Neutralise::Step>{});

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

  addToDefinition(Category::PostProcess, tag<SmoothTrajectory::Step>{});
  addToDefinition(Category::PostProcess, tag<CenterProtein::Step>{});
  addToDefinition(Category::PostProcess, tag<TimeStepControl::Step>{});

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

  //addViewerButton->setIcon(QIcon::fromTheme("screen"));
  setStyleSheet("background-color: red");
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
  disconnect(showAnimation.get(), 0, 0, 0);
  createShowAnimation();
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
  connect(
    showAnimation.get(), &QAbstractAnimation::finished,
    [this] () {
      QWidget::hide();
    });
  showAnimation->start();
}

void AddMenu::createShowAnimation()
{
  showAnimation->clear();
  const unsigned buffer = 15;
  const unsigned height = buffer + trigger->height();
  auto position = QPoint(
    (trigger->width() - buttons[0]->width()) / 2, -height
    );
  auto start = trigger->mapToParent(QPoint(position.x(), 0));
  auto end = trigger->mapToParent(position);
  for (auto& button: buttons)
  {
    addMoveAnimation(button, start, end);
    position.ry() -= button->height() + buffer;
    end = trigger->mapToParent(position);
  }
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
