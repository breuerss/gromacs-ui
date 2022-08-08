#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>
#include "settings.h"

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PreferencesDialog(QWidget *parent = nullptr);
  ~PreferencesDialog();

protected:
  void showEvent(QShowEvent *ev);

  private slots:
  void openSelectProjectDirectory();
  void openSelectGmx();
  void openSelectPdbwc();
  void openSelectPdbchain();
  void openSelectPdbdelhetatm();
  void saveSettings();

private:
  Ui::PreferencesDialog* ui;
  Settings settings;

  void loadSettings();
  void setProjectDirectory(const QString& newProjectDirectory);
  void setGmx(const QString& newGmxPath);
  void setPdbwc(const QString& newPdbwc);
  void setPdbchain(const QString& newPdbchain);
  void setPdbdelhetatm(const QString& newPdbdelhetatm);

  QString projectDirectory;
  QString gmx;
  QString pdbwc;
  QString pdbchain;
  QString pdbdelhetatm;
};

#endif // PREFERENCESDIALOG_H
