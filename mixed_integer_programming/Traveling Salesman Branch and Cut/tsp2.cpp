
#include <ilcplex/ilocplex.h>
#include <stdio.h>
#include <time.h>
#include <iostream>  
#include <string>
#include "data.h"
#include "mincut.h"

#define PRECISAO 0.00001


// Separacao dos cortes de eliminacao de subciclo
// \sum_{i \in S} x_i <= |S| - 1

//ILOUSERCUTCALLBACK0(name)
//ILOUSERCUTCALLBACK1(name, type1, x1)
//ILOUSERCUTCALLBACK2(name, type1, x1, type2, x2)
//ILOUSERCUTCALLBACK3(name, type1, x1, type2, x2, type3, x3)

//ILOLAZYCONSTRAINTCALLBACK0(name)
//ILOLAZYCONSTRAINTCALLBACK1(name, type1, x1)
//ILOLAZYCONSTRAINTCALLBACK2(name, type1, x1, type2, x2)
//ILOLAZYCONSTRAINTCALLBACK3(name, type1, x1, type2, x2, type3, x3)

ILOLAZYCONSTRAINTCALLBACK5(CB_mincut, IloArray<IloBoolVarArray>, x, int, dim, double**, residual, int*, cutset, int*, numc)
//ILOUSERCUTCALLBACK5(CB_mincut, IloArray<IloBoolVarArray>, x, int, dim, double**, residual, int*, cutset, int*, numc)
{
  // x        -> vetor das variaveis do problema
  // dim      -> dimensao do problema (numero de cidades)
  // residual -> armazena grafo com a solucao corrente
  // cutset   -> conjunto S a ser encontrado
  // numc     -> contador de numero de cortes gerados
  
  IloEnv env  = getEnv();
  bool   DEPU = true;

  // cria grafo da solucao corrente fracionaria 
  for(int i=0; i < dim; i++) {
    for(int j=i; j < dim; j++) {
      residual[i][j] = 0;
      residual[j][i] = 0;
    }
  }

  for(int i=0; i < dim-1; i++) {
    for(int j=i+1; j < dim; j++) {
      float x_ij = getValue(x[i][j]);

      if (x_ij < PRECISAO)
	x_ij = 0;

      residual[i][j] = x_ij;
      residual[j][i] = x_ij;
    }
  }

  // onde o corte sera armazenado (conjunto S)
  for(int i=0; i < dim; i++)
    cutset[i] = 0;

  // metodo para corte minimo
  double cutvalue;
  cutvalue = max_back_heuristic(dim, residual, cutset);

  // gera corte
  if (DEPU) cout<<"cutvalue="<<cutvalue<<endl;
  if(cutvalue + PRECISAO < 2) {

    double cutset_size = 0;
    if (DEPU) cout<<"      mincut=";
    for(int i=0; i < dim; i++){
      if (cutset[i]==1){
	if (DEPU) cout<<i<<",";
	cutset_size += cutset[i];
      }
    }
    if (DEPU) cout<<"   size="<<cutset_size<<endl;

    IloExpr corte(env);
    double  val=0;
    for(int i=0; i < dim; i++) {
      for(int j=i+1; j < dim; j++) {
	if(cutset[i] == 1 && cutset[j] == 1) {
	  corte += x[i][j];
	  val   += getValue(x[i][j]);
	}
      }
    }
    if (DEPU) cout<<"      soma = "<<val<<endl;
    add(corte <= cutset_size-1).end();
    *numc = *numc + 1;

    // libera memoria
    corte.end();

    double ub     = getIncumbentObjValue();  // retorna a melhor solucao inteira (limite primal)
    double lb     = getBestObjValue();       // retorna o melhor limite dual
    double rlx    = getObjValue();           // quando chamada dentro do callback, retorna o valor da relaxacao do noh
    double nNodes = getNremainingNodes();    // retorna o numero restante de nos a serem analisados
    cout<<"--- LAZY CUT:"<<"relax="<<rlx<<"\t bounds="<<lb<<"<->"<<ub<<"\t n_rest="<<nNodes<<"\t Ucuts="<<*numc<<endl;

  }

}





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
    /*if (DEPU)
      for ( int i = 0; i < dim; i++ ) {
	for ( int j = 0; j < dim; j++ ) {
	  cout<<data.getDistanceMatrix(i,j)<<" ";
	}
	cout<<endl;
	}*/

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


    // ***********************  callback de corte ******************************

    // estrutara usada para os métodos de geracao de cortes 
    double **residual = new double*[dim];               // grafo residual usado no corte
    for(int i=0; i < dim; i++)
      residual[i] = new double[dim];
    int *cutset = new int[dim];
    int num_l=0,num_u=0;

    // -- LAZY CUTS --
    // Sao cortes que fazem parte do modelo original (i.e., sem eles o modelo ficaria incompleto),
    // mas por algum motivo nao foram todos inseridos inicialmente (possivelmente por terem um numero exponencial)
    solver.use(CB_mincut(env, x, dim, residual, cutset, &num_l)); 

    // -- USER CUTS --
    // Sao cortes que nao eliminam nenhuma solucao viavel (e.g., cortes que eliminam solucoes fracionarias)
    // mas que se nao inseridos nao inviabilizam a corretude do modelo
    /* solver.use(CB_mincut(env, x, dim, residual, cutset, &num_u)); */ 

    // *****************************************************************************
    
    // Parametros do CPLEX
    solver.setParam(IloCplex::Param::WorkMem,1024*2);           //tamanho de RAM utilizada maxima
    solver.setParam(IloCplex::Param::MIP::Strategy::File, 2);   //quando a RAM acaba, 1-guarda nos na memoria e compactado 2-guarda os nos em disco 3-guarda nos em disco e compactado
    solver.setParam(IloCplex::Param::TimeLimit, 3600);          // tempo limite
    solver.setParam(IloCplex::Param::Threads, 1);               // Numero de threads
    solver.setParam(IloCplex::Param::MIP::Interval, 100);       // Log a cada N nos
    solver.setOut(env.getNullStream());        // Desabilitando saida do cplex

    // Parametros do B&C
    solver.setParam(IloCplex::Param::Preprocessing::Presolve, 0);        // desliga o preprocessamento, 1-ligado 0-desligado
    solver.setParam(IloCplex::Param::MIP::Limits::CutsFactor, 0);        // número de cortes que o CPLEX pode gerar, 0-desliga os cortes
    solver.setParam(IloCplex::Param::MIP::Strategy::HeuristicFreq, -1);  // heuristicas primais do CPLEX, -1-desliga 0-liga
    
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

    // libera memoria
    delete [] cutset;
    for(int i=0; i < dim; i++)
      delete [] residual[i];
    delete [] residual;
    
}

