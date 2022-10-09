#ifndef IO_GRAPH_DATA_H
#define IO_GRAPH_DATA_H

#include <QString>
#include <QList>

namespace IO {

class GraphData {

public:
  struct Series {
    QString legend;
    QList<float> entries;
  };
  QString title;
  QString xLabel;
  QString yLabel;
  QList<Series> data;
};

}

#endif
