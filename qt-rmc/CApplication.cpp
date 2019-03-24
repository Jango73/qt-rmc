
// Qt
#include <QDebug>

// Application
#include "CApplication.h"

#define LOG_DEBUG(a)
// #define LOG_DEBUG(a) qDebug() << QString("DEBUG : ") << QString(a) << QString("\n");

#define LOG_INFO(a) qDebug() << QString("INFO : ") << QString(a);

const char* CApplication::s_sSwitch_serve		= "--serve";
const char* CApplication::s_sSwitch_ping		= "--ping";
const char* CApplication::s_sSwitch_command		= "--cmd";
const char* CApplication::s_sSwitch_get			= "--get";
const char* CApplication::s_sSwitch_put			= "--put";
const char* CApplication::s_sSwitch_merge		= "--merge";
const char* CApplication::s_sSwitch_getfilecrc	= "--getfilecrc";
const char* CApplication::s_sSwitch_shutdown	= "--shutdown";
const char* CApplication::s_sSwitch_getprivs	= "--getprivs";

const char* CApplication::s_sSwitch_ip			= "--ip";
const char* CApplication::s_sSwitch_port		= "--port";
const char* CApplication::s_sSwitch_login		= "--login";
const char* CApplication::s_sSwitch_password	= "--password";
const char* CApplication::s_sSwitch_detached	= "--detached";
const char* CApplication::s_sSwitch_source		= "--source";
const char* CApplication::s_sSwitch_target		= "--target";
const char* CApplication::s_sSwitch_newest		= "--newest";
const char* CApplication::s_sSwitch_compcrc		= "--compcrc";
const char* CApplication::s_sSwitch_timeout		= "--timeout";
const char* CApplication::s_sSwitch_shell		= "--shell";
const char* CApplication::s_sDefaultIP			= "127.0.0.1";

int CApplication::s_iDefaultPort				= 8419;
int CApplication::s_iDefaultTimeout				= 3000;
int CApplication::s_iDefaultMaxWaitingTime		= 5000;

CApplication::CApplication(int &argc, char** argv)
    : QCoreApplication(argc, argv)
    , m_pServer(nullptr)
{
}

