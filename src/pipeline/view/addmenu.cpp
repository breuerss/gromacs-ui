#include "addmenu.h"
#include <memory>
#include <QPropertyAnimation>
#include <QDebug>
#include <QIcon>
#include <qabstractanimation.h>
#include <qeasingcurve.h>
#include <qpushbutton.h>

namespace Pipeline { namespace View {

AddMenu::AddMenu(ActionButton* trigger)
  : QWidget(trigger->parentWidget())
  , addDataProviderButton(std::make_shared<ActionButton>(50, "#b1c41b", this))
  , addPreprocessButton(std::make_shared<ActionButton>(50, "orange", this))
  , addSimulationButton(std::make_shared<ActionButton>(50, "orange", this))
  , addViewerButton(std::make_shared<ActionButton>(50, "orange", this))
  , trigger(trigger)
  , showAnimation(std::make_shared<QParallelAnimationGroup>())
{
  addDataProviderButton->setText("D");
  addPreprocessButton->setText("P");
  addSimulationButton->setText("S");
  addViewerButton->setText("V");
  connect(addSimulationButton.get(), &QPushButton::clicked, [this] {
    auto button = new ActionButton(50, "#000055", this);
    qDebug() << addSimulationButton->pos();
    button->show();
    QPropertyAnimation* animation = new QPropertyAnimation(button, "pos");
    animation->setStartValue(addSimulationButton->pos());
    animation->setEndValue(addSimulationButton->pos() + QPoint(70, 0));
    animation->setEasingCurve(QEasingCurve::OutQuad);
    animation->setDuration(200);
    animation->start();
  });
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
  int height = 20 + trigger->width();
  auto position = QPoint(
    (trigger->width() - addDataProviderButton->width()) / 2, -height
    );
  auto start = trigger->mapToParent(QPoint(position.x(), 0));
  auto end = trigger->mapToParent(position);
  addMoveAnimation(addDataProviderButton, start, end);
  position.ry() -= height;
  end = trigger->mapToParent(position);
  addMoveAnimation(addPreprocessButton, start, end);
  position.ry() -= height;
  end = trigger->mapToParent(position);
  addMoveAnimation(addSimulationButton, start, end);
  position.ry() -= height;
  end = trigger->mapToParent(position);
  addMoveAnimation(addViewerButton, start, end);
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
  std::shared_ptr<ActionButton> button, const QPoint& start, const QPoint& end)
{
  auto animation = new QPropertyAnimation(button.get(), "pos");
  animation->setDuration(200);
  animation->setStartValue(start);
  animation->setEndValue(end);
  animation->setEasingCurve(QEasingCurve::OutQuad);
  showAnimation->addAnimation(animation);
}
} }
