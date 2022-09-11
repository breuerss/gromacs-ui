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
#include "../../projectmanager.h"
#include "../../model/project.h"
#include "../pdbdownload/step.h"
#include "../pdbfixer/step.h"
#include "../creategromacsmodel/step.h"
#include "../createbox/step.h"
#include "../solvate/step.h"
#include "../neutralise/step.h"
#include "../simulation/step.h"
#include "../simulation/configuration.h"

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
    { "V", Category::Viewer },
    { "P", Category::PostProcess },
  };

  QMap<Category, QList<AddNodeMenu::ButtonDefinition>> nodeMenuDefinitions;
  nodeMenuDefinitions[Category::DataProvider] = {
    { "PDB Downloader", addStepToProject<Pipeline::PdbDownload::Step> },
    //{ "Load From File", []() {} },
  };
  nodeMenuDefinitions[Category::PreProcess] = {
    { "Preparation Pipeline", [] () {
      auto step1 = addStepToProject<Pipeline::PdbFixer::Step>();
      auto step2 = addStepToProject<Pipeline::CreateGromacsModel::Step>();
      step2->getFileObjectConsumer()->connectTo(
        step1->getFileObjectProvider()->provides()[0]
        );
      auto step3 = addStepToProject<Pipeline::CreateBox::Step>();
      step3->getFileObjectConsumer()->connectTo(
        step2->getFileObjectProvider()->provides()[0]
        );

      auto step4 = addStepToProject<Pipeline::Solvate::Step>();
      step4->getFileObjectConsumer()->connectTo(
        step3->getFileObjectProvider()->provides()[0]
        );

      auto step5 = addStepToProject<Pipeline::Neutralise::Step>();
      step5->getFileObjectConsumer()->connectTo(
        step4->getFileObjectProvider()->provides()[0]
        );

      // TODO
      // Filter
      // Solvate
    }},
    { "PDB Fixer", addStepToProject<Pipeline::PdbFixer::Step> },
    { "Create GROMACS Model", addStepToProject<Pipeline::CreateGromacsModel::Step> },
    { "Create Box", addStepToProject<Pipeline::CreateBox::Step> },
    { "Solvate", addStepToProject<Pipeline::Solvate::Step> },
    { "Neutralise", addStepToProject<Pipeline::Neutralise::Step> },
  };
  nodeMenuDefinitions[Category::Viewer] = {
    //{ "Trajectory Viewer", []() {} },
    //{ "Coordinate Viewer", []() {} },
  };

  using SimulationType = Pipeline::Simulation::Configuration::Type;
  nodeMenuDefinitions[Category::Simulation] = {
    { "Minimisation",
      [] () {
        addSimulationToProject(SimulationType::Minimisation);
      } },
    { "NVT Simulation",
      [] () {
        addSimulationToProject(SimulationType::NVT);
      } },
    { "NPT Simulation",
      [] () {
        addSimulationToProject(SimulationType::NPT);
      } },
  };
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
