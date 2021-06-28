#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix2D;
typedef IloArray<NumVarMatrix2D> NumVarMatrix3D;

typedef IloArray<IloBoolVarArray> BoolVarMatrix2D;
typedef IloArray<BoolVarMatrix2D> BoolVarMatrix3D;

typedef IloArray<IloNumArray> NumMatrix2D;
typedef IloArray<NumMatrix2D> NumMatrix3D;

int main()
{
    cout << "Criando ambiente CPLEX..." << endl;
    IloEnv env;
    cout << "Ambiente CPLEX criado." << endl;

    try
    {
        string filename;
        
        filename="./TPI_COL_3.txt";
        
        // instancia contador de tempo
        IloTimer timer(env);
        // inicializa contagem do tempo
        timer.start();

        // reading data from file
        cout << "Lendo dados de entrada..." << endl;

        ifstream file (filename);
        
        if (file.is_open())
        {    
            cout << "Arquivo aberto corretamente." << endl;        
        }
        else
        {
            cout << "Não foi possível abrir o arquivo." << endl;
            return 0;
        }
        
        IloInt n_vertices, n_arestas;
        string stemp;

        file >> stemp >> stemp >> n_vertices >> n_arestas;

        NumMatrix2D adj_matrix(env, n_vertices);

        for (int i=0; i<n_vertices; i++)
        {
            adj_matrix[i] = IloNumArray(env, n_vertices);
            for (int j=0; j<n_vertices; j++)
            {
                adj_matrix[i][j] = 0;
            }
        }

        cout << "A instância tem: " << endl;
        cout << n_vertices << " vertices, " << endl;
        cout << n_arestas << " arestas, " << endl;

        string line;
        int line_count = 0;

        while (getline(file, line))
        {
            istringstream iss(line);
            
            if (line_count != 0)
            {

                IloInt v1, v2;
                
                if ( !(iss >> stemp >> v1 >> v2) ) 
                {
                    break;
                }
                
                adj_matrix[v1-1][v2-1] = 1;
                adj_matrix[v2-1][v1-1] = 1;

            }

            line_count += 1;

        }

        // end reading data
        
        cout << "matrix de adjacência =" << endl << adj_matrix << endl << endl; 

        // closing file
        file.close();
        cout << "Dados de entrada lidos." << endl;

        // instanciando o modelo
        cout << "Instanciando modelo CPLEX..." << endl;
        IloModel model(env);
        cout << "Modelo CPLEX instanciado." << endl;
        
        // declarando o otimizador
        cout << "Declarando o solver CPLEX..." << endl;
        IloCplex cplex(env);
        cout << "Solver CPLEX declarado." << endl;
        
        // definindo parametros
        cout << "Definindo parametros do CPLEX..." << endl;

        cplex.setParam(IloCplex::Param::TimeLimit, 7200); // in seconds: 3600, 7200, 14400, 21600, 43200, 86400

        cplex.setParam(IloCplex::Param::WorkMem, 2048); // 1024 megabytes
        cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 262144); // 131072 megabytes
        cplex.setParam(IloCplex::Param::Emphasis::Memory, 1); // 1: conservar memoria
        cplex.setParam(IloCplex::Param::MIP::Strategy::File, 3); // 1: em memória, 2: em disco, 3: em disco otimizado 
        cplex.setParam(IloCplex::Param::WorkDir, ".");

        cplex.setParam(IloCplex::Param::RootAlgorithm, 0); // 1: primal, 2: dual, 4: barrier, 6: concurrent
        cplex.setParam(IloCplex::Param::NodeAlgorithm, 0); // 1: primal, 2: dual, 4: barrier, 6: concurrent
        cplex.setParam(IloCplex::Param::MIP::SubMIP::SubAlg, 4); // 1: primal, 2: dual, 4: barrier
        cplex.setParam(IloCplex::Param::MIP::SubMIP::StartAlg, 4); // 1: primal, 2: dual, 4: barrier
        
        cplex.setParam(IloCplex::Param::MIP::Strategy::Probe, 3); // 1: moderado, 2: agressivo, 3: muito agressivo
        //cplex.setParam(IloCplex::Param::MIP::Cuts::Cliques, 3); // 1: moderado, 2: agressivo, 3: muito agressivo
        //cplex.setParam(IloCplex::Param::MIP::Cuts::Covers, 3); // 1: moderado, 2: agressivo, 3: muito agressivo
        //cplex.setParam(IloCplex::Param::MIP::Cuts::LiftProj, 3); // 1: moderado, 2: agressivo, 3: muito agressivo
        cout << "Parametros do CPLEX definidos." << endl;

        // criando variáveis de decisão ---------------------------------------------
        cout << "Criando variáveis de decisão..." << endl;

        BoolVarMatrix2D x_var(env, n_vertices);
        for (int i=0; i<n_vertices; i++)
        {
            x_var[i] = IloBoolVarArray(env, n_vertices);
            for (int j=i+1; j<n_vertices; j++)
            {
                if (adj_matrix[i][j] == 1)
                {
                    x_var[i][j] = IloBoolVar(env);
                }
            }
        }

        IloBoolVarArray w_var(env, n_vertices);

        cout << "Variáveis de decisão criadas." << endl;
        
        // verificando uso de memória
        cout << "Memory usage after creating constraints: " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

        // criando restrições ---------------------------------------------------------
        for (int i=0; i<n_vertices; i++)
        {
            IloExpr expr1(env);
            for (int j=0; j<n_vertices; j++)
            {
                expr1 += x_var[i][j];
            }
            IloConstraint c1(expr1 == 1);
            c1.setName("Restrição de atribuição");
            model.add(c1);
            expr1.end();
        }

        for (int i=0; i<n_vertices; i++)
        {
            for (int k=0; k<n_vertices; k++)
            {
                for (int j=0; j<n_vertices; j++)
                {
                    if (adj_matrix[i][k] == 1)
                    {
                        IloExpr expr2(env);
                        expr2 += x_var[i][j] + x_var[k][j];
                        IloConstraint c2(expr2 <= w_var[j]);
                        c2.setName("Restrição de conflito de cores");
                        model.add(c2);
                        expr2.end();
                    }
                }
            }
        }

        for (int j=0; j<n_vertices-1; j++)
        {
            IloExpr expr3(env);
            expr3 += w_var[j];
            IloConstraint c3(expr3 >= w_var[j+1]);
            c3.setName("Restrição de quebra de simetria");
            model.add(c3);
            expr3.end();
        }

        for (int j=0; j<n_vertices; j++)
        {
            IloExpr expr4(env);
            for (int i=0; i<n_vertices; i++)
            {
                expr4 += x_var[i][j];
            }
            IloConstraint c4(expr4 >= w_var[j]);
            c4.setName("Restrição válida para fortalecer");
            model.add(c4);
            expr4.end();
        }

        // verificando uso de memória
        cout << "Memory usage after creating constraints: " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

        // criando a função objetivo
        cout << "Criando função objetivo..." << endl;
        
        IloExpr obj(env);
        for (int j=0; j<n_vertices; j++)
        {
            obj += w_var[j];
        }
        model.add(IloMinimize(env, obj));
        obj.end();

        cout << "Função objetivo criada." << endl;
        
        // verificando uso de memória
        cout << "Memory usage after creating constraints: " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

        // resolvendo o modelo
        cout << "Extracting model..." << endl;
        cplex.extract(model);
        cout << "Model extracted!" << endl;
        cplex.solve();
        
        // printando tempo de execução
        env.out() << "Tempo de execução = " << timer.getTime() << endl;

        // printando status da otimização
        env.out() << "Solution status = " << cplex.getStatus() << endl;
        
        // printando valor obtido para a função objetivo
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;

        // exportando modelo
        //cplex.exportModel("modelo.lp");

        env.end();
    }
    catch (IloException& e) 
    {
        cerr << "Erro na chamada da API: " << e << endl;
        env.end();
    }
    catch (...) 
    {
        cerr << "Erro desconhecido" << endl;
        env.end();
    }
    return 0;
}