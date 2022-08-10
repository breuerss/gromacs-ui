#include "appprovider.h"
#include "settings.h"
#include <QStandardPaths>

QString AppProvider::get(const QString& binary)
{
  const QMap<QString, QString> configMap = {
    { "gmx", Settings::GMX_PATH },
    { "pdb_wc", Settings::PDB_WC },
    { "pdb_delhetam", Settings::PDB_DELHETATM },
    { "pdb_selchain", Settings::PDB_SELCHAIN },
  };

  QString appPath;
  auto configKey = configMap[binary];
  if (configKey.isEmpty())
  {
    Settings settings;
    appPath = settings.value(configKey).toString();
  }

  if (appPath.isEmpty())
  {
    appPath = QStandardPaths::findExecutable(binary);
  }

  return appPath;
}

