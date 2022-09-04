#ifndef NEUTRALISESETTINGS_H
#define NEUTRALISESETTINGS_H

#include <QWidget>

namespace Ui {
class NeutraliseSettings;
}

namespace Pipeline { namespace Neutralise {
class Configuration;
} }

class NeutraliseSettings : public QWidget
{
    Q_OBJECT

public:
    explicit NeutraliseSettings(Pipeline::Neutralise::Configuration* newConfig);
    ~NeutraliseSettings();

private:
    void setIonFromModel();
    void connectIonSelectors();

    Ui::NeutraliseSettings *ui;
    Pipeline::Neutralise::Configuration* config;
};

#endif
