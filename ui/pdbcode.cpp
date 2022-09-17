#include "pdbcode.h"
#include "ui_pdbcode.h"
#include <memory>
#include "connectionhelper.h"

PdbCode::PdbCode(Pipeline::PdbDownload::Configuration* config)
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
