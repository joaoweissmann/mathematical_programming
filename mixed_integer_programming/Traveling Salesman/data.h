#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <cstdlib>

using namespace std;

   class Data{
      public:
         Data(int, char *);
         ~Data();
         
         static int dim;
         void readData();
         inline int getDimension(){ return dimension; };
         inline double getDistanceMatrix(int i, int j){ return distMatrix[i][j]; };
         inline double** getCostMatrix(){ return distMatrix; };
         
         static string getInstanceName( string );                    // Get instance's name   
         
      private:
         string instanceName;
         int nbOfPar;

         int dimension;
         
         double **distMatrix;
         double *xCoord, *yCoord;
         
         //Computing Distances
			static double CalcDistEuc ( double *, double *, int , int );
			static double CalcDistAtt ( double *, double *, int , int );
			static double CalcDistGeo ( double *, double *, int , int );
			static void CalcLatLong ( double *, double *, int , double *, double* );    
   };
   
#endif
