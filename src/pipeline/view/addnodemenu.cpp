#include "addnodemenu.h"
#include "addmenu.h"
#include "addnodebutton.h"
#include "colors.h"
#include <memory>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QIcon>
#include <qpushbutton.h>

namespace Pipeline { namespace View {

AddNodeMenu::AddNodeMenu(
  const QList<ButtonDefinition>& definitions,
  const QColor& buttonColor,
  QWidget* newTrigger,
  QWidget* parent
  )
  : QWidget(parent)
  , showAnimation(std::make_shared<QParallelAnimationGroup>())
  , trigger(newTrigger)
{
  for (const auto& definition: definitions)
  {
    auto button = new AddNodeButton(definition.label, 30, buttonColor, this);
    buttons << button;
    button->move(0, (buttons.length() - 1) * (button->height() + 10));
    connect(
      button, &QPushButton::clicked,
      [ this, callback = definition.callback ] () {
        hide();
        dynamic_cast<AddMenu*>(trigger->parentWidget())->hide();
        callback();
      });
  }

  setStyleSheet("background-color: transparent");
  setMinimumWidth(200);
  stackUnder(trigger);
  QWidget::hide();
  createShowAnimation();
}

AddNodeMenu::~AddNodeMenu()
{
  for(auto button: buttons)
  {
    delete button;
  }
}

void AddNodeMenu::show()
{
  auto triggerCenter = trigger->geometry().center();
  QWidget::show();
  setGeometry(childrenRect());
  move(triggerCenter - QPoint(-trigger->width(), height() / 2));
  disconnect(showAnimation.get(), 0, 0, 0);
  createShowAnimation();
  showAnimation->setDirection(QAbstractAnimation::Forward);
  showAnimation->start();
}

void AddNodeMenu::hide()
{
  if (showAnimation->animationCount())
  {
    showAnimation->setDirection(QAbstractAnimation::Backward);
    connect(
      showAnimation.get(), &QAbstractAnimation::finished,
      [this] () {
        QWidget::hide();
      });
    showAnimation->start();
  }
}

void AddNodeMenu::hideEvent(QHideEvent*)
{
}

void AddNodeMenu::createShowAnimation()
{
  showAnimation->clear();

  for (auto& button: buttons)
  {
    addMoveAnimation(button,
                     button->pos() - QPoint(button->width(), 0),
                     QPoint(0, button->y())
                     );
  }
}

void AddNodeMenu::toggle()
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

void AddNodeMenu::addMoveAnimation(
  AddNodeButton* button, const QPoint& start, const QPoint& end)
{
  auto animation = new QPropertyAnimation(button, "pos");
  animation->setDuration(200);
  animation->setStartValue(start - QPoint(200 * (buttons.length() - showAnimation->animationCount() - 1), 0));
  animation->setEndValue(end);
  animation->setEasingCurve(QEasingCurve::OutQuad);
  showAnimation->addAnimation(animation);
}

} }
