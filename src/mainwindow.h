#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"
#include "model/step.h"

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class PreferencesDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void openPreferencesDialog();
    void addTabForStep(std::shared_ptr<Step> step, int at = -1);

private:
    Ui::MainWindow* ui;
    PreferencesDialog* preferencesDialog;
    void setupUIForProject();
    Settings settings;
    void setMoleculeFile(const QString& file = "", const QString& traj = "");
    void removeTabAt(int index);
};
#endif // MAINWINDOW_H
