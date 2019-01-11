
#include <iostream>
#include <map>
#include "MySearch.h"

using namespace std;

int main() {
	Node* n1 = new Node(0, 0, 10);
	Node* n2 = new Node(0, 1, 5);
	n2->setCameFrom(n1);
	Node* n3 = new Node(1, 1, 34);
	n3->setCameFrom(n2);
	Node* n4 = new Node(1, 0 , 2323);
	n4->setCameFrom(n3);
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
	MatrixGraph graph = *gr;
	n2->setCameFrom(n1);
	cout << n4->pathFromStart() << endl;
	delete n1;
	delete n2;
	delete n3;
	delete n4;
	delete gr;
	delete arr[0];
	delete arr[1];
	delete arr;
	return 0;
}
