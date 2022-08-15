#include "progresschart.h"

#include <QGraphicsView>
#include <QGraphicsLayout>

namespace Gui {

ProgressChart::ProgressChart()
{
  progressValueChart = new QChart;
  progressValueChart->setMargins(QMargins());
  progressValueChart->legend()->hide();
  progressValueChart->layout()->setContentsMargins(0, 0, 0, 0);
  progressValueChart->setBackgroundVisible(false);

  series = new QSplineSeries();
  progressValueChart->addSeries(series);

  progressValueChart->createDefaultAxes();
  auto horizontal = progressValueChart->axes(Qt::Horizontal).first();
  horizontal->setVisible(false);
  auto vertical = progressValueChart->axes(Qt::Vertical).first();
  vertical->setVisible(false);

  setChart(progressValueChart);
  setFrameStyle(QFrame::NoFrame);
  setRenderHint(QPainter::Antialiasing);
  setStyleSheet("background-color: transparent;");
}

void ProgressChart::setValues(QList<float> values)
{
  clear();
  for (const auto& value: values)
  {
    appendValue(value);
  }
}

void ProgressChart::appendValue(float value)
{
  series->append(QPointF(series->count(), value));
  min = std::min<float>(min, value);
  max = std::max<float>(max, value);
  updateRange();
}

void ProgressChart::clear()
{
  series->removePoints(0, series->count());
  min = INFINITY;
  max = -INFINITY;
}

void ProgressChart::updateRange()
{
  auto vertical = progressValueChart->axes(Qt::Vertical).first();
  vertical->setRange(min, max);

  auto horizontal = progressValueChart->axes(Qt::Horizontal).first();
  horizontal->setRange(0, series->count());
}

}
