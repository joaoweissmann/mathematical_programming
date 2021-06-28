################################

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio1210/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio1210/concert

################

# Compilador 
CCC = g++ -O0

# Opcoes de compilacao
CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes

# Bibliotecas e includes
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIB      = cplex$(dynamic:yes=2010)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CCLNFLAGS = -lconcert -lilocplex -l$(CPLEXLIB) -lm -lpthread -ldl

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

all : ex1 ex2 ex3 tsp

ex1: ex1.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o ex1 ex1.o $(CCLNFLAGS)
ex1.o: ex1.cpp
	$(CCC) -c $(CCFLAGS) ex1.cpp -o ex1.o

ex2: ex2.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o ex2 ex2.o $(CCLNFLAGS)
ex2.o: ex2.cpp
	$(CCC) -c $(CCFLAGS) ex2.cpp -o ex2.o

ex3: ex3.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o ex3 ex3.o $(CCLNFLAGS)
ex3.o: ex3.cpp
	$(CCC) -c $(CCFLAGS) ex3.cpp -o ex3.o

tsp: tsp.o data.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) tsp.o data.o -o tsp $(CCLNFLAGS)
tsp.o: tsp.cpp
	$(CCC) -c $(CCFLAGS) tsp.cpp -o tsp.o
data.o: data.cpp
	$(CCC) -c $(CCFLAGS) data.cpp -o data.o

clean:
	rm --force ex1 ex1.o ex2 ex2.o ex3 ex3.o tsp tsp.o data.o
