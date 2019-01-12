
#include <iostream>
#include <map>
#include "MySearch.h"
#include <queue>
int main() {
	MyPriorQueue queue;
	double** arr;
	arr = new double*[4];
	arr[0] = new double[4];
	arr[1] = new double[4];
	arr[2]= new double[4];
	arr[3] = new double[4];
	arr[0][0] = 2;
	arr[1][1] = -1;
	arr[2][2] = 3;
	arr[3][3] = 8;

	arr[1][0] = 5;
	arr[1][3] = 2;
	arr[1][2] = -1;

	arr[2][1] = 3;
	arr[2][3] = -1;
	arr[2][0] = 7;

	arr[3][0]= 2;
	arr[3][1]=-1;
	arr[3][2] = -1;

	arr[0][1]= 8;
	arr[0][2]=8;
	arr[0][3] = -1;

	Position s, g;
	s.i = 0;
	s.j = 0;
	g.i = 3;
	g.j = 3;
	MatrixGraph* gr = new MatrixGraph(s, g, arr, 4);
	MyDFS * b = new MyDFS();
	std::string s1 = b->search(gr);
	std::cout <<s1<<std::endl;
	delete gr;
	delete b;
	delete arr[0];
	delete arr[1];
	delete arr[2];
	delete arr[3];
	delete arr;
	return 0;
}
