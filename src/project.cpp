#include "project.h"

Project::Project()
{

}

void Project::addStep()
{
    Step step;
    steps.push_back(step);
}

const std::vector<Step>& Project::getSteps() const
{
    return steps;
}
