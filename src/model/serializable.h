#ifndef MODEL_SERIALIZABLE_H
#define MODEL_SERIALIZABLE_H

#include <QObject>
#include <memory>

namespace Model {

class Serializable : public QObject{
  Q_OBJECT
};

QJsonObject &operator<<(QJsonObject &out, const std::shared_ptr<Serializable> model);
QJsonObject &operator>>(QJsonObject &in, std::shared_ptr<Serializable> model);

}

#endif
