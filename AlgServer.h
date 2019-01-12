//Header file for our specific AlgServer
#ifndef ALGSERVER_H_
#define ALGSERVER_H_

#include "ServerSide.h"
#include "MySearch.h"
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <pthread.h>
#define HANDLERS 5

using namespace ServerSide;
//socket output stream
class SockOutStream
{
	int sock;

	public:
		SockOutStream(int s){ this->sock = s; };
		virtual void write(std::string);
		virtual ~SockOutStream(){};
};
//client request data
struct Request
{
	MatrixGraph* graph;
	SockOutStream* stream;
	std::string toStr;
};
//socket input stream
class SockInStream
{
	int sock;

	public:
		SockInStream(){ this->sock = 0; };
		virtual void setSock(int s) { this->sock = s; }
		virtual Request* read();
		virtual ~SockInStream(){};
};
typedef struct Request Request;
//an object adapter suitable for our needs
class MySearchSolver : Solver<Searchable<std::string, Node*>, std::string>
{
	Searcher<std::string, Node*, std::string>* searcher;

	public:
		MySearchSolver(Searcher<std::string, Node*, std::string>* s) { this->searcher = s; };
		virtual std::string solve(Searchable<std::string, Node*>* mg) { return (this->searcher)->search(mg); };
		~MySearchSolver() { delete this->searcher; };
};
//a File cache manager
class FileCacheManager : public CacheManager<std::string, std::string, std::string>
{
	public:
		FileCacheManager(std::string &path);
		virtual void loadSolutions();
		virtual void saveSolutions();
		virtual ~FileCacheManager(){};
};
//handler threads params
struct handleTrdParams
{
	pthread_mutex_t* lock;
	pthread_mutex_t* runLock;
	Request** toHandle;
	MySearchSolver* solver;
	FileCacheManager* manager;
	bool* ifRun;
	int* establishment;
};
typedef struct handleTrdParams handleTrdParams;
//acceptThread params
struct acceptTrdParams
{
	pthread_mutex_t* lock;
	pthread_mutex_t* runLock;
	std::queue<Request*>* requests;
	FileCacheManager* manager;
	bool* ifRun;
	bool* ifError;
};
typedef struct acceptTrdParams acceptTrdParams;
//a ClientHandler suitable for our needs
class AlgClientHandler : public ClientHandler
{
	FileCacheManager* manager;
	MySearchSolver* solvers[HANDLERS];
	MySearchSolver* tmpSolver;
	pthread_t* handleThreads[HANDLERS];
	pthread_t* acceptThread;
	Request* currReq[HANDLERS];
	int wait[HANDLERS];
	bool ifCreated[HANDLERS];
	std::queue<Request*> requests;
	pthread_mutex_t* handleLock;

	public:
		AlgClientHandler(pthread_mutex_t* hl, pthread_t* ht);
		virtual std::queue<Request*>* shareClientQueue();
		virtual FileCacheManager* shareCache();
		virtual pthread_mutex_t* shareLock();
		virtual pthread_t* getAcceptThread();
		virtual void handleClients();
		virtual bool setUpHandlers(pthread_mutex_t* runLock, bool* ir);
		virtual void stopHandlers();
		virtual ~AlgClientHandler();
};
//clientHandlerThread params
struct cliTrdParams
{
	pthread_mutex_t* lock;
	AlgClientHandler* handler;
	bool* ifRun;
	int* status;
};
typedef struct cliTrdParams cliTrdParams;
//a server suitable for our needs
class AlgServer : public Server
{
	pthread_t* clieTrd;
	pthread_mutex_t* lock;
	bool ifRun;

	public:
		AlgServer(pthread_t* t, pthread_mutex_t* l);
		virtual bool open(int port, AlgClientHandler* ch);
		virtual void stop();
		virtual ~AlgServer(){};
};
//thread responsible for managing requests from clients
void* clientHandlerThread(void* args);
//thread responsible for handling a specific client at a given moment
void* handleThread(void* args);
//thread responsible for reading data from clients
void* acceptThread(void* args);

#endif
