#ifndef PIPELINE_VIEW_ADDNODEMENU_H
#define PIPELINE_VIEW_ADDNODEMENU_H

#include "addnodebutton.h"
#include <QWidget>
#include <QParallelAnimationGroup>
#include <functional>
#include <memory>

namespace Pipeline { namespace View {

class AddNodeMenu : public QWidget {
  Q_OBJECT
public:
  struct ButtonDefinition {
    QString label;
    QString buttonType;
    std::function<void()> callback;
  };

  AddNodeMenu(
    const QList<ButtonDefinition>& definitions,
    const QColor& buttonColor,
    QWidget* trigger,
    QWidget* parent
    );

protected:
  void hideEvent(QHideEvent *event) override;

public slots:
  void show();
  void hide();
  void toggle();

private:
  void addMoveAnimation(AddNodeButton* button, const QPoint& start, const QPoint& end);
  typedef QPair<QString, AddNodeButton*> ButtonPair;
  QList<ButtonPair> buttons;
  std::shared_ptr<QParallelAnimationGroup> showAnimation;
  void createShowAnimation();
  QColor buttonColor;
  QWidget* trigger;
};

} }

#endif
