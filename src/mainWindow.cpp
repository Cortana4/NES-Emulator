#include "mainWindow.h"

#include <QtWidgets/QFileDialog>


MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
	: QMainWindow{ parent, flags }
	, menuFile{ menuBar()->addMenu("File") }
	, actionOpenROM{ new QAction{ "Open ROM..." } }
	, widgetCentral{ new QWidget{ this } }
	, hBoxLayout{ new QHBoxLayout{ widgetCentral } }
	, nesRenderTarget{ new NESRenderTarget{ nes, widgetCentral } }
	, statusBar{ new QStatusBar{ this } }
//	, nes{ new NES{} }
{
	// menu bar
	menuFile->addAction(actionOpenROM);
	connect(actionOpenROM, &QAction::triggered, this, &MainWindow::openROM);

	// central widget
	setCentralWidget(widgetCentral);

	// OpenGL widget
	hBoxLayout->setContentsMargins(0, 0, 0, 0);
	hBoxLayout->addWidget(nesRenderTarget);

	// status bar
	setStatusBar(statusBar);
	statusBar->showMessage("Ready");
}

void MainWindow::openROM()
{
	QString filePath = QFileDialog::getOpenFileName(this, "Open ROM file", QDir::homePath(), "NES ROM Files (*.nes)");
	statusBar->showMessage("ROM loaded");
}
