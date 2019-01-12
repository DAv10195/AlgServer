//AlgClientHandler implementation
#include "AlgServer.h"
#define WAIT 0
#define SUCCESS 1
#define FAIL 2
#define ACCEPT 5
//constructor
AlgClientHandler :: AlgClientHandler(pthread_mutex_t* hl, pthread_t* ht)
{
	this->handleLock = hl;
	unsigned int i = 0;
	for (; i < HANDLERS; ++i)
	{
		this->handleThreads[i] = &ht[i];
		this->solvers[i] = nullptr;
		this->currReq[i] = nullptr;
		this->wait[i] = WAIT;
		this->ifCreated[i] = false;
	}
	this->acceptThread = &ht[ACCEPT];
	std::string path = "cache.txt";
	this->manager = new FileCacheManager(path);
	this->tmpSolver = nullptr;
}
//setting up 5 handler threads
bool AlgClientHandler :: setUpHandlers(pthread_mutex_t* runLock, bool* ir)
{
	bool flag = false;
	unsigned int i = 0;
	for (; i < HANDLERS; ++i)
	{	//construct thread params
		handleTrdParams* p = new handleTrdParams();
		p->establishment = &(this->wait[i]);
		p->ifRun = ir;
		p->lock = this->handleLock;
		p->runLock = runLock;
		p->solver = this->solvers[i];
		p->toHandle = &(this->currReq[i]);
		p->manager = this->manager;
		//start thread and handle case of failure
		if (pthread_create(this->handleThreads[i], NULL, &handleThread, (void*)p))
		{
			std::cout << "Threading Error" << std::endl;
			delete p;
			flag = true;
			break;
		}
		this->ifCreated[i] = true;
	}
	//case failure...
	if (flag)
	{
		i = 0;
		for (; i < HANDLERS; ++i)
		{	//"kill" already created threads
			if (this->ifCreated[i])
			{
				pthread_mutex_lock(this->handleLock);

				this->wait[i] = FAIL;

				pthread_mutex_unlock(this->handleLock);

				pthread_join(*(this->handleThreads[i]), NULL);
			}
		}
		return false;
	}
	i = 0;
	//all good!
	for (; i < HANDLERS; ++i)
	{
		pthread_mutex_lock(this->handleLock);

		this->wait[i] = SUCCESS;

		pthread_mutex_unlock(this->handleLock);
	}
	return true;
}
//handling method
void AlgClientHandler :: handleClients()
{
	bool empty = false;
	bool flag = false;
	Request* req = nullptr;
	unsigned int i = 0;

	pthread_mutex_lock(this->handleLock);

	empty = (this->requests).empty();

	pthread_mutex_unlock(this->handleLock);

	if (!empty)
	{
		pthread_mutex_lock(this->handleLock);

		req = (this->requests).front();
		(this->requests).pop();

		pthread_mutex_unlock(this->handleLock);
		//look for an available handler to handle request
		while (!flag)
		{
			for (; i < HANDLERS; ++i)
			{
				pthread_mutex_lock(this->handleLock);

				if (this->currReq[i] == nullptr)
				{
					this->currReq[i] = req;
					flag = true;
				}

				pthread_mutex_unlock(this->handleLock);
			}
			i = 0;
		}
	}
}
//join all handler threads
void AlgClientHandler :: stopHandlers()
{
	unsigned int i = 0;
	Request* tmp = nullptr;
	std::string solution = "";
	for (; i < HANDLERS; ++i)
	{
		pthread_join(*(this->handleThreads[i]), NULL);
	}
	//case there are still pending requests we will handle them with our backup tmpSolver
	//no mutex locking needed because by now the reading and handling threads are "dead"
	while (!(this->requests).empty())
	{
		tmp = (this->requests).front();
		(this->requests).pop();
		solution = (this->tmpSolver)->solve(tmp->graph);
		(tmp->stream)->write(solution);
		(this->manager)->addSolution(tmp->toStr, solution);
		delete tmp->graph;
		delete tmp->stream;
		delete tmp;
	}

}
//share client queue with data reading thread
 std::queue<Request*>* AlgClientHandler :: shareClientQueue()
 {
	 return &(this->requests);
 }
 //share FileCacheManager with significant others
 FileCacheManager* AlgClientHandler :: shareCache()
 {
	 return this->manager;
 }
 //share handleLock
 pthread_mutex_t* AlgClientHandler :: shareLock()
 {
	 return this->handleLock;
 }
 //return thread intended for acceptThread
 pthread_t* AlgClientHandler :: getAcceptThread()
 {
	 return this->acceptThread;
 }
//destructor
AlgClientHandler :: ~AlgClientHandler()
{
	unsigned int i = 0;
	for (; i < HANDLERS; ++i)
	{
		delete this->solvers[i];
	}
	delete this->tmpSolver;
	(this->manager)->saveSolutions();
	delete this->manager;
}
