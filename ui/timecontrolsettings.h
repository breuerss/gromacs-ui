#ifndef TIMECONTROLSETTINGS_H
#define TIMECONTROLSETTINGS_H

#include "../src/pipeline/timestepcontrol/configuration.h"
#include <QWidget>
#include <QMetaObject>

namespace Ui {
class TimeControlSettings;
}

class TimeControlSettings : public QWidget
{
  Q_OBJECT

public:
  explicit TimeControlSettings(Pipeline::TimeStepControl::Configuration* config);
  ~TimeControlSettings();

private:
  void setupTimeUnitOptions();
  Ui::TimeControlSettings *ui;
  Pipeline::TimeStepControl::Configuration* config;
  QList<QMetaObject::Connection> conns;
};

#endif // TIMECONTROLSETTINGS_H
