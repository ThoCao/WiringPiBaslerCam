#include "MyServer.h"



MyServer::MyServer(QObject *parent) :
	QTcpServer(parent)
{
	connect(this, &MyServer::getcommand, this, &MyServer::runcommand, Qt::DirectConnection);
	wiringPiSetup () ;
	pinMode (LED, OUTPUT) ;
	pinMode(LASER,OUTPUT);

	delay (100) ;               // mS
	digitalWrite (LASER, HIGH) ;  // On
}

void MyServer::StartServer()
{
	if (!this->listen(QHostAddress::Any, 8585))
	{
		qDebug() << "Could not start server";
	}
	else
	{
		qDebug() << "Listening...";
	}
}

void MyServer::InitialSensorCamera()
{
	if (_cam.OpenCam()) {
		_isCamConnected = true;
		std::cout << "Camera Connected" << std::endl;
	}
	else {
		_isCamConnected = false;
		std::cout << "Camera Not Connected" << std::endl;
	}
}

void MyServer::readyRead()
{
	QByteArray data = socket->readAll();
	emit getcommand(data);

}

void MyServer::disconnected()
{
	qDebug() << "close all thread";
	// close camera
	_cam.CloseCam();
	_flagConnected = false;
	_isCamConnected = false;
	socket->deleteLater();
}


void MyServer::runcommand(QByteArray data)
{
	QStringList commandSeries = QString(data).split(" ");
	if (commandSeries.size() <= 1) {
		socket->write("Wrong Command!");
		return;
	}

	QString IdCode = commandSeries[0];

	if (IdCode.contains("laser", Qt::CaseInsensitive)) {// send image
		// send size

		if (_isCamConnected == true) {

			digitalWrite (LED, HIGH) ;  // On
			delay (1) ;               // mS
			digitalWrite (LED, LOW) ;   // Off
			//delay (1) ;
			//
			_cam.ReceiveImage(_img);

			digitalWrite (LASER, HIGH) ;   // Off
		}
		else {
			_img = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		if (_img.empty()) {
			_img = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		this->_buf.clear();
		cv::imencode(".bmp", _img, _buf);
		QString codeimg = "image " + QString::number(_buf.size());
		socket->write(codeimg.toUtf8());
	}


	else if (IdCode.contains("sub", Qt::CaseInsensitive)) {// send image
		// send size

		if (_isCamConnected == true) {
			// no laser image
			digitalWrite (LASER, LOW) ;  // On
			delay (1) ;               // mS
			digitalWrite (LED, HIGH) ;  // On
			delay (1) ;               // mS
			digitalWrite (LED, LOW) ;   // Off
			//dela
			//
			_cam.ReceiveImage(_noLaserImg);
			digitalWrite (LASER, HIGH) ;   // Off
			// laser image	
			digitalWrite (LED, HIGH) ;  // On
			delay (15) ;               // mS
			digitalWrite (LED, LOW) ;   // Off
			delay(15);
			_cam.ReceiveImage(_LaserImg);

			_img = _LaserImg - _noLaserImg;
		}
		else {
			_img = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		if (_img.empty()) {
			_noLaserImg = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		this->_buf.clear();
		cv::imencode(".bmp", _img, _buf);
		QString codeimg = "image " + QString::number(_buf.size());
		socket->write(codeimg.toUtf8());
	}

	else if (IdCode.contains("image", Qt::CaseInsensitive)) {// send image
		// send size

		if (_isCamConnected == true) {

			digitalWrite (LASER, LOW) ;  // On
			delay (1) ;               // mS
			digitalWrite (LED, HIGH) ;  // On
			delay (1) ;               // mS
			digitalWrite (LED, LOW) ;   // Off
			//delay (1) ;
			//
			_cam.ReceiveImage(_img);

			digitalWrite (LASER, HIGH) ;   // Off
		}
		else {
			_img = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		if (_img.empty()) {
			_img = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		this->_buf.clear();
		cv::imencode(".bmp", _img, _buf);
		QString codeimg = "image " + QString::number(_buf.size());
		socket->write(codeimg.toUtf8());
	}



	else if (IdCode.contains("abc", Qt::CaseInsensitive)) {// send image
		// send size

		if (_isCamConnected == true) {
			// no laser image
			digitalWrite (LASER, LOW) ;  // On
			delay (1) ;               // mS
			digitalWrite (LED, HIGH) ;  // On
			delay (1) ;               // mS
			digitalWrite (LED, LOW) ;   // Off
			//dela
			//
			_cam.ReceiveImage(_noLaserImg);
			digitalWrite (LASER, HIGH) ;   // Off
			// laser image	
			digitalWrite (LED, HIGH) ;  // On
			delay (15) ;               // mS
			digitalWrite (LED, LOW) ;   // Off
			delay(15);
			_cam.ReceiveImage(_LaserImg);
		}
		else {
			_noLaserImg = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		if (_noLaserImg.empty()) {
			_noLaserImg = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		this->_buf.clear();
		cv::imencode(".bmp", _noLaserImg, _buf);
		QString codeimg = "image " + QString::number(_buf.size());
		socket->write(codeimg.toUtf8());
	}

	else if (IdCode.contains("def", Qt::CaseInsensitive)) {// send image
		// send size

		if (_LaserImg.empty()) {
			_LaserImg = cv::Mat::zeros(cv::Size(200, 200), CV_64FC1);
		}

		this->_buf.clear();
		cv::imencode(".bmp", _LaserImg, _buf);
		QString codeimg = "image " + QString::number(_buf.size());
		socket->write(codeimg.toUtf8());
	}
	else if (IdCode.contains("senddata", Qt::CaseInsensitive)) {
		socket->write((char*)_buf.data(), _buf.size());
	}

	else if (IdCode.contains("exposure", Qt::CaseInsensitive)) {

		int exposure_value = commandSeries[1].toInt();


		socket->write("exposure okay");
	}


	else {
		socket->write("Wrong Command!");
	}

	qDebug() << IdCode;
}

void MyServer::incomingConnection(qintptr  ID)
{
	if (_flagConnected == true) return;
	qDebug() << ID << " Connecting...";
	this->socketDescriptor = ID;

	socket = new QTcpSocket();
	if (!socket->setSocketDescriptor(this->socketDescriptor))
	{
		emit error(socket->error());
		return;
	}
	else {
		qDebug() << "new socket connected";
	}
	// Initial Sensor Camera
	InitialSensorCamera();

	connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);

	_flagConnected = true;
}

MyServer::~MyServer()
{

}

