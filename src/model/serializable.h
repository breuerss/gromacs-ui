#ifndef MODEL_SERIALIZABLE_H
#define MODEL_SERIALIZABLE_H

#include <QObject>
#include <QDataStream>

namespace Model {

class Serializable : public QObject{
  Q_OBJECT
};

QDataStream &operator<<(QDataStream &out, const Serializable &model);
QDataStream &operator>>(QDataStream &in, Serializable& model);

}

#endif
