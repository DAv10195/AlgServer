//Header file for our specific AlgServer
#ifndef ALGSERVER_H_
#define ALGSERVER_H_

#include "ServerSide.h"
#include "MySearch.h"
#include <fstream>
#include <string>
#include <pthread.h>
#define HANDLERS 5

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
		bool read(std::string &problem);
		virtual ~AlgSockInStream(){};
};
//a SockOutStream suitable for our needs
class AlgSockOutStream : public SockOutStream<std::string>
{
	public:
		AlgSockOutStream(int s) : SockOutStream<std::string>(s){}
		virtual bool write(std::string &solution);
		virtual ~AlgSockOutStream(){};
};
//an object adapter suitable for our needs
class MySearchSolver : Solver<Searchable<std::string, Node*>, std::string>
{
	Searcher<std::string, Node*, std::string>* searcher;

	public:
		MySearchSolver(Searcher<std::string, Node*, std::string>* s);
		virtual std::string solve(MatrixGraph* mg);
		~MySearchSolver();
};
//a ClientHandler suitable for our needs
class AlgClientHandler : public ClientHandler
{
	FileCacheManager* manager;
	MySearchSolver* solvers[HANDLERS];
	pthread_t* threads;
	pthread_mutex_t* lock;
};

#endif
