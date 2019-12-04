                                                                                                      
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#include <time.h>
#include <sys/time.h>
#define N 20        /* number of rows and columns in matrix */

MPI_Status status;

float data[N][N],newdata[N][N],temp[N][N];

main(int argc, char *argv[])
{
  int numtasks,taskid,numworkers,source,dest,rows,offset,i,j,k,count;
  float difference=0.0011, fault=0.001;
  int sizeI= 16384, sizeJ=16384;
  



  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  numworkers = numtasks-1;

  /*---------------------------- master ----------------------------*/
  if (taskid == 0) {
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
        data[i][j]=0.1*(i+j);
      }
    }

   
    /* send matrix data to the worker tasks */
    rows = (N/numworkers);
    count = 0;
    
      while(difference>fault){
//      for (count=0;count<10;count++){
      offset = 0;
    
      for (dest=1; dest<=numworkers;dest++)
      {  
        if(dest==1){
        MPI_Send(&offset, 1, MPI_INT, dest,count+1, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, dest, count+1, MPI_COMM_WORLD);
        MPI_Send(&data[offset][0], (rows+1)*N, MPI_DOUBLE,dest,count+1, MPI_COMM_WORLD);
        }
        if(dest>1 && dest<numworkers){
        MPI_Send(&offset, 1, MPI_INT, dest, count+1, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, dest, count+1, MPI_COMM_WORLD);
        MPI_Send(&data[offset-1][0], (rows+2)*N, MPI_DOUBLE,dest, count+1, MPI_COMM_WORLD);
        }
        else{ 
        MPI_Send(&offset, 1, MPI_INT, dest, count+1, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, dest, count+1, MPI_COMM_WORLD);
        MPI_Send(&data[offset-1][0], (rows+1)*N, MPI_DOUBLE,dest, count+1, MPI_COMM_WORLD);
        }
        offset = offset + rows;
      }    


     /* wait for results from all worker tasks */
      for (source=1; source<=numworkers; source++)
      {
        MPI_Recv(&offset, 1, MPI_INT, source, count+2, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, source, count+2, MPI_COMM_WORLD, &status);
        MPI_Recv(&newdata[offset][0], rows*N, MPI_DOUBLE, source, count+2, MPI_COMM_WORLD, &status);

      }

 //   gettimeofday(&stop, 0);


/****** finding differnce matrix ***********/
//        maxdiff = 0.0;
//        for(i=0;i<N;i++)
//           for(j=0;j<N;j++){
//                diff[i][j]=fabs(c[i][j]-a[i][j]);
      
//                if(maxdiff<diff[i][j])
//                    maxdiff = diff[i][j];
//            }

        
        difference = fabs(newdata[1][1]-data[1][1]);
        printf("next iteration: %lf,%lf,%lf\n", difference,newdata[1][1],data[1][1]);
        
        for(i=0;i<N;i++)
            for(j=0;j<N; j++)
                data[i][j]=newdata[i][j];

        
//        printf("max differnece : %6.2f\n", difference);
//        printf("Here is the matrix modified after cycle :%d\n",count);
//        for(i=0;i<N;i++){
//            for(j=0;j<N; j++)
//               printf("%6.2f   ", data[i][j]);
//            printf("\n");
//        }
        count++;
 //   fprintf(stdout,"Time = %.6f\n\n",
 //        (stop.tv_sec+stop.tv_usec*1e-6)-(start.tv_sec+start.tv_usec*1e-6));
        
  }
  }  
  /*---------------------------- worker----------------------------*/
  /***************************  first worker **************************/
  if (taskid ==0){
    source = 0;

    for(count=0;count<1000;count++){
    MPI_Recv(&offset, 1, MPI_INT, source, count+1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, source, count+1, MPI_COMM_WORLD, &status);
    MPI_Recv(&data, (rows+1)*N, MPI_DOUBLE, source, count+1, MPI_COMM_WORLD, &status);
 
//        printf("the data received by worker %d\n",taskid);
//        for (i=0; i<rows; i++) {
//          for (j=0; j<N; j++)
//          printf("%6.2f   ", a[i][j]);
//        printf ("\n");
//        }
      
    /* Matrix multiplication */
    for (i=1; i<rows; i++)
      for (j=1; j<N-1;j++) {
//            c[i][j] = 0.25* a[i][j];
              newdata[i][j] = 0.125* (data[i-1][j-1]+data[i-1][j]+data[i-1][j+1]+data[i][j-1]+
                                      data[i][j+1]+data[i+1][j-1]+data[i+1][j]+data[i+1][j]);
      }

     
    for (i=1; i<rows; i++)
      for (j=0; j<N; j++) {
         if((j==0)||(j==N-1))
            newdata[i][j] = data[i][j];
      }
 
    for (i=0; i<rows; i++)
      for (j=0; j<N; j++) {
         if((i==0))
            newdata[i][j] = data[i][j];
      }

    MPI_Send(&offset, 1, MPI_INT, 0, count+2, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, count+2, MPI_COMM_WORLD);
    MPI_Send(&newdata, rows*N, MPI_DOUBLE, 0, count+2, MPI_COMM_WORLD);
  }
  }

