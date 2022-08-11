#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlist.h"
#include "qobjectdefs.h"
#include "settings.h"

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
namespace Model {
class Simulation;
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
  void addTabForStep(std::shared_ptr<Model::Simulation> step, int at = -1);

private:
  Ui::MainWindow* ui;
  PreferencesDialog* preferencesDialog;
  void setupUIForProject();
  Settings settings;
  void setMoleculeFile(const QString& file = "", const QString& traj = "");
  void removeTabAt(int index);
  QList<QMetaObject::Connection> conns;
};
#endif // MAINWINDOW_H
