#ifndef PIPELINE_TIMESTEPCONTROL_CONFIGURATION_H
#define PIPELINE_TIMESTEPCONTROL_CONFIGURATION_H

#include "../../config/configuration.h"

namespace Pipeline { namespace TimeStepControl {

class Configuration
: public Config::Configuration
{
  Q_OBJECT
public:
  enum class TimeUnit : int {
    FemtoSeconds = 0,
    PicoSeconds,
    NanosSconds,
    MicroSeconds,
    MilliSeconds,
    Seconds,
  };
  Q_ENUM(TimeUnit);

  Configuration();
  Q_PROPERTY(TimeUnit timeUnit MEMBER timeUnit NOTIFY timeUnitChanged)
  Q_PROPERTY(double startTimeStep MEMBER startTimeStep NOTIFY startTimeStepChanged)
  Q_PROPERTY(double endTimeStep MEMBER endTimeStep NOTIFY endTimeStepChanged)
  Q_PROPERTY(double stride MEMBER stride NOTIFY strideChanged)
  Q_PROPERTY(double interval MEMBER interval NOTIFY intervalChanged)

  QWidget* getUI() override;
  QString toString() override;

signals:
  void timeUnitChanged(TimeUnit);
  void startTimeStepChanged(double startTimeStep);
  void endTimeStepChanged(double endTimeStep);
  void strideChanged(int stride);
  void intervalChanged(double interval);

private:
  TimeUnit timeUnit = TimeUnit::PicoSeconds;
  double startTimeStep = 0;
  double endTimeStep = -1;
  int stride = 1;
  double interval = 0;
};

QString toString(Configuration::TimeUnit type);

} }

#endif
