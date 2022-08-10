#ifndef FILECONTENTVIEWER_H
#define FILECONTENTVIEWER_H

#include <QDialog>
#include <QString>

class FileContentViewer : public QDialog
{
public:
    FileContentViewer(const QString& fileName);

private:
    const QString fileName;
};

#endif // FILECONTENTVIEWER_H
