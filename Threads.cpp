//Thread functions we are using in our implementation
#include "AlgServer.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define WAIT 0
#define SUCCESS 1
#define FAIL 2
#define MAX_CONNECTIONS 5
#define TIMEOUT 1
//clientHandling Thread
void* clientHandlerThread(void* args)
{	//extract params...
	cliTrdParams* p = (cliTrdParams*)args;
	pthread_mutex_t* lock = p->lock;
	int port = p->port, clilen = 0, sockfd = 0, currSock = 0;
	int* status = p->status;
	//open socket
	//structs for storing the connection information.
	struct sockaddr_in serv_adr, cli_adr;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0)
	{
		std::cout << "Error with initialization of socket" << std::endl;

		pthread_mutex_lock(lock);
		//inform server of failure
		*status = FAIL;

		pthread_mutex_unlock(lock);

		delete p;
		return NULL;
	}
	bzero((char*) &serv_adr,sizeof(serv_adr)); 	//put zeros in
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = INADDR_ANY;
	serv_adr.sin_port = htons(port); 	//initialize port
	//bind the socket with the address.
	if (bind(sockfd, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
	{
		std::cout << "Socket binding failed" << std::endl;

		pthread_mutex_lock(lock);
		//inform server of failure
		*status = FAIL;

		pthread_mutex_unlock(lock);

		delete p;
		return NULL;
	}
	//listen for connection.
	if (listen(sockfd,MAX_CONNECTIONS) < 0)
	{
		std::cout << "Error establishing socket" << std::endl;
		pthread_mutex_lock(lock);
		//inform server of failure
		*status = FAIL;

		pthread_mutex_unlock(lock);

		delete p;
		return NULL;
	}
	//handle the client address in order to establish connection.
	clilen = sizeof(cli_adr);
	//extract rest of params
	ClientHandler* handler = p->handler;
	bool run = *(p->ifRun);
	//all handlers get ready!
	if (!handler->setUp())
	{
		pthread_mutex_lock(lock);
		//inform server of failure
		*status = FAIL;

		pthread_mutex_unlock(lock);

		delete p;
		return NULL;
	}
	//all good so far!
	pthread_mutex_lock(lock);
	//inform server of success
	*status = SUCCESS;

	pthread_mutex_unlock(lock);

	while (run)
	{
		// Accept actual connection from the client.
		currSock = accept(sockfd, (struct sockaddr *)&cli_adr, (socklen_t*)&clilen);
		if (currSock < 0)
		{	//no one so fast...
			std::cout << "Server Timed out" << std::endl;
			handler->stop();
			shutdown(sockfd, SHUT_RDWR);
			close(sockfd);
			delete p;
			return NULL;
		}

		handler->handleClient(new SockInStream(currSock), new SockOutStream(currSock));

		pthread_mutex_lock(lock);

		run = *(p->ifRun);

		pthread_mutex_unlock(lock);
		// stop waiting for infinity...
		fcntl(sockfd,F_SETFL,O_NONBLOCK);
		// wait a bit, give someone a chance of using this fine server
		sleep(TIMEOUT);
	}
	handler->stop();
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	delete p;
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
	bool run = *(p->ifRun);
	GraphCreator* creator = p->creator;
	MySearchSolver* solver = p->solver;
	Request** toHandle = p->toHandle;
	FileCacheManager* manager = p->manager;
	Request* currReq = nullptr;
	InputStream* in = nullptr;
	OutputStream* out = nullptr;
	MatrixGraph* mg = nullptr;
	std::string solution = "", req = "";
	bool existingSolution = false;
	//start handling the requests the ClientHandler delegates to you
	while (run)
	{
		pthread_mutex_lock(lock);

		currReq = *toHandle;

		pthread_mutex_unlock(lock);
		//a request was inputed by ClientHandler
		if (currReq != nullptr)
		{
			in = currReq->in;
			out = currReq->out;
			//construct graph object
			req = in->readFromStream();
			//look if already solved
			pthread_mutex_lock(lock);

			existingSolution = manager->ifExistingSolution(req);

			pthread_mutex_unlock(lock);

			if (existingSolution)
			{
				pthread_mutex_lock(lock);

				solution = manager->getSolution(req);

				pthread_mutex_unlock(lock);

				out->writeToStream(solution);
			}
			else	//solve, send, and save solution
			{
				mg = creator->create(req);
				//case invalid input
				if (mg == nullptr)
				{
					solution = "Invalid Input";
					out->writeToStream(solution);
				}
				else	//all good!
				{
					solution = solver->solve(mg);
					out->writeToStream(solution);
					delete mg;

					pthread_mutex_lock(lock);

					manager->addSolution(req, solution);

					pthread_mutex_unlock(lock);
				}
			}
			delete in;
			delete out;
			delete currReq;
			//finished handling request...
			pthread_mutex_lock(lock);

			*toHandle = nullptr;

			pthread_mutex_unlock(lock);
		}

		pthread_mutex_lock(lock);

		run = *(p->ifRun);

		pthread_mutex_unlock(lock);

	}
	delete p;
	return NULL;
}
