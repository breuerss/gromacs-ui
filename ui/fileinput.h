#ifndef FILEINPUT_H
#define FILEINPUT_H

#include "../src/pipeline/localstructure/configuration.h"
#include <QWidget>

namespace Ui {
class FileInput;
}

class FileInput : public QWidget
{
    Q_OBJECT

public:
    explicit FileInput(Pipeline::LocalStructure::Configuration* config);
    ~FileInput();

private:
    Ui::FileInput *ui;
    Pipeline::LocalStructure::Configuration* config;
};

#endif // FILEINPUT_H
