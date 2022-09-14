#ifndef UIUPDATER_H
#define UIUPDATER_H

#include <QObject>

class UiUpdater : public QObject {
  Q_OBJECT
public:
  static UiUpdater* getInstance();
  
signals:
  void showConfigUI(QWidget*);
  void showStatusUI(QWidget*);
  void showTrajectory(const QString& coords, const QString& trajectory);
  void showCoordinates(const QString& coords);
  void showTextFile(const QString& textFile);

private:
  UiUpdater() = default;

};

#endif