bool CApplication::init(int &argc, char** argv)
{
    QStringList lArgList;
    QString sLogin = "";
    QString sPassword = "";
    QString sIP = s_sDefaultIP;
    int iPort = s_iDefaultPort;
    int iTimeoutMS = s_iDefaultTimeout;
    int iMaxWaitingTimeMS = s_iDefaultMaxWaitingTime;
    bool bKeepNewest = false;
    bool bCompCRC = false;
    bool bDetached = false;
    bool bDoShell = false;

    if (argc < 2)
    {
        printUsage();
        return false;
    }
    else
    {
        for (int Index = 0; Index < argc; Index++) lArgList.append(QString(argv[Index]));

        // Check for IP address in arguments

        if (lArgList.contains(s_sSwitch_ip))
        {
            sIP = lArgList[lArgList.indexOf(s_sSwitch_ip) + 1];

            LOG_DEBUG(QString("IP = %1").arg(sIP));
        }

        // Check for port in arguments

        if (lArgList.contains(s_sSwitch_port))
        {
            iPort = lArgList[lArgList.indexOf(s_sSwitch_port) + 1].toInt();

            LOG_DEBUG(QString("Port = %1").arg(iPort));
        }

        // Check for login in arguments

        if (lArgList.contains(s_sSwitch_login))
        {
            sLogin = lArgList[lArgList.indexOf(s_sSwitch_login) + 1];

            LOG_DEBUG(QString("Login = %1").arg(iPort));
        }

        // Check for password in arguments

        if (lArgList.contains(s_sSwitch_password))
        {
            sPassword = lArgList[lArgList.indexOf(s_sSwitch_password) + 1];

            LOG_DEBUG(QString("Password = ****"));
        }

        // Check for shell in arguments

        if (lArgList.contains(s_sSwitch_shell))
        {
            bDoShell = true;
        }

        // Check for timeout in arguments

        if (lArgList.contains(s_sSwitch_timeout))
        {
            iTimeoutMS = lArgList[lArgList.indexOf(s_sSwitch_timeout) + 1].toInt();

            LOG_DEBUG(QString("Timeout = %1").arg(iTimeoutMS));
        }

        // Check for keep newest in arguments

        if (lArgList.contains(s_sSwitch_newest))
        {
            bKeepNewest = true;
        }

        // Check for compare crc in arguments

        if (lArgList.contains(s_sSwitch_compcrc))
        {
            bCompCRC = true;
        }

        // Check for detached in arguments

        if (lArgList.contains(s_sSwitch_detached))
        {
            bDetached = true;
        }

        if (lArgList.contains(s_sSwitch_serve))
        {
            // Server mode

            LOG_INFO("Server mode, Starting server");

            m_pServer = new CRemoteControl(quint16(iPort));
            connect(m_pServer, SIGNAL(shutdown()), this, SLOT(onShutdownSignal()));
        }
        else if (lArgList.contains(s_sSwitch_ping))
        {
            // Client mode

            LOG_INFO(QString("Ping %1").arg(sIP));

            m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, false);

            return false;
        }
        else if (lArgList.contains(s_sSwitch_command))
        {
            // Client mode

            int iCommandIndex = lArgList.indexOf(s_sSwitch_command) + 1;

            QString sCommand = "";

            for (int Index = iCommandIndex; Index < lArgList.count(); Index++) sCommand += " " + lArgList[Index];

            sCommand = sCommand.trimmed();

            LOG_DEBUG(QString("Client mode, executing %1").arg(sCommand));

            m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, bDoShell);
            connect(m_pServer, SIGNAL(transactionTerminated(int)), this, SLOT(onQuitSignal(int)));

            if (m_pServer->getClientConnected() == false) return false;
            m_pServer->setLoginPassword(sLogin, sPassword);

            m_pServer->sendCommand(sCommand, bDetached);
        }
        else if (bDoShell)
        {
            LOG_DEBUG(QString("Client mode, entering shell..."));

            m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, bDoShell);
            connect(m_pServer, SIGNAL(transactionTerminated(int)), this, SLOT(onQuitSignal(int)));

            if (m_pServer->getClientConnected() == false) return false;
            m_pServer->setLoginPassword(sLogin, sPassword);
        }
        else if (lArgList.contains(s_sSwitch_shutdown))
        {
            // Client mode

            m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, bDoShell);
            connect(m_pServer, SIGNAL(transactionTerminated(int)), this, SLOT(onQuitSignal(int)));

            if (m_pServer->getClientConnected() == false) return false;
            m_pServer->setLoginPassword(sLogin, sPassword);

            if (m_pServer->sendShutdown() == false)
            {
                return false;
            }
        }
        else if (lArgList.contains(s_sSwitch_getfilecrc))
        {
            // Client mode

            if (lArgList.contains(s_sSwitch_target))
            {
                QString sTarget = lArgList[lArgList.indexOf(s_sSwitch_target) + 1];

                sTarget.replace("\\", "/");

                LOG_DEBUG(QString("Client mode, getting file crc for %1").arg(sTarget));

                m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, bDoShell);
                connect(m_pServer, SIGNAL(transactionTerminated(int)), this, SLOT(onQuitSignal(int)));

                if (m_pServer->getClientConnected() == false) return false;
                m_pServer->setLoginPassword(sLogin, sPassword);

                if (m_pServer->getRemoteFileCRC(sTarget) == false)
                {
                    return false;
                }
            }
            else
            {
                echo("Target missing\n");

                return false;
            }
        }
        else if (lArgList.contains(s_sSwitch_getprivs))
        {
            // Client mode

            LOG_DEBUG(QString("Client mode, getting user rights"));

            m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, bDoShell);
            connect(m_pServer, SIGNAL(transactionTerminated(int)), this, SLOT(onQuitSignal(int)));

            if (m_pServer->getClientConnected() == false) return false;

            LOG_DEBUG(QString("Sending login to server"));
            m_pServer->setLoginPassword(sLogin, sPassword);

            LOG_DEBUG(QString("Sending get privileges request to server"));
            if (m_pServer->getRights() == false)
            {
                return false;
            }
        }
        else if (lArgList.contains(s_sSwitch_get))
        {
            // Client mode

            if (lArgList.contains(s_sSwitch_source) && lArgList.contains(s_sSwitch_target))
            {
                QString sSource = lArgList[lArgList.indexOf(s_sSwitch_source) + 1];
                QString sTarget = lArgList[lArgList.indexOf(s_sSwitch_target) + 1];

                sSource.replace("\\", "/");
                sTarget.replace("\\", "/");

                LOG_DEBUG(QString("Client mode, getting file %1 to %2").arg(sSource).arg(sTarget));

                m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, bDoShell);
                connect(m_pServer, SIGNAL(transactionTerminated(int)), this, SLOT(onQuitSignal(int)));

                if (m_pServer->getClientConnected() == false) return false;
                m_pServer->setLoginPassword(sLogin, sPassword);

                if (m_pServer->getFile(sSource, sTarget, bKeepNewest, bCompCRC) == false)
                {
                    return false;
                }
            }
            else
            {
                echo("Source or target missing\n");

                return false;
            }
        }
        else if (lArgList.contains(s_sSwitch_put) || lArgList.contains(s_sSwitch_merge))
        {
            // Client mode

            if (lArgList.contains(s_sSwitch_source) && lArgList.contains(s_sSwitch_target))
            {
                QString sSource = lArgList[lArgList.indexOf(s_sSwitch_source) + 1];
                QString sTarget = lArgList[lArgList.indexOf(s_sSwitch_target) + 1];

                sSource.replace("\\", "/");
                sTarget.replace("\\", "/");

                if (lArgList.contains(s_sSwitch_put))
                {
                    LOG_DEBUG(QString("Client mode, putting file %1 to %2").arg(sSource).arg(sTarget));
                }
                else
                {
                    LOG_DEBUG(QString("Client mode, merging file %1 with %2").arg(sSource).arg(sTarget));
                }

                m_pServer = new CRemoteControl(sIP, quint16(iPort), iTimeoutMS, iMaxWaitingTimeMS, bDoShell);
                connect(m_pServer, SIGNAL(transactionTerminated(int)), this, SLOT(onQuitSignal(int)));

                if (m_pServer->getClientConnected() == false) return false;
                m_pServer->setLoginPassword(sLogin, sPassword);

                if (lArgList.contains(s_sSwitch_put))
                {
                    if (m_pServer->putFile(sSource, sTarget, bKeepNewest, bCompCRC) == false)
                    {
                        return false;
                    }
                }
                else
                {
                    if (m_pServer->mergeFile(sSource, sTarget) == false)
                    {
                        return false;
                    }
                }
            }
            else
            {
                echo("Source or target missing\n");

                return false;
            }
        }
        else
        {
            printUsage();
            return false;
        }

        return true;
    }
}

