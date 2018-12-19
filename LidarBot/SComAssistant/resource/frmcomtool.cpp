#include "frmcomtool.h"
#include "ui_frmcomtool.h"
#include "myhelper.h"
#include "app.h"
#include "qextserialport/qextserialport.h"

frmComTool::frmComTool(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::frmComTool)
{
     ui->setupUi(this);
     this->initForm();
     this->initConfig();
     QTimer::singleShot(0, this, SLOT(readSendData()));
     QTimer::singleShot(0, this, SLOT(readDeviceData()));
     myHelper::formInCenter(this);
     ui->labelMapDisplay->installEventFilter(this);


    //image = QImage(430,420,QImage::Format_RGB32);
    //image = new QImage();

    //image = QImage(430,420,QImage::Format_RGB32);
    //QColor backColor = qRgb(0,0,0);
    //QColor backColor = qRgb(250,250,250);
    //image.fill(backColor);
    //update();
}

frmComTool::~frmComTool()
{
	delete ui;
}


bool frmComTool::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->labelMapDisplay && event->type() == QEvent::Paint){
        magicTime();
    }
    return QWidget::eventFilter(watched,event);
}


void frmComTool::magicTime()
{
#if 0
    static int i;
    int  row , column;
    QPainter painter(ui->labelMapDisplay);
    painter.setBrush(Qt::green);
    for(row = 0;row < 10;row++,row++)
        for(column = 0;column < 10;column++,column++,column++)
     //painter.drawPoint(row,column);
     painter.drawPoint(i+row,i+column);
     i++;
#else
   QPainter painter(ui->labelMapDisplay);
   painter.setBrush(Qt::green);
   QRect r = ui->labelMapDisplay->geometry();
   for(int i = 0; i < 360; i++) {
      disX[i] = (r.width()/4  + (distance[i] / 70) * cos(3.14159 * i / 180 + 0.13))*2;
      disY[i] = (r.height()/4 + (distance[i] / 70) * sin(3.14159 * i / 180 + 0.13))*2;
      if(distance[i] == 250)
       painter.setPen(Qt::blue);
      else
       painter.setPen(Qt::yellow);
      painter.drawPoint(disX[i],disY[i]);
   }
#endif

    //update();
}


void frmComTool::paintEvent(QPaintEvent *)
{
    //QPainter painter(ui->label);
     //QPainter painter(this);
   // painter.drawLine(QPointF(0, 0), QPointF(100, 100));
}
void frmComTool::initForm()
{
    this->setWindowTitle("雷达地图显示 - M5STACK");

	comOk = false;
	com = 0;
	maxCount = 50;
	currentCount = 0;
	sleepTime = 10;
	receiveCount = 0;
	sendCount = 0;
	isShow = true;

	//读取数据
	timerRead = new QTimer(this);
	timerRead->setInterval(100);
	connect(timerRead, SIGNAL(timeout()), this, SLOT(readData()));
    //connect(timerRead, SIGNAL(timeout()), this, SLOT(eventFilter()));


	//发送数据
	timerSend = new QTimer(this);
	connect(timerSend, SIGNAL(timeout()), this, SLOT(sendData()));
	connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(sendData()));

	//保存数据
	timerSave = new QTimer(this);
	connect(timerSave, SIGNAL(timeout()), this, SLOT(saveData()));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(saveData()));

	changeEnable(false);

	tcpOk = false;
	socket = new QTcpSocket(this);
	socket->abort();
	connect(socket, SIGNAL(readyRead()), this, SLOT(readDataNet()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));

	timerConnect = new QTimer(this);
	connect(timerConnect, SIGNAL(timeout()), this, SLOT(connectNet()));
	timerConnect->setInterval(3000);
	timerConnect->start();
}

