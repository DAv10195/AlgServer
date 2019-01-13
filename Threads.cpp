//Thread functions we are using in our implementation
#include "AlgServer.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <string.h>
#define WAIT 0
#define SUCCESS 1
#define FAIL 2
#define WALL -1
#define MAX_CONNECTIONS 5
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
		{
			std::cout << "Error accepting client connection" << std::endl;
		}

		handler->handleClient(new SockInStream(currSock), new SockOutStream(currSock));

		pthread_mutex_lock(lock);

		run = *(p->ifRun);

		pthread_mutex_unlock(lock);
	}
	handler->stop();
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
	MySearchSolver* solver = p->solver;
	Request** toHandle = p->toHandle;
	FileCacheManager* manager = p->manager;
	Request* currReq = nullptr;
	InputStream* in = nullptr;
	OutputStream* out = nullptr;
	MatrixGraph* mg = nullptr;
	std::string solution = "", req = "", row = "", num = "";
	std::vector<std::string> toParse;
	unsigned int rows = 0, col = 0, i = 0, j = 0, k = 0, size = 0, max = 0;
	bool valid = true, existingSolution = false;
	double** costs = nullptr;
	Position start, goal;
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
			//case a reading error occured
			if (req == "")
			{
				delete currReq->in;
				delete currReq->out;
				delete currReq;
				currReq = nullptr;
				continue;
			}
			size = req.size();
			//get number of rows - '$' sign means end of row, with the two last '$' signs indicating indexes of start and goal
			for(i = 0; i < size; ++i)
			{
				if (req.at(i) == '$')
				{
					++rows;
				}
			}
			--rows;
			--rows;
			i = 0;
			while (i < size && req.at(i) != '$')
			{	//each row has costs separated by ',' and the amount of costs is the amount of ',' + 1
				//for example: "1,2,3,4" has 4 costs and 3 ','
				if (req.at(i) == ',')
				{
					++col;
				}
				++i;
			}
			++col;
			//in any case, we will build a NxN matrix
			max = std::max(rows, col);
			//allocation of a NxN matrix of double cost values
			costs = new double*[max];
			for (i = 0; j < max; ++i)
			{
				costs[i] = new double[max];
				for (j = 0; j < max; ++j)
				{
					costs[i][j] = WALL;
				}
			}
			i = 0;
			j = 0;
			//build vector of rows inputed
			while (j < size && i < rows)
			{
				while (j < size && req.at(j) != '$')
				{
					row.push_back(req.at(j));
					++j;
				}
				toParse.push_back(row);
				row = "";
				++j;
				++i;
			}
			try
			{
				//j now pointing on start entry
				num = "";
				while (j < size && req.at(j) != ',')
				{
					num.push_back(req.at(j));
					++j;
				}
				++j;	//casting to int because stoi returns int and rows and col are >=0 (thus should be unsigned...)
				if (stoi(num) < 0 || stoi(num) > (int)rows)
				{	//out of bounds
					throw std::runtime_error("Invalid Input");
				}
				start.i = stoi(num);
				num = "";
				//',' between indexes
				while (j < size && req.at(j) != '$')
				{
					num.push_back(req.at(j));
					++j;
				}
				++j;	//casting to int because stoi returns int and rows and col are >=0 (thus should be unsigned...)
				if (stoi(num) < 0 || stoi(num) > (int)col)
				{	//out of bounds
					throw std::runtime_error("Invalid Input");
				}
				start.j = stoi(num);
				num = "";
				//j now pointing on goal entry
				while (j < size && req.at(j) != ',')
				{
					num.push_back(req.at(j));
					++j;
				}
				++j;	//casting to int because stoi returns int and rows and col are >=0 (thus should be unsigned...)
				if (stoi(num) < 0 || stoi(num) > (int)rows)
				{	//out of bounds
					throw std::runtime_error("Invalid Input");
				}
				goal.i = stoi(num);
				num = "";
				//',' between indexes
				while (j < size)
				{
					num.push_back(req.at(j));
					++j;
				}	//casting to int because stoi returns int and rows and col are >=0 (thus should be unsigned...)
				if (stoi(num) < 0 || stoi(num) > (int)col)
				{	//out of bounds
					throw std::runtime_error("Invalid Input");
				}
				goal.j = stoi(num);
				//parse costs inputed
				size = toParse.size();
				j = 0;
				k = 0;
				//parse input
				for (i = 0; i < size; ++i)
				{
					row = toParse[i];
					while (k < row.size() && j < col)
					{
						if (row.at(k) == ',')
						{
							++k;
							costs[i][j] = stod(num);
							//zero cost is invalid
							if (!costs[i][j])
							{
								throw std::runtime_error("Invalid Input");
							}
							++j;
							num = "";
							continue;
						}
						num.push_back(row.at(k));
						++k;
					}
					k = 0;
					j = 0;
				}
				//can't start from a none existing node...
				if (costs[start.i][start.j] < 0)
				{
					throw std::runtime_error("Invalid Input");
				}
				//can't go to a none existing node...
				if (costs[goal.i][goal.j] < 0)
				{
					throw std::runtime_error("Invalid Input");
				}
			}
			catch (std::runtime_error &e)
			{
				valid = false;
			}//case parsing failed for invalid input that made stod throw an exception
			if (!valid)
			{
				solution = "Invalid Input!";
				out->writeToStream(solution);
				//delete resources...
				for (i = 0; i < max; ++i)
				{
					delete costs[i];
				}
				delete costs;
				delete currReq->in;
				delete currReq->out;
				delete currReq;
				currReq = nullptr;
				//back to handling state...
				valid = true;
				i = 0;
				j = 0;
				k = 0;
				rows = 0;
				col = 0;
				row = "";
				num = "";
				toParse.clear();
			}
			else	//all good!
			{
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
				else
				{	//not existing solution so solve and save
					mg = new MatrixGraph(start, goal, costs, max);
					solution = solver->solve(mg);
					out->writeToStream(solution);
					//delete resources...
					delete mg;
					mg  = nullptr;
					for (i = 0; i < max; ++i)
					{
						delete costs[i];
					}
					delete costs;
					delete currReq->in;
					delete currReq->out;
					delete currReq;
					currReq = nullptr;
					//back to handling state...
					i = 0;
					j = 0;
					k = 0;
					rows = 0;
					col = 0;
					row = "";
					num = "";
					toParse.clear();
					//save solution
					pthread_mutex_lock(lock);

					manager->addSolution(req, solution);

					pthread_mutex_unlock(lock);
				}
			}
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
