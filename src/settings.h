#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
public:
  static const QString PROJECTS_DIRECTORY;
  static const QString GMX_PATH;
  static const QString APP_GEOMETRY;
  static const QString APP_STATE;
  static const QString PDB_WC;
  static const QString PDB_SELCHAIN;
  static const QString PDB_DELHETATM;

  Settings() = default;
};

#endif // SETTINGS_H
