#ifndef PROJECT_H
#define PROJECT_H


#include <QString>
#include <QDataStream>
#include <QObject>
#include <vector>
#include <memory>

namespace Model {

class SystemSetup;
class Step;

class Project : public QObject
{
  Q_OBJECT
public:
  Project();

  std::shared_ptr<Step> addStep();
  void clearSteps();
  void removeStep(int index);
  const std::vector<std::shared_ptr<Step>>& getSteps() const;
  std::shared_ptr<SystemSetup> getSystemSetup() const;

  QString getProjectPath();

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged);

signals:
  void stepAdded(std::shared_ptr<Step> step, int at);
  void stepRemoved(std::shared_ptr<Step> step, int at);
  void nameChanged(const QString&);

private:
  std::vector<std::shared_ptr<Step>> steps;
  std::shared_ptr<SystemSetup> systemSetup;
  QString name = "";
};

QDataStream &operator<<(QDataStream &out, const Project &project);
QDataStream &operator>>(QDataStream &in, Project &project);

}

#endif // PROJECT_H
