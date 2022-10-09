#include "fileinput.h"
#include "ui_fileinput.h"
#include <QFileDialog>
#include <QStandardPaths>

FileInput::FileInput(Pipeline::LocalStructure::Configuration* newConfig)
  : QWidget(nullptr)
  , ui(new Ui::FileInput)
  , config(newConfig)
{
  ui->setupUi(this);
  connect(ui->openButton, &QPushButton::clicked, [this] () {
    auto fileName = QFileDialog::getOpenFileName(
      this,
      tr("Use Structure"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      "*.pdb"
      );

    if (!fileName.isNull())
    {
      ui->fileName->setText(fileName);
      config->setProperty("fileName", fileName);
    }
  });
}

FileInput::~FileInput()
{
  delete ui;
}
