// Connect web interface to run a script on the system.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SCRIPTRUNNER_H
#define SCRIPTRUNNER_H

#include <QObject>
#include <QProcess>

/**
 * @todo write docs
 */
class ScriptRunner : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString status MEMBER currentStatus);

public:
    /**
     * Default constructor
     */
    ScriptRunner(QObject *parent = 0);

    /**
     * Destructor
     */
    ~ScriptRunner();

    void SetScriptPath(const QString& executablePath);

public slots:
    void OnRun(const QString& runParameter);

private slots:
    void on_script_completed(int exitCode, QProcess::ExitStatus exitStatus);

protected:

    QString currentStatus;
    QString scriptPath;

    QProcess *scriptProcess;
};

#endif // SCRIPTRUNNER_H
