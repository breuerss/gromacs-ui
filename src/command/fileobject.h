#ifndef COMMAND_FILEOBJECT_H
#define COMMAND_FILEOBJECT_H

#include <QString>
#include <QObject>
#include <memory>

namespace Command {

class FileObject : public QObject
{

  Q_OBJECT
public:
  typedef std::shared_ptr<FileObject> Pointer;
  enum class Type {
    Unknown,
    GRO,
    PDB,
    XTC,
    TRR,
    EDR,
    LOG,
    TPR,
    MDP,
    TOP,
  };

  enum class Category {
    Unknown,
    Coordinates,
    Trajectory,
    Energy,
    Velocities,
    Forces,
    Topology,
    Text,
  };

  FileObject(Type newType);
  bool exists() const;
  void setFileName(const QString& fileName);
  const QString& getFileName() const;

  const Type type;
  static Category getCategoryFor(Type);

signals:
  void fileNameChanged(const QString& newFileName);

private:
  QString fileName;
};

QJsonObject &operator<<(QJsonObject &out, const FileObject::Pointer);
QJsonObject &operator>>(QJsonObject &in, FileObject::Pointer step);

}

#endif
