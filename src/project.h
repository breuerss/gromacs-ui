#ifndef PROJECT_H
#define PROJECT_H

#include "step.h"

#include <vector>

class Project
{
public:
    Project();

    void addStep();
    const std::vector<Step>& getSteps() const;

private:
    std::vector<Step> steps;
};

#endif // PROJECT_H
