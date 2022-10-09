#ifndef GUI_PROGRESSCHART_H
#define GUI_PROGRESSCHART_H

#include <QChartView>
#include <QChart>
#include <QSplineSeries>
#include <cmath>

namespace Gui {

using namespace QtCharts;

class ProgressChart: public QChartView
{
public:
  ProgressChart();

  void appendValue(float value);
  void setValues(QList<float> value);
  void clear();

private:
  void updateRange();
  QChart* progressValueChart;
  QSplineSeries* series;
  float min = INFINITY;
  float max = -INFINITY;
};

}

#endif
