#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void openPreferencesDialog();

private:
    Ui::MainWindow* ui;
    PreferencesDialog* preferencesDialog;
    void setupUIForProject();
};
#endif // MAINWINDOW_H
