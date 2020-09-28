#include <iostream>

// importar CPLEX
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

using namespace std;

int main()
{
    // declarar environment
    IloEnv env;
    
    try
    {
        // declarar o modelo de otimização
        IloModel model(env);
        // declarar variaveis
        IloNumVarArray var(env);
        // declarar restrições
        IloRangeArray con(env);
        // declarar objetivo
        IloObjective obj(env);

        // preencher variáveis
        var.add(IloNumVar(env, 0.0, 40.0, ILOFLOAT));
        var.add(IloNumVar(env));
        var.add(IloNumVar(env));
        var.add(IloNumVar(env, 2.0, 3.0, ILOINT));
        
        // preencher restrições
        con.add( - var[0] +     var[1] + var[2] + 10 * var[3] <= 20);
        con.add(   var[0] - 3 * var[1] + var[2]             <= 30);
        con.add(                var[1]          - 3.5* var[3] == 0);
        model.add(con);

        // preencher objetivo
        obj = IloMaximize(env, var[0] + 2 * var[1] + 3 * var[2] + var[3]);
        model.add(obj);

        // declarar o otimizador que resolverá o modelo
        IloCplex cplex(model);

        // resolver o modelo
        cplex.solve();

        // printando status da otimização
        env.out() << "Solution status = " << cplex.getStatus() << endl;

        // printando valor obtido para a função objetivo
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;

        // delcarando objeto que vai conter valor das variáveis de decisão
        IloNumArray vals(env);
        
        // pegando valores atribuídos às variáveis de decisão
        cplex.getValues(vals, var);
        env.out() << "Values        = " << vals << endl;
        
        // exportando modelo
        cplex.exportModel("nome.lp");

    }
    catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }

    // finaliza o environment
    env.end();
    return 0;
}