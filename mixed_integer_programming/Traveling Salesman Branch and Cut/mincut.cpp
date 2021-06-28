#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <string>
#include "data.h"
#include "mincut.h"

using namespace std;

// Max-Back Heuristic for MinCut which returns the value of cut obtained
double max_back_heuristic(int dim, double **graph, int *cutset) {
   
   vector<int> S; // cutset visited on the heuristic  
   vector<int> Smin; // current mincut
   int S0 = 0; // init vertex  
   double Cutmin = 0, Cutval = 0;
   double b[dim]; // max-back value for each vertex	
   double eps = 1e-08;
   
   double max_back_aux = -1;
   int v; // vertex with maximum max-back value

   for(int i=0; i < dim; i++) { 
      Cutmin += graph[S0][i]; 
      b[i] = graph[S0][i]; // calculates max-back initial 
      if(b[i] > max_back_aux) {
         max_back_aux = b[i];
         v = i;
      }  
   }  

   Cutval = Cutmin;
   S.push_back(S0);
   Smin.push_back(S0);

   int n = 1; // number of nodes visited

   //cout << "v = " << v << endl;

   //for(int i=0; i < dim; i++)
     //cout << "b["<<i<<"] = "<< b[i] << endl;

   while(n < dim - 1) {
      
      S.push_back(v);
      
      Cutval = Cutval + 2 - 2 * b[v];
      
      //cout << "Cutval : " << Cutval << endl;

      if(Cutval < (Cutmin - eps)) {
         Cutmin = Cutval;
        // cout << "Cutmin : " << Cutmin << endl;
         Smin = S;
      }  
      
      max_back_aux = -1; // clean max_back_aux	

      n++;
      if(n == dim - 1) continue;
      int v_aux = -1;

      // Update vector b
      for(int i=0; i < dim; i++) {
         if(find(S.begin(), S.end(), i) != S.end()) // checking if S contains the element i
            continue;

         b[i] = b[i] + graph[i][v];
         if(b[i] > max_back_aux) {
            max_back_aux = b[i];
            v_aux = i;
         } 
      }
      v = v_aux;
      //cout << "v = " << v << endl;
      //for(int i=0; i < dim; i++)
        // cout << "b["<<i<<"] = "<< b[i] << endl;
   }
   
   // Actives flag for cutset nodes
   for(int i=0; i < Smin.size(); i++)
      cutset[Smin[i]] = 1;

   return Cutmin;

}

class Node {
   public:     
      int index;
      double value;  
};

class Head {
   public:
      void createListAdj(int dim) { listAdj = new Node[dim]; this->dim = dim; }
      
      bool status = true; // true ativo, false inativo 
      vector<int> nodes;
      int dim;
      Node *listAdj;    
};

void merge(int dim, Head *heads, int s, int t) {
   
   if(s > t) {
      int aux = s;
      s = t;
      t = aux;
   }
   
   // copia conjunto de vertices cluesterizados em t para o cluster de s
   for(int i=0; i < heads[t].nodes.size(); i++) {
      heads[s].nodes.push_back(heads[t].nodes[i]);
   }

   for(int i=0; i < dim; i++) {
      if(i == s) continue;
      
      if(heads[i].status == false) continue;

      if(i == t) {
         heads[s].listAdj[t].value = 0;
         heads[t].listAdj[s].value = 0;
         continue;
      }

      heads[s].listAdj[i].value += heads[t].listAdj[i].value;
      heads[i].listAdj[s].value = heads[s].listAdj[i].value;

      heads[t].listAdj[i].value = 0;
      heads[i].listAdj[t].value = 0;

   }

   heads[t].status = false;

}

double mincut_phase(int dim, Head *heads, int *cutset, int a, int size) {
   
   if(size == 2) {
   
      double cut_t = 0;
      
      for(int i=0; i < dim; i++) 
      	cut_t += heads[a].listAdj[i].value;
      
      for(int i=0; i < dim; i++)
         cutset[i] = 1;
         
      for(int i=0; i < heads[a].nodes.size(); i++) {
      	cutset[heads[a].nodes[i]] = 0;
      } 

   	return cut_t;
   
   }
   
   double b[dim]; // corte de i com respeito ao subconjunto corrente
   vector<int> order; // lista com ordem de vetores capturados 
   int count = 0;
   int v = a;

   order.push_back(a);  
 
   for(int i=0; i < dim; i++) 
      b[i] = 0;
   
   double max = -1;
   int v_aux = -1;

   while(count < size - 1) {

      for(int i=0; i < dim; i++) {

         if(heads[i].status == false) continue;

         if(find(order.begin(), order.end(), i) != order.end()) // checking if order contains the element i
            continue;

         b[i] += heads[v].listAdj[i].value;
         if(b[i] > max) {
            v_aux = i;
            max = b[i];
         }

      } 

      v = v_aux;
      order.push_back(v);

      max = -1;
      v_aux = -1;
      count++; 
   }

   int s,t;
   s = order[order.size() - 2];
   t = order[order.size() - 1];
	
   double cut_phase = 0;

   for(int i=0; i < dim; i++)
      cut_phase += heads[t].listAdj[i].value;
  
   for(int j=0; j < heads[t].nodes.size(); j++) { 
       cutset[heads[t].nodes[j]] = 1;
   }  
   
	merge(dim, heads, s, t); // contração do grafo entre os vertices s e t
   
   return cut_phase; 

}

double mincut(int dim, double **graph, int *cutset, int a) {
   
   double cut_phase = 99999999;
   int cutsetAux[dim];
   int num_visited = dim;   

   Head heads[dim];

   for(int i=0; i < dim; i++) {
      heads[i].createListAdj(dim);
      heads[i].nodes.push_back(i);
         
      for(int j=0; j < dim; j++) {
         heads[i].listAdj[j].index = j; // add index do nó  
         heads[i].listAdj[j].value = graph[i][j]; // add value da aresta entre i e j  
       
      }
       
      cutsetAux[i] = 0;
   }

   /*cout << endl; 
   for(int i=0; i < dim; i++) {
      for(int j=0; j < dim; j++) {
         cout << heads[i].listAdj[j].value << " "; 
      }
      cout << endl;
   }
   cout << endl; */
   
   double cutaux = -1;
   while(num_visited > 1) {
      
     for(int i=0; i < dim; i++)
         cutsetAux[i] = 0;
 
      cutaux = mincut_phase(dim, heads, cutsetAux, a, num_visited);
      
      if(cutaux < cut_phase) {
         cut_phase = cutaux;
         for(int i=0; i < dim;i++) {
            cutset[i] = cutsetAux[i];
         }
      }

      num_visited--;

   }  
  
  	for(int i=0; i < dim; i++)
  	   delete [] heads[i].listAdj;
  
   return cut_phase;
   
}



