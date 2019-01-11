//Node implementation
#include "Search.h"
#include <limits>
//constructor
Node :: Node(unsigned int i, unsigned int j, double c)
{
	(this->pos).i = i;
	(this->pos).j = j;
	this->cost = c;
	this->algCost = std::numeric_limits<double>::infinity();
	this->heuristicCost = 0;
	this->cameFrom = nullptr;
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
}
//get the prev Node
Node* Node :: getCameFrom()
{
	return this->cameFrom;
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
	std::vector<std::string> nodesInPath;
	//case no path or method called before running a search algorithm
	if (curr == nullptr)
	{
		return toRet;
	}
	nodesInPath.push_back(this->toString());
	//reaching nullptr means we backtraced all the way to the start Node
	while (curr != nullptr)
	{
		nodesInPath.push_back(curr->toString());
		curr = curr->getCameFrom();
	}
	//constructing a string representation of the path
	i = nodesInPath.size() - 1;
	for (; i >= 0; --i)
	{
		toRet += nodesInPath[i];
		if (!i)
		{
			break;
		}
		toRet+= "->";
	}
	return toRet;
}
