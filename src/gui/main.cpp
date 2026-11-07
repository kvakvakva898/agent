#include <QApplication>
#include <QMessageBox>

#include "main_window.h"
#include "core/config_loader.h"
#include "app/application.h"

int main(
	int argc,
	char* argv[])
{
	QApplication app(
		argc,
		argv);
	AgentConfig config;

	const auto config_path =
		std::filesystem::path(
			QApplication::applicationDirPath().toStdString())
		/ "config.json";

	try
	{
		if (std::filesystem::exists(config_path))
		{
			ConfigLoader loader;
			config = loader.load(config_path);
		}
		else
		{
			config.prompt_template_path =
				std::filesystem::path(
					QApplication::applicationDirPath().toStdString())
				/ "prompts"
				/ "code_review.txt";
		}

		Application application(config, config_path);

		MainWindow window(application);
		
		window.show();

		return app.exec();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(
			nullptr,
			"Application error",
			QString::fromUtf8(e.what()));

		return 1;
	}
}