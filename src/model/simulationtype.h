#ifndef SIMULATIONTYPE_H
#define SIMULATIONTYPE_H

#include <QObject>
#include <QDataStream>

//namespace SimulationType
//{
//    enum Type : int {
//        None = 0,
//        Minimisation,
//        NVE,
//        NVT,
//        NPT
//    };
//};
//
//Q_DECLARE_METATYPE(SimulationType)

//    enum SimulationTypeType : int {
//        SimulationTypeNone = 0,
//        SimulationTypeMinimisation,
//        SimulationTypeNVE,
//        SimulationTypeNVT,
//        SimulationTypeNPT
//    };
//
//Q_DECLARE_METATYPE(SimulationTypeType)

enum class SimulationType : int {
    None = 0,
    Minimisation,
    NVE,
    NVT,
    NPT
};

Q_DECLARE_METATYPE(SimulationType)

#endif // SIMULATIONTYPE_H
