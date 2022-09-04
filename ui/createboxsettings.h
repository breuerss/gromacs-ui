#ifndef CREATEBOXSETTINGS_H
#define CREATEBOXSETTINGS_H

#include <QWidget>
#include "../src/pipeline/createbox/configuration.h"

namespace Ui {
class CreateBoxSettings;
}

class CreateBoxSettings : public QWidget
{
    Q_OBJECT

public:
    explicit CreateBoxSettings(Pipeline::CreateBox::Configuration* newConfig);
    ~CreateBoxSettings();

private:
    void prepareBoxOptions();

    Ui::CreateBoxSettings *ui;
    Pipeline::CreateBox::Configuration* config;
};

#endif
