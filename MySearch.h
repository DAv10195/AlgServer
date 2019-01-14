//MySearch header, for our specific purposes
#ifndef MYSEARCH_H_
#define MYSEARCH_H_

#include "Search.h"
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
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
	bool isVisited;

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
		virtual void  setIsVisited() { this->isVisited = true; }
		virtual bool  getIsVisted() { return this->isVisited; }
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
		virtual ~MatrixGraph();
};
//an object Parsing input into a MatrixGraph
class GraphCreator
{
	public:
		GraphCreator(){};
		virtual MatrixGraph* create(std::string &s);
		virtual ~GraphCreator(){};
};
//Comperator to maintaine a min heap.
struct MyCmp{
	bool operator() (Node* left, Node* right)
	{
		double leftCost = left->getAlgCost();
		double rightCost = right->getAlgCost();
		return (leftCost > rightCost);
	}
};

typedef struct MyCmp MyCmp;

class MyPriorQueue : public priority_queue <Node* ,std::vector<Node*>, MyCmp>
{
	public:
	// we inherited the c++ priority queue and added some methods.
		bool contain(Node* node);
		void remove(Node* nodeToRemove);
		void removeAll();
};
//Implementation of BFS algorithm.

class MyBFS : public Searcher<std::string, Node*, std::string>
{
	unsigned int visits;

	public:
		MyBFS(){ this->visits = 0; };
		virtual std::string search(Searchable<std::string, Node*>* mg);
		virtual unsigned int getNumOfVisits() { return this->visits; };
		virtual ~MyBFS(){};
};
//Implementation of DFS algorithm.
class MyDFS : public Searcher<std::string, Node*, std::string>
{
	//sets for explored and "half" explored verticies.
	unsigned int visits;

    //recursive function for DFS.
	virtual void recDFS(Searchable<std::string, Node*>* mg, Node* currentNode);

	public:
		MyDFS(){ this->visits = 0; };
		virtual std::string search(Searchable<std::string, Node*>* mg);
		virtual unsigned int getNumOfVisits() { return this->visits; };
		virtual ~MyDFS(){};
};
//Implementation of BestFirstSearch algorithm.
class MyBestFirstSearch : public Searcher<std::string, Node*, std::string>
{

	MyPriorQueue queue;
	unsigned int visits;

	virtual void addToQueue(Node* node) { this->queue.push(node); };
	virtual int getQueueSize() { return this->queue.size(); };
	virtual void removeFromQueue(Node * node){  this->queue.remove(node); }
	virtual bool isContain(Node * node) { return this->queue.contain(node); };
	virtual Node* extractMin();

	public:
		MyBestFirstSearch(){ this->visits = 0; };
		virtual std::string search(Searchable<std::string, Node*>* mg);
		virtual unsigned int getNumOfVisits() { return this->visits; };
		virtual ~MyBestFirstSearch(){};
};
//Implementation of A* algorithm.
class MyAstar : public Searcher<std::string, Node*, std::string>
{
	MyPriorQueue queue;
	unsigned int visits ;

	//private methods regarding the queue and heuristic.
	virtual void addToQueue(Node* node) { this->queue.push(node); };
	virtual void removeFromQueue(Node* node) {  this->queue.remove(node); }
	virtual bool isContain(Node* node) { return this->queue.contain(node); };
	virtual void setHeuristic(Searchable<std::string, Node*>* mg);
	virtual MyPriorQueue getQueue() { return this->queue;};
	virtual Node* extractMin();

public:

	MyAstar() { this->visits=0;}
	virtual std::string search(Searchable<std::string, Node*>* mg);
	//get amount of verticies explored.
	virtual unsigned int getNumOfVisits() { return this->visits; };
	virtual ~MyAstar(){};


};

#endif
