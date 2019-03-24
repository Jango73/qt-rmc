
#pragma once

#include <QCoreApplication>

#include "RemoteControl/CRemoteControl.h"

class CApplication : public QCoreApplication
{
    Q_OBJECT

public:

    //! Constructeur
    CApplication(int &argc, char** argv);

    //! Destructeur
    ~CApplication();

    bool init(int &argc, char** argv);
    void echo(const QString& sText);
    void printUsage();

private:

    // Command line switch constants

    static const char* s_sSwitch_serve;
    static const char* s_sSwitch_ping;
    static const char* s_sSwitch_command;
    static const char* s_sSwitch_get;
    static const char* s_sSwitch_put;
    static const char* s_sSwitch_merge;
    static const char* s_sSwitch_getfilecrc;
    static const char* s_sSwitch_shutdown;
    static const char* s_sSwitch_getprivs;

    static const char* s_sSwitch_ip;
    static const char* s_sSwitch_port;
    static const char* s_sSwitch_login;
    static const char* s_sSwitch_password;
    static const char* s_sSwitch_detached;
    static const char* s_sSwitch_source;
    static const char* s_sSwitch_target;
    static const char* s_sSwitch_newest;
    static const char* s_sSwitch_compcrc;
    static const char* s_sSwitch_timeout;
    static const char* s_sSwitch_shell;
    static const char* s_sDefaultIP;
    static int s_iDefaultPort;
    static int s_iDefaultTimeout;
    static int s_iDefaultMaxWaitingTime;

    CRemoteControl* m_pServer;

signals:

    void quitApplication();

private slots:

    void onQuitSignal(int);
    void onShutdownSignal();
};
