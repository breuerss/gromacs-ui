#ifndef PDBCODE_H
#define PDBCODE_H

#include <QWidget>
#include <memory>
#include "../src/config/pdb.h"

namespace Ui {
class PdbCode;
}

class PdbCode : public QWidget
{
    Q_OBJECT

public:
    explicit PdbCode(std::shared_ptr<Config::Pdb> config, QWidget *parent = nullptr);
    ~PdbCode();

private:
    Ui::PdbCode *ui;
    std::shared_ptr<Config::Pdb> config;
};

#endif // PDBCODE_H
