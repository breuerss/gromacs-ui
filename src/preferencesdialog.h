#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();
    const QString PROJECT_DIRECTORY = "General/projectDirectory";
    const QString GMX_PATH = "Gromacs/gmx";

protected:
      void showEvent(QShowEvent *ev);

private slots:
    void openSelectProjectDirectory();
    void openSelectPdb2gmx();
    void saveSettings();

private:
    Ui::PreferencesDialog* ui;
    QSettings settings;

    QString projectDirectory;
    QString gmx;

    void loadSettings();
    void setProjectDirectory(const QString& newProjectDirectory);
    void setGmx(const QString& newGmxPath);
};

#endif // PREFERENCESDIALOG_H
