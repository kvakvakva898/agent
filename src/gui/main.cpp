#include <QApplication>

#include "main_window.h"
#include <core/agent_config.h>

int main(
	int argc,
	char* argv[])
{
	QApplication app(
		argc,
		argv);
	AgentConfig config;

	config.endpoint 
		= "http://10.108.91.115:1234/v1/chat/completions";

	config.model 
		= "nvidia/nemotron-3-nano-4b";

	config.prompt_template_path 
		= "prompts/code_review.txt";

	qRegisterMetaType<
		std::vector<ReviewIssue>>(
			"std::vector<ReviewIssue>");

	MainWindow window(config);

	window.show();

	return app.exec();
}