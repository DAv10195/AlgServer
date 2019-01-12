//AlgServer Implementation
#include "AlgServer.h"
#define WAIT 0
#define SUCCESS 1
#define FAIL 2
//constructor
AlgServer :: AlgServer(pthread_t* t, pthread_mutex_t* l)
{
	this->lock = l;
	this->clieTrd = t;
	this->ifRun = true;
}
//open method
bool AlgServer :: open(int port, AlgClientHandler* ch)
{	//build params for thread...
	cliTrdParams* p = new cliTrdParams();
	p->ifRun = &(this->ifRun);
	p->lock = this->lock;
	p->handler = ch;
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
void AlgServer :: stop()
{
	pthread_mutex_lock(this->lock);

	this->ifRun = false;

	pthread_mutex_unlock(this->lock);

	pthread_join(*(this->clieTrd), NULL);
}
