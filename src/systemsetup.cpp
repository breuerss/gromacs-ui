#include "systemsetup.h"
#include "project.h"
#include "pdbconverter.h"

#include <QDebug>

SystemSetup::SystemSetup(Project* parent)
    : project(parent)
{

}

Project* SystemSetup::getProject()
{
    return project;
}

void SystemSetup::setSourceStructureFile(const QString &newSourceStructureFile)
{
    const bool changed = sourceStructureFile != newSourceStructureFile;
    sourceStructureFile = newSourceStructureFile;
    if (changed)
    {
        emit sourceStructureFileChanged(sourceStructureFile);
    }
}

void SystemSetup::useChain(const QString& chain, bool use)
{
    qDebug() << chain << use;
    if (use)
    {
        if (!chains.contains(chain))
        {
            chains.append(chain);
        }
    }
    else
    {
        chains.removeAll(chain);
    }

    PdbConverter converter;
    processedStructureFile = converter.convert(sourceStructureFile, chains);
    emit processedStructureFileChanged(processedStructureFile);
    qDebug() << "chains to use" << chains;
}
