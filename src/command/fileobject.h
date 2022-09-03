#ifndef COMMAND_FILEOBJECT_H
#define COMMAND_FILEOBJECT_H

#include <QString>
#include <QObject>

namespace Command {

class FileObject : public QObject
{

  Q_OBJECT
public:
  enum class Type {
    Unknown,
    GRO,
    PDB,
    XTC,
    TRR,
    EDR,
  };

  enum class Category {
    Unknown,
    Coordinates,
    Trajectory,
    Energy,
    Velocities,
    Forces,
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

}

#endif
