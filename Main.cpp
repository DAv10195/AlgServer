#include "AlgServer.h"

int main(int argc, char* argv[])
{
	int port = 0;
	if (argc == 1)
	{
		std::cout << "Not enough paramaters passed (give me a port!)" << std::endl;
		return 1;
	}
	try
	{
		port = std::stoi(argv[1]);
	}
	catch (std::invalid_argument &e)
	{
		std::cout << "Invalid argument passed as port" << std::endl;
		return 1;
	}
	Server* s = new MyParallelServer();
	ClientHandler* c = new MyClientHandler();
	std::string inp = "";
	if (!s->open(port, c))
	{
		delete c;
		delete s;
		return 1;
	}
	while (inp != "stop")
	{
		std::cin >> inp;
	}
	s->stop();
	delete c;
	delete s;
	return 0;
}
