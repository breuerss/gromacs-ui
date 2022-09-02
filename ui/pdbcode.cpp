#include "pdbcode.h"
#include "ui_pdbcode.h"
#include <memory>
#include "../src/form/connectionhelper.h"

PdbCode::PdbCode(std::shared_ptr<Pipeline::PdbDownload::Configuration> config)
  : QWidget(nullptr)
  , ui(new Ui::PdbCode)
  , config(config)
{
    ui->setupUi(this);
    connectToLineEdit(ui->pdbCode, config, "pdbCode");
}

PdbCode::~PdbCode()
{
    delete ui;
}
