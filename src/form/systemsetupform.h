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

class SystemSetupForm : public QWidget
{
  Q_OBJECT

public:
  explicit SystemSetupForm(std::shared_ptr<Model::Project> project, QWidget *parent = nullptr);
  ~SystemSetupForm();

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
  QList<QMetaObject::Connection> conns;
};

#endif // SYSTEMSETUPFORM_H
