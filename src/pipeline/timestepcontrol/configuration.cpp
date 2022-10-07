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

QString Configuration::getForCommand() const
{
  return getArgsForCommand().join(" ");
}

QStringList Configuration::getArgsForCommand() const
{
  QStringList options;
  options << "-b" << QString::number(startTimeStep);
  if (endTimeStep >= 0)
  {
    options << "-e" << QString::number(endTimeStep);
  }
  options << "-tu" << TimeStepControl::toString(timeUnit);
  options << "-skip" << QString::number(stride);
  options << "-dt" << QString::number(interval);
  return options;
}

QString Configuration::toString()
{
  return 
    tr("Start Time Step") + ": " + QString::number(startTimeStep)
    + " " + TimeStepControl::toString(timeUnit) + "<br>"
   + tr("End Time Step") + ": " + QString::number(endTimeStep)
   + " " + TimeStepControl::toString(timeUnit) + "<br>"
   + tr("Skip every nth step") + ": " + QString::number(stride) + "<br>"
   + tr("Take step divisable by") + ": " + QString::number(interval)
   + " " + TimeStepControl::toString(timeUnit) + "<br>";
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
