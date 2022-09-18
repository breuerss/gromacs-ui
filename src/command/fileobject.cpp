#include "fileobject.h"

#include <QFile>
#include <QMap>
#include <QJsonObject>

namespace Command {

FileObject::FileObject(Type newType)
  : type(newType)
{
}

bool FileObject::exists() const
{
  return QFile(fileName).exists();
}

void FileObject::setFileName(const QString& newFileName)
{
  const bool changed = fileName != newFileName;
  fileName = newFileName;

  if (changed)
  {
    fileNameChanged(fileName);
  }
}

const QString& FileObject::getFileName() const
{
  return fileName;
}

FileObject::Category FileObject::getCategoryFor(FileObject::Type type)
{
  static const QMap<Type, Category> categoryMap = {
    { Type::GRO, Category::Coordinates },
    { Type::PDB, Category::Coordinates },
    { Type::XTC, Category::Trajectory },
    { Type::TRR, Category::Trajectory },
    { Type::TOP, Category::Topology },
    { Type::LOG, Category::Text },
    { Type::MDP, Category::Text },
  };

  return categoryMap[type];
}

QString FileObject::toString(Type type)
{
  const static QMap<Type, QString> map = {
    { Type::Unknown, "" },
    { Type::GRO, "gro" },
    { Type::PDB, "pdb" },
    { Type::XTC, "xtc" },
    { Type::TRR, "trr" },
    { Type::EDR, "edr" },
    { Type::LOG, "log" },
    { Type::TPR, "tpr" },
    { Type::MDP, "mdp" },
    { Type::TOP, "top" },
  };

  return map[type];
}

QString FileObject::toString(Category category)
{
  const static QMap<Category, QString> map = {
    { Category::Unknown, "Unknown" },
    { Category::Coordinates, "Coordinates" },
    { Category::Trajectory, "Trajectory" },
    { Category::Energy, "Energy" },
    { Category::Velocities, "Velocities" },
    { Category::Forces, "Forces" },
    { Category::Topology, "Topology" },
    { Category::Text, "Text" },
  };

  return map[category];
}

QJsonObject &operator<<(QJsonObject &out, const FileObject::Pointer obj)
{
  out["fileName"] = obj->getFileName();

  return out;
}

QJsonObject &operator>>(QJsonObject& in, FileObject::Pointer)
{
  //QString fileName;
  //if (in.contains("fileName") && in["fileName"].isString())
  //{
  //  fileName = in["fileName"].toString();
  //}
  //
  //obj->setFileName(fileName);

  return in;
}

}


