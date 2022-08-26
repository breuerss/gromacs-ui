#ifndef COMMAND_FILEOBJECT_H
#define COMMAND_FILEOBJECT_H

#include <QString>

namespace Command {

class FileObject
{

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

  const Type type;
  static Category getCategoryFor(Type);

private:
  QString fileName;
};

}

#endif
