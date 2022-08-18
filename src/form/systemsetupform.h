#ifndef SYSTEMSETUPFORM_H
#define SYSTEMSETUPFORM_H

#include <QWidget>
#include <QMetaObject>

namespace Ui {
class SystemSetupForm;
}

namespace Model {
class SystemSetup;
class Project;
}

namespace Command {
class Queue;
}

class SystemSetupForm : public QWidget
{
  Q_OBJECT

public:
  explicit SystemSetupForm(std::shared_ptr<Model::Project> project, QWidget *parent = nullptr);
  ~SystemSetupForm();

  void showEvent(QShowEvent*);

private:
  Ui::SystemSetupForm *ui;
  std::shared_ptr<Model::Project> project;
  std::shared_ptr<Model::SystemSetup> systemSetup;

  void prepareWaterOptions();
  void prepareForceFieldOptions();
  void prepareBoxOptions();

  void setIonFromModel();
  void connectIonSelectors();

  void setGroupsEnabled(bool enabled);

  void handlePdbDownload(const QString& pdbCode, const QString& filePath);
  void setPdbfixerEnabled(bool enabled);
  void setFilterEnabled(bool enabled);
  void preprocess();
  void stopPreprocess();
  void loadFileFromDisk();
  QList<QMetaObject::Connection> conns;
  std::shared_ptr<Command::Queue> queue;
};

#endif // SYSTEMSETUPFORM_H
