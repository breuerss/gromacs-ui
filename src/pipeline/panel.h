#ifndef PIPELINE_PANEL_H
#define PIPELINE_PANEL_H

#include <QGraphicsScene>

namespace Pipeline {

class Panel : public QGraphicsScene
{

public:
  Panel(QObject* parent = nullptr);
};

}

#endif
