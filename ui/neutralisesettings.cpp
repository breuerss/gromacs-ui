#include "neutralisesettings.h"
#include "ui_neutralisesettings.h"
#include "../src/pipeline/neutralise/configuration.h"
#include "connectionhelper.h"

NeutraliseSettings::NeutraliseSettings(Pipeline::Neutralise::Configuration* newConfig)
  : QWidget(nullptr)
    , ui(new Ui::NeutraliseSettings)
    , config(newConfig)
{
  ui->setupUi(this);
  setIonFromModel();
  connectIonSelectors();
  connectToSpinBox<QDoubleSpinBox, double>(ui->ionConcentration, config, "ionConcentration");
}

NeutraliseSettings::~NeutraliseSettings()
{
  delete ui;
}

void NeutraliseSettings::setIonFromModel()
{
  QString positiveIon = config->property("positiveIon").value<QString>();
  QString negativeIon = config->property("negativeIon").value<QString>();
  QMap<QString, QRadioButton*> map = {
    { "MG", ui->mgIon },
    { "CA", ui->caIon },
    { "LI", ui->liIon },
    { "NA", ui->naIon },
    { "K", ui->kIon },
    { "RB", ui->rbIon },
    { "CS", ui->csIon },
    { "F", ui->fIon },
    { "CL", ui->clIon },
    { "BR", ui->brIon },
    { "I", ui->iIon },
  };

  for (const auto& element : map.keys())
  {
    if (positiveIon == element || negativeIon == element)
    {
      map[element]->setChecked(true);
    }
  }

}

void NeutraliseSettings::connectIonSelectors()
{
  QMap<QRadioButton*, QString> positiveIonMap = {
    { ui->mgIon, "MG" },
    { ui->caIon, "CA" },
    { ui->liIon, "LI" },
    { ui->naIon, "NA" },
    { ui->kIon, "K" },
    { ui->rbIon, "RB" },
    { ui->csIon, "CS" },
  };
  for (auto button: positiveIonMap.keys())
  {
    QString ion = positiveIonMap[button];
    connect(button, &QRadioButton::toggled,
            [this, ion] (bool checked)
            {
              if (checked)
              {
                config->setProperty("positiveIon", ion);
              }
            });
  }

  QMap<QRadioButton*, QString> negativeIonMap = {
    { ui->fIon, "F" },
    { ui->clIon, "CL" },
    { ui->brIon, "BR" },
    { ui->iIon, "I" },
  };
  for (auto button: negativeIonMap.keys())
  {
    QString ion = negativeIonMap[button];
    connect(
      button,
      &QRadioButton::toggled,
      [this, ion] (bool checked)
      {
        if (checked)
        {
          config->setProperty("negativeIon", ion);
        }
      });
  }
}
