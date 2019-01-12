//implementation of our four search algorithms
#include "MySearch.h"
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <algorithm>
using std::priority_queue;
using std::vector;
using std::greater;
#define STARTVAL 0
#define NOTFOUND "-1"

//helper functions for BestFirstSearch.
 void MyBestFirstSearch:: addToQueue(Node* node)
{
    this->queue.push(node);
}
 int MyBestFirstSearch:: getQueueSize()
{
    return this->queue.size();
}
 void MyBestFirstSearch:: removeFromQueue(Node* node)
{
    this->queue.remove(node);
}
 bool MyBestFirstSearch:: isContain(Node* node)
{
	 //return false if node is not in the queue.
    return this->queue.contain(node);
}

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
	this->increaseNumOfVisits();
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
    	this->increaseNumOfVisits();
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


std::string MyDFS :: search(Searchable<std::string, Node*>* mg)
{
    std::unordered_set <Node*> black;
    std::unordered_set <Node*> grey;
    Node* current = mg->getInitialState();
    grey.insert(current);
    if (current == mg->getGoalState())
    { //case we find our node.
        return current->pathFromStart();
    }
    vector<Node*> adj = mg->getAdj(current); //extract neighbors.
    for (Node* son : adj)
    {
    	if (grey.find(son) == grey.end() && !(black.find(son) == black.end()))
    	{
    		this->increaseNumOfVisits();
    		son->setCameFrom(current);

    		//this->search(mg);

    	}
        black.insert(current);
    }
    return NOTFOUND;
}

