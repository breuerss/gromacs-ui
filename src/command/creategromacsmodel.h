#ifndef CREATEGROMACSMODEL_H
#define CREATEGROMACSMODEL_H

#include "executor.h"
#include <memory>

namespace Model {
  class SystemSetup;
}

namespace Command {

class CreateGromacsModel : public Executor
{
public:
    explicit CreateGromacsModel(
        std::shared_ptr<Model::SystemSetup> systemSetup,
        QObject *parent = nullptr);
    void exec() override;

private:
    const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif // CREATEGROMACSMODEL_H
