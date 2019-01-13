//implementation of our four search algorithms
#include "MySearch.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
using std::priority_queue;
using std::vector;
using std::greater;
#define STARTVAL 0
#define NOTFOUND "-1"


bool MyPriorQueue::contain(Node* node)
{
	//check if element is in the queue.
	  auto it = std::find(this->c.begin(), this->c.end(), node);
	  return (it != this->c.end());
}
void MyPriorQueue::remove(Node* nodeToRemove) {
	//remove element from the queue and fix the order using heapify.
    auto it = std::find(this->c.begin(), this->c.end(), nodeToRemove);
    if (it != this->c.end()) {
        this->c.erase(it);
        std::make_heap(this->c.begin(),this->c.end(), this->comp);
    }
}

Node* MyBestFirstSearch:: extractMin()
{ //takes out the best node from the graph, in our case the node with the best cost.
	Node* toPop = this->queue.top();
	this->queue.pop();
	return toPop;
 }
Node* MyAstar:: extractMin()
{ //takes out the best node from the graph, in our case the node with the best cost.
	Node* toPop = this->queue.top();
	this->queue.pop();
	return toPop;
 }

//search using FirstBestSearch.
std::string MyBestFirstSearch :: search(Searchable<std::string, Node*>* mg)
{
	//we use set to mark our used Nodes.
    std::unordered_set <Node*> closedMap;
    Node* start = mg->getInitialState();
    //set the source destination.
    start->setAlgCost(STARTVAL);
    this->addToQueue(mg->getInitialState());
    while (this->getQueueSize() > 0) //continue until there is no nodes left,we will check all the nodes eventually.
    {
    	Node * current = this->extractMin();
    	this->visits++;
        closedMap.insert(current);
        if (current == mg->getGoalState())
        { //case we find our node.
            return current->pathFromStart();
        }
        vector<Node*> adj = mg->getAdj(current); //extract neighbors.
        for (Node* son : adj)
        {
        	double distance = current->getAlgCost() + son->getCost();
        	//check if not in the queue and not handled.
            if ((closedMap.find(son) == (closedMap.end())) && !(this->isContain(son))) {
            	//set the path and the previous node in this path.
                son->setAlgCost(distance);
                son->setCameFrom(current);
                this->addToQueue(son);
            }
            else if(distance < son->getAlgCost()) {
                if (!this->isContain(son)) { //check if the queue contains the node.
                    this->addToQueue(son);
                } else {
                    son->setAlgCost(distance);
                    this->removeFromQueue(son);
                    //when node enters to priority queue it updates the heap
                    this->addToQueue(son);
                }
            }
        }
    }

    return NOTFOUND;
}

//find a path using BFS algorithm.

std::string MyBFS :: search(Searchable<std::string, Node*>* mg)
{
	//queue and a set for the nodes.
	std::queue<Node*> queue;
    std::unordered_set <Node*> handled;
    queue.push(mg->getInitialState());
    handled.insert(mg->getInitialState());
    while (!queue.empty())
    {
    	Node * current = queue.front();
    	this->visits++;
    	queue.pop();
        if (current == mg->getGoalState())
        { //case we find our node.
            return current->pathFromStart();
        }
        vector<Node*> adj = mg->getAdj(current); //extract neighbors.
        for (Node* son : adj)
        {
            if (handled.find(son) == handled.end() )
            {
            	//set the node as handled and set the path.
                son->setCameFrom(current);
                handled.insert(son);
                queue.push(son);
            }
        }
    }
    return NOTFOUND; // in case our node was not found.
}


//find a path using DFS algorithm.
std::string MyDFS :: search(Searchable<std::string, Node*>* mg)
{
	Node* start = mg->getInitialState();
	start->setAlgCost(STARTVAL);
	recDFS(mg,start);
	std::string answer = mg->getGoalState()->pathFromStart();
	if (answer == "") //if path was not found (we didnt reach the target node).
	{
		return NOTFOUND;
	}
	return answer;
}


// the recursive function.
void  MyDFS :: recDFS(Searchable<std::string, Node*>* mg, Node* currentNode)
{
	//visit every son node recursively.
	this->visits++;
	currentNode->setIsVisited();
	vector<Node*> adj = mg->getAdj(currentNode); //extract neighbors.
	for (Node* son : adj) //iterate over the neighbors.
	{
		if (!(son->getIsVisted()))
		{
			//calculate the path.
			double distance = currentNode->getAlgCost() + son->getCost();
			son->setAlgCost(distance); //set the cost.
			son->setCameFrom(currentNode); //set the previous node for restoration.
			this->recDFS(mg,son); //repeat recursively until reaching the end node.
		}
	}
}

std::string MyAstar:: search(Searchable<std::string, Node*>* mg)
{
	this->setHeuristic(mg);
	//we use set to mark our used Nodes.
    std::unordered_set <Node*> closedMap;
    Node* start = mg->getInitialState();
    //set the source destination.
    start->setAlgCost(STARTVAL);
    this->addToQueue(start);
    while (this->getQueue().size() > 0) //continue until there is no nodes left,we will check all the nodes eventually.
    {
    	Node * current = this->extractMin();
    	this->visits++;
        closedMap.insert(current);
        if (current == mg->getGoalState())
        { //case we find our node.
            return current->pathFromStart();
        }
        vector<Node*> adj = mg->getAdj(current); //extract neighbors.
        for (Node* son : adj)
        {
        	double distance = current->getAlgCost() + son->getCost() + son->getHCost();
        	//check if not in the queue and not handled.
            if ((closedMap.find(son) == (closedMap.end())) && !(this->isContain(son)))
            {
            	//set the path and the previous node in this path.
                son->setAlgCost(distance);
                son->setCameFrom(current);
                this->addToQueue(son);
            }
            else if(distance < son->getAlgCost()) {
                if (!this->isContain(son)) { //check if the queue contains the node.
                    this->addToQueue(son);
                } else {
                    son->setAlgCost(distance);
                    this->removeFromQueue(son);
                    //when node enters to priority queue it updates the heap
                    this->addToQueue(son);
                }
            }
        }
    }

    return NOTFOUND;


}
//preprocess the nodes with their heuristic cost.
void MyAstar:: setHeuristic(Searchable<std::string, Node*>* mg)
{
	//we evaluate the heuristic function with a sort of euqlidic distance.
	Node* temp = nullptr, * goal = nullptr;
	goal = mg->getGoalState();
	std::map<std::string, Node*> mapOfNodes = *(mg->getAll());
	std::map<std::string, Node*> :: iterator it;
	for (it = mapOfNodes.begin(); it != mapOfNodes.end(); ++it)
	{
		temp = it->second;
		double minX = std::min (temp->getPosition().i , goal->getPosition().i);
		double maxX = std::max (temp->getPosition().i , goal->getPosition().i);
		double minY = std::min (temp->getPosition().j , goal->getPosition().j);
		double maxY = std::max (temp->getPosition().j , goal->getPosition().j);

		double absX = maxX - minX;
		double absY = maxY - minY;
		double h = std::max(absX,absY);
		temp->setHCost(h);
	}
}