void frmComTool::initConfig()
{
	App::ReadConfig();

	QStringList comList;

    for (int i = 1; i <= 50; i++) {
		comList << QString("COM%1").arg(i);
	}

	//comList << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4";
	ui->cboxPortName->addItems(comList);
	ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(App::PortName));
	connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	QStringList baudList;
	baudList << "50" << "75" << "100" << "134" << "150" << "200" << "300" << "600" << "1200"
	         << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
	         << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

	ui->cboxBaudRate->addItems(baudList);
	ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(App::BaudRate)));
	connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	QStringList dataBitsList;
	dataBitsList << "5" << "6" << "7" << "8";

	ui->cboxDataBit->addItems(dataBitsList);
	ui->cboxDataBit->setCurrentIndex(ui->cboxDataBit->findText(QString::number(App::DataBit)));
	connect(ui->cboxDataBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	QStringList parityList;
	parityList << "无" << "奇" << "偶";
#ifdef Q_OS_WIN
	parityList << "标志";
#endif
	parityList << "空格";

	ui->cboxParity->addItems(parityList);
	ui->cboxParity->setCurrentIndex(ui->cboxParity->findText(App::Parity));
	connect(ui->cboxParity, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	QStringList stopBitsList;
	stopBitsList << "1";
#ifdef Q_OS_WIN
	stopBitsList << "1.5";
#endif
	stopBitsList << "2";

	ui->cboxStopBit->addItems(stopBitsList);
	ui->cboxStopBit->setCurrentIndex(ui->cboxStopBit->findText(QString::number(App::StopBit)));
	connect(ui->cboxStopBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	ui->ckHexSend->setChecked(App::HexSend);
	connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

	ui->ckHexReceive->setChecked(App::HexReceive);
	connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

	ui->ckDebug->setChecked(App::Debug);
	connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

	ui->ckAutoClear->setChecked(App::AutoClear);
	connect(ui->ckAutoClear, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

	ui->ckAutoSend->setChecked(App::AutoSend);
	connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

	ui->ckAutoSave->setChecked(App::AutoSave);
	connect(ui->ckAutoSave, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

	QStringList sendInterval;
	QStringList saveInterval;
	sendInterval << "100" << "300" << "500";

	for (int i = 1000; i <= 10000; i = i + 1000) {
		sendInterval << QString::number(i);
		saveInterval << QString::number(i);
	}

	ui->cboxSendInterval->addItems(sendInterval);
	ui->cboxSaveInterval->addItems(saveInterval);

	ui->cboxSendInterval->setCurrentIndex(ui->cboxSendInterval->findText(QString::number(App::SendInterval)));
	connect(ui->cboxSendInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
	ui->cboxSaveInterval->setCurrentIndex(ui->cboxSaveInterval->findText(QString::number(App::SaveInterval)));
	connect(ui->cboxSaveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	timerSend->setInterval(App::SendInterval);
	timerSave->setInterval(App::SaveInterval);

	if (App::AutoSend) {
		timerSend->start();
	}

	if (App::AutoSave) {
		timerSave->start();
	}

	//串口转网络部分
	ui->cboxMode->setCurrentIndex(ui->cboxMode->findText(App::Mode));
	connect(ui->cboxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	ui->txtServerIP->setText(App::ServerIP);
	connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

	ui->txtServerPort->setText(QString::number(App::ServerPort));
	connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

	ui->txtListenPort->setText(QString::number(App::ListenPort));
	connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

	QStringList values;
	values << "0" << "10" << "50";

	for (int i = 100; i < 1000; i = i + 100) {
		values << QString("%1").arg(i);
	}

	ui->cboxSleepTime->addItems(values);

	ui->cboxSleepTime->setCurrentIndex(ui->cboxSleepTime->findText(QString::number(App::SleepTime)));
	connect(ui->cboxSleepTime, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

	ui->ckAutoConnect->setChecked(App::AutoConnect);
	connect(ui->ckAutoConnect, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
}

void frmComTool::saveConfig()
{
	App::PortName = ui->cboxPortName->currentText();
	App::BaudRate = ui->cboxBaudRate->currentText().toInt();
	App::DataBit = ui->cboxDataBit->currentText().toInt();
	App::Parity = ui->cboxParity->currentText();
	App::StopBit = ui->cboxStopBit->currentText().toDouble();

	App::HexSend = ui->ckHexSend->isChecked();
	App::HexReceive = ui->ckHexReceive->isChecked();
	App::Debug = ui->ckDebug->isChecked();
	App::AutoClear = ui->ckAutoClear->isChecked();

	App::AutoSend = ui->ckAutoSend->isChecked();
	App::AutoSave = ui->ckAutoSave->isChecked();

	int sendInterval = ui->cboxSendInterval->currentText().toInt();

	if (sendInterval != App::SendInterval) {
		App::SendInterval = sendInterval;
		timerSend->setInterval(App::SendInterval);
	}

	int saveInterval = ui->cboxSaveInterval->currentText().toInt();

	if (saveInterval != App::SaveInterval) {
		App::SaveInterval = saveInterval;
		timerSave->setInterval(App::SaveInterval);
	}

	App::Mode = ui->cboxMode->currentText();
	App::ServerIP = ui->txtServerIP->text().trimmed();
	App::ServerPort = ui->txtServerPort->text().toInt();
	App::ListenPort = ui->txtListenPort->text().toInt();
	App::SleepTime = ui->cboxSleepTime->currentText().toInt();
	App::AutoConnect = ui->ckAutoConnect->isChecked();

	App::WriteConfig();
}

void frmComTool::changeEnable(bool b)
{
	ui->cboxBaudRate->setEnabled(!b);
	ui->cboxDataBit->setEnabled(!b);
	ui->cboxParity->setEnabled(!b);
	ui->cboxPortName->setEnabled(!b);
	ui->cboxStopBit->setEnabled(!b);
	ui->btnSend->setEnabled(b);
	ui->ckAutoSend->setEnabled(b);
	ui->ckAutoSave->setEnabled(b);
}

void frmComTool::append(quint8 type, QString msg)
{
	if (!isShow) {
		return;
	}

	if (currentCount >= maxCount) {
		ui->txtMain->clear();
		currentCount = 0;
	}

	QString str;

	if (type == 0) {
		str = ">> 串口发送 :";
		ui->txtMain->setTextColor(QColor("dodgerblue"));
	} else if (type == 1) {
		str = "<< 串口接收 :";
        ui->txtMain->setTextColor(QColor("green"));
	} else if (type == 2) {
		str = ">> 处理延时 :";
		ui->txtMain->setTextColor(QColor("gray"));
	} else if (type == 3) {
		str = ">> 正在校验 :";
		ui->txtMain->setTextColor(QColor("green"));
	} else if (type == 4) {
		str = ">> 网络发送 :";
		ui->txtMain->setTextColor(QColor(24, 189, 155));
	} else if (type == 5) {
		str = "<< 网络接收 :";
		ui->txtMain->setTextColor(QColor(255, 107, 107));
	} else if (type == 6) {
		str = ">> 提示信息 :";
		ui->txtMain->setTextColor(QColor(100, 184, 255));
	}

	ui->txtMain->append(QString("时间[%1] %2 %3").arg(TIMEMS).arg(str).arg(msg));
	currentCount++;
}

void frmComTool::readData()
{
	if (com->bytesAvailable() <= 0) {
		return;
	}

    myHelper::sleep(sleepTime);
	QByteArray data = com->readAll();
    com->flush();
	int dataLen = data.length();
    qDebug() << "data = " << dataLen;
	if (dataLen <= 0) {
		return;
	}
    //sendData(QString data)
    if (dataLen == 180) {
        for (int i = 0; i < dataLen/4; i++){
            int temp = (uint8_t)data[4 * i] * 256 + (uint8_t)data[4 * i + 1];
            if((temp > 0) && (temp < 360))
             distance[temp] = (uint8_t)data[4 * i + 2] * 256 + (uint8_t)data[4 * i + 3];
            //temp = data.at(i) * 256 + data.at(i+1);
            //qDebug() << "temp = " << temp;
        }
        magicTime();
        update();

        sendData("ack");
        return;
    }

	if (isShow) {
		QString buffer;

		if (ui->ckHexReceive->isChecked()) {
			buffer = myHelper::byteArrayToHexStr(data);
		} else {
			buffer = myHelper::byteArrayToAsciiStr(data);
		}

		//启用调试则模拟调试数据
		if (ui->ckDebug->isChecked()) {
			int count = keys.count();

			for (int i = 0; i < count; i++) {
				if (buffer.startsWith(keys.at(i))) {
					sendData(values.at(i));
					break;
				}
			}
		}

		append(1, buffer);
		receiveCount = receiveCount + data.size();
		ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));
 //qDebug() << QString("接收 : %1 字节").arg(receiveCount);
		//启用网络转发则调用网络发送数据
		if (tcpOk) {
			socket->write(data);
			append(4, QString(buffer));
		}

        //qDebug() << data << "dataLen = " << dataLen;
       // qDebug() << "dataLen = " << dataLen;
        if(dataLen == 720){
        magicTime();
        update();
        //buffer.toUShort();
       // qDebug() << "buffer = " << buffer;

        }
	}
}

void frmComTool::sendData()
{
	QString str = ui->cboxSend->currentText();

	if (str.isEmpty()) {
		ui->cboxSend->setFocus();
		return;
	}

	sendData(str);

	if (ui->ckAutoClear->isChecked()) {
		ui->cboxSend->setCurrentIndex(-1);
		ui->cboxSend->setFocus();
	}
}

void frmComTool::sendData(QString data)
{
	if (com == 0 || !com->isOpen()) {
		return;
	}

	QByteArray buffer;

	if (ui->ckHexSend->isChecked()) {
		buffer = myHelper::hexStrToByteArray(data);
	} else {
		buffer = myHelper::asciiStrToByteArray(data);
	}

	com->write(buffer);
	append(0, data);
	sendCount = sendCount + buffer.size();
	ui->btnSendCount->setText(QString("发送 : %1 字节").arg(sendCount));
}

void frmComTool::saveData()
{
	QString tempData = ui->txtMain->toPlainText();
    //qDebug() << tempData;
	if (tempData == "") {
		return;
	}

	QDateTime now = QDateTime::currentDateTime();
	QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
	QString fileName = QString("%1/%2.txt").arg(AppPath).arg(name);

	QFile file(fileName);
	file.open(QFile::WriteOnly | QIODevice::Text);
	QTextStream out(&file);
	out << tempData;
	file.close();

	currentCount = 0;
	ui->txtMain->clear();
}

void frmComTool::on_btnOpen_clicked()
{
	if (ui->btnOpen->text() == "打开串口") {
		com = new QextSerialPort(ui->cboxPortName->currentText(), QextSerialPort::Polling);
		comOk = com->open(QIODevice::ReadWrite);

		if (comOk) {
			//清空缓冲区
			com->flush();
			//设置波特率
			com->setBaudRate((BaudRateType)ui->cboxBaudRate->currentText().toInt());
			//设置数据位
			com->setDataBits((DataBitsType)ui->cboxDataBit->currentText().toInt());
			//设置校验位
			com->setParity((ParityType)ui->cboxParity->currentIndex());
			//设置停止位
			com->setStopBits((StopBitsType)ui->cboxStopBit->currentIndex());
			com->setFlowControl(FLOW_OFF);
			com->setTimeout(10);

			changeEnable(true);
			ui->btnOpen->setText("关闭串口");
			timerRead->start();
		}
	} else {
		timerRead->stop();
		com->close();
		changeEnable(false);
		ui->btnOpen->setText("打开串口");
		on_btnClear_clicked();
		comOk = false;
	}
}

void frmComTool::on_btnSendCount_clicked()
{
	sendCount = 0;
	ui->btnSendCount->setText("发送 : 0 字节");
}

void frmComTool::on_btnReceiveCount_clicked()
{
	receiveCount = 0;
	ui->btnReceiveCount->setText("接收 : 0 字节");
}

void frmComTool::on_btnStopShow_clicked()
{
	if (ui->btnStopShow->text() == "停止显示") {
		isShow = false;
		ui->btnStopShow->setText("开始显示");
	} else {
		isShow = true;
		ui->btnStopShow->setText("停止显示");
	}
}

void frmComTool::readSendData()
{
	QString fileName = QString("%1/%2").arg(AppPath).arg("send.txt");
	QFile file(fileName);

	if (!file.exists()) {
		return;
	}

	ui->cboxSend->clear();
	file.open(QFile::ReadOnly | QIODevice::Text);
	QTextStream in(&file);
	QString line;

	do {
		line = in.readLine();

		if (line != "") {
			ui->cboxSend->addItem(line);
		}
	} while (!line.isNull());

	file.close();
}

void frmComTool::readDeviceData()
{
	QString fileName = QString("%1/%2").arg(AppPath).arg("device.txt");
	QFile file(fileName);

	if (!file.exists()) {
		return;
	}

	file.open(QFile::ReadOnly | QIODevice::Text);
	QTextStream in(&file);
	QString line;

	do {
		line = in.readLine();

		if (line != "") {
			QStringList list = line.split(";");
			QString key = list.at(0);
			QString value;

			for (int i = 1; i < list.count(); i++) {
				value += QString("%1;").arg(list.at(i));
			}

			//去掉末尾分号
			value = value.mid(0, value.length() - 1);

			keys.append(key);
			values.append(value);
		}
	} while (!line.isNull());

	file.close();
}

void frmComTool::on_btnData_clicked()
{
	QString fileName = QString("%1/%2").arg(AppPath).arg("send.txt");
	QFile file(fileName);

	if (!file.exists()) {
		return;
	}

	if (ui->btnData->text() == "管理数据") {
		ui->txtMain->setReadOnly(false);
		ui->txtMain->clear();
		file.open(QFile::ReadOnly | QIODevice::Text);
		QTextStream in(&file);
		ui->txtMain->setText(in.readAll());
		file.close();
		ui->btnData->setText("保存数据");
	} else {
		ui->txtMain->setReadOnly(true);
		file.open(QFile::WriteOnly | QIODevice::Text);
		QTextStream out(&file);
		out << ui->txtMain->toPlainText();
		file.close();
		ui->txtMain->clear();
		ui->btnData->setText("管理数据");
		this->readSendData();
	}
}

void frmComTool::on_btnClear_clicked()
{
	ui->txtMain->clear();
	currentCount = 0;
}

void frmComTool::on_ckAutoSend_stateChanged(int arg1)
{
	if (arg1 == 0) {
		ui->cboxSendInterval->setEnabled(false);
		timerSend->stop();
	} else {
		ui->cboxSendInterval->setEnabled(true);
		timerSend->start();
	}
}

void frmComTool::on_ckAutoSave_stateChanged(int arg1)
{
	if (arg1 == 0) {
		ui->cboxSaveInterval->setEnabled(false);
		timerSave->stop();
	} else {
		ui->cboxSaveInterval->setEnabled(true);
		timerSave->start();
	}
}

void frmComTool::connectNet()
{
	if (!tcpOk && App::AutoConnect && ui->btnStart->text() == "启动") {
		if (App::ServerIP != "" && App::ServerPort != 0) {
			socket->connectToHost(App::ServerIP, App::ServerPort);

			if (socket->waitForConnected(100)) {
				ui->btnStart->setText("停止");
				append(6, "连接服务器成功");
				tcpOk = true;
			}
		}
	}
}

void frmComTool::readDataNet()
{
	if (socket->bytesAvailable() > 0) {
		myHelper::sleep(App::SleepTime);
		QByteArray data = socket->readAll();

		QString buffer;

		if (ui->ckHexReceive->isChecked()) {
			buffer = myHelper::byteArrayToHexStr(data);
		} else {
			buffer = myHelper::byteArrayToAsciiStr(data);
		}

		append(5, buffer);

		//将收到的网络数据转发给串口
		if (comOk) {
			com->write(data);
			append(0, buffer);
		}
	}
}

void frmComTool::readErrorNet()
{
	ui->btnStart->setText("启动");
	append(6, QString("连接服务器失败,%1").arg(socket->errorString()));
	socket->disconnectFromHost();
	tcpOk = false;
}

void frmComTool::on_btnStart_clicked()
{
	if (ui->btnStart->text() == "启动") {
		if (App::ServerIP == "" || App::ServerPort == 0) {
			append(6, "IP地址和远程端口不能为空");
			return;
		}

		socket->connectToHost(App::ServerIP, App::ServerPort);

		if (socket->waitForConnected(100)) {
			ui->btnStart->setText("停止");
			append(6, "连接服务器成功");
			tcpOk = true;
		}
	} else {
		socket->disconnectFromHost();

		if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(100)) {
			ui->btnStart->setText("启动");
			append(6, "断开服务器成功");
			tcpOk = false;
		}
	}
}
