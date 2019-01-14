//SockStreams implementation
#include "AlgServer.h"
#include <string.h>
#include "sys/socket.h"
#include <unistd.h>
//write data to client
bool SockOutStream :: writeToStream(std::string &message)
{
	int status = 0;
	//send message to client
	const char* cMesg = message.c_str();
	status = write(this->sock, cMesg, strlen(cMesg));
	if (status < 0)
	{
		std::cout << "Error writing to client" << std::endl;
		return false;
	}
	return true;
}
//destructor
SockOutStream :: ~SockOutStream()
{
	shutdown(this->sock, SHUT_RDWR);
	close(this->sock);
}
//read data from client, an empty string indicates a reading error
std::string SockInStream :: readFromStream()
{
	int status = 0;
	unsigned int i = 0;
	bool flag = false;
	std::string toRet = "", line = "";
	//start reading
	while (!flag)
	{
		bzero(this->buffer, BUFFER_SIZE);
		status = read(this->sock, this->buffer, BUFFER_SIZE - 1);
		if (status < 0)
		{
			std::cout << "Error reading from client" << std::endl;
			//empty string indicates error
			return "";
		}
		while (this->buffer[i] && this->buffer[i] != '\n')
		{
			line.push_back(this->buffer[i]);
			++i;
		}
		if (line == "end")
		{
			flag = true;
			continue;
		}
		toRet += line;
		//'$' indicates end of line
		toRet += "$";
		line = "";
		i = 0;
	}
	return toRet;
}
