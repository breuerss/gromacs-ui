#ifndef CONFIG_PDB_H
#define CONFIG_PDB_H

#include "../model/serializable.h"
#include <QString>

namespace Config {

class Pdb : public Model::Serializable
{
  Q_OBJECT
public:
  Pdb() = default;

  Q_PROPERTY(QString pdbCode MEMBER pdbCode NOTIFY pdbCodeChanged);

signals:
  void pdbCodeChanged();

private:
  QString pdbCode;
};

}

#endif
