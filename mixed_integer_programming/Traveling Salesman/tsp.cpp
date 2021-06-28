
#include <ilcplex/ilocplex.h>
#include <stdio.h>
#include <time.h>
#include <iostream>  
#include <string>
#include "data.h"

// Modelo TSP sem as restrições de subciclo

#define PRECISAO 0.00001

int main(int argc, char *argv[])
{
  bool DEPU = true;
  
  if(argc < 2)
    {
      cout<<"Correct call: ./tsp filename"<<endl;
      exit(1);
    }

    Data data(argc, argv[1]);
    data.readData();                    // lê arquivo da instância

    int dim = data.getDimension();      // dimensao do problema (data.cpp)

    // impressao da matriz
    if (DEPU)
      for ( int i = 0; i < dim; i++ ) {
	for ( int j = 0; j < dim; j++ ) {
	  cout<<data.getDistanceMatrix(i,j)<<" ";
	}
	cout<<endl;
      }

    clock_t start;
    start = clock();

    // create model
    IloEnv env;
    IloModel model(env);
    IloArray<IloBoolVarArray> x(env, dim);       // vetor de ponteiros para variaveis binarias (x_u) de tamanha "dim"
    
    double **costMatrix = data.getCostMatrix();  // captura matriz custo de distancias (data.cpp)
    char var_name[100];

    //variaveis x_uv
    for(int i=0; i < dim - 1; i++) {

      x[i] = IloBoolVarArray(env, dim);                 // aloca outro vetor de ponteiros para variaveis binarias para cada posição de (x_uv)
      
      for(int j=i+1; j < dim; j++) {
	sprintf (var_name, "x_%d_%d", (int)i,(int)j);   // nome da variavel

	x[i][j] = IloBoolVar(env, var_name);            // aloca variavel 
	model.add(x[i][j]);                             // adiciona variavel ao modelo
      }
    }

    // funcao objetivo
    IloExpr fo(env);                                    // expressao numerica do CPLEX

    for(int i = 0; i < dim-1; i++)
      for(int j = i+1; j < dim; j++)
	fo += costMatrix[i][j]*x[i][j];                 // constroi função objetivo


    model.add(IloMinimize(env,fo,"f"));                 // adiciona função objetivo ao modelo
    
    // restricoes de grau
    IloConstraintArray degree2_constraints(env);        // vetor de ponteiros para restrições do CPLEX

    for(int i=0; i < dim; i++) {
      IloExpr constraint(env);                          // expressao numerica do CPLEX
      for(int j=i+1; j < dim; j++) {
	constraint += x[i][j];                                 
      }
      for(int j=0; j < i; j++) {
	constraint += x[j][i];
      }
      degree2_constraints.add(constraint == 2);         // adiciona restrição ao vetor de restrições
      constraint.end();                                 // libera memoria
    }

    model.add(degree2_constraints);                     // adiciona restrições ao modelo
    
    IloCplex solver(model);                             // aloca solver do modelo
    solver.exportModel("model.lp");                     // escreve modelo no arquivo no formato .lp

    // Parametros do CPLEX
    solver.setParam(IloCplex::Param::WorkMem,1024*2);           //tamanho de RAM utilizada maxima
    solver.setParam(IloCplex::Param::MIP::Strategy::File, 2);   //quando a RAM acaba, 1-guarda nos na memoria e compactado 2-guarda os nos em disco 3-guarda nos em disco e compactado
    solver.setParam(IloCplex::Param::TimeLimit, 3600);          // tempo limite
    solver.setParam(IloCplex::Param::Threads, 1);               // Numero de threads
    solver.setParam(IloCplex::Param::MIP::Interval, 100);       // Log a cada N nos
    solver.setOut(env.getNullStream());                         // Desabilitando saida do cplex
    
    // Otimiza
    try
       {
	 solver.solve();
       }
     catch(IloException& e)
       {
	 cout << e;
       }


    // Saida
    bool   solved;
    int    nNodes;
    double res, lb, ub, time_s;
    try
      {
	if(solver.getStatus() == IloAlgorithm::Optimal)
	  solved = true;
	else
	  solved = false;
	
	res    = solver.getObjValue ();                               // solução
	lb     = solver.getBestObjValue();                            // limite dual (inferior)
	nNodes = solver.getNnodes();                                  // numero de nos analisados na arvore de busca
	time_s = ((double)clock() - (double)start) / CLOCKS_PER_SEC;  // tempo de processamento
	
	cout<<"----- ACABOU ----"<<endl;
	cout<<"SOLVED?: "<<solved<<endl;
	cout<<"TEMPO  : "<<time_s<<endl;
	cout<<"LB     : "<<lb<<endl;
	cout<<"UB     : "<<res<<endl;
	cout<<"NOS    : "<<nNodes<<endl;
	cout<<"arestas da solucao otima:"<<endl;
	for(int i=0; i < dim-1; i++) {
	  for(int j=i+1; j < dim; j++) {
	    float x_ij = solver.getValue(x[i][j]);             // retorna o valor da variavel
	    if(x_ij >= (1-PRECISAO))
	      {
		cout<<"("<<i<<", "<<j<<") , ";
	      }
	  }
	}
	cout<<endl;
      }
    catch(IloCplex::Exception &e)
      {
	cout << e;
      }
    env.end();

}

