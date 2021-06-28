#ifndef _MINCUT_H_
#define _MINCUT_H_

double max_back_heuristic(int dim, double **graph, int *cutset);
double mincut(int dim, double **graph, int *cutset, int a);

#endif
