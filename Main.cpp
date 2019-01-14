#include "AlgServer.h"

int main(int argc, char* argv[])
{
	Server* s = new MyParallelServer();
	ClientHandler* c = new MyClientHandler();
	std::string inp = "";
	if (!s->open(5677, c))
	{
		std::cout << "Server opening error" << std::endl;
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
