#include <iostream>
#include <fstream>
#include <string>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

using namespace std;

int main()
{
    IloEnv env;
    try
    {
        string filename="knapsack.dat";
        ifstream file;
        
        // reading data from file
        file.open(filename);
        if (!file)
        {
            cerr << "ERRO ao abrir o arquivo " << filename << endl;
        }
        IloInt n, cap_total;
        IloNumArray values(env), caps(env);
        file >> n >> cap_total;
        cout << n << " itens, and total capacity equal to " << cap_total << endl;
        file >> values >> caps;
        cout << "Values: " << values << endl;
        cout << "Weights: " << caps << endl;
        // end reading data

        // criando a variável de decisão
        IloNumVarArray x(env, n, 0, 1, ILOINT);
        
        // criando expressão para a restrição
        IloExpr expr1(env); 
        for (int i = 0; i < n; ++i) 
        {
            expr1 += caps[i] * x[i];
        }
        // criando a restrição 1
        IloRange r1(env, 0, expr1, cap_total);
        // deletando expressão
        expr1.end();

        // criando expressão para a função objetivo
        IloExpr expr2(env); 
        for (int i = 0; i < n; ++i) 
        {
            expr2 += values[i] * x[i];
        }
        // criando a função objetivo
        IloObjective obj(env, expr2, IloObjective::Maximize);
        // deletando expressão
        expr2.end();

        // criando o modelo e adicionando restrições e FO
        IloModel model(env);
        model.add(r1);
        model.add(obj);

        // declarando o otimizador
        IloCplex cplex(model);
        // resolvendo o modelo
        cplex.solve();
        
        // printando status da otimização
        env.out() << "Solution status = " << cplex.getStatus() << endl;

        // printando valor obtido para a função objetivo
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;

        // delcarando objeto que vai conter valor das variáveis de decisão
        IloNumArray vals(env);
        
        // pegando valores atribuídos às variáveis de decisão
        cplex.getValues(vals, x);
        env.out() << "Values        = " << vals << endl;
        
        // exportando modelo
        cplex.exportModel("knapsack_model_CPLEX.lp");

        // closing file
        file.close();
    }
    catch (IloException& e) 
    {
        cerr << "Erro na chamada da API: " << e << endl;
    }
    catch (...) 
    {
        cerr << "Erro desconhecido" << endl;
    }
    env.end();
    return 0;
}