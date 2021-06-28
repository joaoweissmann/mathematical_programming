#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

#define PRECISAO 0.00001

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix2D;
typedef IloArray<NumVarMatrix2D> NumVarMatrix3D;

typedef IloArray<IloBoolVarArray> BoolVarMatrix2D;
typedef IloArray<BoolVarMatrix2D> BoolVarMatrix3D;

typedef IloArray<IloNumArray> NumMatrix2D;
typedef IloArray<NumMatrix2D> NumMatrix3D;

ILOUSERCUTCALLBACK7(CliqueCut, BoolVarMatrix2D, x, int, dim, double**, residual, int*, cutset, int*, numc, 
                                             NumMatrix2D, adj_matrix, IloBoolVarArray, w)
{
    // x            -> vetor das variaveis do problema (se vértice i recebe cor j)
    // dim          -> dimensao do problema (numero de cidades)
    // residual     -> armazena grafo com a solucao corrente
    // cutset       -> conjunto S a ser encontrado
    // numc         -> contador de numero de cortes gerados
    // adj_matrix   -> matriz de adjacência do grafo
    // w            -> vetor com as variáveis booleanas de uso de cada cor

    IloEnv env  = getEnv();
    bool DEPU = false;

    if (DEPU) cout << "Criando grafo residual..." << endl;
    // cria grafo da solucao corrente fracionaria 
    for(int i=0; i<dim; i++) 
    {
        for(int j=0; j<dim; j++) 
        {
            residual[i][j] = 0;
        }
    }

    for(int i=0; i<dim; i++) 
    {
        for(int j=0; j<dim; j++) 
        {
            float x_ij = getValue(x[i][j]);

            if (x_ij < PRECISAO) x_ij = 0;

            residual[i][j] = x_ij;
        }
    }
    if (DEPU) cout << "Grafo residual criado." << endl;

    // método heuristico para resolver problema de separação =========================================
    // achar clique S maximal

    for (int k=0; k<10; k++)
    {
        if (DEPU) cout << "Inicializando conjunto S" << endl;
        // onde o corte será armazenado (conjunto S)
        for(int i=0; i < dim; i++)
        {
            cutset[i] = 0;
        }
        if (DEPU) cout << "Conjunto S inicializado" << endl;

        // escolher vértice v inicial
        int v;
        v = rand() % dim; // escolha aleatória
        if (DEPU) cout << "Vértice inicial escolhido: " << v << endl;

        // S = {v}
        cutset[v] = 1;
        int cut_size = 1;

        if (DEPU) cout << "Inicializando lista L de vértices a serem visitados..." << endl;
        // inicializa lista L de vértices a serem verificados: N(v)
        vector <int> L;
        for (int i=0; i<dim; i++)
        {
            if (adj_matrix[v][i] == 1)
            {
                L.push_back(i);
            }
        }
        if (DEPU) cout << "Lista L inicializada." << endl;
        if (DEPU) cout << "Lista: ";
        for (int el : L) if (DEPU) cout << el << " ";
        if (DEPU) cout << endl;
        if (DEPU) cout << "Tamanho: " << L.size() << endl;

        if (DEPU) cout << "Rodando heurística para encontrar clique maximal..." << endl;
        int v_new, v_new_pos;
        // enquanto L não estiver vazia
        while (L.size() > 0)
        {
            // escolher v' em L
            v_new_pos = rand() % L.size();
            v_new = L[v_new_pos];

            // L = L\{v'}
            L.erase(L.begin() + v_new_pos);

            // se v' estiver conectado a todos os vértices em S
            bool flag = true;
            for (int i=0; i<dim; i++)
            {
                if (cutset[i] == 1)
                {
                    if (adj_matrix[v_new][i] == 0) 
                    {
                        flag = false;
                        break;
                    }
                }
            }

            if (flag == true)
            {
                // S = S U {v'}
                cutset[v_new] = 1;
                cut_size++;

                // L = N(v')
                L.clear();
                for (int i=0; i<dim; i++)
                {
                    if (adj_matrix[v_new][i] == 1)
                    {
                        L.push_back(i);
                    }
                }
            }
        }
        if (DEPU) cout << "Heurística concluída." << endl;
        
        if (DEPU)
        {
            cout << "Clique de tamanho " << cut_size << " encontrada: ";
            for (int i=0; i<dim; i++)
            {
                if (cutset[i] == 1) cout << i << " ";
            }
            cout << endl;
        }

        if (DEPU)
        {
            // clique válida?
            bool flag_clique = true;
            for (int i=0; i<dim; i++)
            {
                if (cutset[i] == 1)
                {
                    for (int j=0; j<dim; j++)
                    {
                        if (cutset[j] == 1)
                        {
                            if (i != j)
                            {
                                if (adj_matrix[i][j] == 0) flag_clique = false;
                            }
                        }
                    }
                }
            }
            if (flag_clique == true) cout << "Clique encontrada é válida." << endl;
            if (flag_clique == false) cout << "Clique encontrada é inválida." << endl;
        }

        if (DEPU) cout << "Verificando se clique está violada..." << endl;
        // verificar se viola corte clique
        double cutvalue;
        for(int j=0; j<dim; j++) 
        {
            cutvalue = 0;
            for(int i=0; i<dim; i++) 
            {
                if (cutset[i] == 1)
                {
                    cutvalue += residual[i][j];
                }
            }
            if (cutvalue > 1 + PRECISAO) break;
        }
        if (DEPU) cout << "Verificação concluída." << endl;

        if (DEPU)
        {
            if (cutvalue > 1 + PRECISAO) cout << "Clique violada." << endl; 
            else cout << "Clique não violada." << endl;
        }

        // ================================================================================================

        if (DEPU) cout << "Gerando o corte se a clique estiver violada..." << endl;
        // gera corte
        if( (cutvalue > 1 + PRECISAO) && (cut_size > 2) )
        {
            for(int j=0; j<dim; j++) 
            {
                IloExpr corte(env);
                for(int i=0; i<dim; i++) 
                {
                    if (cutset[i] == 1)
                    {
                        corte += x[i][j];
                    }
                }
                add(corte <= w[j]).end();
                *numc = *numc + 1;
                corte.end();
            }
        }
        if (DEPU) cout << "Geração de cortes finalizada." << endl;
    }

    double ub     = getIncumbentObjValue();  // retorna a melhor solucao inteira (limite primal)
    double lb     = getBestObjValue();       // retorna o melhor limite dual
    double rlx    = getObjValue();           // quando chamada dentro do callback, retorna o valor da relaxacao do noh
    double nNodes = getNremainingNodes();    // retorna o numero restante de nos a serem analisados
    cout << "--- CUT:" << "relax=" << rlx << "\t bounds=" << lb << "<->" << ub<< "\t n_rest=" << nNodes << "\t Ucuts=" << *numc << endl;

}

