#ifndef TEMPERATUREGROUPCONFIGFORM_H
#define TEMPERATUREGROUPCONFIGFORM_H

#include <QWidget>
#include <memory>
#include "../pipeline/simulation/temperaturecouplinggroup.h"

namespace Model {
class TemperatureCouplingGroup;
}

namespace Ui {
class TemperatureGroupConfigForm;
}

class TemperatureGroupConfigForm : public QWidget
{
  Q_OBJECT

public:
  using TemperatureCouplingGroup = Pipeline::Simulation::TemperatureCouplingGroup;
  explicit TemperatureGroupConfigForm(
    std::shared_ptr<TemperatureCouplingGroup> model, QWidget *parent = nullptr);
  ~TemperatureGroupConfigForm();

signals:
  void removeRequested();

private:
  std::shared_ptr<TemperatureCouplingGroup> model;
  Ui::TemperatureGroupConfigForm *ui;
};

#endif // TEMPERATUREGROUPCONFIGFORM_H
