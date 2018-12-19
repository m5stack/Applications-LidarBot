#include "app.h"
#include "myhelper.h"


QString App::PortName = "COM1";
int App::BaudRate = 9600;
int App::DataBit = 8;
QString App::Parity = "无";
double App::StopBit = 1;

bool App::HexSend = false;
bool App::HexReceive = false;
bool App::Debug = false;
bool App::AutoClear = false;

bool App::AutoSend = false;
int App::SendInterval = 1000;
bool App::AutoSave = false;
int App::SaveInterval = 5000;

QString App::SendFileName = "send.txt";
QString App::DeviceFileName = "device.txt";

QString App::Mode = "Tcp_Client";
QString App::ServerIP = "127.0.0.1";
int App::ServerPort = 6000;
int App::ListenPort = 6000;
int App::SleepTime = 100;
bool App::AutoConnect = true;

void App::ReadConfig()
{
    if (!CheckConfig()) {
        return;
    }

    QString fileName = QString("%1/%2_Config.ini").arg(AppPath).arg(AppName);
    QSettings set(fileName, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    App::PortName = set.value("PortName").toString();
    App::BaudRate = set.value("BaudRate").toInt();
    App::DataBit = set.value("DataBit").toInt();
    App::Parity = set.value("Parity").toString();
    App::StopBit = set.value("StopBit").toInt();

    App::HexSend = set.value("HexSend").toBool();
    App::HexReceive = set.value("HexReceive").toBool();
    App::Debug = set.value("Debug").toBool();
    App::AutoClear = set.value("AutoClear").toBool();

    App::AutoSend = set.value("AutoSend").toBool();
    App::SendInterval = set.value("SendInterval").toInt();
    App::AutoSave = set.value("AutoSave").toBool();
    App::SaveInterval = set.value("SaveInterval").toInt();

    App::SendFileName = set.value("SendFileName").toString();
    App::DeviceFileName = set.value("DeviceFileName").toString();
    set.endGroup();

    set.beginGroup("NetConfig");
    App::Mode = set.value("Mode").toString();
    App::ServerIP = set.value("ServerIP").toString();
    App::ServerPort = set.value("ServerPort").toInt();
    App::ListenPort = set.value("ListenPort").toInt();
    App::SleepTime = set.value("SleepTime").toInt();
    App::AutoConnect = set.value("AutoConnect").toBool();
    set.endGroup();
}

void App::WriteConfig()
{
    QString fileName = QString("%1/%2_Config.ini").arg(AppPath).arg(AppName);
    QSettings set(fileName, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    set.setValue("PortName", App::PortName);
    set.setValue("BaudRate", App::BaudRate);
    set.setValue("DataBit", App::DataBit);
    set.setValue("Parity", App::Parity);
    set.setValue("StopBit", App::StopBit);

    set.setValue("HexSend", App::HexSend);
    set.setValue("HexReceive", App::HexReceive);
    set.setValue("Debug", App::Debug);
    set.setValue("AutoClear", App::AutoClear);

    set.setValue("AutoSend", App::AutoSend);
    set.setValue("SendInterval", App::SendInterval);
    set.setValue("AutoSave", App::AutoSave);
    set.setValue("SaveInterval", App::SaveInterval);

    set.setValue("SendFileName", App::SendFileName);
    set.setValue("DeviceFileName", App::DeviceFileName);
    set.endGroup();

    set.beginGroup("NetConfig");
    set.setValue("Mode", App::Mode);
    set.setValue("ServerIP", App::ServerIP);
    set.setValue("ServerPort", App::ServerPort);
    set.setValue("ListenPort", App::ListenPort);
    set.setValue("SleepTime", App::SleepTime);
    set.setValue("AutoConnect", App::AutoConnect);
    set.endGroup();
}

void App::NewConfig()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    App::Parity = App::Parity.toLatin1();
#endif
    WriteConfig();
}

bool App::CheckConfig()
{
    QString fileName = QString("%1/%2_Config.ini").arg(AppPath).arg(AppName);

    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(fileName);

    if (file.size() == 0) {
        NewConfig();
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;

        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                if (list.at(1) == "") {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            NewConfig();
            return false;
        }
    } else {
        NewConfig();
        return false;
    }

    return true;
}

void App::WriteError(QString str)
{
    QString fileName = QString("%1/%2_Error_%3.txt").arg(AppPath).arg(AppName).arg(QDATE);
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << str << "\n";
}

void App::NewDir(QString dirName)
{
    //如果路径中包含斜杠字符则说明是绝对路径
    //linux系统路径字符带有 /  windows系统 路径字符带有 :/
    if (!dirName.startsWith("/") && !dirName.contains(":/")) {
        dirName = QString("%1/%2").arg(AppPath).arg(dirName);
    }

    QDir dir(dirName);

    if (!dir.exists()) {
        dir.mkpath(dirName);
    }
}
