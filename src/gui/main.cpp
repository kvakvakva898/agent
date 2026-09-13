#include <QApplication>

#include "main_window.h"

int main(
	int argc,
	char* argv[])
{
	QApplication app(
		argc,
		argv);

	MainWindow window(
		"http://10.153.92.115:1234/v1/chat/completions",
		"nvidia/nemotron-3-nano-4b",
		"prompts/code_review.txt");

	window.show();

	return app.exec();
}