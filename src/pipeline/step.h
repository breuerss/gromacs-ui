#ifndef PIPELINE_STEP_H
#define PIPELINE_STEP_H

#include "../command/fileobjectconsumer.h"
#include "../command/fileobjectprovider.h"
#include "../config/supportedconfigs.h"
#include <QString>

namespace Pipeline {

class Step {
public:
  Step(
    const QMap<Command::FileObject::Category, QList<Command::FileObject::Type>>& requiresMap,
    const QList<Command::FileObject::Type> providesList,
    Config::Type configuration
    );
  virtual QString getName() const = 0;

  const Command::FileObjectConsumer& getFileObjectConsumer() const { return fileObjectConsumer; }
  Command::FileObjectConsumer& getFileObjectConsumer() { return fileObjectConsumer; }
  const Command::FileObjectProvider& getFileObjectProvider() { return fileObjectProvider; }
  const Config::Type& getConfiguration() { return configuration; }

protected:
  Command::FileObjectConsumer fileObjectConsumer;
  Command::FileObjectProvider fileObjectProvider;
  Config::Type configuration;
};

}

#endif
