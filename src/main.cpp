#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QProcess>
#include "projectmanager.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("Gromacs");
  QCoreApplication::setApplicationName("Gromacs-UI");

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
    const QString baseName = "Gromacs-UI_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }

  MainWindow w;
  w.showMaximized();

  if (argc == 2)
  {
    QString fileName(argv[1]);
    ProjectManager::getInstance()->open(fileName);
  }

  return a.exec();
}
