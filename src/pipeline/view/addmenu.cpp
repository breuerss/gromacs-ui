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

namespace Pipeline { namespace View {

AddMenu::AddMenu(ActionButton* trigger)
  : QWidget(trigger->parentWidget())
  , trigger(trigger)
  , showAnimation(std::make_shared<QParallelAnimationGroup>())
{
  QList<ButtonDefinition> definitions = {
    { Colors::Pink, "D", "addDataProvider" },
    { Colors::Orange, "P", "addPreprocess" },
    { Colors::Blue, "S", "addSimulation" },
    { Colors::Violet, "V", "addViewer" },
  };

  QMap<QString, QList<AddNodeMenu::ButtonDefinition>> nodeMenuDefinitions;
  nodeMenuDefinitions["addDataProvider"] = {
    {
      "PDB Downloader",
      "pdbDownloader",
      [] () {
      }
    },
    { "Load From File", "fileloader", []() {} },
  };
  nodeMenuDefinitions["addViewer"] = {
    { "Trajectory Viewer", "trajectoryViewer", []() {} },
    { "Coordinate Viewer", "coordinateViewer", []() {} },
  };
  nodeMenuDefinitions["addSimulation"] = {
    { "Minimisation", "nptSimulation",
      [] () {
        ProjectManager::getInstance()->getCurrentProject()->addStep<Command::RunSimulation>();
      } },
    { "NVT Simulation", "nvtSimulation", []() {} },
    { "NPT Simulation", "nptSimulation", []() {} },
  };
  for (const auto& definition: definitions)
  {
    auto button = new ActionButton(50, definition.color, this);
    button->setText(definition.label);
    buttons << ButtonPair({ definition.buttonType, button });
    menus[definition.buttonType] = new AddNodeMenu(
      nodeMenuDefinitions[definition.buttonType], definition.color, button, this);
    connect(
      button, &QPushButton::clicked,
      [
        this,
        buttonType = definition.buttonType
      ] () {
        for (const auto& currentButtonType: menus.keys())
        {
          auto menu = menus[currentButtonType];
          if (currentButtonType == buttonType)
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
    (trigger->width() - buttons[0].second->width()) / 2, -height
    );
  auto start = trigger->mapToParent(QPoint(position.x(), 0));
  auto end = trigger->mapToParent(position);
  for (auto& buttonPair: buttons)
  {
    addMoveAnimation(buttonPair.second, start, end);
    position.ry() -= buttonPair.second->height() + buffer;
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
