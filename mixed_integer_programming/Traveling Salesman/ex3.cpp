#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void usage (const char *progname);

int
main (int argc, char **argv)
{
   IloEnv env;               // ambiente
   try {
      IloModel model(env);   // modelo
      IloCplex cplex(env);   // solver

      if ( argc != 2 ) {
         usage (argv[0]);
         throw(-1);
      }

      IloObjective   obj;        // Variavel que define um objetivo
      IloNumVarArray var(env);
      IloRangeArray  rng(env);

      cplex.importModel(model, argv[1], obj, var, rng); // importa um modelo do arquivo arg[1], e preenche modelo, objetivo, variaveis e limites das variaveis

      cplex.extract(model);      // "linka" o solver ao modelo
      cplex.solve();             // aplica solver ao modelo
      
      env.out() << "Solution status = " << cplex.getStatus() << endl;   // retorna status da solucao, pode ser :
                                                                        // IloAlgorithm::Optimal
                                                                        // IloAlgorithm::Unknow
                                                                        // IloAlgorithm::Infeasible
                                                                        // IloAlgorithm::Unbounded
                                                                        // IloAlgorithm::Error
      
      env.out() << "Solution value  = " << cplex.getObjValue() << endl; // retorna solução encontrada

      IloNumArray vals(env);
      cplex.getValues(vals, var);                                       // retorna o valor das variaveis
      env.out() << "Values        = " << vals << endl;
   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
   }

   env.end();
   return 0;
}  // END main


static void usage (const char *progname)
{
   cerr << "Usage: " << progname << " filename" << endl;
   cerr << "   where filename is a file with extension " << endl;
   cerr << "      MPS, SAV, or LP (lower case is allowed)" << endl;
   cerr << " Exiting..." << endl;
} // END usage

