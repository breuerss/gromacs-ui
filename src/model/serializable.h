#ifndef MODEL_SERIALIZABLE_H
#define MODEL_SERIALIZABLE_H

#include <QObject>
#include <memory>

namespace Model {

class Serializable : public QObject
{
  Q_OBJECT
public:
  bool setProperty(const char* name, const QVariant& value, bool createUndoRedo = true);
  QString getSignalStringForProperty(const QString& name);
  QMetaMethod getSignalForProperty(const QString& name);

protected:
  void connectAllSignals();
signals:
  void anyChanged();
};

QJsonObject &operator<<(QJsonObject &out, const Serializable* model);
QJsonObject &operator>>(QJsonObject &in, Serializable* model);
QJsonObject &operator<<(QJsonObject &out, const std::shared_ptr<Serializable> model);
QJsonObject &operator>>(QJsonObject &in, std::shared_ptr<Serializable> model);

}

#endif
