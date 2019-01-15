//AlgClientHandler implementation
#include "AlgServer.h"
#define WAIT 0
#define SUCCESS 1
#define FAIL 2
//constructor
MyClientHandler :: MyClientHandler()
{
	this->handleLock = new pthread_mutex_t();
	unsigned int i = 0;
	for (; i < HANDLERS; ++i)
	{
		this->handleThreads[i] = new pthread_t();
		this->creators[i] = new GraphCreator();
		this->solvers[i] = new MySearchSolver(new MyAstar());
		this->currReq[i] = nullptr;
		this->wait[i] = WAIT;
		this->ifCreated[i] = false;
		this->ifRun[i] = true;
	}
	std::string path = "cache.txt";
	this->manager = new FileCacheManager(path);
}
//setting up 5 handler threads
bool MyClientHandler :: setUp()
{
	bool flag = false;
	unsigned int i = 0;
	(this->manager)->loadSolutions();
	if (pthread_mutex_init(this->handleLock, NULL))
	{
		std::cout << "mutex initialization error" << std::endl;
		return false;
	}
	for (; i < HANDLERS; ++i)
	{	//construct thread params
		handleTrdParams* p = new handleTrdParams();
		p->establishment = &(this->wait[i]);
		p->ifRun = &(this->ifRun[i]);
		p->lock = this->handleLock;
		p->creator = this->creators[i];
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
void MyClientHandler :: handleClient(InputStream* in, OutputStream* out)
{
	bool flag = false;
	Request* req = new Request();
	req->in = in;
	req->out = out;
	unsigned int i = 0;
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

				pthread_mutex_unlock(this->handleLock);

				break;
			}

			pthread_mutex_unlock(this->handleLock);
		}
		i = 0;
	}
}
//join all handler threads
void MyClientHandler :: stop()
{
	unsigned int i = 0;
	for (; i < HANDLERS; ++i)
	{
		pthread_mutex_lock(this->handleLock);

		this->ifRun[i] = false;

		pthread_mutex_unlock(this->handleLock);

		pthread_join(*(this->handleThreads[i]), NULL);
	}
	pthread_mutex_destroy(this->handleLock);
	(this->manager)->saveSolutions();
}
//destructor
MyClientHandler :: ~MyClientHandler()
{
	unsigned int i = 0;
	for (; i < HANDLERS; ++i)
	{
		delete this->handleThreads[i];
		delete this->creators[i];
		delete this->solvers[i];
	}
	delete this->manager;
	delete this->handleLock;
}
