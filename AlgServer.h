//Header file for our specific AlgServer
#ifndef ALGSERVER_H_
#define ALGSERVER_H_

#include "ServerSide.h"
#include "MySearch.h"
#include <fstream>
#include <iostream>
#include <string>
#include <pthread.h>

#define HANDLERS 1
#define BUFFER_SIZE 1024

using namespace ServerSide;
//socket output stream suitable for our needs
class SockOutStream : public OutputStream
{
	int sock;
	char buffer[BUFFER_SIZE];

	public:
		SockOutStream(int s){ this->sock = s; };
		virtual bool writeToStream(std::string &message);
		virtual ~SockOutStream();
};
//socket input stream suitable for our needs
class SockInStream : public InputStream
{
	int sock;
	char buffer[BUFFER_SIZE];

	public:
		SockInStream(int s){ this->sock = s; };
		virtual std::string readFromStream();
		virtual ~SockInStream(){};
};
//client request data
struct Request
{
	InputStream* in;
	OutputStream* out;
};
typedef struct Request Request;
//an object adapter suitable for our needs
class MySearchSolver : public Solver<Searchable<std::string, Node*>*, std::string>
{
	Searcher<std::string, Node*, std::string>* searcher;

	public:
		MySearchSolver(Searcher<std::string, Node*, std::string>* s) { this->searcher = s; };
		virtual std::string solve(Searchable<std::string, Node*>* problem) { return (this->searcher)->search(problem); };
		virtual ~MySearchSolver() { delete this->searcher; };
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
	Request** toHandle;
	GraphCreator* creator;
	MySearchSolver* solver;
	FileCacheManager* manager;
	bool* ifRun;
	int* establishment;
};
typedef struct handleTrdParams handleTrdParams;
//a ClientHandler suitable for our needs
class MyClientHandler : public ClientHandler
{
	FileCacheManager* manager;
	GraphCreator* creators[HANDLERS];
	MySearchSolver* solvers[HANDLERS];
	pthread_t* handleThreads[HANDLERS];
	Request* currReq[HANDLERS];
	int wait[HANDLERS];
	bool ifRun[HANDLERS];
	bool ifCreated[HANDLERS];
	pthread_mutex_t* handleLock;

	public:
		MyClientHandler();
		virtual void handleClient(InputStream* in, OutputStream* out);
		virtual bool setUp();
		virtual void stop();
		virtual ~MyClientHandler();
};
//clientHandlerThread params
struct cliTrdParams
{
	pthread_mutex_t* lock;
	ClientHandler* handler;
	bool* ifRun;
	int* status;
	int port;
};
typedef struct cliTrdParams cliTrdParams;
//a server suitable for our needs
class MyParallelServer : public Server
{
	pthread_t* clieTrd;
	pthread_mutex_t* lock;
	bool ifRun;

	public:
		MyParallelServer();
		virtual bool open(int port, ClientHandler* ch);
		virtual void stop();
		virtual ~MyParallelServer();
};
//thread responsible for managing requests from clients
void* clientHandlerThread(void* args);
//thread responsible for handling a specific client at a given moment
void* handleThread(void* args);
//thread responsible for reading data from clients

#endif
