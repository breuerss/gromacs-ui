#include "pdbcode.h"
#include "ui_pdbcode.h"
#include <memory>
#include "../src/form/connectionhelper.h"

PdbCode::PdbCode(std::shared_ptr<Config::Pdb> config, QWidget *parent)
  : QWidget(parent)
  , config(config)
  , ui(new Ui::PdbCode)
{
    ui->setupUi(this);
    connectToLineEdit(ui->pdbCode, config, "pdbCode");
}

PdbCode::~PdbCode()
{
    delete ui;
}
