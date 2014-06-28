#include <stdio.h>
#include "CanA_3D_Structure.h"

void update_3Dcanopy_structure(double **canopy3Dstructure,double canparms, int nrow, int ncol)
{
  /*************************************************************************************
   * Purpose:
   * purpose of this function is to obtain a matrix which represents 3D canopy structure, along with some leaf properties
   * this structure will be used by ray tracing program to calculate light profile of the canopy
   * 
   * Input
   * struct Can3DParms, this contain all the parameters such as leaf curvatures, leaf angel, etc to construct canopy
   * struct LAI contains leaf area index
   * 
   * nrow: number of rows or number of triangles in the 3D canopy structure
   * ncol: =20 number of columns in the matri ..18 for inputs and two additional for outputs (PPFD and cLAI)
   * 
   * Output
   * canopy3Dstructure is a 2D matrix
   * Number of Rows: Number of triangular elements to represent canopy structure
   * Number of Columns:16
   *  Column1 :- X coordinate of 1st vertex of triangle
   *  Column2 :- Y coordinate of 1st vertes of triangle
   *  Column3 :- Z coordinate of 1st vertex of triangle
   *  Column4 :- X coordinate of 2nd vertex of triangle
   *  Column5 :- Y coordinate of 2nd vertes of triangle
   *  Column6 :- Z coordinate of 2nd vertex of triangle
   *  Column7 :- X coordinate of 3rd vertex of triangle
   *  Column8 :- Y coordinate of 3rd vertes of triangle
   *  Column9 :- Z coordinate of 3rd vertex of triangle
   *  Column10:- Leaf ID 
   *  Column11:- Leaf Length
   *  Column12:- Position of the triangle from leaf base (cm)
   *  Column13:- Plant column ID (in a rectangular N-S-E-W domain ??)
   *  Column14:- Plant row ID  (in a rectangular N-S-E-W domain ??)
   *  Column15:-SPAD
   *  Column16:- Kt (0-1) transmission coefficient 
   *  Column17:- Kr (0-1) reflectance coefficient
   *  Column18:  Nitrogen content per unit area
   * ***********************************************************************************/
   int i,j;
   double num;
   char filename[] = "/home/djaiswal/Research/R-dev/biocro/inst/extdata/CM_SC.txt"; 
   
   if(ncol!=18)
   {
     printf("Number of columns of the 3D canopy structure is not 18\n");
     return;
   }
   
   // This is temporary solution--reading output to our matrix, this needs to be replaced by actual function after meka completes his task
   
   FILE *fp = fopen(filename, "r");   
   if(fp==NULL)
   { 
    printf("\n Can't find file to read canopy structure\n");
 return;
   }
   
   for (i=0;i<nrow;i++)
    {
      for (j=0;j<ncol;j++)
       {
         fscanf(fp, "%lf", &num);
         canopy3Dstructure[i][j] =num;
         printf("%d, %d, %f\n",i,j,canopy3Dstructure[i][j]);
       }
    }
   fclose(fp);
   return;
}