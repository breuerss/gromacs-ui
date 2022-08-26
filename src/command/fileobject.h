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

  FileObject(const QString& newFileName, Type newType);
  bool exists() const;

  const QString fileName;
  const Type type;
};

}

#endif
