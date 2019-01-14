#include "AlgServer.h"
#include <unistd.h>
int main()
{
	GraphCreator* gc = new GraphCreator();
	std::string str = "1,1,1,1$1,1,1,1$0,0$1,1$";
	MatrixGraph* mg = gc->create(str);
	Server* s = new MyParallelServer();
	ClientHandler* c = new MyClientHandler();
	s->open(5694, c);
	while (true)
	{

	}
	s->stop();
	delete s;
	delete c;
}




