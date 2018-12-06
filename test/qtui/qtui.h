#ifndef QTUI_H
#define QTUI_H

#include <QtWidgets/QMainWindow>
#include "ui_qtui.h"

class qtui : public QMainWindow
{
	Q_OBJECT

public:
	qtui(QWidget *parent = 0);
	~qtui();

private:
	Ui::qtuiClass ui;
};

#endif // QTUI_H
