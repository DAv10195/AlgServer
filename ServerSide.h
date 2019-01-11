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
			virtual S solve(const P &problem) = 0;
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
		std::map<P, S> solutions;
		L toLoadFrom;

		public:
			virtual void loadSolutions() = 0;
			virtual void saveSolutions() = 0;
			virtual void addSolution(const P &problem, const S &solution)
			{
				this->solutions[problem] = solution;
			}
			virtual bool ifExistingSolution(const P &problem)
			{	//case no solution cached...
				if ((this->solutions)->find(problem) == (this->solutions)->end())
				{
					return false;
				}
				//solution was already cached...
				return true;
			}
			virtual S getSolution(const P &problem)
			{	//if for some obscure reason a dumb programmer didn't check for existence before usage...
				if (!this->ifExistingSolution(problem))
				{
					throw new std::runtime_error("Solution doesn't exist");
				}

				return this->solutions[problem];
			}
			virtual ~CacheManager(){};
	};
	//the two following interfaces are streams for handling sockets
	template<typename S>
	class SockOutStream
	{
		int sock;

		public:
			SockOutStream(int s) { this->sock = s; };
			virtual bool operator<<(const S &solution) = 0;
			virtual ~SockOutStream(){};
	};
	template<typename P>
	class SockInStream
	{
		int sock;

		public:
			SockInStream(int s) { this->sock = s; };
			virtual bool operator>>(P &problem) = 0;
			virtual ~SockInStream(){};
	};
	//ClientHandler interface
	class ClientHandler
	{
		public:
			template<typename P>
			virtual void handleClients(SockInStream<P>* stream) = 0;
			virtual ~ClientHandler(){};
	};
	//Server interface
	class Server
	{
		public:
			virtual bool open(int port, ClientHandler* ch) = 0;
			virtual ~Server(){};
	};
}
#endif
