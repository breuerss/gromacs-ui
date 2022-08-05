#ifndef SIMULATIONTYPE_H
#define SIMULATIONTYPE_H

#include <QObject>

namespace SimulationType {

enum Type {
    None = 0,
    Minimisation,
    NVE,
    NVT,
    NPT
};

}

Q_DECLARE_METATYPE(SimulationType::Type)

#endif // SIMULATIONTYPE_H
