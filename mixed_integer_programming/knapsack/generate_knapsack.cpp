#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    int n=10, capacity=1000, max_val=1000, max_peso=1000;
    ofstream arqout;
    
    arqout.open("knapsack.dat");
    arqout << n << " " << capacity << endl;
    arqout << "[" ;
    for (int i=0; i<n; i++)
    {
        arqout << rand()%max_val;
        if (i != n-1){arqout << ", ";}
    }
    arqout << "]" << endl;
    arqout << "[" ;
    for (int i=0; i<n; i++)
    {
        arqout << rand()%max_peso;
        if (i != n-1){arqout << ", ";}
    }
    arqout << "]" << endl;
    arqout.close();

    return 0;
}