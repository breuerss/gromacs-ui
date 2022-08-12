#ifndef CREATEGROMACSMODEL_H
#define CREATEGROMACSMODEL_H

#include "executor.h"
#include "inputoutputfilelink.h"
#include <memory>

namespace Model {
  class SystemSetup;
}

namespace Command {

class CreateGromacsModel : public Executor, public InputOutputFileLink
{
public:
    explicit CreateGromacsModel(
        std::shared_ptr<Model::SystemSetup> systemSetup,
        QObject *parent = nullptr);
    void doExecute() override;
    QString getOutputFilename() const override;

private:
    const std::shared_ptr<Model::SystemSetup> systemSetup;
};

}

#endif // CREATEGROMACSMODEL_H
