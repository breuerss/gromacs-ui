#ifndef PDBCODE_H
#define PDBCODE_H

#include <QWidget>
#include <memory>
#include "../src/pipeline/pdbdownload/configuration.h"

namespace Ui {
class PdbCode;
}

class PdbCode : public QWidget
{
    Q_OBJECT

public:
    explicit PdbCode(std::shared_ptr<Pipeline::PdbDownload::Configuration> config);
    ~PdbCode();

private:
    Ui::PdbCode *ui;
    std::shared_ptr<Pipeline::PdbDownload::Configuration> config;
};

#endif // PDBCODE_H
