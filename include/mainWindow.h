#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QHBoxLayout>

#include "nesRenderTarget.h"
#include "nes.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

private:
	QMenu* menuFile;
	QAction* actionOpenROM;
	QWidget* widgetCentral;
	QHBoxLayout* hBoxLayout;
	NESRenderTarget* nesRenderTarget;
	QStatusBar* statusBar;

	NES nes;


private slots:
	void openROM();

};
