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

private:
  UiUpdater() = default;

};

#endif
