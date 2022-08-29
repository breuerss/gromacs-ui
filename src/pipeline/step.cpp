#include "step.h"
#include "../uiupdater.h"
#include <memory>
#include <variant>
#include <QDebug>
#include "../form/simulationsetupform.h"
#include "../../ui/simulationstatus.h"
#include "../../ui/pdbcode.h"
#include "src/command/runsimulation.h"
#include "../config/supportedconfigs.h"

namespace Pipeline {

Step::Step(
  const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap,
  const QList<Command::FileObject::Type> providesList,
  Config::Type configuration,
  Category newCategory
  )
  : category(newCategory)
  , fileObjectConsumer(requiresMap)
  , fileObjectProvider(providesList)
  , configuration(configuration)
{
}

void Step::showConfigUI(bool show)
{
  QWidget* widget = nullptr;
  if (show)
  {
    widget = std::visit(overloaded {
      [](auto) { return nullptr; },
      [](std::shared_ptr<Config::Simulation> config) -> QWidget* { return new SimulationSetupForm(config); },
      [](std::shared_ptr<Config::Pdb> config) -> QWidget* { return new PdbCode(config); },
    }, configuration);
  }
  UiUpdater::getInstance()->showConfigUI(widget);
}

void Step::showStatusUI(bool show)
{
  QWidget* widget = nullptr;
  if (show)
  {
    widget = std::visit(overloaded {
      [](auto) -> QWidget* { return nullptr; },
      [this] (
        std::shared_ptr<Config::Simulation>
        ) -> QWidget* {
        using RunSimulation = Command::RunSimulation;
        auto step = std::dynamic_pointer_cast<RunSimulation>(shared_from_this());
        qDebug() << step.get() << std::holds_alternative<std::shared_ptr<Config::Simulation>>(step->configuration);
        return new SimulationStatus(step);
      },
    }, configuration);
  }
  UiUpdater::getInstance()->showStatusUI(widget);
}

}

