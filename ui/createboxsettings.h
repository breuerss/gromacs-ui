#ifndef CREATEBOXSETTINGS_H
#define CREATEBOXSETTINGS_H

#include "../src/pipeline/createbox/configuration.h"
#include <QWidget>
#include <QMetaObject>

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
    QList<QMetaObject::Connection> conns;
};

#endif
