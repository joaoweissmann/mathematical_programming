Forma de uso:

    - abrir terminal
    - ir até o diretório com os arquivos
    - executar "make" ou "make run"
    - escolher entre duas opções:
        -- 1 para rodar com o branch and cut feito
        -- 0 para rodar sem o branch and cut
    - executar "make clean" para limpar arquivos gerados
    
    * para mudar a instância, editar o arquivo "graph_coloring_branch_and_cut.cpp"

Resultados obtidos, por instância:

- TPI_BC_COL_0.txt ( sem branch and cut)

    Total (root+branch&cut) =    3.94 sec. (3812.90 ticks)
    Tempo de execução = 13.61
    Solution status = Optimal
    Solution value  = 5
    Número de nodes percorridos  = 63
    Número cortes gerados  = 0

- TPI_BC_COL_0.txt ( com branch and cut)

    Total (root+branch&cut) =    0.62 sec. (881.21 ticks)
    Tempo de execução = 0.63
    Solution status = Optimal
    Solution value  = 5
    Número de nodes percorridos  = 0
    Número cortes gerados  = 625

- TPI_BC_COL_1 ( sem branch and cut)

    Total (root+branch&cut) = 1653.19 sec. (429200.01 ticks)
    Tempo de execução = 10933.5
    Solution status = Optimal
    Solution value  = 6
    Número de nodes percorridos  = 39222
    Número cortes gerados  = 0

- TPI_BC_COL_1 (com branch and cut)

    Total (root+branch&cut) =  390.79 sec. (804253.85 ticks)
    Tempo de execução = 390.14
    Solution status = Optimal
    Solution value  = 6
    Número de nodes percorridos  = 755
    Número cortes gerados  = 1920

- TPI_BC_COL_2 (sem branch and cut) - limite de 2hrs

    Total (root+branch&cut) = 7227.73 sec. (653971.27 ticks)
    Tempo de execução = 48829.5
    Solution status = Feasible
    Solution value  = 10
    Número de nodes percorridos  = 41243
    Número cortes gerados  = 0
    GAP   40.71%

- TPI_BC_COL_2 (com branch and cut) - limite de 2hrs

    Total (root+branch&cut) = 7200.23 sec. (5505522.84 ticks)
    Tempo de execução = 7186.3
    Solution status = Feasible
    Solution value  = 10
    Número de nodes percorridos  = 1110
    Número cortes gerados  = 15808
    GAP 20.00%

