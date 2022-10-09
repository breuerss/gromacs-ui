#ifndef GROMACSSYSTEMSETTINGS_H
#define GROMACSSYSTEMSETTINGS_H

#include <QWidget>
#include "../src/pipeline/creategromacsmodel/configuration.h"

namespace Ui {
class GromacsSystemSettings;
}

class GromacsSystemSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GromacsSystemSettings(Pipeline::CreateGromacsModel::Configuration* config);
    ~GromacsSystemSettings();

private:
    void prepareForceFieldOptions();
    void prepareWaterOptions();

    Ui::GromacsSystemSettings *ui;
    Pipeline::CreateGromacsModel::Configuration* config;
};

#endif // GROMACSSYSTEMSETTINGS_H
