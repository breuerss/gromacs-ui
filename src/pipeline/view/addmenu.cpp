#include "addmenu.h"
#include "addnodebutton.h"
#include "addnodemenu.h"
#include "colors.h"
#include <memory>
#include <QPropertyAnimation>
#include <QDebug>
#include <QIcon>
#include "../../projectmanager.h"
#include "../../model/project.h"
#include "../pdbdownload/step.h"
#include "../simulation.h"

namespace Pipeline { namespace View {

AddMenu::AddMenu(ActionButton* trigger)
  : QWidget(trigger->parentWidget())
  , trigger(trigger)
  , showAnimation(std::make_shared<QParallelAnimationGroup>())
{
  using Category = Step::Category;
  QList<ButtonDefinition> definitions = {
    { "D", Category::DataProvider },
    { "P", Category::Preprocess },
    { "S", Category::Simulation },
    { "V", Category::Viewer },
  };

  QMap<Category, QList<AddNodeMenu::ButtonDefinition>> nodeMenuDefinitions;
  nodeMenuDefinitions[Category::DataProvider] = {
    {
      "PDB Downloader",
      "pdbDownloader",
      [] () {
        ProjectManager::getInstance()->getCurrentProject()->addStep<Pipeline::PdbDownload::Step>();
      }
    },
    { "Load From File", "fileloader", []() {} },
  };
  nodeMenuDefinitions[Category::Viewer] = {
    { "Trajectory Viewer", "trajectoryViewer", []() {} },
    { "Coordinate Viewer", "coordinateViewer", []() {} },
  };
  nodeMenuDefinitions[Category::Simulation] = {
    { "Minimisation", "nptSimulation",
      [] () {
        ProjectManager::getInstance()->getCurrentProject()->addStep<Pipeline::Simulation>();
      } },
    { "NVT Simulation", "nvtSimulation", []() {} },
    { "NPT Simulation", "nptSimulation", []() {} },
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
