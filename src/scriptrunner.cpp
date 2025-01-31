// Connect web interface to run a script on the system.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "scriptrunner.h"

#include <QDebug>

ScriptRunner::ScriptRunner(QObject *parent) : QObject(parent)
{
    scriptPath.clear();
    scriptProcess = NULL;
}

ScriptRunner::~ScriptRunner()
{
    if (scriptProcess)
    {
        delete scriptProcess;
    }
}

void ScriptRunner::SetScriptPath(const QString &executablePath)
{
    scriptPath = executablePath;
}

void ScriptRunner::OnRun(const QString &runParameter)
{
    QStringList arguments;
    if (scriptPath.size() > 0)
    {
        qDebug() << "Running script " << scriptPath << " with parameter " << runParameter;

        arguments << runParameter;

        scriptProcess = new QProcess(parent());
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        scriptProcess->setProcessEnvironment(env);

        connect(scriptProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
                SLOT(on_script_completed(int, QProcess::ExitStatus)));
        scriptProcess->start(scriptPath, arguments);
        // scriptProcess->start("/usr/bin/date", arguments);

        if (!scriptProcess->waitForStarted(1000))
        {
            qDebug() << "Process not started";
        }
    }
}

void ScriptRunner::on_script_completed(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit)
    {
        qDebug() << "script completed with exit code " << exitCode;
    }
    else
    {
        qDebug() << "Script crashed!";
    }
    delete scriptProcess;
    scriptProcess = NULL;
}
