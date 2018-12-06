#include "qtui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qtui w;
	w.show();
	return a.exec();
}
