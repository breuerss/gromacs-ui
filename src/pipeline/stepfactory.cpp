#include "stepfactory.h"

#include "simulation/step.h"
#include "pdbdownload/step.h"
#include "pdbfixer/step.h"
#include "createbox/step.h"
#include "creategromacsmodel/step.h"
#include "neutralise/step.h"
#include "solvate/step.h"
#include "src/model/project.h"
#include <memory>

namespace Pipeline {

StepFactory* StepFactory::getInstance()
{
  static StepFactory instance;

  return &instance;
}

StepFactory::StepFactory()
{
  auto simulationPrototype = std::make_unique<Simulation::Step>();
  factoryMap[simulationPrototype->getType()] = std::move(simulationPrototype);

  auto pdbDownloadPrototype = std::make_unique<PdbDownload::Step>();
  factoryMap[pdbDownloadPrototype->getType()] = std::move(pdbDownloadPrototype);

  auto pdbFixerPrototype = std::make_unique<PdbFixer::Step>();
  factoryMap[pdbFixerPrototype->getType()] = std::move(pdbFixerPrototype);

  auto createboxPrototype = std::make_unique<CreateBox::Step>();
  factoryMap[createboxPrototype->getType()] = std::move(createboxPrototype);

  auto createGromacsModelPrototype = std::make_unique<CreateGromacsModel::Step>();
  factoryMap[createGromacsModelPrototype->getType()] = std::move(createGromacsModelPrototype);

  auto solvatePrototype = std::make_unique<Solvate::Step>();
  factoryMap[solvatePrototype->getType()] = std::move(solvatePrototype);

  auto neutralisePrototype = std::make_unique<Neutralise::Step>();
  factoryMap[neutralisePrototype->getType()] = std::move(neutralisePrototype);
}

std::shared_ptr<Step> StepFactory::createFromString(
  const QString& type,
  std::shared_ptr<Model::Project> project
  )
{
  return factoryMap[type]->create(project);
}

}
