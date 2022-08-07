#ifndef STEP_H
#define STEP_H

#include <QString>
#include <QObject>

#include "simulationtype.h"

class Step : public QObject
{
    Q_OBJECT
public:
    Step();
    QString getName() const;
    QString getDirectory() const;

    Q_PROPERTY(SimulationType simulationType MEMBER simulationType NOTIFY simulationTypeChanged);
    Q_PROPERTY(QString algorithm MEMBER algorithm NOTIFY algorithmChanged);

    Q_PROPERTY(int numberOfSteps MEMBER numberOfSteps NOTIFY numberOfStepsChanged);
    Q_PROPERTY(double minimisationMaximumForce MEMBER minimisationMaximumForce NOTIFY minimisationMaximumForceChanged);
    Q_PROPERTY(double minimisationStepSize MEMBER minimisationStepSize NOTIFY minimisationStepSizeChanged);
    Q_PROPERTY(int energyOutputFrequency MEMBER energyOutputFrequency NOTIFY energyOutputFrequencyChanged);
    Q_PROPERTY(int positionOutputFrequency MEMBER positionOutputFrequency NOTIFY positionOutputFrequencyChanged);
    Q_PROPERTY(int compressedPositionOutputFrequency MEMBER compressedPositionOutputFrequency NOTIFY compressedPositionOutputFrequencyChanged);
    Q_PROPERTY(int velocityOutputFrequency MEMBER velocityOutputFrequency NOTIFY velocityOutputFrequencyChanged);
    Q_PROPERTY(int forceOutputFrequency MEMBER forceOutputFrequency NOTIFY forceOutputFrequencyChanged);
    Q_PROPERTY(int logOutputFrequency MEMBER logOutputFrequency NOTIFY logOutputFrequencyChanged);
    Q_PROPERTY(QString electrostaticAlgorithm MEMBER electrostaticAlgorithm NOTIFY electrostaticAlgorithmChanged);
    Q_PROPERTY(double electrostaticCutoffRadius MEMBER electrostaticCutoffRadius NOTIFY electrostaticCutoffRadiusChanged);
    Q_PROPERTY(double fourierSpacing MEMBER fourierSpacing NOTIFY fourierSpacingChanged);
    Q_PROPERTY(double vdwCutoffRadius MEMBER vdwCutoffRadius NOTIFY vdwCutoffRadiusChanged);

signals:
    void nameChanged();
    void directoryChanged();

    void simulationTypeChanged(SimulationType);
    void algorithmChanged(QString);

    void numberOfStepsChanged(int);
    void minimisationMaximumForceChanged(double);
    void minimisationStepSizeChanged(double);

    void energyOutputFrequencyChanged(int);
    void positionOutputFrequencyChanged(int);
    void compressedPositionOutputFrequencyChanged(int);
    void logOutputFrequencyChanged(int);
    void velocityOutputFrequencyChanged(int);
    void forceOutputFrequencyChanged(int);

    void electrostaticAlgorithmChanged(QString);
    void electrostaticCutoffRadiusChanged(double);
    void fourierSpacingChanged(double);
    void vdwCutoffRadiusChanged(double);

private:
    SimulationType simulationType = SimulationType::None;
    QString algorithm = "";

    int numberOfSteps = -1;
    double minimisationMaximumForce = 1000.0;
    double minimisationStepSize = 0.01;

    int energyOutputFrequency = 500;
    int positionOutputFrequency = 0;
    int compressedPositionOutputFrequency = 1000;
    int logOutputFrequency = 1000;
    int velocityOutputFrequency = 0;
    int forceOutputFrequency = 0;

    QString electrostaticAlgorithm = "PME";
    double electrostaticCutoffRadius = 1.0;
    double fourierSpacing = 0.125;
    double vdwCutoffRadius = 1.0;

};

QDataStream &operator<<(QDataStream &out, const Step& step);
QDataStream &operator>>(QDataStream &in, Step& step);

#endif // STEP_H
