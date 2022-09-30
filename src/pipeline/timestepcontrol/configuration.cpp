#include "configuration.h"
#include "../ui/timecontrolsettings.h"
#include <memory>
#include <QMetaProperty>

namespace Pipeline { namespace TimeStepControl {

Configuration::Configuration()
{
  static bool registered = false;
  if (!registered)
  {
    qRegisterMetaType<TimeUnit>("Configuration::TimeUnit");
    qRegisterMetaTypeStreamOperators<int>("Configuration::TimeUnit");
    registered = true;
  }

  connectAllSignals();
}

QWidget* Configuration::getUI()
{
  return new TimeControlSettings(this);
}

QString Configuration::toString()
{
  return 
    tr("Start Time Step") + ": " + QString::number(startTimeStep)
    + " " + TimeStepControl::toString(timeUnit) + "\n"
   + tr("End Time Step") + ": " + QString::number(endTimeStep)
   + " " + TimeStepControl::toString(timeUnit) + "\n"
   + tr("Skip every nth step") + ": " + QString::number(stride) + "\n"
   + tr("Take step divisable by") + ": " + QString::number(interval)
   + " " + TimeStepControl::toString(timeUnit) + "\n";
}

using TimeUnit = Configuration::TimeUnit;
QString toString(TimeUnit type)
{
  const static QMap<TimeUnit, QString> map = {
    { TimeUnit::FemtoSeconds, "fs" },
    { TimeUnit::PicoSeconds, "ps" },
    { TimeUnit::NanosSconds, "ns" },
    { TimeUnit::MicroSeconds, "us" },
    { TimeUnit::MilliSeconds, "ms" },
    { TimeUnit::Seconds, "s" },
  };

  return map[type];
}

} }
