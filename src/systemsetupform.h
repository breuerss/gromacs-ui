#ifndef SYSTEMSETUPFORM_H
#define SYSTEMSETUPFORM_H

#include <QWidget>
#include "model/systemsetup.h"

namespace Ui {
class SystemSetupForm;
}

class SystemSetupForm : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSetupForm(std::shared_ptr<SystemSetup> systemSetup, QWidget *parent = nullptr);
    ~SystemSetupForm();

private:
    Ui::SystemSetupForm *ui;
    std::shared_ptr<SystemSetup> systemSetup;

    void prepareWaterOptions();
    void prepareForceFieldOptions();
    void prepareBoxOptions();
};

#endif // SYSTEMSETUPFORM_H
