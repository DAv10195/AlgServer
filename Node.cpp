//Node implementation
#include "MySearch.h"
#include <limits>
//basic constructor
Node :: Node()
{
	(this->pos).i = 0;
	(this->pos).j = 0;
	this->cost = 0;
	this->algCost = std::numeric_limits<double>::infinity();
	this->heuristicCost = 0;
	this->cameFrom = nullptr;
	this->cameFromDir = "";
	this->isVisited = false;
}
//position constructor (mainly for matrix graphs...)
Node :: Node(unsigned int i, unsigned int j) : Node()
{
	(this->pos).i = i;
	(this->pos).j = j;
}
//position and cost constructor (mainly for matrix graphs...)
Node :: Node(unsigned int i, unsigned int j, double c) : Node(i, j)
{
	this->cost = c;
}
//heuristic constructor (mainly for matrix graphs using heuristic algorithms...)
Node :: Node(unsigned int i, unsigned int j, double c, double hc) : Node(i, j, c)
{
	this->heuristicCost = hc;
}
//add adjacent Node
void Node :: addAdjNode(Node* n)
{
	(this->adj).push_back(n);
}
//get Position (relevant for Matrix Graphs...)
Position Node :: getPosition()
{
	return this->pos;
}
//get cost
double Node :: getCost()
{
	return this->cost;
}
//get algorithmic cost
double Node :: getAlgCost()
{
	return this->algCost;
}
//get heuristic cost
double Node :: getHCost()
{
	return this->heuristicCost;
}
//set algorithmic cost
void Node :: setAlgCost(double ac)
{
	this->algCost = ac;
}
//set heurisitic cost
void Node :: setHCost(double hc)
{
	this->heuristicCost = hc;
}
//set the prev Node
void Node :: setCameFrom(Node* n)
{
	this->cameFrom = n;
	Position pos = n->getPosition();
	unsigned int nI = pos.i, nJ = pos.j, thisI = (this->pos).i, thisJ = (this->pos).j;
	//right
	if (nI < thisI)
	{
		this->cameFromDir = "Down";
	}
	//left
	if (nI > thisI)
	{
		this->cameFromDir = "Up";
	}
	//down
	if (nJ < thisJ)
	{
		this->cameFromDir = "Right";
	}
	//up
	if (nJ > thisJ)
	{
		this->cameFromDir = "Left";
	}
}
//get the prev Node
Node* Node :: getCameFrom()
{
	return this->cameFrom;
}
std::string Node :: getCameFromDir()
{
	return this->cameFromDir;
}
//get all adjacent Nodes
std::vector<Node*> Node :: getAdj()
{
	return this->adj;
}
//compare to other Node
bool Node :: equals(Node* n)
{
	if (n != nullptr)
	{
		Position nPos = n->getPosition();
		if ((this->pos).i == nPos.i && (this->pos).j == nPos.j)
		{	//same Position <-> same Node
			return true;
		}
	}

	return false;
}
//return toString identifier
std::string Node :: toString()
{
	return "(" + std::to_string((this->pos).i) + "," + std::to_string((this->pos).j) + ")";
}
//construct path from start Node (defined by the graph to be searched...)
std::string Node :: pathFromStart()
{
	std::string toRet = "";
	Node* curr = this->cameFrom;
	unsigned int i = 0;
	std::vector<std::string> path;
	//case no path or method called before running a search algorithm
	if (curr == nullptr)
	{
		return toRet;
	}
	path.push_back(this->cameFromDir);
	//reaching nullptr means we backtraced all the way to the start Node
	while (curr != nullptr)
	{
		path.push_back(curr->getCameFromDir());
		curr = curr->getCameFrom();
	}
	//constructing a string representation of the path
	i = path.size() - 1;
	for (; i >= 0; --i)
	{
		toRet += path[i];
		if (!i)
		{
			break;
		}
		if (i == path.size() - 1)
		{
			continue;
		}
		toRet+= ",";
	}
	return toRet;
}
