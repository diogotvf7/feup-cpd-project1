#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <papi.h>
#include <omp.h>
#include <chrono>
#include <omp.h>

using namespace std;

#define SYSTEMTIME clock_t

 
void OnMult(int m_ar, int m_br) 
{
	SYSTEMTIME Time1, Time2;
	
	char st[100];
	double temp;
	int i, j, k;

	double *pha, *phb, *phc;
		
    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

	// Fill pha
	for (i=0; i<m_ar; i++)
    	for (j=0; j<m_ar; j++)
        	pha[i*m_ar + j] = (double)1.0;

	// Fill phb
    for (i=0; i<m_br; i++)
        for (j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);
    
	// Fill phc with 0's
	for (i=0; i<m_br; i++){
		for (j=0; j<m_br; j++){
			phc[i * m_br + j] = 0;
		}
	}

    Time1 = clock();

	for (i=0; i<m_ar; i++)
	{	for (j=0; j<m_br; j++)
		{	temp = 0;
			for (k=0; k<m_ar; k++)
			{
				temp += pha[i*m_ar+k] * phb[k*m_br+j];
			}
			phc[i*m_ar+j]=temp;
		}
	}


    Time2 = clock();
	double elapsed_time = (double)(Time2 - Time1) / CLOCKS_PER_SEC;
	sprintf(st, "Time: %3.3f seconds\n", elapsed_time);
	cout << st;

	// display 10 elements of the result matrix tto verify correctness
	for (i=0; i<1; i++)
	{	for (j=0; j<min(10,m_br); j++)
			cout << phc[j] << " ";
	}
	cout << endl <<  "Results:" << endl;
	cout << m_ar << "," << fixed << elapsed_time << ","; 

    free(pha);
    free(phb);
    free(phc);	
}

void OnMultLine(int m_ar, int m_br)
{
	
	char st[100];
	double temp;
	int i, j, k;

	double *pha, *phb, *phc;
	
    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

	// Fill pha
	for (int i=0; i<m_ar; i++)
    	for (int j=0; j<m_ar; j++)
        	pha[i*m_ar + j] = (double)1.0;

	// Fill phb
    for (int i=0; i<m_br; i++)
        for (int j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);
    
	// Fill phc with 0's
	for (int i=0; i<m_br; i++){
		for (int j=0; j<m_br; j++){
			phc[i * m_br + j] = 0;
		}
	}

    auto Time1 = chrono::high_resolution_clock::now();
	
	// #pragma omp parallel private(i,j,k)
	for (i=0; i<m_ar; i++) {
		for (k=0; k<m_ar; k++) {   
			// #pragma omp for
			for (j=0; j<m_br; j++) {
					phc[i*m_ar+j] += pha[i*m_ar+k] * phb[k*m_br+j];
				}
		}
	}
    
	auto Time2 = chrono::high_resolution_clock::now();
	double elapsed_time = (Time2 - Time1).count() / 1000000000.0f;
	sprintf(st, "Time: %3.3f seconds\n", elapsed_time);
	cout << st;

	for (i=0; i<1; i++)
	{	for (j=0; j<min(10,m_br); j++)
			cout << phc[j] << " ";
	}
	cout << endl <<  "Results:" << endl;
	cout << m_ar << "," << fixed << elapsed_time << ","; 

    free(pha);
    free(phb);
    free(phc);


}

// Block Matrix Multiplication
void OnMultBlock(int m_ar, int m_br, int bkSize)
{
	SYSTEMTIME Time1, Time2;

	char st[100];
	int i, j, k;

	double *pha, *phb, *phc;
		
    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

	// Fill pha
	for (i=0; i<m_ar; i++)
    	for (j=0; j<m_ar; j++)
        	pha[i*m_ar + j] = (double)1.0;

	// Fill phb
    for (i=0; i<m_br; i++)
        for (j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);
    
	// Fill phc with 0's
	for (i=0; i<m_br; i++){
		for (j=0; j<m_br; j++){
			phc[i * m_br + j] = 0;
		}
	}

	Time1 = clock();
	int ii, kk, jj;
	
    for(ii=0; ii<m_ar; ii+=bkSize) {    
        for( kk=0; kk<m_ar; kk+=bkSize){ 
            for( jj=0; jj<m_br; jj+=bkSize) {
                for (i = ii ; i < ii + bkSize ; i++) {    
                    for (k = kk ; k < kk + bkSize ; k++) {
                        for (j = jj ; j < jj + bkSize ; j++) {
                            phc[i*m_ar+j] += pha[i*m_ar+k] * phb[k*m_br+j];
                        }
                    }
                }
            }
        }
    }

    Time2 = clock();
	double elapsed_time = (double)(Time2 - Time1) / CLOCKS_PER_SEC;
	sprintf(st, "Time: %3.3f seconds\n", elapsed_time);
	cout << st;

	// display 10 elements of the result matrix tto verify correctness
	for (i=0; i<1; i++)
	{	for (j=0; j<min(10,m_br); j++)
			cout << phc[j] << " ";
	}
	cout << endl <<  "Results:" << endl;
	cout << m_ar << "," << fixed << elapsed_time << ","; 

    free(pha);
    free(phb);
    free(phc);
}

