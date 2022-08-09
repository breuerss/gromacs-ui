#ifndef TEMPERATUREGROUPCONFIGFORM_H
#define TEMPERATUREGROUPCONFIGFORM_H

#include <QWidget>

namespace Ui {
class TemperatureGroupConfigForm;
}

class TemperatureGroupConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit TemperatureGroupConfigForm(QWidget *parent = nullptr);
    ~TemperatureGroupConfigForm();

private:
    Ui::TemperatureGroupConfigForm *ui;
};

#endif // TEMPERATUREGROUPCONFIGFORM_H
