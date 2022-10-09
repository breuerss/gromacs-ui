#ifndef COMMAND_FILEOBJECT_H
#define COMMAND_FILEOBJECT_H

#include "inputoutput.h"

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
    XVG,
  };

  FileObject(Type newType);
  bool exists() const;
  void setFileName(const QString& fileName);
  const QString& getFileName() const;

  const Type type;
  static InputOutput::Category getCategoryFor(Type);
  static QString toString(Type);
  static QString toString(InputOutput::Category);

signals:
  void fileNameChanged(const QString& newFileName);

private:
  QString fileName;
};

QJsonObject &operator<<(QJsonObject &out, const FileObject::Pointer);
QJsonObject &operator>>(QJsonObject &in, FileObject::Pointer step);

}

#endif