CApplication::~CApplication()
{
    if (m_pServer != nullptr)
        delete m_pServer;
}

void CApplication::onQuitSignal(int iError)
{
    LOG_DEBUG("onQuitSignal");

    exit(iError);
}

void CApplication::onShutdownSignal()
{
    LOG_DEBUG("onShutdownSignal");

    exit(0);
}

void CApplication::echo(const QString& sText)
{
    QTextStream sOut(stdout);

    sOut << sText;
}

void CApplication::printUsage()
{
    QTextStream sOut(stdout);

    sOut << "\n";
    sOut << "Usage:\n";
    sOut << "RemoteControl [options] <action>\n";
    sOut << "\n";
    sOut << "actions:\n";
    sOut << "  " << s_sSwitch_serve << ": enter server mode and listen\n";
    sOut << "  " << s_sSwitch_ping << ": just try to connect\n";
    sOut << "  " << s_sSwitch_command << ": execute a command on remote server\n";
    sOut << "  " << s_sSwitch_get << ": get files from remote server\n";
    sOut << "  " << s_sSwitch_put << ": put files to remote server\n";
    sOut << "  " << s_sSwitch_getfilecrc << ": get remote file crc\n";
    sOut << "  " << s_sSwitch_getprivs << ": get your user privileges\n";
    sOut << "\n";
    sOut << "options:\n";
    sOut << "  " << s_sSwitch_ip << ": specify server self IP or remote server IP\n";
    sOut << "  " << s_sSwitch_port << ": specify server self port or remote server port\n";
    sOut << "  " << s_sSwitch_source << ": specify source file name for file transfer\n";
    sOut << "  " << s_sSwitch_target << ": specify target file name for file transfer\n";
    sOut << "  " << s_sSwitch_shell << ": enter shell mode\n";
    sOut << "  " << s_sSwitch_compcrc << ": get/put files only if crc is different\n";
    sOut << "  " << s_sSwitch_detached << ": execute commands as detached process\n";
    sOut << "  " << s_sSwitch_timeout << ": connection timeout in milliseconds\n";
    sOut << "  " << s_sSwitch_login << ": user name for login\n";
    sOut << "  " << s_sSwitch_password << ": user password for login\n";
    sOut << "\n";
    sOut << "Exemple for " << s_sSwitch_command << ":\n";
    sOut << "  RemoteControl " << s_sSwitch_ip << " 10.1.0.90 " << s_sSwitch_command << " dir *.*\n";
    sOut << "\n";
    sOut << "Exemple for " << s_sSwitch_get << ":\n";
    sOut << "  RemoteControl " << s_sSwitch_get << " " << s_sSwitch_source << " ./Docs/*.txt " << s_sSwitch_target << " D:/Output";
    sOut << "\n";
    sOut << "\n";
    sOut << "Version " << REMOTECONTROL_VERSION << "\n";
    sOut.flush();
}
