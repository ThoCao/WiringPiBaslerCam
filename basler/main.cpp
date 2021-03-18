#include <QtCore/QCoreApplication>
#include "MyServer.h"

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
        printf ("Raspberry Pi blink\n") ;


	MyServer myServer;

	myServer.StartServer();

	return a.exec();
}

