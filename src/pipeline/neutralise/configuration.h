#ifndef PIPELINE_CREATEBOX_CONFIGURATION_H
#define PIPELINE_CREATEBOX_CONFIGURATION_H

#include "../../model/serializable.h"
#include "../../config/configuration.h"
#include <QString>
#include <memory>

namespace Pipeline { namespace Neutralise {

class Configuration
: public Model::Serializable
, public Config::Configuration
{
  Q_OBJECT
public:
  Configuration() = default;

  Q_PROPERTY(QString negativeIon MEMBER negativeIon NOTIFY negativeIonChanged)
  Q_PROPERTY(QString positiveIon MEMBER positiveIon NOTIFY positiveIonChanged)
  Q_PROPERTY(double ionConcentration MEMBER ionConcentration NOTIFY ionConcentrationChanged)

  QWidget* getUI() override;

signals:
  void negativeIonChanged(const QString&);
  void positiveIonChanged(const QString&);
  void ionConcentrationChanged(double);

private:
  double ionConcentration = 0.15;
  QString positiveIon = "NA";
  QString negativeIon = "CL";
};

} }

#endif
