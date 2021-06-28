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
        
        filename="./TPI_F_2.txt";
        
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
        
        IloInt n_facilidades, n_clientes, custo_facilidade, recursos_facilidade, n_linhas;

        file >> n_facilidades >> n_clientes >> custo_facilidade >> recursos_facilidade >> n_linhas;

        NumMatrix2D g(env, n_facilidades), p(env, n_facilidades);
        
        for (int i=0; i<n_facilidades; i++)
        {
            g[i] = IloNumArray(env, n_clientes);
            p[i] = IloNumArray(env, n_clientes);
            for (int j=0; j<n_clientes; j++)
            {
                g[i][j] = 9999;
                p[i][j] = 9999;
            }
        }

        cout << "A instância tem: " << endl;
        cout << n_facilidades << " facilidades, " << endl;
        cout << n_clientes << " clientes, " << endl;
        cout << "custo por facilidade de " << custo_facilidade << ", " << endl;
        cout << "recursos por facilidade de " << recursos_facilidade << ", " << endl;
        cout << "e " << n_linhas << " linhas" << endl;

        string line;
        int line_count = 0;

        while (getline(file, line))
        {
            istringstream iss(line);
            
            if (line_count != 0)
            {

                IloInt facilidade, cliente;
                IloNum g_temp, p_temp;

                //cout << line << endl;
                
                if (!(iss >> facilidade >> cliente >> g_temp >> p_temp)) 
                {
                    break;
                }
                
                g[facilidade - 1][cliente - 1] = g_temp;
                p[facilidade - 1][cliente - 1] = p_temp;

                //cout << " g = " << g_temp << " e p = " << p_temp << endl;

            }

            line_count += 1;

        }

        // end reading data
        
        cout << "g =" << g << endl;
        cout << "p =" << p << endl;

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

        IloBoolVarArray y_var(env, n_facilidades);

        BoolVarMatrix2D x_var(env, n_facilidades);
        for (int i=0; i<n_facilidades; i++)
        {
            x_var[i] = IloBoolVarArray(env, n_clientes);
        }

        cout << "Variáveis de decisão criadas." << endl;
        
        // verificando uso de memória
        cout << "Memory usage after creating constraints: " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

        // criando restrições ---------------------------------------------------------
        for (int j=0; j<n_clientes; j++)
        {
            IloExpr expr1(env);
            for (int i=0; i<n_facilidades; i++)
            {
                expr1 += x_var[i][j];
            }
            IloConstraint c1(expr1 == 1);
            c1.setName("Restrição 1");
            model.add(c1);
            expr1.end();
        }

        for (int i=0; i<n_facilidades; i++)
        {
            for (int j=0; j<n_clientes; j++)
            {
                IloExpr expr2(env);
                expr2 += x_var[i][j];
                IloConstraint c2(expr2 <= y_var[i]);
                c2.setName("Restrição 2");
                model.add(c2);
                expr2.end();
            }
        }
        
        for (int i=0; i<n_facilidades; i++)
        {
            IloExpr expr3(env);
            for (int j=0; j<n_clientes; j++)
            {
                expr3 += (p[i][j] * x_var[i][j]);
            }
            IloConstraint c3(expr3 <= (recursos_facilidade * y_var[i]));
            c3.setName("Restrição 3");
            model.add(c3);
            expr3.end();
        }

        // verificando uso de memória
        cout << "Memory usage after creating constraints: " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

        // criando a função objetivo
        cout << "Criando função objetivo..." << endl;
        
        IloExpr obj(env);
        for (int i=0; i<n_facilidades; i++)
        {
            obj += custo_facilidade * y_var[i];
            for (int j=0; j<n_clientes; j++)
            {
                obj += g[i][j] * x_var[i][j];
            }
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