//AlgServer Implementation
#include "AlgServer.h"
#define WAIT 0
#define SUCCESS 1
#define FAIL 2
//constructor
MyParallelServer :: MyParallelServer()
{
	this->lock = new pthread_mutex_t();
	this->clieTrd = new pthread_t();
	this->ifRun = true;
	this->ifStop = false;
	this->ifOpen = false;
}
//open method
bool MyParallelServer :: open(int port, ClientHandler* ch)
{
	if (pthread_mutex_init(this->lock, NULL))
	{
		std::cout << "mutex initialization error" << std::endl;
		return false;
	}
	this->ifOpen = true;
	//build params for thread...
	cliTrdParams* p = new cliTrdParams();
	p->ifRun = &(this->ifRun);
	p->lock = this->lock;
	p->handler = ch;
	p->port = port;
	int status = WAIT;
	int currStat = status;
	p->status = &status;
	//start the thread and handle case of failure
	if (pthread_create(this->clieTrd, NULL, &clientHandlerThread, (void*)p))
	{
		delete p;
		std::cout << "Threading Error" << std::endl;
		return false;
	}
	//wait for all relevant resources to open
	while (currStat == WAIT)
	{
		pthread_mutex_lock(this->lock);

		currStat = status;

		pthread_mutex_unlock(this->lock);
	}
	//case failure occured
	if (currStat == FAIL)
	{
		pthread_join(*(this->clieTrd), NULL);
		return false;
	}
	//all good!
	return true;
}
//stop method
void MyParallelServer :: stop()
{
	if (this->ifOpen)
	{
		this->ifStop = true;

		pthread_mutex_lock(this->lock);

		this->ifRun = false;

		pthread_mutex_unlock(this->lock);

		pthread_join(*(this->clieTrd), NULL);
		pthread_mutex_destroy(this->lock);
	}
}
//destructor
MyParallelServer :: ~MyParallelServer()
{	//case stop method wasn't called...
	if (!this->ifStop)
	{
		this->stop();
	}
	delete this->clieTrd;
	delete this->lock;
}
