//Header File for Server and other related tools
#ifndef SERVERSIDE_H_
#define SERVERSIDE_H_

#include <map>
//ServerSide namespace for handling various Servers
//P - type of problem to be solved, S - type of solution
namespace ServerSide
{	//Solver interface
	template<typename P, typename S>
	class Solver
	{
		public:
			virtual S solve(P &problem) = 0;
			virtual ~Solver(){};
	};
	//CacheManager abstract class, L - where to load solutions from (a file stream for example...)
	template<typename P, typename S, typename L>
	class CacheManager
	{
		/*
		 * in case you wish to use a type which isn't a standard one (primitives, pointers, language standard objects, etc..)
		 * you should Hash the key object you want to use via your own Hash function, and insert the Hashed object as a key
		 */
		protected:
			std::map<P, S> solutions;
			L toLoadAndSaveFrom;

		public:
			virtual void loadSolutions() = 0;
			virtual void saveSolutions() = 0;
			virtual void addSolution(P &problem, S &solution)
			{
				if ((this->solutions).find(problem) == (this->solutions).end())
				{	//only unsolved problems will be added...
					this->solutions[problem] = solution;
				}
			}
			virtual bool ifExistingSolution(P &problem)
			{	//case no solution cached...
				if ((this->solutions).find(problem) == (this->solutions).end())
				{
					return false;
				}
				//solution was already cached...
				return true;
			}
			virtual S getSolution(P &problem)
			{	//if for some obscure reason a dumb programmer didn't check for existence before usage...
				if (!this->ifExistingSolution(problem))
				{
					throw new std::runtime_error("Solution doesn't exist");
				}

				return this->solutions[problem];
			}
			virtual ~CacheManager(){};
	};
	//InputStream for ClientHandler
	class InputStream
	{
		public:
			virtual std::string readFromStream() = 0;
			virtual ~InputStream(){};
	};
	//OutputStream for ClientHandler
	class OutputStream
	{
		public:
			virtual bool writeToStream(std::string message) = 0;
			virtual ~OutputStream(){};
	};
	//ClientHandler interface
	class ClientHandler
	{
		public:
			virtual bool setUp() = 0;
			virtual void handleClient(InputStream* in, OutputStream* out) = 0;
			virtual void stop() = 0;
			virtual ~ClientHandler(){};
	};
	//Server interface
	class Server
	{
		public:
			virtual bool open(int port, ClientHandler* ch) = 0;
			virtual void stop() = 0;
			virtual ~Server(){};
	};
}

#endif