/********midle worker ********************/
  if (taskid >1 && taskid < numworkers) {
    source = 0;
    for(count=0;count<1000;count++){
    MPI_Recv(&offset, 1, MPI_INT, source, count+1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, source, count+1, MPI_COMM_WORLD, &status);
    MPI_Recv(&data, (rows+2)*N, MPI_DOUBLE, source, count+1, MPI_COMM_WORLD, &status);
    
//    printf("a[0][0]%6.2f\n",a[0][0]);
    /* Jacobi transformation */ 
    for (i=1; i<rows+1; i++)
      for (j=1; j<N-1;j++) {
//          c[i-1][j] = 0.25* a[i][j];
            newdata[i-1][j] = 0.125* (data[i-1][j-1]+data[i-1][j]+data[i-1][j+1]+data[i][j-1]+data[i][j+1]+
                             data[i+1][j-1]+data[i+1][j]+data[i+1][j]);

      }

     
    for (i=1; i<rows+1; i++)
      for (j=0; j<N;j++) {
         if((j==0)||(j==N-1))
            newdata[i-1][j] = data[i][j];
      }
 

    MPI_Send(&offset, 1, MPI_INT, 0, count+2, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, count+2, MPI_COMM_WORLD);
    MPI_Send(&newdata, rows*N, MPI_DOUBLE, 0, count+2, MPI_COMM_WORLD);
  }
  }
/**********last worker ********/

  if (taskid = numworkers) {
    source = 0;
    for(count=0;count<1000;count++){
    MPI_Recv(&offset, 1, MPI_INT, source, count+1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, source,count+1, MPI_COMM_WORLD, &status);
    MPI_Recv(&data, (rows+1)*N, MPI_DOUBLE, source, count+1, MPI_COMM_WORLD, &status);


    /* Jacobi transformation */
    for (i=1; i<rows; i++)
      for (j=1; j<N-1;j++) {
//              c[i][j]= 0.1*a[i][j];
            newdata[i-1][j] = 0.125* (data[i-1][j-1]+data[i-1][j]+data[i-1][j+1]+data[i][j-1]+data[i][j+1]+
                             data[i+1][j-1]+data[i+1][j]+data[i+1][j]);
      }

     
    for (i=1; i<rows; i++)
      for (j=0; j<N;j++) {
         if((j==0)||(j==N-1))
            newdata[i-1][j] = data[i][j];
      }
 
   for (i=1; i<rows+1; i++)
      for (j=0; j<N;j++) {
         if((i==rows))
            newdata[i-1][j] = data[i][j];
      }

    MPI_Send(&offset, 1, MPI_INT, 0, count+2, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, count+2, MPI_COMM_WORLD);
    MPI_Send(&newdata, rows*N, MPI_DOUBLE, 0, count+2, MPI_COMM_WORLD);
  }
  }
  MPI_Finalize();

}

