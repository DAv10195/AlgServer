
#include <iostream>
#include <map>
#include "Search.h"
using namespace std;


int main() {
	Node* n1 = new Node(0, 0, 10);
	Node* n2 = new Node(0, 1, 5);
	double** arr;
	arr = new double*[2];
	arr[0] = new double[2];
	arr[1] = new double[2];
	arr[0][0] = 1;
	arr[0][1] = -1;
	arr[1][0] = 1;
	arr[1][1] = 1;
	Position s, g;
	s.i = 0;
	s.j = 0;
	g.i = 1;
	g.j = 1;
	MatrixGraph* gr = new MatrixGraph(s, g, arr, 2);
	n2->setCameFrom(n1);
	cout << n2->pathFromStart() << endl;
	delete n1;
	delete n2;
	delete gr;
	delete arr[0];
	delete arr[1];
	delete arr;
	return 0;
}
