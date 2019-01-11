//Header file for Search Algorithms and Search Problems
#ifndef SEARCH_H_
#define SEARCH_H_

#include <map>
#include <vector>
//Searchable interface, each Node should be given unique string representation to be held in a map, for running time purposes...
template<typename K, typename T> //T - State type, K - key for State
class Searchable
{	//if one wishes to use this interface, he must Hash the K object if it is not primitive or language standard...
	public:
		virtual T getInitialState() = 0;
		virtual T getGoalState() = 0;
		virtual std::map<K, T>* getAll() = 0;
		virtual std::vector<T> getAdj(T) = 0;
		virtual ~Searchable(){};
};
//Searcher abstract class
template<typename S, typename T, typename K>
class Searcher
{
	public:
		virtual S search(Searchable<K, T>* searchable) = 0;
		virtual unsigned int getNumOfVisits() = 0;
		virtual ~Searcher(){};
};

#endif