int main()
{
    cout << "Criando ambiente CPLEX..." << endl;
    IloEnv env;
    cout << "Ambiente CPLEX criado." << endl;

    try
    {
        string filename;
        
        filename="./TPI_BC_COL_1.txt";
        
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

        // cplex.setParam(IloCplex::Param::MIP::Interval, 100); // Log a cada N nos

        cplex.setParam(IloCplex::Param::Preprocessing::Presolve, 0); // desliga o preprocessamento, 1-ligado 0-desligado
        cplex.setParam(IloCplex::Param::MIP::Limits::CutsFactor, 0); // número de cortes que o CPLEX pode gerar, 0-desliga os cortes
        cplex.setParam(IloCplex::Param::MIP::Strategy::HeuristicFreq, -1); // heuristicas primais do CPLEX, -1-desliga 0-liga

        //cplex.setParam(IloCplex::Param::RootAlgorithm, 0); // 1: primal, 2: dual, 4: barrier, 6: concurrent
        //cplex.setParam(IloCplex::Param::NodeAlgorithm, 0); // 1: primal, 2: dual, 4: barrier, 6: concurrent
        //cplex.setParam(IloCplex::Param::MIP::SubMIP::SubAlg, 4); // 1: primal, 2: dual, 4: barrier
        //cplex.setParam(IloCplex::Param::MIP::SubMIP::StartAlg, 4); // 1: primal, 2: dual, 4: barrier
        
        //cplex.setParam(IloCplex::Param::MIP::Strategy::Probe, 3); // 1: moderado, 2: agressivo, 3: muito agressivo
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

        // estrutura usada para os métodos de geracao de cortes
        double **residual = new double * [n_vertices];
        for (int i=0; i<n_vertices; i++)
            residual[i] = new double [n_vertices];
        int *cutset = new int[n_vertices];
        int num_u=0;

        // dizendo para o solver usar o corte de usuário
        int opcao;
        cout << "Você gostaria de rodar o solver com o corte clique (1) ou sem (0)? Escolha dentre as opções {0,1}" << endl;
        cin >> opcao;
        if (opcao)
        {
            cplex.use(CliqueCut(env, x_var, n_vertices, residual, cutset, &num_u, adj_matrix, w_var));
        }

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

        env.out() << "Número de nodes percorridos  = " << cplex.getNnodes() << endl;

        env.out() << "Número cortes gerados  = " << num_u << endl;

        // exportando modelo
        //cplex.exportModel("modelo.lp");

        env.end();

        // libera memoria
        delete [] cutset;
        for (int i=0; i < n_vertices; i++) 
            delete [] residual[i];
        delete [] residual;

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