#ifndef CONFIG_PDB_H
#define CONFIG_PDB_H

#include "../model/serializable.h"
#include "configuration.h"
#include <QString>
#include <memory>

namespace Config {

class Pdb : public Model::Serializable, public Configuration, public std::enable_shared_from_this<Pdb>
{
  Q_OBJECT
public:
  Pdb() = default;

  Q_PROPERTY(QString pdbCode MEMBER pdbCode NOTIFY pdbCodeChanged);
  QWidget* getUI() override;

signals:
  void pdbCodeChanged();

private:
  QString pdbCode;
};

}

#endif
