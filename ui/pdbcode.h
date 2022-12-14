#ifndef PDBCODE_H
#define PDBCODE_H

#include <QWidget>
#include "../src/pipeline/pdbdownload/configuration.h"

namespace Ui {
class PdbCode;
}

class PdbCode : public QWidget
{
    Q_OBJECT

public:
    explicit PdbCode(Pipeline::PdbDownload::Configuration* config);
    ~PdbCode();

private:
    Ui::PdbCode *ui;
    Pipeline::PdbDownload::Configuration* config;
};

#endif // PDBCODE_H
