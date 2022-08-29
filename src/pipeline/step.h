#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"
#include "../config/supportedconfigs.h"
#include <QString>

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace Pipeline {

class Step : public std::enable_shared_from_this<Step> {
public:
  enum class Category {
    Unknown,
    DataProvider,
    Preprocess,
    Simulation,
    Viewer,
  };
  Step(
    const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap,
    const QList<Command::FileObject::Type> providesList,
    Config::Type configuration,
    Category category
    );
  virtual QString getName() const = 0;

  const Command::FileObjectConsumer& getFileObjectConsumer() const { return fileObjectConsumer; }
  Command::FileObjectConsumer& getFileObjectConsumer() { return fileObjectConsumer; }
  const Command::FileObjectProvider& getFileObjectProvider() { return fileObjectProvider; }
  const Config::Type& getConfiguration() { return configuration; }

  void showConfigUI(bool show = true);
  void showStatusUI(bool show = true);

  const Category category;

protected:
  Command::FileObjectConsumer fileObjectConsumer;
  Command::FileObjectProvider fileObjectProvider;
  Config::Type configuration;
};

}

#endif
