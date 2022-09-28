#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"

#include <QMainWindow>
#include <QChartView>
#include <QList>
#include <memory>

namespace Ui {
class MainWindow;
}

using namespace QtCharts;

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
  void setMoleculeFile(const QString& file = "", const QString& traj = "");
  void setTextFile(const QString& fileName);
  void setGraph(const QString& fileName);
  QList<QMetaObject::Connection> conns;
  QChartView* graphView;
};
#endif // MAINWINDOW_H
