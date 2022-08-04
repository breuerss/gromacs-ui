#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
public:
    static const QString PROJECTS_DIRECTORY;
    static const QString GMX_PATH;
    static const QString APP_GEOMETRY;
    static const QString PDB_WC;
    static const QString PDB_CHAIN;

    Settings() = default;
};

#endif // SETTINGS_H
