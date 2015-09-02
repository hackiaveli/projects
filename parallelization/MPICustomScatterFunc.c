

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void myScatter(float *allData, float* gottenData,int count,MPI_Datatype datatype,MPI_Comm communicator,int pnum)
{


	

			int pid;
			MPI_Comm_rank(MPI_COMM_WORLD, &pid);

			switch(pid)
			{

				case 0:
				
				{

					// PHASE 1
					// 	zero sends to 2 
					// 		2 -> keep 2
					// 		3
					// 		6
					// 		7
					// 		10
					// 		11
					// 		14
					// 		15
					// printf("allData of 10 is %f\n", allData[10]);
					int i;
					for(i=0;i<262144;i++)
					{
						gottenData[i]=allData[i];
					}

						float *sender1;
						sender1 = (float*)malloc( sizeof(float) * 2048 * 128 );
						

						float *sender2;
						sender2 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender3;
						sender3 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender4;
						sender4 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender5;
						sender5 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender6;
						sender6 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender7;
						sender7 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender8;
						sender8 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender9;
						sender9 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender10;
						sender10 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender11;
						sender11 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender12;
						sender12 = (float*)malloc( sizeof(float) * 2048 * 128 );


						float *sender13;
						sender13 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender14;
						sender14 = (float*)malloc( sizeof(float) * 2048 * 128 );

						float *sender15;
						sender15 = (float*)malloc( sizeof(float) * 2048 * 128 );


						//send 2
						int m;
						int pos=0;
						for(m=262144*2;m<(262144*(3));m++)

								{
									sender2[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender2,128*2048,MPI_FLOAT,2,2,MPI_COMM_WORLD); //send 2
						//send 3
						pos=0;
						for(m=262144*3;m<(262144*(4));m++)
								{
									sender3[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender3,128*2048,MPI_FLOAT,2,3,MPI_COMM_WORLD); //send 3		

						
						//send 6
						pos=0;
						for(m=262144*6;m<(262144*(7));m++)
								{
									sender6[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender6,128*2048,MPI_FLOAT,2,6,MPI_COMM_WORLD); //send 6

						// send 7
						pos=0;
						for(m=262144*7;m<(262144*(8));m++)
								{
									sender7[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender7,128*2048,MPI_FLOAT,2,7,MPI_COMM_WORLD); //send 7

						//send 10
						pos=0;
						for(m=262144*10;m<(262144*(11));m++)

								{
									sender10[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender10,128*2048,MPI_FLOAT,2,10,MPI_COMM_WORLD); //send 10


						//send 11
						pos=0;
						for(m=262144*11;m<(262144*(12));m++)
								{
									sender11[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender11,128*2048,MPI_FLOAT,2,11,MPI_COMM_WORLD); //send 11

						//send 14
						pos=0;
						for(m=262144*14;m<(262144*(15));m++)

								{
									sender14[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender14,128*2048,MPI_FLOAT,2,14,MPI_COMM_WORLD); //send 14

						//send 15
						pos=0;
						for(m=262144*15;m<(262144*(16));m++)
				

								{
									sender15[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender15,128*2048,MPI_FLOAT,2,15,MPI_COMM_WORLD); //send 15


					// PHASE 2
					// 	zero sends to 1
					// 		1 -> keep 1
					// 		5
					// 		9
					// 		13

						//1
						
						pos=0;
						for(m=262144*1;m<(262144*(2));m++)

								{
									sender1[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender1,128*2048,MPI_FLOAT,1,1,MPI_COMM_WORLD); //send 1

						//5
						
						pos=0;
						for(m=262144*5;m<(262144*(6));m++)

								{
									sender5[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender5,128*2048,MPI_FLOAT,1,5,MPI_COMM_WORLD); //send 5

						//9
						
						pos=0;
						for(m=262144*9;m<(262144*(10));m++)

								{
									sender9[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender9,128*2048,MPI_FLOAT,1,9,MPI_COMM_WORLD); //send 5

						//13
						pos=0;
						for(m=262144*13;m<(262144*(14));m++)

								{
									sender13[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender13,128*2048,MPI_FLOAT,1,13,MPI_COMM_WORLD); //send 13


					// PHASE 3
					// 	zero sends to 8
					// 		8-> keep 8
					// 		12
						pos=0;
						for(m=262144*8;m<(262144*(9));m++)

								{
									sender8[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender8,128*2048,MPI_FLOAT,8,8,MPI_COMM_WORLD); //send 8
						pos=0;
						for(m=262144*12;m<(262144*(13));m++)

								{
									sender12[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender12,128*2048,MPI_FLOAT,8,12,MPI_COMM_WORLD); //send 12


					// phase 4
					// 	zero sends to 4
					// 		4 -> keeps 4
						pos=0;
						for(m=262144*4;m<(262144*(5));m++)

								{
									sender4[pos]=allData[m];
									pos++;
								}	
						MPI_Send(sender4,128*2048,MPI_FLOAT,4,4,MPI_COMM_WORLD); //send 4
							
				}
				
				break;
				case 1:
				{

					//gets 1,5,9,13
					//PHASE 3
						//send 9
							//9,13
					// float *rec1;
					MPI_Status status;
					// rec1 = (float*)malloc( sizeof(float) * 2048 * 128 );
													//source,tag
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,0,1,MPI_COMM_WORLD,&status);

					float *rec5;
					MPI_Status status5;
					rec5 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec5,128*2048,MPI_FLOAT,0,5,MPI_COMM_WORLD,&status5);

					float *rec9;
					MPI_Status status9;
					rec9= (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec9,128*2048,MPI_FLOAT,0,9,MPI_COMM_WORLD,&status9);

					float *rec13;
					MPI_Status status13;
					rec13= (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec13,128*2048,MPI_FLOAT,0,13,MPI_COMM_WORLD,&status13);


					MPI_Send(rec9,128*2048,MPI_FLOAT,9,9,MPI_COMM_WORLD); //send 9
					MPI_Send(rec13,128*2048,MPI_FLOAT,9,13,MPI_COMM_WORLD); //send 13


					//PHASE 4
						//send 5
					MPI_Send(rec5,128*2048,MPI_FLOAT,5,5,MPI_COMM_WORLD); //send 5
					free(rec9);
					free(rec13);
					free(rec5);
				}

				break;
				case 2:
				{
					//PHASE 2
						//get own 2
						//get 3,6,7,10,11,14,15
					// float *rec2;
					MPI_Status status2;
					// rec2 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,0,2,MPI_COMM_WORLD,&status2);

					float *rec3;
					MPI_Status status3;
					rec3 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec3,128*2048,MPI_FLOAT,0,3,MPI_COMM_WORLD,&status3);

					float *rec6;
					MPI_Status status6;
					rec6 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec6,128*2048,MPI_FLOAT,0,6,MPI_COMM_WORLD,&status6);


					float *rec7;
					MPI_Status status7;
					rec7 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec7,128*2048,MPI_FLOAT,0,7,MPI_COMM_WORLD,&status7);


					float *rec10;
					MPI_Status status10;
					rec10 = (float*)malloc( sizeof(float) * 2048 * 128 );

					MPI_Recv(rec10,128*2048,MPI_FLOAT,0,10,MPI_COMM_WORLD,&status10);


					float *rec11;
					MPI_Status status11;
					rec11 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec11,128*2048,MPI_FLOAT,0,11,MPI_COMM_WORLD,&status11);


					float *rec14;
					MPI_Status status14;
					rec14 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec14,128*2048,MPI_FLOAT,0,14,MPI_COMM_WORLD,&status14);


					float *rec15;
					MPI_Status status15;
					rec15 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec15,128*2048,MPI_FLOAT,0,15,MPI_COMM_WORLD,&status15);



					//send 3
							//7,11,15

					MPI_Send(rec3,128*2048,MPI_FLOAT,3,3,MPI_COMM_WORLD); //send 9
					MPI_Send(rec7,128*2048,MPI_FLOAT,3,7,MPI_COMM_WORLD); //send 9
					MPI_Send(rec11,128*2048,MPI_FLOAT,3,11,MPI_COMM_WORLD); //send 9
					MPI_Send(rec15,128*2048,MPI_FLOAT,3,15,MPI_COMM_WORLD); //send 9


						
					//PHASE 3
						//send 10
							//10,14
					MPI_Send(rec10,128*2048,MPI_FLOAT,10,10,MPI_COMM_WORLD); //send 9
					MPI_Send(rec14,128*2048,MPI_FLOAT,10,14,MPI_COMM_WORLD); //send 9


					//PHASE 4
						//send 6
					MPI_Send(rec6,128*2048,MPI_FLOAT,6,6,MPI_COMM_WORLD); //send 9

					free(rec3);
					free(rec7);
					free(rec11);
					free(rec15);
					free(rec10);
					free(rec14);
					free(rec6);

				}
				
				break;
				case 3:
				{
					//PHASE 3 //get 7,11,15

					// float *rec3;
					MPI_Status status3;
					// rec3 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,2,3,MPI_COMM_WORLD,&status3);

					float *rec7;
					MPI_Status status7;
					rec7 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec7,128*2048,MPI_FLOAT,2,7,MPI_COMM_WORLD,&status7);
					float *rec11;
					MPI_Status status11;
					rec11 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec11,128*2048,MPI_FLOAT,2,11,MPI_COMM_WORLD,&status11);

					float *rec15;
					MPI_Status status15;
					rec15 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec15,128*2048,MPI_FLOAT,2,15,MPI_COMM_WORLD,&status15);

					

					

					//send 11
						//11,15
					MPI_Send(rec11,128*2048,MPI_FLOAT,11,11,MPI_COMM_WORLD); //send 9
					MPI_Send(rec15,128*2048,MPI_FLOAT,11,15,MPI_COMM_WORLD); //send 9

					//PHASE 4

					MPI_Send(rec7,128*2048,MPI_FLOAT,7,7,MPI_COMM_WORLD); 


					free(rec11);
					free(rec15);
					free(rec7);

				}
			
				break;
				case 4:
				{

					// float *rec4;
					MPI_Status status4;
					// rec4 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,0,4,MPI_COMM_WORLD,&status4);


				}
				
				break;
				case 5:
				{
					// float *rec5;
					MPI_Status status5;
					// rec5 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,1,5,MPI_COMM_WORLD,&status5);
				}
				
				break;
				case 6:
				{
					// float *rec6;
					MPI_Status status6;
					// rec6 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,2,6,MPI_COMM_WORLD,&status6);
				}
				
				
				break;
				case 7:
				{
					// float *rec7;
					MPI_Status status7;
					// rec7 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,3,7,MPI_COMM_WORLD,&status7);
				}
				
				break;
				case 8:
				{
					//PHASE 4
						//get 8,12
						//send 12
					// float *rec8;
					MPI_Status status8;
					// rec8 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,0,8,MPI_COMM_WORLD,&status8);
					
					float *rec12;
					MPI_Status status12;
					rec12 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec12,128*2048,MPI_FLOAT,0,12,MPI_COMM_WORLD,&status12);


					MPI_Send(rec12,128*2048,MPI_FLOAT,12,12,MPI_COMM_WORLD); //send 9
					free(rec12);

				}
				
				break;
				case 9:
				{
					// float *rec9;
					MPI_Status status9;
					// rec9 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,1,9,MPI_COMM_WORLD,&status9);
					
					float *rec13;
					MPI_Status status13;
					rec13 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec13,128*2048,MPI_FLOAT,1,13,MPI_COMM_WORLD,&status13);

					MPI_Send(rec13,128*2048,MPI_FLOAT,13,13,MPI_COMM_WORLD);
					free(rec13);

				}
				
				break;
				case 10:
				{
					// float *rec10;
					MPI_Status status10;
					// rec10 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,2,10,MPI_COMM_WORLD,&status10);
					
					float *rec14;
					MPI_Status status14;
					rec14 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec14,128*2048,MPI_FLOAT,2,14,MPI_COMM_WORLD,&status14);

					MPI_Send(rec14,128*2048,MPI_FLOAT,14,14,MPI_COMM_WORLD);
					free(rec14);
				}
				
				break;
				case 11:
				{
					// float *rec11;
					MPI_Status status11;
					// rec11 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,3,11,MPI_COMM_WORLD,&status11);
					
					float *rec15;
					MPI_Status status15;
					rec15 = (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(rec15,128*2048,MPI_FLOAT,3,15,MPI_COMM_WORLD,&status15);

					MPI_Send(rec15,128*2048,MPI_FLOAT,15,15,MPI_COMM_WORLD);
					free(rec15);
				}
				
				break;
				case 12:
				{
					// float *rec12;
					MPI_Status status12;
					// rec12= (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,8,12,MPI_COMM_WORLD,&status12);
				}
				
				break;
				case 13:
				{
					// float *rec13;
					MPI_Status status13;
					// rec13= (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,9,13,MPI_COMM_WORLD,&status13);
				}
				
				break;
				case 14:
				{
					// float *rec14;
					MPI_Status status14;
					// rec14= (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,10,14,MPI_COMM_WORLD,&status14);
				}
				
				break;
				case 15:
				{
					// float *rec15;
					MPI_Status status15;
					// rec15= (float*)malloc( sizeof(float) * 2048 * 128 );
					MPI_Recv(gottenData,128*2048,MPI_FLOAT,11,15,MPI_COMM_WORLD,&status15);
				}
				
				break;
				

				default:
				
					printf("never gets here\n");
					printf("never gets here2\n");
				
				break;


			}

	
}

int main(int argc, char** argv){
  

	int pnum, pid;
	double elapsed_time;
	float *A, *B, *C, *Cans;
	float diff;
	int n = 2048; //change here for matrix size 
	int i, j, k;
	unsigned short seed[3];

	if( argc != 1 ){
		if( argc == 2 ){
			n = atoi(argv[1]);
		}
		else{
			printf("mmul [n]\n");
			exit(0);
		}
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &pnum);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	// printf("Processor %d out of %d says hi!\n", pid, pnum);

	if( pid == 0 ){
		//printf("Intializing matrix size : %d x %d x %d\n", n, n, n);

		A = (float*)malloc( sizeof(float) * n * n );

		// seed[0] = 0; seed[1] = 1; seed[2] = 2;

		for (i=0; i<n; i++) {
			for (k=0; k<n; k++) {
				A[i*2048+k] = (i*2048+k);
			}
		}
		

	}

	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = -1*MPI_Wtime();
	
	//Please modify the content of this function
	//PUT MY FUNCTION HERE 
	float *toGet;
	toGet = (float*)malloc( sizeof(float) * 2048 * 128 );

	myScatter(A,toGet,2048*2048,MPI_FLOAT,MPI_COMM_WORLD,pnum);

    
	// printf("toGet of 10 for proc %d is %f and should be %f\n", pid, toGet[10], recreate_A[(262144*pid)+10]);

	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time += MPI_Wtime();


//////////begin theirs
	double elapsed_time2;
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time2 = -1*MPI_Wtime();

	float *rec;
	rec = (float*)malloc( sizeof(float) * 2048 * 128 );
	MPI_Scatter(A,2048*128,MPI_FLOAT,rec,(2048 * 128),MPI_FLOAT,0,MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time2 += MPI_Wtime();



	//THIS SECTION TESTS DIFFERENCE
	float* original;
	original=(float*)malloc(sizeof(float)*2048*2048);
	for(i=0; i<2048; i++)
	{
	   for(j=0;j<2048;j++)
	   {
	      original[i*2048+j] = i*2048+j;
	   }
	}
	
	float block=0;

    for(i=0; i<262144; i++)
	{
	   block+=toGet[i];
	}

	
	float makeSure=0;

	for(i=262144*pid; i<262144*(pid+1); i++)
	{
	   makeSure+=original[i];
	}
        float final = block-makeSure;
	printf("Diff for core %d is %f \n", pid,final);

	/////END TEST SECTION 
      MPI_Barrier(MPI_COMM_WORLD);


	if( pid == 0 ){
		printf("Elapsed Time for mine: %f secs, for theirs: %f \n", elapsed_time,elapsed_time2);
		free(A);
	}

	MPI_Finalize();

	return 0;	
}


