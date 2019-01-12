//Thread functions we are using in our implementation
#include "AlgServer.h"
#define WAIT 0
#define SUCCESS 1
#define FAIL 2
//clientHandling Thread
void* clientHandlerThread(void* args)
{	//extract params...
	cliTrdParams* p = (cliTrdParams*)args;
	AlgClientHandler* handler = p->handler;
	bool run = *(p->ifRun);
	int* status = p->status;
	pthread_mutex_t* lock = p->lock;
	//all handlers get ready!
	if (!handler->setUpHandlers(lock, p->ifRun))
	{
		pthread_mutex_lock(lock);
		//inform server of failure
		*status = FAIL;

		pthread_mutex_unlock(lock);

		delete p;
		return NULL;
	}
	pthread_t* at = handler->getAcceptThread();
	//build params for thread...
	acceptTrdParams* pr = new acceptTrdParams();
	pr->ifRun = p->ifRun;
	pr->lock = handler->shareLock();
	pr->runLock = lock;
	pr->manager = handler->shareCache();
	pr->requests = handler->shareClientQueue();
	//start listening and handling requests
	if (pthread_create(at, NULL, &acceptThread, (void*)pr))
	{
		std::cout << "Threading Error" << std::endl;
		delete p;
		delete pr;

		pthread_mutex_lock(lock);
		//handler threads are already in the air...
		*(p->ifRun) = false;

		pthread_mutex_unlock(lock);

		handler->stopHandlers();

		pthread_mutex_lock(lock);
		//inform server of failure
		*status = FAIL;

		pthread_mutex_unlock(lock);

		return NULL;
	}
	//all good so far!
	pthread_mutex_lock(lock);
	//inform server of success
	*status = SUCCESS;

	pthread_mutex_unlock(lock);

	while (run)
	{
		handler->handleClients();

		pthread_mutex_lock(lock);

		run = *(p->ifRun);

		pthread_mutex_unlock(lock);
	}
	pthread_join(*at, NULL);
	handler->stopHandlers();
	delete p;
	return NULL;
}
//thread responsible for welcoming incoming requests
void* acceptThread(void* args)
{
	return NULL;
}
//thread(s) responsible for handling a given client request
void* handleThread(void* args)
{	//extract some params...
	handleTrdParams* p = (handleTrdParams*)args;
	pthread_mutex_t* lock = p->lock;
	int* establishment = p->establishment;
	int estabState = WAIT;
	//wait for other resources to be established...
	while (estabState == WAIT)
	{
		pthread_mutex_lock(lock);

		estabState = *establishment;

		pthread_mutex_unlock(lock);
	}
	//case failed establishment
	if (estabState == FAIL)
	{
		delete p;
		return NULL;
	}
	//extract rest of params..
	pthread_mutex_t* runLock = p->runLock;
	bool run = *(p->ifRun);
	MySearchSolver* solver = p->solver;
	Request** toHandle = p->toHandle;
	FileCacheManager* manager = p->manager;
	Request* currReq = nullptr;
	MatrixGraph* mg = nullptr;
	SockOutStream* stream = nullptr;
	std::string solution = "";
	//start handling the requests the ClientHandler delegates to you
	while (run)
	{
		pthread_mutex_lock(lock);

		currReq = *toHandle;

		pthread_mutex_unlock(lock);
		//a request was inputed by ClientHandler
		if (currReq != nullptr)
		{
			mg = currReq->graph;
			stream = currReq->stream;
			solution = solver->solve(mg);
			stream->write(solution);

			pthread_mutex_lock(lock);
			//save solution
			manager->addSolution(currReq->toStr, solution);

			pthread_mutex_unlock(lock);

			delete mg;
			delete stream;
			delete currReq;
			currReq = nullptr;

			pthread_mutex_lock(lock);

			*toHandle = nullptr;

			pthread_mutex_unlock(lock);
		}

		pthread_mutex_lock(runLock);

		run = *(p->ifRun);

		pthread_mutex_unlock(runLock);
	}
	delete p;
	return NULL;
}




