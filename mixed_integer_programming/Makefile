SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio1210/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio1210/concert

CCC = g++ -O0

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

# For dynamic linking
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIB      = cplex$(dynamic:yes=12100)
run           = $(dynamic:yes=LD_LIBRARY_PATH=$(CPLEXBINDIR))

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CCLNFLAGS = -lconcert -lilocplex -l$(CPLEXLIB) -lm -lpthread -ldl

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

execute:
	#$(CCC) -c $(CCFLAGS) cplex_exemplo.cpp -o cplex_exemplo.o
	#$(CCC) $(CCFLAGS) $(CCLNDIRS) -o cplex_exemplo cplex_exemplo.o $(CCLNFLAGS)
	#$(run) ./cplex_exemplo

	#$(CCC) -c $(CCFLAGS) knapsack_solver_CPLEX.cpp -o knapsack_solver_CPLEX.o
	#$(CCC) $(CCFLAGS) $(CCLNDIRS) -o knapsack_solver_CPLEX knapsack_solver_CPLEX.o $(CCLNFLAGS)
	#$(run) ./knapsack_solver_CPLEX

	$(CCC) -c $(CCFLAGS) solver_mestrado_CPLEX.cpp -o solver_mestrado_CPLEX.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o solver_mestrado_CPLEX solver_mestrado_CPLEX.o $(CCLNFLAGS)
	$(run) ./solver_mestrado_CPLEX