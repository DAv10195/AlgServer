//MySearch header, for our specific purposes
#ifndef MYSEARCH_H_
#define MYSEARCH_H_

#include "Search.h"
#include <vector>
#include <string>
#include <queue>
using std::priority_queue;
//a pair representing Position at a matrix.
struct Position
{
	unsigned int i;
	unsigned int j;
};
typedef struct Position Position;
//our Node class
class Node
{
	Position pos;
	double cost;
	double algCost;
	double heuristicCost;
	std::vector<Node*> adj;
	Node* cameFrom;
	std::string cameFromDir;

	public:
		Node();
		Node(unsigned int i, unsigned j);
		Node(unsigned int i, unsigned int j, double c);
		Node(unsigned int i, unsigned int j, double c, double hc);
		virtual void addAdjNode(Node* n);
		virtual Position getPosition();
		virtual double getCost();
		virtual double getAlgCost();
		virtual double getHCost();
		virtual void setAlgCost(double ac);
		virtual void setHCost(double hc);
		virtual void setCameFrom(Node* n);
		virtual Node* getCameFrom();
		virtual std::string getCameFromDir();
		virtual std::vector<Node*> getAdj();
		virtual bool equals(Node* n);
		virtual std::string toString();
		virtual std::string pathFromStart();
		virtual ~Node(){};
};
//the Searchable we'll use at our implementation
class MatrixGraph : public Searchable<std::string ,Node*>
{
	std::map<std::string, Node*> nodes;
	Node* start;
	Node* goal;

	virtual void initNodes(double** costs, unsigned int size);

	public:
		MatrixGraph(Position start, Position goal, double** costs, unsigned int size);
		virtual Node* getInitialState();
		virtual Node* getGoalState();
		virtual std::map<std::string ,Node*>* getAll();
		virtual std::vector<Node*> getAdj(Node* n);
		virtual void setInitialState(Node* init);
		virtual ~MatrixGraph();
};

struct MyCmp{
	bool operator() (Node* left, Node* right)
	{
		double leftCost = left->getCost();
		double rightCost = right->getCost();
		return (leftCost > rightCost);
	}
};
typedef struct MyCmp MyCmp;

class MyPriorQueue : public priority_queue <Node* ,std::vector<Node*>, MyCmp>
{
	public:
		bool contain(Node* node);
		void remove(Node* nodeToRemove);
};

class MyBFS : public Searcher<std::string, Node*, std::string>
{
	unsigned int visits;
	public:
		MyBFS(){ this->visits = 0; };
		virtual void increaseNumOfVisits(){ ++(this->visits); }
		virtual std::string search(Searchable<std::string, Node*>* mg);
		virtual unsigned int getNumOfVisits() { return this->visits; };
		virtual ~MyBFS(){};
};

class MyDFS : public Searcher<std::string, Node*, std::string>
{
	unsigned int visits;

	public:
		MyDFS();
		virtual void increaseNumOfVisits(){ ++(this->visits); }
		virtual std::string search(Searchable<std::string, Node*>* mg);
		virtual unsigned int getNumOfVisits() { return this->visits; };
		virtual ~MyDFS(){};
};

class MyBestFirstSearch : public Searcher<std::string, Node*, std::string>
{

	MyPriorQueue queue;
	unsigned int visits;

	public:
		MyBestFirstSearch(){ this->visits = 0; };
		virtual std::string search(Searchable<std::string, Node*>* mg);
		virtual unsigned int getNumOfVisits() { return this->visits; };
		virtual void increaseNumOfVisits(){ ++(this->visits); }
		virtual ~MyBestFirstSearch(){};

	private:
		virtual void addToQueue(Node* node);
		virtual int getQueueSize();
		virtual void removeFromQueue(Node * node);
		virtual bool isContain(Node * node);
		virtual Node* extractMin();
};

class MyAstar : public Searcher<std::string, Node*, std::string>
{
	unsigned int visits;

	public:
		MyAstar();
		virtual std::string search(MatrixGraph* mg);
		virtual unsigned int getNumOfVisits() { return this->visits; };
		virtual ~MyAstar(){};
};

#endif
