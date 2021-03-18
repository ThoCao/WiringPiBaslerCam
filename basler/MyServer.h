#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QtGui/QImage>
#include <QBuffer>
#include <QtGui/QImageWriter>
#include <QFile>
#include "BaslerCam.h"
#include <wiringPi.h>

#define LED    0
#define LASER  25

class MyServer: public QTcpServer
{
	Q_OBJECT

public:
	explicit MyServer(QObject *parent = 0);
	~MyServer();
	void StartServer();
	void InitialSensorCamera();
signals:
	void error(QTcpSocket::SocketError socketerror);
	void getcommand(QByteArray data);

	public slots :
	void readyRead();
	void disconnected();


	void runcommand(QByteArray data);

private:
	QTcpSocket *socket;
	qintptr socketDescriptor;
	bool _flagConnected = false;

protected:
	void incomingConnection(qintptr  socketDescriptor);

	// laser and motor
private:
	BaslerCam _cam;
	cv::Mat _noLaserImg;
	cv::Mat _LaserImg;
	bool _isCamConnected = false;
	cv::Mat _img;
	std::vector<uchar> _buf;

};


