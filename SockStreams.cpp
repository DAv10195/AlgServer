//SockStreams implementation
#include "AlgServer.h"
#include <string.h>
#include <unistd.h>
//write data to client
bool SockOutStream :: writeToStream(std::string message)
{
	int status = 0;
	bzero(this->buffer, BUFFER_SIZE);
	strcpy(this->buffer, message.c_str());
	//send message to client
	status = write(this->sock, this->buffer, strlen(this->buffer));
	if (status < 0)
	{
		std::cout << "Error writing to client" << std::endl;
		return false;
	}
	return true;
}
//read data from client, an empty string indicates a reading error
std::string SockInStream :: readFromStream()
{
	int status = 0;
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
		line = std::string(this->buffer);
		if (line == "end")
		{
			flag = true;
			continue;
		}
		toRet += line;
		//'$' indicates end of line
		toRet += "$";
	}
	return toRet;
}
