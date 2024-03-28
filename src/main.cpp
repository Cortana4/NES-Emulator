#include <QtWidgets/QApplication>

#include "mainWindow.h"
#include "nes.h"


int main(int argC, char* argV[])
{
	QApplication app{ argC, argV };

	//NES nes;

	MainWindow window;
	window.show();

	return app.exec();
}
