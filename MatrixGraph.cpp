//MatrixGraph Implementation
#include "MySearch.h"
//build Nodes and Node map
void MatrixGraph :: initNodes(double** costs, unsigned int size)
{
	Node* n = nullptr;
	Position pos;
	pos.i = 0;
	pos.j = 0;
	unsigned int i = 0, j = 0, mInd = size - 1;
	for (; i < size; ++i)
	{	//building Nodes...
		for (; j < size; ++j)
		{
			if (costs[i][j] < 0)
			{	//a wall or an infinite cost...
				continue;
			}
			n = new Node(i, j, costs[i][j]);
			(this->nodes)[n->toString()] = n;
		}
		j = 0;
	}
	std::map<std::string, Node*>:: iterator it;
	for (it = (this->nodes).begin(); it != (this->nodes).end(); ++it)
	{	//build adjency vector for each Node
		n = it->second;
		pos = n->getPosition();
		i = pos.i, j = pos.j;
		//left (checking if i is positive, thus going left doesn't take us out of bounds and of course if there's a Node...)
		if (i && ((this->nodes).find("(" + std::to_string(i - 1) + "," + std::to_string(j) + ")") != (this->nodes).end()))
		{
			n->addAdjNode((this->nodes).at("(" + std::to_string(i - 1) + "," + std::to_string(j) + ")"));
		}
		//up (checking if j is positive, thus going up doesn't take us out of bounds and of course if there's a Node...)
		if (j && ((this->nodes).find("(" + std::to_string(i) + "," + std::to_string(j - 1) + ")") != (this->nodes).end()))
		{
			n->addAdjNode((this->nodes).at("(" + std::to_string(i) + "," + std::to_string(j - 1) + ")"));
		}
		//right (checking if i is less than maxIndex, thus going right doesn't take us out of bounds and of course if there's a Node...)
		if (i < mInd && ((this->nodes).find("(" + std::to_string(i + 1) + "," + std::to_string(j) + ")") != (this->nodes).end()))
		{
			n->addAdjNode((this->nodes).at("(" + std::to_string(i + 1) + "," + std::to_string(j) + ")"));
		}
		//down (checking if j is less than maxIndex, thus going down doesn't take us out of bounds and of course if there's a Node...)
		if (j < mInd && ((this->nodes).find("(" + std::to_string(i) + "," + std::to_string(j + 1) + ")") != (this->nodes).end()))
		{
			n->addAdjNode((this->nodes).at("(" + std::to_string(i) + "," + std::to_string(j + 1) + ")"));
		}
	}
}
//constructor
MatrixGraph :: MatrixGraph(Position start, Position goal, double** costs, unsigned int size)
{
	this->initNodes(costs, size);
	this->start = (this->nodes)["(" + std::to_string(start.i) + "," + std::to_string(start.j) + ")"];
	this->goal = (this->nodes)["(" + std::to_string(goal.i) + "," + std::to_string(goal.j) + ")"];
}
//returns start Node
Node* MatrixGraph :: getInitialState()
{
	return this->start;
}
//returns goal Node
Node* MatrixGraph :: getGoalState()
{
	return this->goal;
}
void MatrixGraph :: setInitialState(Node* init)
{
	this->start = init;
}
//returns a pointer to the map (will be used to set heuristics for example...)
std::map<std::string, Node*>* MatrixGraph :: getAll()
{
	return &(this->nodes);
}
//returns inputed Node's all adjacent Nodes
std::vector<Node*> MatrixGraph :: getAdj(Node* n)
{
	return n->getAdj();
}
//destructor
MatrixGraph :: ~MatrixGraph()
{
	std::map<std::string, Node*>:: iterator it;
	for (it = (this->nodes).begin(); it != (this->nodes).end(); ++it)
	{
		delete (it->second);
	}
}
