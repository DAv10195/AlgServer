//Header file for Search Algorithms and their dependencies
#ifndef SEARCH_H_
#define SEARCH_H_

#include <vector>
#include <string>
#include <map>
//a pair representing Position at a matrix.
struct Position
{
	unsigned int i;
	unsigned int j;
};
typedef struct Position Position;
/*
 * Node class which is mainly adjusted to out Matrix Graphs, but can be also used for other kinds of graphs with slight modification.
 * for example, one can use the adjency vector of a Node to get all of the edges coming from it and construct an edge set, thus using this object
 * for other search problems...
 */
class Node
{
	Position pos;
	double cost;
	double algCost;
	double heuristicCost;
	std::vector<Node*> adj;
	Node* cameFrom;

	public:
		Node(unsigned int i, unsigned int j, double c);
		virtual void addAdjNode(Node* n);
		virtual Position getPosition();
		virtual double getCost();
		virtual double getAlgCost();
		virtual double getHCost();
		virtual void setAlgCost(double ac);
		virtual void setHCost(double hc);
		virtual void setCameFrom(Node* n);
		virtual Node* getCameFrom();
		virtual std::vector<Node*> getAdj();
		virtual bool equals(Node* n);
		virtual std::string toString();
		virtual std::string pathFromStart();
		virtual ~Node(){};
};
//Searchable interface, each Node should be given unique string representation to be held in a map, for running time purposes...
class Searchable
{
	public:
		virtual Node* getInitialState() = 0;
		virtual Node* getGoalState() = 0;
		virtual std::map<std::string, Node*>* getAll() = 0;
		virtual ~Searchable(){};
};
//Searcher abstract class
template<typename S>
class Searcher
{
	unsigned int visits;

	public:
		virtual S search(Searchable* searchable) = 0;
		virtual unsigned int getNumOfVisits() { return this->visits; }
		virtual ~Searcher(){};
};
//the Searchable we'll use at our implementation
class MatrixGraph : public Searchable
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
		virtual ~MatrixGraph();
};

#endif
