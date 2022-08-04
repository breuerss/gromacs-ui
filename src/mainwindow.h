#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "settings.h"

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

private:
    Ui::MainWindow* ui;
    PreferencesDialog* preferencesDialog;
    void setupUIForProject();
    Settings settings;
    void setMoleculeFile(const QString& file = "");
};
#endif // MAINWINDOW_H
