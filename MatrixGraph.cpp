//MatrixGraph Implementation
#include "MySearch.h"
#include <iostream>
#define WALL -1
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
//Parse input from client and create MatrigGraph
MatrixGraph* GraphCreator :: create(std::string &s)
{
	unsigned int rows = 0, cols = 0, i = 0, j = 0, k = 0, max = 0, size = 0;
	int rowInd = 0, colInd = 0;
	std::vector<std::string> toParse;
	std::string row = "", num = "";
	Position start, goal;
	start.i = 0;
	start.j = 0;
	goal.i = 0;
	goal.j = 0;
	double ** costs;
	MatrixGraph* toRet = nullptr;
	//count rows - each row ends with '$', two last rows are start and goal indexes of the matrix
	size = s.size();
	while (i < size)
	{
		if (s.at(i) == '$')
		{
			++rows;
			toParse.push_back(row);
			row = "";
			++i;
			continue;
		}
		row.push_back(s.at(i));
		++i;
	}
	//decrease rows by 2 because last 2 rows are start and goal indexes
	rows -= 2;
	//count number of columns, which is the number of ',' in each row + 1. for example: "1,2,3,4" has 3 ',' and 4 columns
	size = toParse[0].size();
	for (i = 0; i < size; ++i)
	{
		if (toParse[0].at(i) == ',')
		{
			++cols;
		}
	}
	//+1...
	++cols;
	i = 0;
	//build graph paramaters
	try
	{	//until index rows, we have the matrix rows and the last two are start and goal positions
		//extracting start position
		size = toParse[rows].size();
		while (i < size && toParse[rows].at(i) != ',')
		{
			num.push_back(toParse[rows].at(i));
			++i;
		}
		rowInd = stoi(num);
		//case invalid index, rows is always non-negative so casting is valid (only if rows is enormous its bad...)
		if (rowInd < 0 || rowInd > (int)(rows - 1))
		{
			return nullptr;
		}
		start.i = rowInd;
		num = "";
		++i;
		while (i < size)
		{
			num.push_back(toParse[rows].at(i));
			++i;
		}
		colInd = stoi(num);
		//case invalid index, cols is always non-negative so casting is valid (only if cols is enormous its bad...)
		if (colInd < 0 || colInd > (int)(cols - 1))
		{
			return nullptr;
		}
		start.j = colInd;
		num = "";
		i = 0;
		//extracting goal position
		size = toParse[rows + 1].size();
		while (i < size && toParse[rows + 1].at(i) != ',')
		{
			num.push_back(toParse[rows + 1].at(i));
			++i;
		}
		rowInd = stoi(num);
		//case invalid index, rows is always non-negative so casting is valid (only if rows is enormous its bad...)
		if (rowInd < 0 || rowInd > (int)(rows - 1))
		{
			return nullptr;
		}
		goal.i = rowInd;
		num = "";
		++i;
		while (i < size)
		{
			num.push_back(toParse[rows + 1].at(i));
			++i;
		}
		colInd = stoi(num);
		//case invalid index, cols is always non-negative so casting is valid (only if cols is enormous its bad...)
		if (colInd < 0 || colInd > (int)(cols - 1))
		{
			return nullptr;
		}
		goal.j = colInd;
		//allocate matrix to build graph from. we'll always allocate a quadratic one, initializing it with WALLS(-1)
		max = std::max(rows, cols);
		costs = new double*[max];
		for (i = 0; i < max; ++i)
		{
			costs[i] = new double[max];
			for (j = 0; j < max; ++j)
			{
				costs[i][j] = WALL;
			}
		}
		//fill matrix with inputed values
		j = 0;
		num = "";
		for (i = 0; i < rows; ++i)
		{	//get row
			row = toParse[i];
			size = toParse[i].size();
			while (k < size)	//run until rows end
			{	//each ',' indicates end of value
				while (k < size && toParse[i].at(k) != ',')
				{
					num.push_back(toParse[i].at(k));
					++k;
				}
				++k;	//update costs matrix
				costs[i][j] = stod(num);
				num = "";
				++j;
			}
			k = 0;
			j = 0;
		}
		toRet = new MatrixGraph(start, goal, costs, max);
		//delete costs matrix
		for (i = 0; i < max; ++i)
		{
			for (j = 0; j < max; ++j)
			{
				std::cout << costs[i][j] << ",";
			}
			std::cout << std::endl;
		}
		for (i = 0; i < max; ++i)
		{
			delete costs[i];
		}
		delete costs;
		return toRet;
	}
	catch (std::runtime_error &e)
	{
		if (costs != nullptr)
		{
			for (i = 0; i < max; ++i)
			{
				delete costs[i];
			}
			delete costs;
		}
		return nullptr;
	}
}
