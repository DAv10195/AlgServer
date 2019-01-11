//Header file for our specific AlgServer
#ifndef ALGSERVER_H_
#define ALGSERVER_H_

#include "ServerSide.h"
#include "Search.h"
#include <fstream>
#include <string>

using namespace ServerSide;
//a File cache manager
class FileCacheManager : public CacheManager<std::string, std::string, std::ifstream>
{
	public:
		FileCacheManager(std::string &path);
		virtual void loadSolutions();
		virtual void saveSolutions();
		virtual ~FileCacheManager(){};
};
//a SockInStream suitable for our needs
class AlgSockInStream : public SockInStream<std::string>
{
	public:
		AlgSockInStream(int s) : SockInStream<std::string>(s){}
		bool operator>>(std::string &problem);
		virtual ~AlgSockInStream(){};
};
//a SockOutStream suitable for our needs
class AlgSockOutStream : public SockOutStream<std::string>
{
	public:
		AlgSockOutStream(int s) : SockOutStream<std::string>(s){}
		virtual bool operator<<(const std::string &solution);
		virtual ~AlgSockOutStream(){};
};

class AlgClientHandler : public ClientHandler
{
	FileCacheManager* manager;
	Solver<std::string, std::string>* solver;


};
#endif