void handle_error (int retval)
{
	
	printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
	exit(1);
	
}

void init_papi() {
	
	int retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT && retval < 0) {
		printf("PAPI library version mismatch!\n");
		exit(1);
	}
	if (retval < 0) handle_error(retval);

	std::cout << "PAPI Version Number: MAJOR: " << PAPI_VERSION_MAJOR(retval)
				<< " MINOR: " << PAPI_VERSION_MINOR(retval)
				<< " REVISION: " << PAPI_VERSION_REVISION(retval) << "\n";
	
}


int main (int argc, char *argv[])
{
	char c;
	int lin, col, blockSize;
	int op;
	
	
	int EventSet = PAPI_NULL;
  	long long values[2];
  	int ret;
	
	ret = PAPI_library_init( PAPI_VER_CURRENT );
	if ( ret != PAPI_VER_CURRENT )
		std::cout << "FAIL" << endl;


	ret = PAPI_create_eventset(&EventSet);
		if (ret != PAPI_OK) cout << "ERROR: create eventset" << endl;


	ret = PAPI_add_event(EventSet,PAPI_L1_DCM );
	if (ret != PAPI_OK) cout << "ERROR: PAPI_L1_DCM" << endl;


	ret = PAPI_add_event(EventSet,PAPI_L2_DCM);
	if (ret != PAPI_OK) cout << "ERROR: PAPI_L2_DCM" << endl;

	ret = PAPI_add_event(EventSet,PAPI_TOT_INS);
	if (ret != PAPI_OK) cout << "ERROR: PAPI_TOT_INS" << endl;

	ret = PAPI_add_event(EventSet,PAPI_TOT_CYC);
	if (ret != PAPI_OK) cout << "ERROR: PAPI_TOT_CYC" << endl;
	

	op=1;
	do {
		cout << endl << "1. Multiplication" << endl;
		cout << "2. Line Multiplication" << endl;
		cout << "3. Block Multiplication" << endl;
		cout << "Selection: " << endl;
		cin >> op;
		if (op == 0)
			break;
		cout << "Matrix Dimensions: " << endl;
   		cin >> lin;
   		col = lin;


		
		// Start counting
		ret = PAPI_start(EventSet);
		if (ret != PAPI_OK) cout << "ERROR: Start PAPI" << endl;
		

		switch (op){
			case 1: 
				OnMult(lin, col);
				break;
			case 2:
				OnMultLine(lin, col);  
				break;
			case 3:
				cout << "Block Size: " << endl;
				cin >> blockSize;
				OnMultBlock(lin, col, blockSize);  
				break;

		}

  		ret = PAPI_stop(EventSet, values);
  		if (ret != PAPI_OK) cout << "ERROR: Stop PAPI" << endl;
		cout << values[0] << "," << values[1] << "," << values[2] << "," << values[3] << endl;

		ret = PAPI_reset( EventSet );
		if ( ret != PAPI_OK )
			std::cout << "FAIL reset" << endl; 



	} while (op != 0);

	
	ret = PAPI_remove_event( EventSet, PAPI_L1_DCM );
	if ( ret != PAPI_OK )
		std::cout << "FAIL remove event" << endl; 

	ret = PAPI_remove_event( EventSet, PAPI_L2_DCM );
	if ( ret != PAPI_OK )
		std::cout << "FAIL remove event" << endl; 

	ret = PAPI_destroy_eventset( &EventSet );
	if ( ret != PAPI_OK )
		std::cout << "FAIL destroy" << endl;
	
}