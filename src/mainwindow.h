#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"

#include <QMainWindow>
#include <QList>
#include <memory>

namespace Ui {
class MainWindow;
}
namespace Config {
class Simulation;
}

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
  void addTabForStep(std::shared_ptr<Config::Simulation> step, int at = -1);

private:
  Ui::MainWindow* ui;
  PreferencesDialog* preferencesDialog;
  void setupUIForProject();
  Settings settings;
  void setMoleculeFile(const QString& file = "", const QString& traj = "");
  void setTextFile(const QString& fileName);
  void removeTabAt(int index);
  QList<QMetaObject::Connection> conns;
};
#endif // MAINWINDOW_H
