#include "addnodemenu.h"
#include "addmenu.h"
#include "addnodebutton.h"
#include "colors.h"
#include <memory>
#include <QPropertyAnimation>
#include <QDebug>
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
    buttons << ButtonPair({ definition.buttonType, button });
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

void AddNodeMenu::show()
{
  auto triggerCenter = trigger->geometry().center();
  QWidget::show();
  qDebug() << geometry();
  setGeometry(childrenRect());
  move(triggerCenter - QPoint(-trigger->width(), height() / 2));
  disconnect(showAnimation.get(), 0, 0, 0);
  createShowAnimation();
  showAnimation->setDirection(QAbstractAnimation::Forward);
  showAnimation->start();
}

void AddNodeMenu::hide()
{
  qDebug() << "hide" << __PRETTY_FUNCTION__;

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
  //qDebug() << "hideEvent";
  //showAnimation->clear();
  //hide();
}

void AddNodeMenu::createShowAnimation()
{
  showAnimation->clear();

//  auto position = QPoint(
//    (trigger->width() - buttons[0].second->width()) / 2, -height
//    );
//  auto start = trigger->mapToParent(QPoint(position.x(), 0));
//  auto end = trigger->mapToParent(position);
  for (auto& buttonPair: buttons)
  {
    auto button = buttonPair.second;
    addMoveAnimation(button,
                     button->pos() - QPoint(button->width(), 0),
                     QPoint(0, button->y())
                     );
//    position.ry() -= buttonPair.second->height() + buffer;
//    end = trigger->mapToParent(position);
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
