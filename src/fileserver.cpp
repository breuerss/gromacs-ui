#include "fileserver.h"
#include "settings.h"

#include <QProcess>
#include <QSettings>
#include <QDebug>
#include <memory>

FileServer* FileServer::getInstance()
{
    static std::unique_ptr<FileServer> instance;
    if (!instance)
    {
        instance.reset(new FileServer());
    }

    return instance.get();
}

FileServer::FileServer()
    : port(8880)
{
    watcher.addPath(settings.fileName());
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, &FileServer::start);
    connect(&servingProcess, &QProcess::errorOccurred, [] (QProcess::ProcessError error)
    {
       qDebug() << error;
    });
    connect(&servingProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [] (int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << exitCode << exitStatus;
    });
    start();
}

FileServer::~FileServer()
{
    servingProcess.kill();
    servingProcess.waitForFinished();
}

void FileServer::start()
{
    if (servingProcess.state() == QProcess::Running)
    {
        servingProcess.terminate();
    }
    const QString projectsDir = settings.value(Settings::PROJECTS_DIRECTORY).toString();
    // TODO settings python 3
    QString command = "/home/breuerss/miniconda3/bin/python3 -m http.server";
    command += " --directory " + projectsDir;
    command += " " + QString::number(port);
    qDebug() << "starting command" << command;
    servingProcess.start(command);
}

QString FileServer::getServerUrl()
{
    return "http://127.0.0.1:" + QString::number(port);
}

QString FileServer::getUrlForFile(const QString& absFilePath)
{
    const QString projectsDir = settings.value(Settings::PROJECTS_DIRECTORY).toString();
    QString url;
    if (absFilePath.startsWith(projectsDir))
    {
        qDebug() << absFilePath << "is in served directory.";

        QString filePathInSharedDir = absFilePath;
        filePathInSharedDir.remove(0, projectsDir.length());

        url = getServerUrl() + "/" + filePathInSharedDir;
    }

    return url;
}
