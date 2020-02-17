//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						SProjectionPursuit.cpp
//
//	Authors:					Luis Jiminez, Larry L. Biehl
//
//	Revision date:			02/27/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems	
//
//	Brief description:	This file contains functions that do projection pursuit.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		Preprocess
//			GetClassInfoStructure (in SProjectUtilities.cpp)
//			AssignClassInfoMemory (in SProjectUtilities.cpp)
//			GetClassCovarianceMatrix (in SProjectComputeStatistics.cpp)
//			ReduceMeanVector (in SMatrixUtilities.cpp)
//			SetupMatrixInversionMemory  (in SProjectMatrixUtilities.cpp)
//			InitializeGlobalAlertVariables (in SProjectUtilities.cpp)
//			ProjectionPursuit
//				ivector
//				SetupGroupingVector
//				First_stage_matrix
//					get_sub_matrix
//					get_sub_matrix
//					BlockMoveData (in SStubs.cpp)
//					InvertSymmetricMatrix (SProjectMatrixUtilities.cpp)
//					get_sub_matrix
//					get_sub_matrix
//					BlockMoveData (in SStubs.cpp)
//					InvertSymmetricMatrix (SProjectMatrixUtilities.cpp)
//					sum_matrix
//					BlockMoveData (in SStubs.cpp)
//					InvertSymmetricMatrix (SProjectMatrixUtilities.cpp)
//					subtract_matrix
//						CheckSomeEvents (in MMultiSpec.c and SStubs.cpp)
//	
//					MatrixMultiply (in SMatrixUtilities.cpp)
//					CheckSomeEvents (in MMultiSpec.c & SStubs.cpp)
//					ZeroMatrix (SMatrixUtilities.cpp)
//					minBhatt
//						MatrixMultiply  (in SMatrixUtilities.cpp)
//						TransformSymmetricMatrix (in SMatrixUtilities.cpp)
//						InvertLowerTriangularMatrix (in SProjectMatrixUtilities.cpp)
//						Bhattacharyya (in SProjectUtilities.cpp)
//						FindMinValueInVector (in SProjectMatrixUtilities.cpp)
//					
//				SaveTransformationToVector
//				First_stage_matrix2TD
//					First_stage_matrix
//						(See above)
//
//					SaveTransformationToVector
//					dvector
//					ivector
//					imatrix
//					ListGroupOfBands
//					First_stage_topDownAlgorithm
//						group_bands_insert
//							PPRandom
//
//						First_stage_matrix
//							(See above)
//
//						SaveTransformationToVector
//						ListGroupOfBands
//						
//					free_ivector
//					free_dvector
//					free_imatrix
//
//				First_stage_matrix2HB
//					First_stage_matrix
//						(See above)
//
//					SaveTransformationToVector
//					free_ivector
//					free_imatrix
//					free_dvector
//					ListGroupOfBands
//					First_stage_topDownAlgorithm
//						(See above)
//
//					group_bands_delete
//					First_stage_matrix
//					FindMaxValueInVector (in SProjectMatrixUtilities.cpp)
//					ListGroupOfBands
//					free_imatrix
//					free_ivector
//					free_dvector
//					
//				ZeroMatrix (SMatrixUtilities.cpp)
//				free_dmatrix
//				free_ivector
//				ListGroupOfBands
//				ListTransformationInformation (in SMatrixUtilities.cpp)
//				ZeroMatrix (SMatrixUtilities.cpp)
//				Second_Stage_matrix
//					dvector
//					dmatrix
//					dvector
//					imax_elm_vector
//					powell
//						linmin
//						
//					minBhatt
//						(See above)
//
//					CheckSomeEvents (in MMultiSpec.c & SStubs.cpp)
//					FindMaxValueInVector (in SProjectMatrixUtilities.cpp)
//					free_dvector
//					free_dmatrix
//					
//					
//				minBhatt
//						(See above)
//
//				ListTransformationInformation (in SMatrixUtilities.cpp)
//				BlockMoveData (in SStubs.cpp)
//				free_dmatrix
//				free_ivector
//				free_dvector
//				free_ivector
//				
//			ClearGlobalAlertVariables (in SProjectUtilities.cpp)
//			ReleaseMatrixInversionMemory (SProjectMatrixUtilities.cpp)
//			ZeroMatrix (SMatrixUtilities.cpp)
//			GetTranformationFeatureMeans (in SFeatureExtraction.cpp)
//			ReleaseClassInfoMemory (in SProjectUtilities.cpp)
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_wx
#endif
  
#if defined multispec_win  || defined multispec_wx
	#define	memFullErr		-108
#endif	// defined multispec_win ...
		
#define NR_END 1
#define FREE_ARG char*

static double dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)

static double dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

/* Matrices MM and SS
   MM-> column i contains the mean vector of class i.
   SS-> contains the covariances of each class. class i covariance is
        located from (i-1)*number_of_features + 1 to i*number_of_features.

    number_of_features -> number of features used in the computation of the 
    statistics.
*/
// double**			MM = NULL;
// double**			SS = NULL;

ClassInfoPtr	gClassInfoPtr = NULL;
double*			gTransformMatrixPtr = NULL;
double*			gAMatrixPtr = NULL;
double** 		gPmatrix = NULL;
UInt32			gPAIndex; 

		// min_num_samples -> minimum number of samples/class

SInt32			min_num_samples = 0;

SInt32 			ncom;
double			(*nrfunc)(double []);

		// Global variables for Second stage preprocessing function

		// Global version of the matrix that will be computed in order to
		//    maximize the minimum Bhattacharyya distance.
		// PAmatrix, global_final_features, can be output stage 1.

double*			PAmatrix = NULL;


		// global_num_features     A global variable that contains the number of
		//                         features that are going to be reduce.
		// 
		// global_final_features   A global variables that contains the final number
		//                         of features to which the data will be project.
		// 
		// global_num_classes      A global variable which contains the number of 
		//                         classes that are being used.
		// 
		// global_group_bands      Vector that contains the combinations of group 
		//                         of adjacent bands.
		// 
		// global_Index            Variable which indicate the group of adjacent 
		//                         bands which is going to be optimize.

SInt32 			global_num_features; 
SInt32 			global_final_features;
SInt32 			global_num_classes;
SInt32 			global_Index;
SInt32* 			global_group_bands = NULL; 

	
		// Globals for use by the First_stage_matrix routine.
		
double*		gFirstStage_M1 = NULL;
double*		gFirstStage_M2 = NULL;
double*		gFirstStage_S1 = NULL;
double*		gFirstStage_S2 = NULL;

		// Global used by First_stage_matrix and minBhatt routines.
		
SInt32*		gClass_log_det_OK = NULL;

		// Globals for use by the minBhatt routine

double*		gMinBhatt_Bdist = NULL;

		// Globals for Second Stage Matrix;
		
double*		g_f1dim_xt = NULL;
double*		g_linmin_pcom = NULL;
double*		g_linmin_xicom = NULL;
double*		g_powell_pt = NULL;
double*		g_powell_ptt = NULL;
double*		g_powell_xit = NULL;



		// Routine profiles.
						
double brent (
				double 								ax,
				double 								bx, 
				double 								cx, 
				double 								(*f)(double), 
				double 								tol,
				double* 								xmin);
						
double** dmatrix (
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch);

double* dvector (
				SInt32 								nl,
				SInt32								nh);

Boolean First_stage_matrix (
				SInt32*								group_of_bands,
				SInt32*								last_group_of_bands,
				SInt32 								final_num_features, 
				SInt32 								number_of_features, 
				SInt32 								number_of_classes,
				SInt16 								feature_type,
				double* 								pMAX,
				UInt32*								pMinBIndexPtr);

Boolean First_stage_topDownAlgorithm (
				SInt32								final_num_features,
				SInt16	 							feature_type,
				UInt16								bothOddChoicesNumberFeatures, 
				SInt32	 							number_of_features, 
				SInt32	 							number_of_classes,
				double*								mBvaluePrevPtr,
				SInt16*								Q1Ptr,
				SInt32*								group_of_bands,
				SInt32*								last_group_of_bands,
				double*								group_minBhatt, 
				SInt32*								group_plus_one,  
				SInt32**								GPOmatrix,
				double 								delta_TD);

Boolean First_stage_matrix2TD (
				SInt32 								number_of_features,
				SInt32 								number_of_classes,
				double 								delta_TD, 
				SInt16 								feature_type,
				UInt16								bothOddChoicesNumberFeatures, 
				SInt32*								group_of_bands,
				SInt32*								last_group_of_bands,
				SInt32 								max_number_features,
				SInt32*								pfinal_num_features);

Boolean First_stage_matrix2HB (
				SInt32								number_of_features,
				SInt32 								number_of_classes,
				double 								delta_TD, 
				double 								delta_BU, 
				SInt16 								feature_type,
				UInt16								bothOddChoicesNumberFeatures, 
				SInt32*								group_of_bands,
				SInt32*								last_group_of_bands,
				SInt32 								max_number_features,
				SInt32*								pfinal_num_features);

double* free_dvector (
				double*								lv,
				SInt32 								nl,
				SInt32 								nh);

double** free_dmatrix (
				double**								m,
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch);

SInt32** free_imatrix (
				SInt32**								m,
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch);

SInt32* free_ivector (
				SInt32*								lv,
				SInt32 								nl,
				SInt32 								nh);

double f1dim (
				double 								x);
						
void get_sub_matrix (
				double*								inputMatrixPtr,
				double*								outputMatrixPtr, 
				UInt32 								rowStart, 
				UInt32 								rowEnd, 
				UInt32 								colStart, 
				UInt32 								colEnd,
				UInt32								numberFeatures);

void group_bands_delete (
				SInt32*								group_of_bands,
				SInt32 								final_num_features,
				SInt32 								index_of_change, 
				SInt32*								group_minus_one);

void group_bands_insert (
				SInt32*								group_of_bands,
				SInt32 								final_num_features,
				SInt32 								index_of_change, 
				SInt32*								group_plus_one);

SInt32** imatrix (
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch);

SInt32 imax_elm_vector (
				SInt32*								vect,
				UInt32								number_of_elm);

SInt32* ivector (
				SInt32 								nl,
				SInt32 								nh);

void linmin (
				double 								p[],
				double 								xi[], 
				SInt32 								n, 
				double* 								fret, 
				double 								(*func)(double []));

Boolean ListGroupOfBands (
				double								mBvalue,
				CMFileStream*						resultsFileStreamPtr,
				SInt32*								group_of_bands, 
				SInt32 								final_num_features,
				SInt32								number_of_classes,
				UInt32								minClassIndex);

Boolean minBhatt (
				double*								PA,
				SInt32 								number_of_features, 
				SInt32 								final_num_features, 
				SInt32 								number_of_classes,
				double*								minBhatPtr,
				UInt32*								minIndexPtr);

double minBhatt_function (
				double* 								a);
						
void mnbrak (
				double*								ax,
				double*								bx, 
				double*								cx, 
				double*								fa, 
				double*								fb, 
				double*								fc, 
				double 								(*func)(double));

Boolean powell (
				double 								p[],
				double**								xi, 
				SInt32 								n, 
				double 								ftol, 
				SInt32*								iter, 
				double*								fret,
				double 								(*func)(double []));

SInt16 PPRandom (void);
						
void SaveTransformationToVector (
				double*								savedTransformationVector,
				double*								maxIndexVector,
				double**								transformationInformation,
				SInt32*								group_bands,
				UInt32								final_number_features);

Boolean Second_Stage_matrix (
				double								delta_Optimization);
						
void SetupGroupingVector (
				SInt32*								group_of_bands,
				UInt32								numberOfInputBands,
				UInt32								numberOfOutputFeatures);

void subtract_matrix (
				double* 								inputMatrix1Ptr,
				double* 								inputMatrix2Ptr, 
				double*								outputMatrixPtr, 
				UInt32								numberRows, 
				UInt32								numberColumns);

void sum_matrix (
				double* 								inputMatrix1Ptr,
				double* 								inputMatrix2Ptr, 
				double*								outputMatrixPtr, 
				UInt32								numberRows, 
				UInt32								numberColumns);

        					
        					
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double brent
//
//	Software purpose:	This routine is used by the function that minimizes a function of 
//							n variables.
//
//	Parameters in:				
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

#define ITMAX 100
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

double brent (
				double 								ax, 
				double 								bx, 
				double 								cx, 
				double 								(*f)(double), 
				double 								tol,
        		double* 								xmin)
        		
{
	double 								a,
											b,
											d,
				 							e = 0.0,
											etemp,
											fu,
											fv,
											fw,
											fx,
											p,
											q,
											r,
											tol1,
											tol2,
											u,
											v,
											w,
											x,
											xm;
										
	SInt32 								iter;
	
	

	a = (ax < cx ? ax : cx);
	b = (ax > cx ? ax : cx);
	x = w = v = bx;
	fw = fv = fx = (*f)(x);
	
	for (iter=1; iter<=ITMAX; iter++) 
		{
		xm = 0.5 * (a+b);
		tol2 = 2.0 * (tol1=tol*fabs (x) + ZEPS);
		
		if (fabs (x-xm) <= (tol2 - 0.5 * (b-a))) 
			{
			*xmin = x;
																								return fx;
			}	// end "if (fabs (x-xm) <= (tol2-0.5*(b-a)))"
			
		if (fabs (e) > tol1) 
			{
			r = (x-w) * (fx-fv);
			q = (x-v) * (fx-fw);
			p = (x-v)*q - (x-w)*r;
			q = 2.0 * (q-r);
			
			if (q > 0.0) 
				p = -p;
				
			q = fabs (q);
			etemp = e;
			e = d;
			
			if (fabs (p) >= fabs (0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
				d = CGOLD * (e=(x >= xm ? a-x : b-x));
				
			else	// fabs (p) < fabs (0.5*q*etemp) && ...
				{
				d = p/q;
				u = x + d;
				if (u-a < tol2 || b-u < tol2)
					d = SIGN (tol1, xm-x);
					
				}	// end "else fabs (p) < fabs (0.5*q*etemp) && ..."
				
			}	// end "if (fabs (e) > tol1)" 
		
		else	//fabs (e) <= tol1
			{
			d = CGOLD * (e=(x >= xm ? a-x : b-x));
			
			}	// end "else fabs (e) <= tol1"
			
		u = (fabs (d) >= tol1 ? x+d : x+SIGN (tol1, d));
		fu = (*f)(u);
		if (fu <= fx) 
			{
			if (u >= x) 
				a = x; 
			else 
				b = x;
				
			SHFT (v, w, x, u)
			SHFT (fv, fw, fx, fu)
			
			}	// end "if (fu <= fx)" 
		
		else	// fu > fx
			{
			if (u < x) 
				a = u; 
			else 
				b = u;
				
			if (fu <= fw || w == x) 
				{
				v = w;
				w = u;
				fv = fw;
				fw = fu;
				
				}	// end "if (fu <= fw || w == x)" 
				
			else if (fu <= fv || v == x || v == w) 
				{
				v = u;
				fv = fu;
				
				}	// end "else if (fu <= fv || v == x || v == w)"
	
			}	// end "else fu > fx"
			
		}	// end "for (iter=1; iter<=ITMAX; iter++)"
	
	gOperationCanceledFlag = TRUE;
	*xmin = x;
	return fx;
        
}	// end "brent"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double **dmatrix
//
//	Software purpose:	The purpose of this routine is allocate a double matrix with 
//							subscript range m[nrl..nrh][ncl..nch]
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:					
// 
// Called By:
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

double **dmatrix (
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch)
				
{
	double								**m;
	
	SInt32 								i, 
											nrow=nrh-nrl+1, 
											ncol=nch-ncl+1;
											

			// allocate pointers to rows
			
	m = (double**)MNewPointerClear ((UInt32)(nrow+NR_END)*sizeof (double*));
	if (m != NULL)
		{
		m += NR_END;
		m -= nrl;

      		// allocate rows and set pointers to them
      		
		m[nrl]=(double*)MNewPointerClear ((UInt32)(nrow*ncol+NR_END) * sizeof (double));

		if (m[nrl] != NULL)
			{
			m[nrl] += NR_END;
			m[nrl] -= ncl;

			for (i=nrl+1; i<=nrh; i++) 
				m[i] = m[i-1] + ncol;
				
			}	// end "if (m[nrl] != NULL)"
			
		else	// Not enough memory
			{
					// Free the vector for the addresses.
					
			CheckAndDisposePtr ((FREE_ARG)(m+nrl-NR_END));
					
			throw ((OSErr)memFullErr);
			
			}
			
		}	// end "if (m != NULL)"
			
	else	// Not enough memory
		throw ((OSErr)memFullErr);

			// return pointer to array of pointers to rows
					
	return m;
	
}	// end "dmatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double **dmatrix
//
//	Software purpose:	The purpose of this routine is allocate a allocate a double 
//							vector with subscript range lv[nl..nh]
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:					
// 
// Called By:
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

#undef ITMAX
#undef CGOLD
#undef ZEPS
#undef SHFT

double *dvector (
				SInt32 								nl,
				SInt32								nh)
				
{
	double*								lv;
	

	lv = (double *)MNewPointerClear ((UInt32)(nh-nl+1+NR_END) * sizeof (double));
	
	if (lv != NULL)
		lv -= nl-NR_END;
			
	else	// Not enough memory
		throw ((OSErr)memFullErr);
        
   return lv;
   
}	// end dvector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean First_stage_matrix
//
//	Software purpose:	Given the group_of_bands combination, this function will 
//							construct the a priori matrix PA based on some vectors that 
//							are store in blocks of data.
//
//							The value 'F' in the character feature_type is for feature 
//							selection
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:
//
// Description of variables used in routine:
//
// MM      Matrix that columns holds the mean vector for each class.
// 
// SS      Matrix that holds the covariances matrices for each class.
// 
// M1      Matrix that holds the mean vector of one class.
// 
// M2      Matrix that holds the mean vector of another class.
// 
// S1      Matrix that holds the covariance matrix of one class.
// 
// S2      Matrix that holds the covariance matrix of one class.
// 
// invS2   Matrix that holds the inverse of S2.
// 
// S2_S1   Matrix that holds: S2^(-1)*S1.
// 
// sumS1S2 Matrix that holds the sum of S1 and S2.
// 
// deltaM  Matrix that holds the substraction: M2-M1.
// 
// deltaMT Matrix that holds the transpose of deltaM.
// 
// detS1   Determinant of S1.
// 
// detS2   Determinant of S2.
// 
// detsumS1S2      Determinant of sumS1S2.
// 
// MAXFS   Holds the maximum value of groups of adjacent bands for
//         feature selection condition.
// 
// MAX     For feature selection case is equal to one. For other case is
//         MAXFS + 1 (average vectors) + number_of_classes*(number of classes-1)
//         (for adition of vectors that maximize the mean and covariance
//         components of Bhattacharyya distance {for every two classes}).
// 
// Maxindex        
// 
// Maxvalue
// 
// gPmatrix Matrix that holds the banks of possible apriori vectors for each 
//         group of adjacent bands.
// 
// Bdist   Vector that holds the minimum Bhattacharyya distance for 
//         each possible combinations of vectors in PA.    
// 
// Imax    
// 
// a       Vector that maximize the mean component for each two classes.
// 
// b       Vector that maximize the covariance component for each two
//         classes. 
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996
// Revised by:			Larry L. Biehl			Date: 06/17/2006

Boolean First_stage_matrix (
				SInt32*								group_bands,
   			SInt32*								last_group_of_bands, 
      		SInt32 								final_features, 
      		SInt32 								number_of_features, 
      		SInt32 								number_of_classes,
      		SInt16 								feature_type,
      		double*								pMaxPtr,
      		UInt32*								pMinBIndexPtr)

{			
	double 								Bdist,
  											Maxvalue;
  					
  	HDoublePtr							Pa;
  						
	UInt32								featureSelectionStart,
											i, 
					  						j, 
					  						k, 
					  						Maxindex, 
					  						sum, 
					  		 				MAXFS, 
					  						MAX,
											MaxminBIndex;
  	
  	SInt16*								classPairWeightPtr;

	UInt32 								counter, 
  											k1, 
  											noChangeCount;
  
	Boolean								continueFlag = TRUE,
  											determinantOKFlag,
  											firstTimeFlag;
  						
		
	counter = 0;
	
			// Fill the first line of the gPmatrix
			//	It be the average vector (One in every Position).
			
	Boolean skipAverageFlag = TRUE;
	if (feature_type != kPPFeatureSelection || !skipAverageFlag)
		{		
		counter++;
		for (j=1; j<=(UInt32)number_of_features; j++)
			gPmatrix[counter][j] = 1;
			
		}	// end "if (feature_type != kPPFeatureSelection || !skipAverageFlag)"
		
			// Initial Values for the non FS condition

	if (feature_type != kPPFeatureSelection) 
		{		
		double			aa,
							logDeterminant,
							norm;
					
		double 			*aPtr,
							*deltaMPtr,
							*M1Ptr, 
		  					*M2Ptr,
		  					*S1Ptr, 
		  					*S2Ptr, 
		  					*sumS1S2Ptr;
					
		UInt32			bytesToCopy,
							classPairCount,
							startClassPairIndex = counter;

			
				// Initial matrices that will hold the statistics for the group k
				// below. The matrices and vectors were obtained to allow
				// for the largest group of bands.
				
		M1Ptr = gFirstStage_M1;
		M2Ptr = gFirstStage_M2;
		S1Ptr = gFirstStage_S1;
		S2Ptr = gFirstStage_S2;
		
				// Use "gInverseMatrixMemory.inversePtr" and 
				// "gInverseMatrixMemory.pivotPtr" for storage as they
				// are currently not being used at the time that deltaMPtr, sumS1S2Ptr 
				// and aPtr are being used.
		
		sumS1S2Ptr = gTempMatrixPtr;
		deltaMPtr = gInverseMatrixMemory.pivotPtr;
		aPtr = gTransformMatrixPtr;
			
		//LoadDItemString (gStatusDialogPtr, 
		//							IDC_Status15, 
		//							(Str255*)"\pGet Vectors:");

				// Get the inter class weight list pointer.
				
		classPairWeightPtr = (SInt16*)GetHandlePointer (
														gProjectInfoPtr->classPairWeightsHandle);

				// Vectors that maximize minimum Bhattacharyya Distances

		sum = 0;
		for (k=1; k<=(UInt32)final_features; ++k) 
			{
			if (group_bands[k] != last_group_of_bands[k])
				{
				counter = startClassPairIndex;
				classPairCount = 0;
				for (i=0; i<(UInt32)number_of_classes-1; i++)
					{
					get_sub_matrix (gClassInfoPtr[i].meanPtr,
											M1Ptr, 
											1, 
											1, 
											sum+1, 
											sum+group_bands[k],
											number_of_features);
											
					get_sub_matrix (gClassInfoPtr[i].covariancePtr,
											S1Ptr, 
											sum+1, 
											sum+group_bands[k],
											sum+1, 
											sum+group_bands[k],
											number_of_features);
			
					if (gClass_log_det_OK[i] <= 0)
						{
						bytesToCopy = group_bands[k] * group_bands[k] * sizeof (double);
						BlockMoveData (S1Ptr, 
											gInverseMatrixMemory.inversePtr, 
											bytesToCopy);	
		
						determinantOKFlag = InvertSymmetricMatrix (
																gInverseMatrixMemory.inversePtr, 
																(UInt16)group_bands[k], 
																gInverseMatrixMemory.pivotPtr, 
																gInverseMatrixMemory.indexRowPtr, 
																gInverseMatrixMemory.indexColPtr, 
																gInverseMatrixMemory.ipvotPtr, 
																NULL,
																&logDeterminant,
																kGetDeterminantOnly);
							
						if (determinantOKFlag)
							{
							if (group_bands[k] == number_of_features)
										// This is the full matrix. The determinant is okay.
										// Therefore all subsets will be okay.
								gClass_log_det_OK[i] = 1;
								
							else	// group_bands[k] != number_of_features
										// This is a subset. We need to continue
										// to check each time through.
								gClass_log_det_OK[i] = 0;
							
							}	// end "if (determinantOKFlag)"
							
						else	// !determinantOKFlag
							gClass_log_det_OK[i] = -1;
							
						if (gOperationCanceledFlag)
							break;
							
						}	// end "if (gClass_log_det_OK[i] == -1)"
		
					for (j=1+i; j<(UInt32)number_of_classes; j++) 
						{
						if (classPairWeightPtr[classPairCount] > 0)
							{
							if (gClass_log_det_OK[i] >= 0)
								{
								get_sub_matrix (gClassInfoPtr[j].meanPtr,
														M2Ptr, 
														1, 
														1, 
														sum+1, 
														sum+group_bands[k],
														number_of_features);
														
								get_sub_matrix (gClassInfoPtr[j].covariancePtr,
														S2Ptr, 
														sum+1, 
														sum+group_bands[k],
														sum+1, 
														sum+group_bands[k],
														number_of_features);
									
								if (gClass_log_det_OK[j] <= 0)
									{
									bytesToCopy = group_bands[k] * group_bands[k] * 
																							sizeof (double);
									BlockMoveData (S2Ptr, 
														gInverseMatrixMemory.inversePtr, 
														bytesToCopy);	
					
									determinantOKFlag = InvertSymmetricMatrix (
																	gInverseMatrixMemory.inversePtr, 
																	(UInt16)group_bands[k], 
																	gInverseMatrixMemory.pivotPtr, 
																	gInverseMatrixMemory.indexRowPtr, 
																	gInverseMatrixMemory.indexColPtr, 
																	gInverseMatrixMemory.ipvotPtr,
																	NULL, 
																	&logDeterminant,
																	kGetDeterminantOnly);
								
									if (determinantOKFlag)
										{
										if (group_bands[k] == number_of_features)
													// This is the full matrix. The determinant is 
													// okay. Therefore all subsets will be okay.
											gClass_log_det_OK[j] = 1;
											
										else	// group_bands[k] != number_of_features
													// This is a subset. We need to continue
													// to check each time through.
											gClass_log_det_OK[j] = 0;
										
										}	// end "if (determinantOKFlag)"
								
									else	// !determinantOKFlag
										gClass_log_det_OK[j] = -1;
													
									if (gOperationCanceledFlag)
										break;
										
									}	// end "if (gClass_log_det_OK[j] <= 0)"
									
								}	// end "if (gClass_log_det_OK[i] >= 0)"
								
							determinantOKFlag = FALSE;
							if (gClass_log_det_OK[j] >= 0)
								{	
								sum_matrix (S1Ptr, 
												S2Ptr, 
												sumS1S2Ptr, 
												group_bands[k], 
												group_bands[k]);
		
								bytesToCopy = group_bands[k] * group_bands[k] * sizeof (double);
								BlockMoveData (sumS1S2Ptr, 
													gInverseMatrixMemory.inversePtr, 
													bytesToCopy);	
				
								determinantOKFlag = InvertSymmetricMatrix (
																gInverseMatrixMemory.inversePtr, 
																(UInt16)group_bands[k], 
																gInverseMatrixMemory.pivotPtr, 
																gInverseMatrixMemory.indexRowPtr, 
																gInverseMatrixMemory.indexColPtr, 
																gInverseMatrixMemory.ipvotPtr, 
																NULL,
																&logDeterminant,
																kGetDeterminantOnly);
									
								if (gOperationCanceledFlag)
									break;
									
								}	// end "if (gClass_log_det_OK[j] == 1)"
				
							if (determinantOKFlag) 
								{
										// calculate the vector that maximize the mean component 
										// of the Bhattacharyya distance */
				
								subtract_matrix (M2Ptr, M1Ptr, deltaMPtr, group_bands[k], 1);
								MatrixMultiply (deltaMPtr, 
														sumS1S2Ptr, 
														aPtr,
														1, 
														(UInt16)group_bands[k], 
														(UInt16)group_bands[k], 
														kMat1ByMat2);
								
										// Normalize the vector
				
								norm = 0.0;
								for (k1=0; k1<(UInt32)group_bands[k]; k1++) 
									{
									aa = aPtr[k1];
									norm += aa * aa;
									 
									}	// end "for (k1=0; k1<group_bands[k]; k1++)"
				        
								if (norm == 0)
									norm = 1;
				
								counter++;
								norm = sqrt (norm);
				
										// Store the value of a (normalized) in gPmatrix.
				
								for (k1=1; k1<=(UInt32)group_bands[k]; k1++)
									gPmatrix[counter][sum+k1] = aPtr[k1-1]/norm;
				
										// Calculate the vector that maximize the covariance 
										// component of the Bhattacharyya distance 
				
										// Need a eigen_funct for a non symetric case
										//  Algorithm from Fukunagua.
								/*				 
								inverse (S2, invS2, group_bands[k]);
								mult_matrix (invS2, S1, S2_S1, group_bands[k], group_bands[k],
					                    group_bands[k], group_bands[k]); 
					
								eigen_funct (S2_S1, group_bands[k], d, lv);
					        
								for (k1 = 1; k1<=group_bands[k]; ++k1)
									d1[k1] = d[k1] + 1.0/d[k1] + 2;
					
								max_elm_vector (d1, group_bands[k], &mvalue, &Imax);
		
								counter++;
								for (k1=1; k1<=group_bands[k]; k1++) 
									{
											// comment -> lv is the eigenvector matrix that every 
											// column is the normalized eigenvector.
				
				          		//b[1][k1] = lv[k1][Imax];
				
											// Store the value of b in gPmatrix.
											// Should be normalized.
											
									//b[1][k1] = 1;
									gPmatrix[counter][sum+k1] = 1;
									
									}	// end "for (k1=1; k1<=group_bands[k]; ++k1)"
								*/				
										// Store the value of b in gPmatrix.
										// Should be normalized.
				
								//for (k1=1; k1<=group_bands[k]; ++k1)
								//	gPmatrix[counter][k1+sum] = b[1][k1]; 
			
								if (TickCount () >= gNextTime && !CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
									break;
									
								}	// end "if (determinantOKFlag)"
								
							else	// !determinantOKFlag
								{
										// Load 0's for this set.
										
								counter++;
								for (k1=1; k1<=(UInt32)group_bands[k]; k1++)
									gPmatrix[counter][sum+k1] = 0;
									
								}	// end "else !determinantOKFlag"
								
							}	// end "if (classPairWeightPtr[classPairCount] > 0)"
							
						classPairCount++;
			      
						}	// end "for (j=1+i; j<=number_of_classes; j++)"
							
					if (gOperationCanceledFlag)
						{
						continueFlag = FALSE;
						break;
						
						}	// end "if (gOperationCanceledFlag)"
					
					}	// end "for (i=1; i<=number_of_classes-1; ++i)"
					
				last_group_of_bands[k] = group_bands[k];
					
				}	// end "if (group_bands[k] != last_group_of_bands[k])"
					
			if (!continueFlag)
				break;
				
	    	sum += group_bands[k];
					
			}	// end "for (k=1; k<=final_features; ++k)"
			
		}	// end "if (feature_type != kPPFeatureSelection)" Finish non FS conditions
				
	//LoadDItemString (gStatusDialogPtr, 
	//							IDC_Status15, 
	//							(Str255*)"\pGet Best Vectors:");
  
	if (continueFlag)
		{
				// Now finish filling gPmatrix. 

				// Initialize the Pmatrix with vectors holding zeroes and one 1 for
				// the feature selection case.
	
		featureSelectionStart = counter + 1;
		MAXFS = imax_elm_vector (group_bands, final_features);
		MAX = counter + MAXFS;
				
		for (i=featureSelectionStart; i<=MAX; i++)
			{
			for (j=1; j<=(UInt32)number_of_features; j++)
	      	gPmatrix[i][j] = 0.0;
	
			}	// end "for (i=featureSelectionStart; i<=featureSelectionEnd; i++)"
	
		sum = 1;
		for (i=1; i<=(UInt32)final_features; i++)
			{
			for (j=0; j<(UInt32)group_bands[i]; j++)
	      	gPmatrix[featureSelectionStart+j][sum+j] = 1.0;
	      	
	      sum += group_bands[i];
	
			}	// end "for (i=1; i<=final_features; i++)"
		
		Pa = gTransformMatrixPtr;
		ZeroMatrix (Pa, final_features, number_of_features, TRUE);
	
		sum = 0;
		for (i=1; i<=(UInt32)final_features; i++)
			{
			for (k=sum; k<(sum+group_bands[i]); k++)
				{
				Pa[(i-1)*number_of_features+k] = gPmatrix[1][k+1];
				
				}	// end "for (k=sum; k<=(sum+group_bands[i]-1); k++)"
				
			sum += group_bands[i];
			
			gPmatrix[gPAIndex-1][i] = 1;
			
			}	// end "for (i=1; i<=final_features; ++i)"
			
				// Now set up the gPmatrix so that replicated entries are not
				// used in the search. This is done to speed the algorithm 
				// up.
		
		SInt32		m;
		UInt32		minBIndex;
		Boolean		repeatFlag;
				
		sum = 1;
		for (i=1; i<=(UInt32)final_features; i++) 
			{
			for (j=2; j<=MAX; j++) 
				{
						// Make sure that the group is not all zeros.
						
				for (k=sum; k<(sum+group_bands[i]); k++)
					{
					repeatFlag = TRUE;
					if (gPmatrix[j][k] != 0)
						{
						repeatFlag = FALSE;
						break;
						
						}	// end "if (gPmatrix[j][k] != gPmatrix[m][k])"
							
					}	// end "for (k=sum; k<(sum+group_bands[i]); k++)"
					
				if (!repeatFlag)
					{
							// Compare with groups in list above to see if it is a
							// repeat.
							
					for (m=1; m<(SInt32)j; m++)
						{ 
						repeatFlag = TRUE;
						for (k=sum; k<(sum+group_bands[i]); k++)
							{
							if (gPmatrix[j][k] != gPmatrix[m][k])
								{
								repeatFlag = FALSE;
								break;
								
								}	// end "if (gPmatrix[j][k] != gPmatrix[m][k])"
								
							}	// end "for (k=sum; k<= (sum+group_bands[i]-1); ++k)"
						
						if (repeatFlag)
							break;
						
						}	// end "for (m=1; m<=j; m++)"
							
					}	// end "if (!repeatFlag)"
						
				if (repeatFlag)
					gPmatrix[j][sum] = -2;
				
				}	// end "for (j=1; j<=MAX; j++)"
				
			sum += group_bands[i];
				
			}	// end "for (i=1; i<=final_features; i++)"
		
				// Loop to construct PA
				// Keep looping until we have been through group_bands one
				// complete time with no change in MaxIndex for each 
				// group_bands.
				
		noChangeCount = 0;
		Maxvalue = -1.0;
		i = 1; 
		sum = 1;
		firstTimeFlag = TRUE;
		while (noChangeCount < (UInt32)final_features) 
			{
			if (i > (UInt32)final_features)
				{
				i = 1;
				sum = 1;
				
				}	// end "if (i > final_features)"
				
			Maxindex = (UInt32)gPmatrix[gPAIndex-1][i];
			
			if (firstTimeFlag || group_bands[i] > 1)
				{
						// Only go through this loop if we have not been through it
						// one time or the number of bands in the group is more than 1. If 
						// only 1 band then nothing changes.
				
						// Construct Pa with different vectors stored in gPmatrix.
	
				for (j=1; j<=MAX; j++) 
					{
					if (gPmatrix[j][sum] > -1.5)
						{
						for (k=sum; k<(sum+group_bands[i]); k++)
							Pa[(i-1)*number_of_features+k-1] = gPmatrix[j][k];
	
						continueFlag = minBhatt (Pa, 
															number_of_features, 
															final_features, 
															number_of_classes,
															&Bdist,
															&minBIndex);
							
						if (!continueFlag)
							break; 
	
								// Obtain the maximum value of minimum Bhattacharyya distance 
								// and its index.
			
						if (Maxvalue < Bdist) 
							{
		          		Maxvalue = Bdist;
		           		Maxindex = j;
		           		MaxminBIndex = minBIndex;
		            
							}	// end "if (Maxvalue < Bdist)"
							
						}	// end "if (gPmatrix[j][sum] > -1.5)"
	
					}	// end "for (j=1; j<=MAX; j++)"
					
						// Substitute a specific row of Pa with the vector that 
						// produce the maximum value.

				for (k=sum; k<(sum+group_bands[i]); k++)
					Pa[(i-1)*number_of_features+k-1] = gPmatrix[Maxindex][k];
					
				firstTimeFlag = FALSE;
					
				if (!continueFlag)
					break; 
					
				}	// end "if (firstTimeFlag || group_bands[i] > 1)"
					
						// Check if the index for the max Bhattacharyya distance
						// changed for this group_bands.
						
			if (gPmatrix[gPAIndex-1][i] != Maxindex)
				{
				noChangeCount = 0;
				gPmatrix[gPAIndex-1][i] = Maxindex;
					
				}	// end "if (gPmatrix[gPAIndex-1][i] != Maxindex)"
				
			else	// gPmatrix[gPAIndex-1][i] == Maxindex
				noChangeCount++;
				
			sum += group_bands[i];
						
			if (!continueFlag)
				break; 
			
					// Do not need to go through again if  final_features is 1.
					
			if (final_features == 1) 
				noChangeCount = final_features;

			i++;
			
			}	// end "while (noChangeCount < final_features)"  
				// End of Loop to construct PA
			
		}	// end "if (continueFlag)"
					
	//LoadDItemString (gStatusDialogPtr, 
	//							IDC_Status15, 
	//							(Str255*)"\pDone with first stage:");
	
	if (continueFlag)
		{
				// Return the maximum value of minimum Bhattacharyya distance.

		*pMaxPtr = Maxvalue;
		*pMinBIndexPtr = MaxminBIndex;
		
		}	// end "if (continueFlag)"
	
	return (continueFlag);

}	// end "First_stage_matrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean First_stage_matrix2HB
//
//	Software purpose:	First_stage_matrix2HB the hybrid version
//						   This function construct the matrix PA (the initial matrix)
//							for Feature Selection or with another types of vectors
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:
//
// Variables used in routine:
//
// IGB  is the Index for different combinations of 
//      group of bands. 
// IGBMax is the maximum IGB (combination of group of bands with
// 		 the maximum Bhattacharyya (minimum among the classes)
// 		 distance.
// 
// Q1    is the quantity (= 0 or 1) that indicate when Top Down
// 		algorithm has been succesful growing more than the 
// 		threshold delta_TD.
// 
// Q2    is the quantity that indicate if final_num_feature 
// 		is less than the minimum # of samples/class-1
// 		to ensure that the projection will be non-singular.
//   
// Q3    is the quantity that indicate when Bottom Up has
// 		been succesful, shrinking with a loss less than 
// 		delta_BU. . When Q1=0 and Q3=0 the algorithm stop.
// 
// group_plus_one  is a vector that contains the group of adjacent
// 					 bands when one of the groups has been divided
// 					 in two. Is for the Top Down part.
// 
// group_minus_one  is the vector that contains the group of 
// 					  adjacent bands when two groups had been
// 					  merge into one. Is for the Bottom Up part.
// 
// GPOmatrix        Matrix that store all possible combinations of
// 					  group_plus_one and group_minus_one.
// 
// group_minBhatt   Store the minimum Bhattacharyya distance for
// 					  all possible combination of group_plus_one
// 					  or group_minus_one.
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

Boolean First_stage_matrix2HB (
				SInt32 								number_of_features, 
				SInt32								number_of_classes,
   			double 								delta_TD, 
   			double 								delta_BU, 
   			SInt16 								feature_type,
      		UInt16								bothOddChoicesNumberFeatures, 
   			SInt32*								group_of_bands,
   			SInt32*								last_group_of_bands,
   			SInt32 								max_number_features,
    			SInt32*								pfinal_num_featuresPtr)

{
	CMFileStream*						resultsFileStreamPtr = NULL;
	double*								group_minBhatt = NULL;
	
	SInt32								*group_minus_one = NULL,
											*group_plus_one = NULL,
											*saved_group_of_bands = NULL;
	
	SInt32								**GPOmatrix = NULL;
	
	double 								mBvaluePrev, 
											mBvalueNew,
											portionDifference; 
	
	SInt32	 							final_num_features,
											i,
											IGBMax,
											j, 
											k,
											last_num_features;
						
	UInt32								mBIndex;
						
	SInt16								QQ,
											Q3;
						
	Boolean								continueFlag = TRUE,
											diffFlag;
	

			// Initialization of variables

	QQ = 1;
	Q3 = 1;
	
			// Starting case

	final_num_features = *pfinal_num_featuresPtr;
	last_num_features = final_num_features;

	try
		{
		continueFlag = First_stage_matrix (group_of_bands,
														last_group_of_bands,  
														final_num_features,
														number_of_features, 
														number_of_classes, 
														feature_type,
														&mBvaluePrev,
														&mBIndex);
	          					
		SaveTransformationToVector (gPmatrix[gPAIndex],
												gPmatrix[gPAIndex-1],
												gPmatrix,
												group_of_bands,
												final_num_features);
	
	   group_minus_one = ivector (1, number_of_features);
		group_plus_one = ivector (1, number_of_features);
		saved_group_of_bands = ivector (1, number_of_features);
		GPOmatrix = imatrix (1, number_of_features, 1, number_of_features);
	   group_minBhatt = dvector (1, number_of_features);
		
		}	// end try
		
	catch (...)
		{
		continueFlag = FALSE;
		
		}	// end catch
	
			// List the initial of group of bands used.

	if (continueFlag)
		continueFlag = ListGroupOfBands (mBvaluePrev, 
													NULL, 
													group_of_bands, 
													final_num_features,
													number_of_classes,
													mBIndex);
		
	if (!continueFlag)
		{
		QQ = 0;
		Q3 = 0;
		
		}	// end "!continueFlag"

			// Starting to contruct PA matrix interchanging Top Down and
			// Bottom Up 

	while ((QQ+Q3 != 0) && 
				(final_num_features < max_number_features) &&
					(final_num_features < min_num_samples-1)) 
		{ 
		if (gAlertReturnCode == 1)
			break;
			
		final_num_features++;
		
		LoadDItemStringNumber (kFeatureExtractStrID,
										IDS_FeatureExtract59,
										gStatusDialogPtr, 
										IDC_Status15, 
										(Str255*)gTextString);
		
		continueFlag = First_stage_topDownAlgorithm (final_num_features, 
																	feature_type,
																	bothOddChoicesNumberFeatures, 
																	number_of_features, 
																	number_of_classes,
																	&mBvaluePrev,
																	&QQ,
																	group_of_bands,
																	last_group_of_bands,
																	group_minBhatt, 
																	group_plus_one,  
																	GPOmatrix,
																	delta_TD);
			
		if (!continueFlag || gAlertReturnCode == 1)
			break;
			
				// If this 'final_num_features' is the same as the last 
				// 'final_num_features', then check if the group of bands is the same. 
				// If it is then do not do the bottom up because we will start an 
				// infinite loop.
				
     	diffFlag = TRUE;
		if (final_num_features == last_num_features)
			{
     		diffFlag = FALSE;
   		for (i=1; i<=final_num_features; i++)
   			{
     			if (saved_group_of_bands[i] != group_of_bands[i])
     				{
     				diffFlag = TRUE;
     				break;
     				
     				}	// end "if (saved_group_of_bands[i] != group_of_bands[i])"
     				
     			}	// end "for (i=1; i<=final_num_features; i++)"		
				
			}	// end "if (final_num_features == last_num_features)"
			
		if (diffFlag)
			{
					// Save the final_num_features and the group_of_bands.
				
			last_num_features = final_num_features;
	   	for (i=1; i<=final_num_features; i++)
	  			saved_group_of_bands[i] = group_of_bands[i];	
	  			
	  		}	// end "if (diffFlag)"	
				
		else	//	!diffFlag
			Q3 = 0;
			
				// Bottom Up Part

				// Check if the final number of features is less than the minimum 
				// number of samples/class-1 (to avoid singularity and if the final 
				// number of features is larger than one in order to apply the 
				// Bottom Up part of the hybrid algorithm
				
 		if (diffFlag &&
				(final_num_features < min_num_samples-1) && 
 					(final_num_features > 1)) 
 			{
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract60, // "\pBU Number feature groups:"
											gStatusDialogPtr, 
											IDC_Status15, 
											(Str255*)gTextString);

					// Start to construct all possible ways of combining adjacent 
					// groups of adjacent bands into one group.

			mBvalueNew = -1;
   		for (i=1; i<= final_num_features-1; ++i) 
   			{
						// Sum the number of bands in group "i" with the one in 
						// group "i+1".

     			group_bands_delete (group_of_bands, 
     										final_num_features, 
     										i, 
         								group_minus_one);

						// Store all possible combinations of group of bands in 
						// GPOmatrix.

     			for (j=1; j<=final_num_features-1; ++j)
       			GPOmatrix[i][j] = group_minus_one[j];

						// Compute the a priori matrix PA from stored vectors, for 
						// the present combination of bands.

     			continueFlag = First_stage_matrix (group_minus_one,
																last_group_of_bands, 
																final_num_features-1,
																number_of_features, 
																number_of_classes, 
																feature_type,
																&mBvalueNew,
																&mBIndex);

						// Store the value of the minimum Bhattacharyya distance.
						
     			group_minBhatt[i] = mBvalueNew;
   			
	   		if (!continueFlag)
	   			break;

   			}	// end "for (i=1; i<= final_num_features-1; ++i)"
   			
   		if (!continueFlag)
   			break;

					// Calculate the maximum numbers and the index of location.

			IGBMax = FindMaxValueInVector (
									&group_minBhatt[1], final_num_features-1, &mBvalueNew) + 1;

					// If the decrement is greater than the threshold delta_BU then 
					// Bottom Up failed and set Q3 = 0. If the percentage of decrement 
					// is less than the threshold then choose that particular 
					// combination of bands and redefine the minBhatt value for future 
					// comparison. Decrease value of final number of features by one.

   		if ((mBvaluePrev-mBvalueNew)/mBvaluePrev > delta_BU)
     			Q3 = 0;
     			
   		else	// (mBvaluePrev-mBvalueNew)/mBvaluePrev <= delta_BU 
   			{
     			for (k=1; k<= final_num_features-1; ++k)
       			group_of_bands[k] = GPOmatrix[IGBMax][k];
			
				portionDifference = (mBvalueNew-mBvaluePrev) / mBvaluePrev * 100.;
				LoadDItemRealValue (gStatusDialogPtr, IDC_Status14, portionDifference, 1);
       			
     			group_of_bands[final_num_features] = 0;
     			Q3 = 1;
     			mBvaluePrev = mBvalueNew;
     
     			final_num_features--;
   		
						// List Actual Group_of_bands
	   		
	   		if (continueFlag)
					continueFlag = ListGroupOfBands (mBvaluePrev, 
																resultsFileStreamPtr, 
																group_of_bands, 
																final_num_features,
																number_of_classes,
																mBIndex);
	     			
   			}	// end "else (mBvalue1-mBvalue)/mBvalue1 <= delta_BU"
   			
   		if (!continueFlag)
   			break;
				
 			}	// end "if ((final_num_features < min_num_samples-1) && ..."
 			
 		else	// (final_num_features >= min_num_samples-1) || ...
   		Q3 = 0;
   		
 		}	// end "while ((QQ+Q3 != 0) && ..."
 		
   free_imatrix (GPOmatrix, 1, number_of_features, 1, number_of_features);
	free_ivector (saved_group_of_bands, 1, number_of_features);
   free_ivector (group_minus_one, 1, number_of_features);
	free_ivector (group_plus_one, 1, number_of_features);
	free_dvector (group_minBhatt, 1, number_of_features);
  		
			// Set the value of the pointer as the final number of features.
			
  	*pfinal_num_featuresPtr = final_num_features;
  	
  	return (continueFlag);

}	// end "First_stage_matrix2HB"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean First_stage_matrix2TD
//
//	Software purpose:	This function constructs the apriori matrix PA. The Top_Down 
//							Version of the binary tree
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:
//
// Variable used in routine:
//
// IGB    is the Index for different combinations of
// 	    group of bands.
// IGBMax is the maximum IGB (combination of group of bands with
// 		 the maximum Bhattacharyya (minimum among the classes)
// 		 distance.
// 
// Q1     is the quantity (= 0 or 1) that indicate when Top Down
// 		 algorithm has been succesful growing more than the
// 		 threshold delta_TD.
// 
// group_plus_one  is a vector that contains the group of adjacent
// 					 bands when one of the groups has been divided
// 					 in two. Is for the Top Down part.
// 
// GPOmatrix        Matrix that store all possible combinations of
// 					  group_plus_one and group_minus_one.
// 
// group_minBhatt   Store the minimum Bhattacharyya distance for
// 					  all possible combination of group_plus_one
// 					  or group_minus_one.
// 
//	Coded By:			Luis Jiminez			Date: ??/??/1996

Boolean First_stage_matrix2TD (
				SInt32	 							number_of_features, 
				SInt32	 							number_of_classes,
    			double 								delta_TD, 
    			SInt16 								feature_type,
      		UInt16								bothOddChoicesNumberFeatures, 
    			SInt32*								group_of_bands,
   			SInt32*								last_group_of_bands,
   			SInt32	 							max_number_features, 
    			SInt32*								pfinal_num_featuresPtr)

{
	double 								mBvaluePrev;
	
	double 								*group_minBhatt = NULL;
	SInt32 								*group_plus_one = NULL;
	SInt32 								**GPOmatrix = NULL; 

	SInt32 								final_num_features;
						
	UInt32								mBIndex;
	
	SInt16								Q1;
	
	Boolean								continueFlag = TRUE;
	

			// Initialization of variables

	Q1 = 1;
	mBvaluePrev = -1.;
	
	final_num_features = *pfinal_num_featuresPtr;

	try
		{
				// Construct apriori matrix PA for first case.
	
		continueFlag = First_stage_matrix (group_of_bands,
														last_group_of_bands,  
														final_num_features,
														number_of_features, 
														number_of_classes, 
														feature_type,
														&mBvaluePrev,
														&mBIndex);
	          					
		SaveTransformationToVector (gPmatrix[gPAIndex],
												gPmatrix[gPAIndex-1],
												gPmatrix,
												group_of_bands,
												final_num_features);
	
				// Initialize matrices.
	
		group_minBhatt = dvector (1, number_of_features); 
		group_plus_one = ivector (1, number_of_features + 1);  
		GPOmatrix = imatrix (1, number_of_features, 1, number_of_features);
		
		}	// end try
		
	catch	(...)
		{
		continueFlag = FALSE;
		
		}	// end catch
	
			// List the initial of group of bands used.
			
	if (continueFlag)
		continueFlag = ListGroupOfBands (mBvaluePrev, 
													NULL, 
													group_of_bands, 
													final_num_features,
													number_of_classes,
													mBIndex); 

			// Start the Top Down algorithm under some conditions.
			
	if (!continueFlag)
		Q1 = 0;

	while ((Q1 != 0) && 
				(final_num_features < max_number_features) &&
					(final_num_features < min_num_samples-1)) 
		{
		final_num_features++;
		
		continueFlag = First_stage_topDownAlgorithm (final_num_features, 
																	feature_type,
																	bothOddChoicesNumberFeatures, 
																	number_of_features, 
																	number_of_classes,
																	&mBvaluePrev,
																	&Q1,
																	group_of_bands,
																	last_group_of_bands,
																	group_minBhatt, 
																	group_plus_one,  
																	GPOmatrix,
																	delta_TD);
			
		if (!continueFlag || gAlertReturnCode == 1)
			Q1 = 0;
			
		}	// end "while ((Q1 != 0) && ..."

	free_ivector (group_plus_one, 1, number_of_features);
	free_dvector (group_minBhatt, 1, number_of_features);
	free_imatrix (GPOmatrix, 1, number_of_features, 1, number_of_features);
	
			// Set value of pointer to final_num_features.

	*pfinal_num_featuresPtr = final_num_features;
  
	return (continueFlag);

}	// end "First_stage_matrix2TD"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 First_stage_topDownAlgorithm
//
//	Software purpose:	The purpose of this routine is to run the Projection
//							Pursuit top-down algorithm.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	0 if threshold has been met
//							1 if threshold has not been met
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: 04/??/1996
//	Revised By:			Larry L. Biehl			Date: 02/07/2018
	
Boolean First_stage_topDownAlgorithm (
				SInt32								final_num_features, 
    			SInt16 								feature_type,
      		UInt16								bothOddChoicesNumberFeatures, 
				SInt32	 							number_of_features, 
				SInt32	 							number_of_classes,
    			double*								mBvaluePrevPtr,
    			SInt16*								Q1Ptr,
    			SInt32*								group_of_bands,
   			SInt32*								last_group_of_bands,
				double*								group_minBhatt, 
				SInt32*								group_plus_one,  
				SInt32**								GPOmatrix,
    			double 								delta_TD)

{
	UInt32			IGBMax,
						num_minus_ones;
	
	UInt32 			i, 
						IGB,
						j, 
						k,
						mBIndex,
						numberLoops,
						Q1,
						tempNumber;
	
	double 			maxBValue,
						mBvalueNew,
						portionDifference;
	
	Boolean			continueFlag = TRUE;
	

	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status16, 
							(SInt32)final_num_features);
										
			// To make sure there is not a projection with singular matrices
			// the algorithm checks if final_num_features is less than the
			// minimum number of samples/class -1.

   Q1 = 0;
   if (final_num_features < min_num_samples)
   	{
		IGB = 0;
		maxBValue = -1;

				// Start to divide every group of adjacent bands in two adjacent groups.
		
		num_minus_ones = 0;
		for (i=1; i<=(UInt32)final_num_features-1; i++) 
			{
					// Divide the group if it is not equal to one.

			if (group_of_bands[i] != 1) 
				{
						// Divide the group "i" in two new groups and store that 
						// combination of group of bands in group_plus_one.

				group_bands_insert (group_of_bands, 
											final_num_features-1,
											i, 
											group_plus_one);
          	
          			// Set up if both choices for odd groups are to be evaluated.
          			
          	numberLoops = 1;
				if (final_num_features < bothOddChoicesNumberFeatures && 
																(group_of_bands[i] & 0x00000001))
          		numberLoops = 2;
									
          	for (j=1; j<=numberLoops; j++)
          		{	
					IGB++;
			
							// Store that combination in GPOmatrix.
	
					for (k=1; k<=(UInt32)final_num_features; k++)
						GPOmatrix[IGB][k] = group_plus_one[k];
	        
							// Construct the apriori matrix PA for that particular 
							// combination of group of bands.
	
					continueFlag = First_stage_matrix (group_plus_one,
																	last_group_of_bands,  
																	final_num_features,
																	number_of_features, 
																	number_of_classes, 
																	feature_type,
																	&mBvalueNew,
																	&mBIndex);
	          								
	          	if (!continueFlag)
	          		break;
	          		
	          	if (mBvalueNew > maxBValue)
	          		{
						SaveTransformationToVector (gPmatrix[gPAIndex],
																gPmatrix[gPAIndex-1],
																gPmatrix,
																group_plus_one,
																final_num_features);
									
						maxBValue = mBvalueNew;
						IGBMax = IGB;
									
						}	// end "if (mBvalueNew > maxBValue)"
					
					if (j < numberLoops)
						{
								// This is a split of an odd group. Check the other
								// combination.
								
						tempNumber = group_plus_one[i];
						group_plus_one[i] = group_plus_one[i+1];
						group_plus_one[i+1] = tempNumber;
						
						}	// end "if (j < numberLoops)"
						
					num_minus_ones++;
					
					}	// end "for (j=1; j<=numberLoops; j++)"
	          								
          	if (!continueFlag)
          		break;

				}	// end "if (group_of_bands[i] != 1)"
				
			}	// end "for (i=1; i<=final_num_features-1; i++)"
          								
    	if (continueFlag)
    		{
			for (k=1; k<=(UInt32)final_num_features; k++)
				group_of_bands[k] = GPOmatrix[IGBMax][k];
	
					// List actual combination of group of bands.
					
			continueFlag = ListGroupOfBands (maxBValue, 
														NULL, 
														group_of_bands, 
														final_num_features,
														number_of_classes,
														mBIndex);
	
					// If the increment is less than the threshold then Top Down 
					// has completed. If is larger then Q1 = 1 and loop though again
					// with final_num_features increased by one.
			
			portionDifference = (maxBValue-*mBvaluePrevPtr) / *mBvaluePrevPtr;
			if (portionDifference < delta_TD)
				Q1 = 0;
				
			else	// (mBvalue-mBvalue1)/mBvalue1 >= delta_TD
				Q1 = 1;
			
			LoadDItemRealValue (
									gStatusDialogPtr, IDC_Status14, portionDifference*100, 1);
				
			*mBvaluePrevPtr = maxBValue; 
				
			}	// end "if (continueFlag)"

		}	// end "if (final_num_features < min_num_samples)"

	*Q1Ptr = (SInt16)Q1;
		
	return (continueFlag);
			
}	// end "First_stage_topDownAlgorithm"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double** free_dmatrix
//
//	Software purpose:	The purpose of this routine is to free a double matrix 
//							allocated by dmatrix ().
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

double** free_dmatrix (
				double**								m,
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch)
				
{
	if (m != NULL)
		{
		if (m[nrl] + ncl - NR_END != NULL)
			CheckAndDisposePtr ((FREE_ARG)(m[nrl] + ncl - NR_END));
			
		if (m + nrl - NR_END != NULL)
			CheckAndDisposePtr ((FREE_ARG)(m + nrl - NR_END));
			
		}	// end "if (m != NULL)"
        
   return (NULL);
   
}	// end "free_dmatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double** free_dvector
//
//	Software purpose:	The purpose of this routine is to free a double vector 
//							allocated with dvector ().
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

double* free_dvector (
				double*								lv,
				SInt32 								nl,
				SInt32 								nh)
				
{
	if (lv != NULL && lv+nl-NR_END != NULL)
		CheckAndDisposePtr ((FREE_ARG)(lv+nl-NR_END));
        
   return (NULL);
   
}	// end "free_dvector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32** free_imatrix
//
//	Software purpose:	The purpose of this routine is to free an SInt32 matrix 
//							allocated by imatrix ().
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

SInt32** free_imatrix (
				SInt32**								m,
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch)
				
{
	if (m != NULL)
		{
		if (m[nrl] + ncl - NR_END != NULL)
			CheckAndDisposePtr ((FREE_ARG)(m[nrl] + ncl - NR_END));
			
		if (m + nrl - NR_END != NULL)
			CheckAndDisposePtr ((FREE_ARG)(m + nrl - NR_END));
			
		}	// end "if (m != NULL)"
        
   return (NULL);
   
}	// end "free_imatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32* free_ivector
//
//	Software purpose:	The purpose of this routine is to free an SInt32 vector 
//							allocated with ivector ().
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

SInt32* free_ivector (
				SInt32*								lv,
				SInt32 								nl,
				SInt32 								nh)
				
{
	if (lv != NULL && lv +nl - NR_END != NULL)
        CheckAndDisposePtr ((FREE_ARG)(lv + nl - NR_END));
        
   return (NULL);
   
}	// end "free_ivector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double f1dim
//
//	Software purpose:	
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

double f1dim (
				double 								x)
				
{
	double 								f;
	
	UInt32								j;
	
	
	for (j=1; j<=(UInt32)ncom; j++) 
		g_f1dim_xt[j] = g_linmin_pcom[j] + x*g_linmin_xicom[j];
		
	f = (*nrfunc)(g_f1dim_xt);
	
	return f;
	
}	// end "f1dim"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void get_sub_matrix
//
//	Software purpose:	The purpose of this routine is to compute the submatrix 
//							of SS, starting from row1 and col1 to  row2 and col2.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

void get_sub_matrix (
				double*								inputMatrixPtr,
				double*								outputMatrixPtr, 
				UInt32 								rowStart, 
				UInt32 								rowEnd, 
				UInt32 								colStart, 
				UInt32 								colEnd,
				UInt32								numberFeatures)

{
	UInt32 								i, 
											inputMatrixSkip, 
											j;
  

	inputMatrixPtr += (rowStart - 1)*numberFeatures + colStart - 1;
	inputMatrixSkip = numberFeatures - colEnd + colStart - 1;
	
	for (i=rowStart; i<=rowEnd; i++) 
		{
		for (j=colStart; j<=colEnd; j++)
			{
			*outputMatrixPtr = *inputMatrixPtr;
			inputMatrixPtr++;
			outputMatrixPtr++;
			
			}	// end "for (j=colStart; j<=colEnd; j++)"
			
		inputMatrixPtr += inputMatrixSkip;

 		}	// end "for (i=rowStart; i<=rowEnd; i++)"

  return;

}	// end "get_sub_matrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void group_bands_delete
//
//	Software purpose:	from i = 1 to # features-1 it adds i and i+1 in
//						   the group of adjacent bands.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

void group_bands_delete (
				SInt32*								group_of_bands, 
				SInt32 								final_num_features,
       		SInt32 								index_of_change, 
       		SInt32*								group_minus_one) 

{ 
	UInt32		 i;
	
	
	for (i=1; i<(UInt32)index_of_change; i++)
		group_minus_one[i] = group_of_bands[i];
		
	group_minus_one[index_of_change] = group_of_bands[index_of_change]
                                  				+ group_of_bands[index_of_change+1];

	if (final_num_features > index_of_change+1) 
		{
		for (i=(UInt32)index_of_change+2; i<=(UInt32)final_num_features; i++)
			group_minus_one[i-1] = group_of_bands[i];
			
		}

}	// end "group_bands_delete"

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void group_bands_insert
//
//	Software purpose:	function where group "index_of_change" of adjacent bands 
//							is divided in two other adjacent groups.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996
  
void group_bands_insert (
				SInt32*								group_of_bands, 
				SInt32 								final_num_features,
        		SInt32 								index_of_change, 
        		SInt32*								group_plus_one)

{
	SInt32		 						i;
	
	SInt16								randomValue;
	

			// For case is index_of_change is greater than first element.

	for (i=1; i<index_of_change; i++)
		group_plus_one[i] = group_of_bands[i];
		
	if ((group_of_bands[index_of_change] & 0x00000001) == 0)
		{
		group_plus_one[index_of_change] = group_of_bands[index_of_change]/2;
		group_plus_one[index_of_change+1] = group_plus_one[index_of_change];
		
		}	// end "if ((group_of_bands[index_of_change] & 0x00000001) == 0)"
		
	else	// (group_of_bands[index_of_change] & 0x00000001) == 1
		{
		randomValue = PPRandom ();
		group_plus_one[index_of_change] = 
									group_of_bands[index_of_change]/2 + randomValue;
		group_plus_one[index_of_change+1] = 
									group_of_bands[index_of_change]/2 + (1-randomValue);
			
		}	// end "else (group_of_bands[index_of_change] & 0x00000001) == 1"

	for (i=index_of_change+2; i<=final_num_features+1; i++)
		group_plus_one[i] = group_of_bands[i-1];
  
}	// end "group_bands_insert"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 **imatrix
//
//	Software purpose:	The purpose of this routine is to allocate a SInt32 
//							matrix with subscript range m[nrl..nrh][ncl..nch].
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996
 
SInt32 **imatrix (
				SInt32 								nrl,
				SInt32 								nrh,
				SInt32 								ncl,
				SInt32 								nch)
				
{
	SInt32 								**m;

	SInt32 								i, 
											nrow=nrh-nrl+1,
											ncol=nch-ncl+1;
											
											
			// allocate pointers to rows
        
	m = (SInt32 **) MNewPointerClear ((UInt32)(nrow+NR_END)*sizeof (SInt32*));
	
	if (m != NULL)
		{
		m += NR_END;
		m -= nrl;

				// allocate rows and set pointers to them
        
		m[nrl]=(SInt32 *)MNewPointerClear ((UInt32)(nrow*ncol+NR_END)*sizeof (SInt32));
		if (m[nrl] != NULL)
			{
			m[nrl] += NR_END;
			m[nrl] -= ncl;

			for (i=nrl+1;i<=nrh;i++) 
        		m[i]=m[i-1]+ncol;
        		
        	}			// end "if (m[nrl] != NULL)"
			
		else	// Not enough memory
			{
					// Free the vector for the addresses.
					
			CheckAndDisposePtr ((FREE_ARG)(m+nrl-NR_END));
			
			throw ((OSErr)memFullErr);
			
			}
        	
      }	// end "if (m != NULL)"
			
	else	// Not enough memory
		throw ((OSErr)memFullErr);

			// return pointer to array of pointers to rows
			
	return m;
	
}	// end "imatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 imax_elm_vector
//
//	Software purpose:	The purpose of this routine is to find the maximum 
//							element of a integer vector.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

SInt32 imax_elm_vector (
				SInt32*								vect, 
				UInt32								number_of_elm)

{
	SInt32 								iMax;
	UInt32 								i;

		
	iMax = 0;
	for (i=1; i<=number_of_elm; i++)
		iMax = MAX (iMax, vect[i]);

	return iMax;
  
}	// end "imax_elm_vector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 *ivector
//
//	Software purpose:	The purpose of this routine is to allocate an SInt32 
//							vector with subscript range lv[nl..nh].
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

SInt32 *ivector (
				SInt32 								nl,
				SInt32								nh)
				
{
	SInt32*								lv;
	

	lv = (SInt32 *)MNewPointerClear ((size_t)(nh-nl+1+NR_END)*sizeof (SInt32));
       
   if (lv != NULL)
		lv -= nl-NR_END;
			
	else	// Not enough memory
		throw ((OSErr)memFullErr);
		
	return lv;
	
}	// end "ivector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void linmin
//
//	Software purpose:	The purpose of this routine is to allocate an SInt32 
//							vector with subscript range lv[nl..nh].
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

#define TOL 2.0e-4

void linmin (
				double 								p[], 
				double 								xi[], 
				SInt32 								n, 
				double*								fret, 
				double 								(*func)(double []))
				
{
	double 								xx,
											xmin,
											fx,
											fb,
											fa,
											bx,
											ax;
											
	SInt32 								j;
	

	ncom = n;
	nrfunc = func;
	for (j=1; j<=n; j++) 
		{
		g_linmin_pcom[j]=p[j];
		g_linmin_xicom[j]=xi[j];
		
		}	// end "for (j=1; j<=n; j++)"
		
	ax = 0.0;
	xx = 1.0;
	mnbrak (&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
  	if (!gOperationCanceledFlag)
  		{																							
		*fret = brent (ax, xx, bx, f1dim, TOL, &xmin);
		for (j=1; j<=n; j++) 
			{
			xi[j] *= xmin;
			p[j] += xi[j];
			
			}	// end "for (j=1; j<=n; j++)"
			
		}	// end "if (!gOperationCanceledFlag)"
        
}	// end "linmin"

#undef TOL



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListGroupOfBands
//
//	Software purpose:	The purpose of this routine is List group of bands for 
//							this iteration.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: ??/??/1996
// Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean ListGroupOfBands (
				double								mBvalue,
				CMFileStream*						resultsFileStreamPtr,
				SInt32*								group_of_bands, 
				SInt32 								final_num_features,
				SInt32								number_of_classes,
				UInt32								minClassIndex) 

{
	SInt16*								classPairWeightPtr;
	
	UInt32								counter,
											classPairCount,
											class1,
											class2,
											i,
											j;
	
	SInt32								k;
	
	Boolean								continueFlag = TRUE;
	
	
			// Get the minimum Bhattacharyya class pair.
			
	classPairWeightPtr = (SInt16*)GetHandlePointer (
															gProjectInfoPtr->classPairWeightsHandle);
			
	class1 = 0;
	counter = 1;
	classPairCount = 0;
	for (i=1; i<(UInt32)number_of_classes; i++) 
		{   
		for (j=1+i; j<=(UInt32)number_of_classes; j++) 
			{
			if (classPairWeightPtr[classPairCount] > 0)
				{
				if (counter == minClassIndex)
					{
					class1 = i;
					class2 = j;
					break;
					
					}	// end "if (counter == minClassIndex)"
					
				counter++;
				
				}	// end "if (classPairWeightPtr[classPairCount] > 0)"
				
			classPairCount++;

			}	// end "for (j=1+i; j<=number_of_classes; j++)"
			
		if (class1 != 0)
			break;

		}	// end "for (i=1; i<number_of_classes; i++)"

			// List line in Channel Decision Tree Table
			
	sprintf ((char*)gTextString, "    %5d       %9.4f      %3u-%3u   [",
					(int)final_num_features,
					mBvalue,
					(unsigned int)class1,
					(unsigned int)class2);
	continueFlag = OutputString (resultsFileStreamPtr, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code,
											continueFlag);
											
	for (k=1; k<=final_num_features; ++k)
		{
		sprintf ((char*)gTextString, "%5d", (int)group_of_bands[k]);
		continueFlag = OutputString (resultsFileStreamPtr, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code,
												continueFlag);
		
		}	// end "for (k=1; k<= final_num_features; ++k)"
			
	sprintf ((char*)gTextString, "   ]%s", gEndOfLine);
	continueFlag = OutputString (resultsFileStreamPtr, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code,
											continueFlag);
				
			// Scroll output window to the selection and adjust the 		
			// scroll bar. This allows the user to see the last minimum
			// Bhattacharyya distance.																
	
	//UpdateOutputWScrolls (gOutputWindow, 1, kDisplayNoMessage); 
											
	return (continueFlag);

}	// end "ListGroupOfBands"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean minBhatt
//
//	Software purpose:	Given a matrix PA, the purpose of this routine is to 
//							calculate the minimum Bhattacharyya distance among the classes.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	0 if the user cancelled processing
//							1 if processing is to continue
//
// Called By:			
//
// Variables used in routine:
// MM      Matrix that columns holds the mean vector for each class.
// 
// SS      Matrix that holds the covariances matrices for each class.
// 
// M1      Matrix that holds the mean vector of one class.
// 
// M2      Matrix that holds the mean vector of another class.
// 
// S1      Matrix that holds the covariance matrix of one class.
// 
// S2      Matrix that holds the covariance matrix of one class.
// 
// invS2   Matrix that holds the inverse of S2.
// 
// sumS1S2 Matrix that holds the sum of S1 and S2.
// 
// detS1   Determinant of S1.
// 
// detS2   Determinant of S2.
// 
// detsumS1S2      Determinant of sumS1S2.
// 
// M1p     Matrix M1 after projection.
// 
// M2p     Matrix M2 after projection.
// 
// S1p     Matrix S1 after projection.
// 
// S2p     Matrix S2 after projection.
// 
// SSp
//
// Coded By:			Luis Jiminez			Date: 04/??/1996
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

Boolean minBhatt (
				double*								PA,
				SInt32 								number_of_features, 
				SInt32 								final_num_features, 
				SInt32 								number_of_classes,
				double*								minBhatPtr,
				UInt32*								minIndexPtr)

{ 			
	SInt16*								classPairWeightPtr;
	
	UInt32 								classPairCount,
											counter,
											i,
											index, 
											j;
	
	Boolean								continueFlag = TRUE,
											determinantOKFlag;
	
	
	*minBhatPtr = 0;
		
			// Get the inter class weight list pointer.
			
	classPairWeightPtr = (SInt16*)GetHandlePointer (
														gProjectInfoPtr->classPairWeightsHandle);
 			
	for (index=0; index<(UInt32)number_of_classes; index++)
		{
		MatrixMultiply (PA, 
								gClassInfoPtr[index].meanPtr, 
								gClassInfoPtr[index].transformedMeanPtr, 
								(UInt16)final_num_features,
								(UInt16)number_of_features,
								1,
								kMat1ByMat2);
	 				
	 	TransformSymmetricMatrix (PA, 
											gClassInfoPtr[index].covariancePtr, 
											gTempMatrixPtr, 
											gClassInfoPtr[index].transformedCovPtr,
											(UInt16)final_num_features,
											(UInt16)number_of_features, 
											kTriangleOutputMatrix);									
					
		 determinantOKFlag = InvertLowerTriangularMatrix (
															gClassInfoPtr[index].transformedCovPtr, 
															NULL,
															(SInt16)final_num_features, 
															&gClassInfoPtr[index].determinant,
															&gClassInfoPtr[index].logDeterminant,
															kGetDeterminantOnly);
												
		if (!determinantOKFlag)
																			return (continueFlag);
					
		}	// end "for (index=0; index<number_of_classes; index++)"
	
	counter = 0;
	classPairCount = 0;
	for (i=0; i<(UInt32)number_of_classes-1; i++) 
		{   
		for (j=1+i; j<(UInt32)number_of_classes; j++) 
			{
			if (classPairWeightPtr[classPairCount] > 0)
				{
				counter++;
																		
				gMinBhatt_Bdist[counter] = Bhattacharyya (
																gClassInfoPtr[i].transformedMeanPtr, 
																gClassInfoPtr[j].transformedMeanPtr, 
																gClassInfoPtr[i].transformedCovPtr, 
																gClassInfoPtr[j].transformedCovPtr,
																gClassInfoPtr[i].logDeterminant, 
																gClassInfoPtr[j].logDeterminant,
																gInverseMatrixMemory.pivotPtr, 
																(SInt16)final_num_features);
			
				if (gOperationCanceledFlag)
					break;
				
				}	// end "if (classPairWeightPtr[classPairCount] > 0)"
				
			classPairCount++;

			}	// end "for (j=1+i; j<=number_of_classes; j++)"
			
		if (gOperationCanceledFlag)
			{
			continueFlag = FALSE;
			break;
			
			}	// end "if (gOperationCanceledFlag"
			
		}	// end "for (i=0; i<number_of_classes-1; i++)" 

			// Calculate the minimum Bhattacharyya distance.

	if (continueFlag)
		*minIndexPtr = FindMinValueInVector (
												&gMinBhatt_Bdist[1], counter, minBhatPtr) + 1;
		
	return (continueFlag);

}	// end "minBhatt"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double minBhatt_function
//
//	Software purpose:	Given the matrix PAmatrix and the other global variables, 
//							vectors and matrices, this function will compute the 
//							minimum Bhattacharyya distance for a combination of bands in 
//							the group of adjacent bands which correspond to the global_Index 
//							value.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

double minBhatt_function (
				double*								a)

{
	double 								mBhatt;
	
	double*								Pa;
		
	SInt32 								suma, 
											i, 
											j;
					
	UInt32								minBIndex;
	
	Boolean 								continueFlag = TRUE;
	
	
			// The matrix Pa is constructed from the matrix PAmatrix (which is global)
			// except for the values in the vector in a for the specified group of 
			// bands.
			
	Pa = gAMatrixPtr;
	for (i=1; i<=global_final_features; i++)
		{
		for (j=1; j<=global_num_features; j++)
			{
			*Pa = *PAmatrix;
			Pa++;
			PAmatrix++;
			
			}	// end "for (j=1; j<=global_num_features; j++)"
			
		}	// end "for (i=1; i<=global_final_features; i++)"
		
	Pa = gAMatrixPtr;
	PAmatrix = gTransformMatrixPtr;

	suma = 0;
	if (global_Index > 1) 
		{
		for (i=1; i< global_Index; i++)
      	suma += global_group_bands[i];
    
  		}	// end "f (global_Index > 1)"
  
	for (j=1; j<=global_group_bands[global_Index]; j++)
		Pa[(global_Index-1)*global_num_features+j+suma-1] = a[j];

			// Compute the negative value of minimum Bhattacharyya distance. This 
			// is in case a minimization algorithm is used. If a maximization 
			// algorithm is used then a positive value should be computed.

	continueFlag = minBhatt (Pa, 
										global_num_features, 
										global_final_features, 
										global_num_classes,
										&mBhatt,
										&minBIndex);

	return -mBhatt;

}	// end "minBhatt_function"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void mnbrak
//
//	Software purpose:	
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

#define GOLD 1.618034
#define GLIMIT 100.0
#define TINY 1.0e-20
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

void mnbrak (
				double*								ax, 
				double*								bx, 
				double*								cx, 
				double*								fa, 
				double*								fb, 
				double*								fc,
        		double 								(*func)(double))
        		
{
	double 								dum,
											fu,
											q,
											r,
											u,
											ulim;
	

	*fa = (*func)(*ax);										
  	if (gOperationCanceledFlag)
  																								return;
	*fb = (*func)(*bx);
  	if (gOperationCanceledFlag)
  																								return;
	if (*fb > *fa) 
		{
		SHFT (dum,*ax,*bx,dum)
		SHFT (dum,*fb,*fa,dum)
		
		}	// end "if (*fb > *fa)"
		
	*cx = (*bx)+GOLD*(*bx-*ax);
	*fc = (*func)(*cx);
  	if (gOperationCanceledFlag)
  																								return;
	
	while (*fb > *fc) 
		{
		r = (*bx-*ax) * (*fb-*fc);
		q = (*bx-*cx) * (*fb-*fa);
		u = (*bx) - ((*bx-*cx)*q - (*bx-*ax)*r) /
                        (2.0 * SIGN (DMAX (fabs (q-r), TINY), q-r));
		ulim = (*bx) + GLIMIT * (*cx-*bx);
		if ((*bx-u) * (u-*cx) > 0.0) 
			{
			fu = (*func)(u);
			if (fu < *fc) 
				{
				*ax = (*bx);
				*bx = u;
				*fa = (*fb);
				*fb = fu;
										                                				return;
				}	// end "if (fu < *fc)"						                                
			
			else if (fu > *fb) 
				{
				*cx = u;
				*fc = fu;
                                														return;
				}	// end "else if (fu > *fb)"
					
			u = (*cx) + GOLD * (*cx-*bx);
			fu = (*func)(u);
			
			}	// end "if ((*bx-u)*(u-*cx) > 0.0)" 
			
		else if ((*cx-u) * (u-ulim) > 0.0) 
			{
			fu = (*func)(u);
			if (fu < *fc) 
				{
				SHFT (*bx, *cx, u, *cx+GOLD*(*cx-*bx))
				SHFT (*fb, *fc, fu, (*func)(u))
				
				}	// end "if (fu < *fc)"
				
			}	// end "else if ((*cx-u)*(u-ulim) > 0.0)" 
		
		else if ((u-ulim) * (ulim-*cx) >= 0.0) 
			{
			u = ulim;
			fu = (*func)(u);
			
			}	// end "else if ((u-ulim)*(ulim-*cx) >= 0.0)" 
			
		else 
			{
			u = (*cx) + GOLD * (*cx-*bx);
			fu = (*func)(u);
                
			}	// end "else 'rest'"
			
  		if (gOperationCanceledFlag)
  																								return;
		
		SHFT (*ax, *bx, *cx, u)
		SHFT (*fa, *fb, *fc, fu)
		
		}	// end "while (*fb > *fc)"
        
}	// end "mnbrak"

#undef GOLD
#undef GLIMIT
#undef TINY
#undef SHFT



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean powell
//
//	Software purpose:	
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

#define ITMAX 200

Boolean powell (
				double 								p[], 
				double**								xi, 
				SInt32 								n, 
				double 								ftol, 
				SInt32*								iter, 
				double*								fret,
        		double 								(*func)(double []))
        
{
	double 								del,
											fp,
											fptt,
											t;
	
	SInt32 								i,
											ibig,
											j;
											

	*fret = (*func)(p);
  	if (gOperationCanceledFlag)
  																						return (FALSE);
	
	for (j=1; j<=n; j++) 
		g_powell_pt[j] = p[j];
		
	for (*iter=1; ; ++(*iter)) 
		{
		fp = (*fret);
		ibig = 0;
		del = 0.0;
		for (i=1; i<=n; i++) 
			{
			for (j=1; j<=n; j++) 
				g_powell_xit[j] = xi[j][i];
				
			fptt=(*fret);
			linmin (p, g_powell_xit, n, fret, func);
  			if (gOperationCanceledFlag)
  				break;
  				
			if (fabs (fptt-(*fret)) > del) 
				{
				del = fabs (fptt-(*fret));
				ibig = i;
				
 				}	// end "if (fabs (fptt-(*fret)) > del)"
 				
			}	// end "for (i=1;i<=n;i++)"
			
  		if (gOperationCanceledFlag)
  			break;
			
		if (2.0*fabs (fp-(*fret)) <= ftol*(fabs (fp)+fabs (*fret))) 
			break;	
			
		if (*iter >= ITMAX)
			{ 
			//nrerror ("powell exceeding maximum iterations.");
			gOperationCanceledFlag = TRUE;
			
			}	// end "if (*iter >= ITMAX)"
			
		for (j=1; j<=n; j++) 
			{
			g_powell_ptt[j] = 2.0*p[j] - g_powell_pt[j];
			g_powell_xit[j] = p[j] - g_powell_pt[j];
			g_powell_pt[j] = p[j];
			
			}	// end "for (j=1; j<=n; j++)"
			
		fptt = (*func)(g_powell_ptt);
		if (fptt < fp) 
			{
			t = 2.0*(fp-2.0*(*fret)+fptt) * DSQR (fp-(*fret)-del);
			t -= del * DSQR (fp-fptt);
			if (t < 0.0) 
				{
				linmin (p, g_powell_xit, n, fret, func);
  				if (gOperationCanceledFlag)
  					break;
				
				for (j=1; j<=n; j++) 
					{
					xi[j][ibig] = xi[j][n];
					xi[j][n] = g_powell_xit[j];
					
					}	// end "for (j=1; j<=n; j++)"
					
				}	// end "if (t < 0.0)"
				
			}	// end "if (fptt < fp)"
			
		}	// end "for (*iter=1; ; ++(*iter))"
	
	return (!gOperationCanceledFlag);
        
}	// end "powell"

#undef ITMAX



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ProjectionPursuit
//
//	Software purpose:	
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
//
// Called By:			
//
// Variables used in routine:
// 
// number_of_features -> total number of features used in the statistics.
// final_num_features -> the final number of features after PP has been
//                       applied. 
// 
// number_of_classes     The total number of classes in the statistics used
//                       in this function.
// 
// group_of_bands        Vector that contains the number of adjacent bands
//                       that will be linearly combined.
// 
// delta_TD              the threshold were the Top Down will stop.
// delta_BU              The threshold at which Bottom Up algorithm will
//                       stop.
//
// Coded By:			Luis Jiminez			Date: ??/??/1996
//	Revised By:			Larry L. Biehl			Date: 11/05/2005

Boolean ProjectionPursuit (
				SInt32								numberFeatures, 
				SInt32								numberClasses, 
				ClassInfoPtr				 		quickClassInfoPtr,
				double*								eigenVectorPtr,
				double*								tempMatrixPtr)

{
	double 								delta_TD, 
											delta_BU, 
											delta_Optimization,
											value,
											value1;
					
	CMFileStream*						resultsFileStreamPtr = NULL;

	ProjectionPursuitSpecsPtr 		projectionPursuitSpecsPtr;

	UInt16*								channelsPtr;

	SInt32								*group_of_bands = NULL,
											*last_group_of_bands = NULL;
	
	SInt32 								comb, 
											final_num_features,
											i,
											max_number_features, 
											number_of_classes,
											number_of_features;
										
	UInt32								k,
											MAX,
											mBIndex,
											minBIndex,
											sum;
										
	Boolean								continueFlag = TRUE;
										
										
			// Initialize local variables.
			
	projectionPursuitSpecsPtr = &gFeatureExtractionSpecsPtr->projectionPursuitParams;
									
	gClassInfoPtr = quickClassInfoPtr;
	gTempMatrixPtr = tempMatrixPtr;
	gTransformMatrixPtr = &tempMatrixPtr[numberFeatures*numberFeatures];
											
			// Threshold variables, delta_TD > deltaBU
			
	delta_TD = projectionPursuitSpecsPtr->tdThreshold/100.;
	delta_BU = projectionPursuitSpecsPtr->buThreshold/100.;
	delta_Optimization = projectionPursuitSpecsPtr->optimizationThreshold/100.;
	
			// This can control the final number of features.

	min_num_samples = projectionPursuitSpecsPtr->minimumNumberSamples;
	
	number_of_features = numberFeatures;
	number_of_classes = numberClasses;
	max_number_features = MIN (projectionPursuitSpecsPtr->maximumNumberFeatures,
										number_of_features);

	channelsPtr = (UInt16*)GetHandlePointer (
												gFeatureExtractionSpecsPtr->channelsHandle);
		
	try
		{										
				// Definition of group_of_bands to include the possibility that 
				// all group of adjacent bands contains only one feature
	
		//group_of_bands = ivector (1, max_number_features);
		group_of_bands = ivector (1, number_of_features);
		
		if (projectionPursuitSpecsPtr->algorithm != kPPFeatureSelection)
			//last_group_of_bands = ivector (1, max_number_features);
			last_group_of_bands = ivector (1, number_of_features);
		
				// Get memory for use by First_stage_matrix routine for
				// Full Projection Pursuit Algorithm.
				
		if (projectionPursuitSpecsPtr->algorithm != kPPFeatureSelection)
			{
			gFirstStage_M1 = (HDoublePtr)MNewPointer (
											(UInt32)number_of_features * sizeof (double));
			gFirstStage_M2 = (HDoublePtr)MNewPointer (
											(UInt32)number_of_features * sizeof (double));
			gFirstStage_S1 = (HDoublePtr)MNewPointer (
					(UInt32)number_of_features * number_of_features * sizeof (double));
			gFirstStage_S2 = (HDoublePtr)MNewPointer (
					(UInt32)number_of_features * number_of_features * sizeof (double));
					
			gClass_log_det_OK = (SInt32*)MNewPointer (
															number_of_classes * sizeof (SInt32));
	
	    			// Initialize determinant OK vector. Each class is an element in the
	    			// vector. If the determinant is for a class is okay then the element
	    			// for that class is set to 1. This is used in the 'First_stage_matrix'
	    			// routine where all that is need is to know whether the matrix is
	    			// ill-conditioned or not. If the determinant is okay at a high 
	    			// dimension then all sub-matrices are also okay.
	    			
			for (i=0; i<number_of_classes; i++)
	    		gClass_log_det_OK[i] = -1;
			
			}	// end "if (...->algorithm != kPPFeatureSelection)"
	
				// Get memory for use by the minBhatt routine.
				
		comb = (number_of_classes*(number_of_classes-1))/2;
		gMinBhatt_Bdist = dvector (1, comb);

				// Get memory for the gPmatrix used in 'First_stage_matrix'. Allow
				// for largest size needed.
				// The extra row at the end of gPmatrix is used for storage
				// of the max index in gPmatrix for each group_of_bands.
	
		if (projectionPursuitSpecsPtr->algorithm == kPPFeatureSelection)
			MAX = 1 + number_of_features;
			
		else
			MAX = number_of_classes*(number_of_classes-1) + 1 + number_of_features;
				
		gPmatrix = dmatrix (1, MAX+2, 1, number_of_features);
		gPAIndex = MAX+2;	
		
		ShowStatusDialogItemSet (kStatusCommand);
		ShowStatusDialogItemSet (kStatusCluster);		// For number of features
		
		if (projectionPursuitSpecsPtr->algorithm == kProjectionPursuit)
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract61,	// "\pProjection Pursuit - First Stage"
											gStatusDialogPtr, 
											IDC_Status21, 
											(Str255*)gTextString);
						
		else	// ...->algorithm == kFeatureSelection
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract62,  // "\pProjection Pursuit - Feature Selection"
											gStatusDialogPtr, 
											IDC_Status21, 
											(Str255*)gTextString);
			
				// Initialize the grouping vectors
				
		for (i=1; i<=max_number_features; ++i)
			group_of_bands[i] = 0;
			
		if (last_group_of_bands != NULL)
			for (i=1; i<=max_number_features; i++)
				last_group_of_bands[i] = 0;
				
		if (projectionPursuitSpecsPtr->initialGroupingCode == kLastGrouping)
			{
			final_num_features = projectionPursuitSpecsPtr->finalNumberFeatures;
								
			UInt16* bandGroupingPtr = (UInt16*)GetHandlePointer (
												projectionPursuitSpecsPtr->bandGroupingHandle);
				
			for (i=1; i<=final_num_features; ++i)
				group_of_bands[i] = bandGroupingPtr[i-1];
				
			}	// end "if (...->initialGroupingCode == kLastGrouping)"
			
		else	// ...->initialGroupingCode != kLastGrouping
			{
			UInt32 newInitialNumberFeatures;
			UInt32 initialNumberFeatures = 
											projectionPursuitSpecsPtr->initialNumberFeatures;
											
			if (projectionPursuitSpecsPtr->firstStageMethod == kUniformGrouping)
				initialNumberFeatures = max_number_features;
			
			newInitialNumberFeatures = initialNumberFeatures;								
			if (projectionPursuitSpecsPtr->algorithm != kPPFeatureSelection)
				{
						// Allow for case if original stats being used and the number
						// of training samples in a class is less than the number
						// of features being used.
						
				if (gProjectInfoPtr->covarianceStatsToUse == kOriginalStats ||
							gProjectInfoPtr->covarianceStatsToUse == kMixedStats)
					{
					if (min_num_samples > 1)
						newInitialNumberFeatures = MAX (
									initialNumberFeatures,
									(UInt32)(number_of_features-1)/(min_num_samples-1) + 1);
													
					}	// end "if (...->covarianceStatsToUse == kOriginalStats || ..."
													
				if (newInitialNumberFeatures > initialNumberFeatures)
					{
							//	"  The number of initial features was changed to %ld so that 
							// " the number of bands in%s   any one group would be at 
							// " least one fewer than the minimum number of class%s   
							// " samples of %ld.%s%s",
						
					continueFlag = ListSpecifiedStringNumber (
																	kFeatureExtractStrID, 
																	IDS_FeatureExtract30,
																	resultsFileStreamPtr, 
																	gOutputForce1Code, 
																	(SInt32)newInitialNumberFeatures,
																	min_num_samples, 
																	continueFlag);
									
					}	// end "if (if (newInitialNumberFeatures > ...)"
													
				}	// end "if (...->algorithm != kPPFeatureSelection)"
			
			final_num_features = newInitialNumberFeatures;
							
			SetupGroupingVector (group_of_bands, number_of_features, final_num_features);
				
			}	// end "if (...->initialGroupingCode != kLastGrouping)"
						
		if (projectionPursuitSpecsPtr->firstStageMethod != kUniformGrouping)
			{
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract59,	// "\pTD Number feature groups:"
											gStatusDialogPtr, 
											IDC_Status15, 
											(Str255*)gTextString);
										
			ShowStatusDialogItemSet (kStatusMinutes);		// For last Bhattacharyya difference
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract63,	// "\pBhattacharyya % change:"
											gStatusDialogPtr, 
											IDC_Status13, 
											(Str255*)gTextString);
			LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)"\0");
								
			}	// end "...->firstStageMethod != kUniformGrouping"
								
		else	// ...->firstStageMethod == kUniformGrouping
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract64,	// "\pNumber output features:"
											gStatusDialogPtr, 
											IDC_Status15, 
											(Str255*)gTextString);
			
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status16, 
								(SInt32)final_num_features);
			    							
				// "  Channel Decision Tree Table%s"
				// "   Number of    Minimum     Min Class%s"
				// "   Features  Bhattacharyya     Pair      Channel Grouping%s",
											
		continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID, 
																IDS_FeatureExtract32, 
																(unsigned char*)gTextString,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
			
				// Now do the requested 'first stage' method.
	
		switch (projectionPursuitSpecsPtr->firstStageMethod)
			{
			case kUniformGrouping:
				//SetupGroupingVector (
				//					group_of_bands, number_of_features, max_number_features);
				//					
				//final_num_features = max_number_features;
				
				value1 = -1;
				continueFlag = First_stage_matrix (group_of_bands,
																last_group_of_bands,  
																final_num_features,
																number_of_features, 
																number_of_classes, 
																projectionPursuitSpecsPtr->algorithm,
																&value1,
																&mBIndex);
	          						
				SaveTransformationToVector (gPmatrix[gPAIndex],
														gPmatrix[gPAIndex-1],
														gPmatrix,
														group_of_bands,
														final_num_features);
				break;
			
			case kTopDown:
				continueFlag = First_stage_matrix2TD (
										number_of_features, 
		  								number_of_classes,
		    							delta_TD, 
		    							projectionPursuitSpecsPtr->algorithm,
			    						projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures, 
		    							group_of_bands,
		    							last_group_of_bands,
		    							max_number_features,
		    							&final_num_features);
				break;
				
			case kTopDownBottomUp:
		 		continueFlag = First_stage_matrix2HB (
		 								number_of_features, 
		  								number_of_classes,
		    							delta_TD, 
		    							delta_BU,
		    							projectionPursuitSpecsPtr->algorithm,
			    						projectionPursuitSpecsPtr->bothOddChoicesNumberFeatures, 
		    							group_of_bands,
		    							last_group_of_bands,
		    							max_number_features,
		    							&final_num_features);
				break;
				
			}	// end "switch (projectionPursuitSpecsPtr->firstStageMethod)"
			
			
		if (continueFlag && 
				final_num_features < max_number_features &&
					final_num_features == min_num_samples - 1)
			{
					// "%s  The number of final features stopped at %ld because it can not"
					// " grow larger than%s   one fewer than the minimum number of class"
					// " samples of %ld.%s",
					
			continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID, 
																	IDS_FeatureExtract31,
																	resultsFileStreamPtr, 
																	gOutputForce1Code, 
																	final_num_features,
																	min_num_samples, 
																	continueFlag);
							
			}	// end "if (continueFlag && ..."
							
		if (continueFlag)
			{							
					// Create the transformation matrix that was found from the saved
					// vector.
					
			ZeroMatrix (gTransformMatrixPtr, 
								final_num_features, 
								number_of_features, 
								TRUE);
			sum = 0;
			for (i=1; i<=final_num_features; i++)
				{
				for (k=sum; k<(sum+group_of_bands[i]); k++)
					 gTransformMatrixPtr[(i-1)*number_of_features+k] = 
					 															gPmatrix[gPAIndex][k+1];
					
				sum += group_of_bands[i];
				
				}	// end "for (i=1; i<=final_features; ++i)"
				
			}	// end "if (continueFlag)"
				
				// Release memory that we do not need anymore.
				
		gPmatrix = free_dmatrix (gPmatrix, 1, MAX+2, 1, number_of_features);
		last_group_of_bands = free_ivector (last_group_of_bands, 1, max_number_features);
		
		if (continueFlag)
			{				
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract64,	// "\pNumber output features:"
											gStatusDialogPtr, 
											IDC_Status15, 
											(Str255*)gTextString);
			    						
    		if (continueFlag && 
    						projectionPursuitSpecsPtr->firstStageMethod == kUniformGrouping)
				continueFlag = ListGroupOfBands (value1, 
															NULL, 
															group_of_bands, 
															final_num_features,
															number_of_classes,
															mBIndex);
											
			normalize_rows (gTransformMatrixPtr, final_num_features, number_of_features);

			if (continueFlag && gFeatureExtractionSpecsPtr->listTransformationFlag)
				{
				continueFlag = ListSpecifiedStringNumber (
											kFeatureExtractStrID, 
												// "%s Preprocessing Channel Transformation Matrix"
											IDS_FeatureExtract33, 
											(unsigned char*)gTextString,
											resultsFileStreamPtr, 
											gOutputForce1Code,
											continueFlag);
			 
		    	if (continueFlag)
					continueFlag = ListTransformationInformation (
												NULL, 
												gTransformMatrixPtr, 
												0, 
												0,
												channelsPtr,
												(UInt16)number_of_features,
												(UInt16)final_num_features,
												FALSE,
												gFeatureExtractionSpecsPtr->listTransformationFlag,
												2);
			
				}	// end "if (continueFlag && ..."
			
			}	// end "if (continueFlag)"
				
		if (continueFlag && 
				projectionPursuitSpecsPtr->numericalOptimizationFlag &&
				gAlertReturnCode != 1)
			{
			LoadDItemStringNumber (kFeatureExtractStrID, 
												// "\pProjection Pursuit - Numerical Optimization"
											IDS_FeatureExtract65, 
											gStatusDialogPtr, 
											IDC_Status21, 
											(Str255*)gTextString);

			global_num_features = numberFeatures;
			global_num_classes = numberClasses;
			global_final_features = final_num_features;
		
			global_group_bands = group_of_bands;
			
					// Get some memory for the minBhatt_function.
					
			PAmatrix = gTransformMatrixPtr;
			ZeroMatrix (PAmatrix, global_final_features, global_num_features, TRUE);
			PAmatrix[0] = 1;
			PAmatrix[global_final_features*global_num_features-1] = 1;
		
			gAMatrixPtr = eigenVectorPtr;
			
			continueFlag = Second_Stage_matrix (delta_Optimization);

			if (continueFlag)
				{
				continueFlag = minBhatt (PAmatrix, 
													global_num_features, 
													global_final_features,
													global_num_classes,
													&value,
													&minBIndex);
									
				// "%s After Numerical Optimization%s Minimum Bhattacharyya = %9.4f%s",
				continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID, 
																		IDS_FeatureExtract34,
																		resultsFileStreamPtr, 
																		gOutputForce1Code,
																		value,
																		continueFlag);
													
				if (continueFlag && gFeatureExtractionSpecsPtr->listTransformationFlag)
					{ 
							// "%s Optimized Preprocessing Channel Transformation Matrix",
							
					continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID, 
																			IDS_FeatureExtract35, 
																			(unsigned char*)gTextString,
																			resultsFileStreamPtr, 
																			gOutputForce1Code,
																			continueFlag);
				
					if (continueFlag)
						continueFlag = ListTransformationInformation (
												NULL, 
												gTransformMatrixPtr, 
												0, 
												0,
												channelsPtr,
												(UInt16)global_num_features,
												(UInt16)global_final_features,
												FALSE,
												gFeatureExtractionSpecsPtr->listTransformationFlag,
												2);
							
					}	// end "if (continueFlag && ..."
													
				}	// end "if (continueFlag)"
			
			}	// end "if (...->numericalOptimizationFlag)"
				
				// Save transformation matrix if everything okay to here.
		
		if (continueFlag)
			BlockMoveData (gTransformMatrixPtr, 
								eigenVectorPtr, 
								final_num_features*number_of_features*sizeof (double));
	
		}	// end try
		
	catch (...)
		{
		continueFlag = FALSE;		
		
		}
		
	gFirstStage_M1 = CheckAndDisposePtr (gFirstStage_M1);
	gFirstStage_M2 = CheckAndDisposePtr (gFirstStage_M2);
	gFirstStage_S1 = CheckAndDisposePtr (gFirstStage_S1);
	gFirstStage_S2 = CheckAndDisposePtr (gFirstStage_S2);
	
	gClass_log_det_OK = CheckAndDisposePtr (gClass_log_det_OK);
				
	gPmatrix = free_dmatrix (gPmatrix, 1, MAX+2, 1, number_of_features);
	last_group_of_bands = free_ivector (last_group_of_bands, 1, number_of_features);
	
			// Free memory used by the minBhatt routine.
			
	gMinBhatt_Bdist = free_dvector (gMinBhatt_Bdist, 1, comb);
			
			// Save the final number of features and the band grouping vector if 
			// everything  was okay.
	
	if (continueFlag)
		{
		Boolean changedFlag;
					
		projectionPursuitSpecsPtr->bandGroupingHandle = 
							UnlockAndDispose (projectionPursuitSpecsPtr->bandGroupingHandle);
								
		UInt16* bandGroupingPtr = (UInt16*)CheckHandleSize (
													&projectionPursuitSpecsPtr->bandGroupingHandle,
													&continueFlag,
													&changedFlag, 
													final_num_features * sizeof (UInt16));
								
		if (continueFlag)
			{						
			projectionPursuitSpecsPtr->finalNumberFeatures = (UInt16)final_num_features;
			
			for (i=1; i<=final_num_features; i++) 
				{
				*bandGroupingPtr = (UInt16)group_of_bands[i];
				bandGroupingPtr++;
				
				}	// end "for (i=1; i<=final_num_features; i++)"
			
			}	// end "if (continueFlag)"
				
		CheckAndUnlockHandle (projectionPursuitSpecsPtr->bandGroupingHandle);
	
		}	// end "if (continueFlag)"
		
	HideStatusDialogItemSet (kStatusCluster);
	HideStatusDialogItemSet (kStatusMinutes);
	
	group_of_bands = free_ivector (group_of_bands, 1, number_of_features);

	return (continueFlag);

}	// end "ProjectionPursuit"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean Preprocess
//
//	Software purpose:	The purpose of this routine is to manage the memory required
//							for the projection pursuit preprocessing step and if everything
//							is okay start the preprocessing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/02/1996
//	Revised By:			Larry L. Biehl			Date: 08/26/2010	

Boolean Preprocess (void)

{
	Boolean								changedFlag,
											continueFlag;
										
	ChannelStatisticsPtr				channelStatsPtr = NULL;
										
	ClassInfoPtr						classInfoPtr;
	
	HDoublePtr							transformValuePtr = NULL,
											transformVectorPtr = NULL,
											tempMatrixPtr = NULL;
										
	ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr;

	UInt16*								channelsPtr;
	
	SInt32								bytesNeeded,
											numberChannels,
											numberClasses,
											numberOutputCovarianceEntries,
											numberOutputCovarianceEntries2;
	
	SInt16								*classPtr;
	
	SInt32								index;
	SInt16								classStorage,
											statClassNumber;
	
	
			// If spare memory had to be used to load code resources, then exit
			// routine.																		
			
	if (gMemoryTypeNeeded < 0)
																						return (FALSE);
		    		
			// Code resources loaded okay, so set flag back for non-Code
			// resources.																
			
	gMemoryTypeNeeded = 0;
										
			// Initialize local variables.
			
	projectionPursuitSpecsPtr = &gFeatureExtractionSpecsPtr->projectionPursuitParams;

	channelsPtr = (UInt16*)GetHandlePointer (
												gFeatureExtractionSpecsPtr->channelsHandle);
	
	classPtr = (SInt16*)GetHandlePointer (gFeatureExtractionSpecsPtr->classHandle);
	numberChannels = gFeatureExtractionSpecsPtr->numberChannels;
	numberClasses = gFeatureExtractionSpecsPtr->numberClasses;
	numberOutputCovarianceEntries = numberChannels * numberChannels;
	numberOutputCovarianceEntries2 = numberChannels * (numberChannels+1)/2;
			
			// Release memory used by last set of eigenvalues/eigenvectors			
			// if they exist.																		
			
	projectionPursuitSpecsPtr->transformVectorHandle = 
					UnlockAndDispose (projectionPursuitSpecsPtr->transformVectorHandle);							
			
	projectionPursuitSpecsPtr->transformValueHandle = 
					UnlockAndDispose (projectionPursuitSpecsPtr->transformValueHandle);
	
	ShowStatusDialogItemSet (kStatusTitle2);
	LoadDItemStringNumber (kFeatureExtractStrID, 
									IDS_FeatureExtract12,
									gStatusDialogPtr, 
									IDC_Status21, 
									(Str255*)gTextString);
	
			// Get memory for eigenvectors.													
			
	bytesNeeded = numberOutputCovarianceEntries * sizeof (double);
	transformVectorPtr = (HDoublePtr)CheckHandleSize (
											&projectionPursuitSpecsPtr->transformVectorHandle,
											&continueFlag,
											&changedFlag, 
											bytesNeeded);
								
	if (continueFlag)
		{
				// Get memory for the structure which contains pointers to the 	
				// class statistics and data values.	
				
		classInfoPtr = GetClassInfoStructure	(
											(UInt16)gFeatureExtractionSpecsPtr->numberClasses);
		continueFlag = (classInfoPtr != NULL);
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		continueFlag = AssignClassInfoMemory (
					classInfoPtr,
					classPtr, 
					numberClasses,
					numberChannels,
					numberChannels,
					0,													// no float data Values
					kAssignMemory+kNumberChannels+kSquare,	// covariances - square matrix
					0,													// no inverses
					kAssignMemory+kNumberChannels,			// means
					kAssignMemory+kNumberChannels+kSquare, // transformed cov - square matrix
					kAssignMemory+kNumberChannels);			// transformed means
		
	if (continueFlag)
		{
					// Get pointer to memory to use for class channel statistics	
					// that will be used in computing the covariance matrix and		
					// getting the data for the class mean vector.						
					
		channelStatsPtr = (ChannelStatisticsPtr)MNewPointer (
													numberChannels * sizeof (ChannelStatistics));
		continueFlag = (channelStatsPtr != NULL);	
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Turn spin cursor on

		gPresentCursor = kSpin;

				// Load the class covariance matrix and mean vector.
				// Also, determine the minimum number of training samples available 
				// for the classes to be used.
		
		projectionPursuitSpecsPtr->minimumNumberSamples = ULONG_MAX;
		for (index=0; index<numberClasses; index++)
			{
			statClassNumber = classPtr[index] - 1;
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
				
			GetClassCovarianceMatrix (
						(UInt16)numberChannels, 
						channelStatsPtr, 
						classInfoPtr[index].covariancePtr, 
						channelsPtr, 
						statClassNumber,
						kSquareOutputMatrix,
						kMeanCovariance,
						gProjectInfoPtr->classNamesPtr[classStorage].covarianceStatsToUse);
						
					// Copy mean data into mean vector.										
				
			ReduceMeanVector (channelStatsPtr, 
									classInfoPtr[index].meanPtr,
									(SInt16)numberChannels,
									NULL);
		
						// Get the number of pixels in the class.							
						
			classInfoPtr[index].no_samples = 
					gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
		
			projectionPursuitSpecsPtr->minimumNumberSamples = MIN (
							projectionPursuitSpecsPtr->minimumNumberSamples,
							(UInt32)gProjectInfoPtr->
											classNamesPtr[classStorage].numberStatisticsPixels);
					
			}	// end "for (index=0; index<numberClasses; index++)" 
			
		}	// end "if (continueFlag)" 
	
	channelStatsPtr = (ChannelStatisticsPtr)CheckAndDisposePtr ((Ptr)channelStatsPtr);
	
	if (continueFlag)
		{
				// Get memory for temporary matrix.
				
		tempMatrixPtr = (HDoublePtr)MNewPointer (
									2 * numberChannels * numberChannels * sizeof (double));
		continueFlag = (tempMatrixPtr != NULL);
		
		}	// end "if (continueFlag)"
		
			// Get pointer to memory to use for inverse matrix computation.
			
	if (continueFlag)
		continueFlag = SetupMatrixInversionMemory ((UInt16)numberChannels,
																	TRUE,
																	&gInverseMatrixMemory.inversePtr, 
																	&gInverseMatrixMemory.pivotPtr, 
																	&gInverseMatrixMemory.indexRowPtr, 
																	&gInverseMatrixMemory.indexColPtr, 
																	&gInverseMatrixMemory.ipvotPtr);	
				
			// Initialize some global variables to allow the user 	
			// to cancel the operation at the end of the current 		
			// iteration instead of canceling immediately.				
			
	InitializeGlobalAlertVariables (kFinishDontCancelAlertID, 
												kAlertStrID, 
												IDS_Alert49);
	
	if (continueFlag)
		continueFlag = ProjectionPursuit (numberChannels,
														numberClasses,
														classInfoPtr,
														transformVectorPtr,
														tempMatrixPtr);
												
			// Reset the cancel operation globals.
								
	ClearGlobalAlertVariables ();

	ReleaseMatrixInversionMemory ();
	
	tempMatrixPtr = CheckAndDisposePtr (tempMatrixPtr);
	
			// Get memory for eigenvalues.													

	bytesNeeded = 2 * projectionPursuitSpecsPtr->finalNumberFeatures * sizeof (double);
	if (continueFlag)
		transformValuePtr = (HDoublePtr)CheckHandleSize (
												&projectionPursuitSpecsPtr->transformValueHandle,
												&continueFlag,
												&changedFlag, 
												bytesNeeded);
							
	if (continueFlag)
		{	
				// Zero the eigen value vector for now.
						
		ZeroMatrix (transformValuePtr,
						projectionPursuitSpecsPtr->finalNumberFeatures,
						1,
						TRUE);
				
				// Now get the transformation feature means.
				
		GetTranformationFeatureMeans (
					classInfoPtr,
					(UInt16)numberClasses,
					(UInt16)numberChannels,
					projectionPursuitSpecsPtr->finalNumberFeatures,
					transformVectorPtr,
					&transformValuePtr[projectionPursuitSpecsPtr->finalNumberFeatures]);
					
		projectionPursuitSpecsPtr->numberFeatureChannels = (UInt16)numberChannels;
								
		}	// end "if (continueFlag)"
	
	ReleaseClassInfoMemory (classInfoPtr, (UInt16)numberClasses);
										
	return (continueFlag);
		
}	// end "Preprocess"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 PPRandom
//
//	Software purpose:	Pseudo random # generator : Generates with equal probability 
//							an integer 1 or 0.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	
// 
// Called By:
//
//	Coded By:			Luis Jiminez		Date: ??/??/1996
// Revised By:			Larry Biehl			Date: 02/05/2010
  
SInt16 PPRandom (void)

{
	ldiv_t							lDivideStruct;
	unsigned int					currentTime;
	SInt16							iMax;

  
	currentTime = (unsigned int)time (NULL);
	srand (currentTime);
	
	lDivideStruct = ldiv ((SInt32)rand (), 2);
				 
	iMax = (SInt16)lDivideStruct.rem;

	return iMax;
	
}	// end "PPRandom"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopyTransformationToVector
//
//	Software purpose:	The purpose of this routine is to the input transformation
//							matrix to a vector using the input group of bands vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/23/1996
//	Revised By:			Larry L. Biehl			Date: 07/23/1996	

void SaveTransformationToVector (
				double*								savedTransformationVector,
				double*								maxIndexVector,
				double**								transformationInformation,
				SInt32*								group_bands,
				UInt32								final_number_features)

{
	UInt32								i,
											k,
											maxIndex,
											sum;
	
	
			// Save the maximum Pa in a vector and the current band grouping
			// at the end of the band grouping vector.
			
	sum = 1;
	for (i=1; i<=final_number_features; i++)
		{
		maxIndex = (UInt32)maxIndexVector[i];
		
		for (k=sum; k<sum+group_bands[i]; k++)
			savedTransformationVector[k] = transformationInformation[maxIndex][k];
			
		sum += group_bands[i];
		
		}	// end "for (i=1; i<=final_number_features; ++i)"
	
}	// end "SaveTransformationToVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean Second_Stage_matrix
//
//	Software purpose:	Given the global variables -> global_num_features, 
//												global_final_features,
//	                                 global_num_classes, global_Index, 
//	                                 *global_group_bands.
//	
//							the function will calculate the matrix PAmatrix optimizing 
//							the minimum Bhattacharyya distance.
//	
//							The matrix PAmatrix contains the initial point to start the 
//							maximization. After the function is over the matrix will 
//							contains the new values of maximization computed by the 
//							numerical optimization.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:  	
// 
// Called By:
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

Boolean Second_Stage_matrix (
   			double 								delta_Optimization)

{
	double 								norm,
											value, 
											value1, 
											value2;
			
	double 								*a = NULL, 
											*mBhatt = NULL, 
											**xi = NULL;
					
	UInt32								maxGroupBands,
											minBIndex;
	
	SInt32 								i, 
											j, 
											k, 
											sum, 
											iter;
	
	Boolean								continueFlag = TRUE;
  

	try
		{
		value1 = 1;
		value = 2;
	
				// Vector that will linearly combine the group of adjacent bands 
				// whose number is indicated by global_Index.
				
		a = dvector (1, global_num_features);
		
				// Matrix used for the function powell. Is an initial matrix whose 
				// columns contains the initial set of directions (usually the n 
				// unit vectors).
	
		xi = dmatrix (1, 
							global_num_features, 
							1,
							global_num_features);
	
				// Define vector which will contain the minimum Bhattacharyya distance 
				// after each optimization for each group of adjacent bands.
	
		mBhatt = dvector (1, global_final_features);
		mBhatt[global_final_features] = 0;
	
				// Find the maximum number of bands that will be used.
	
		maxGroupBands = imax_elm_vector (global_group_bands, global_final_features);
		
		g_f1dim_xt = dvector (1, maxGroupBands);
		
		g_linmin_pcom = dvector (1, maxGroupBands);
		g_linmin_xicom = dvector (1, maxGroupBands);
	
		g_powell_pt = dvector (1, maxGroupBands);
		g_powell_ptt = dvector (1, maxGroupBands);
		g_powell_xit = dvector (1, maxGroupBands);
		
		}	// end try
		
	catch (...)
		{
		continueFlag = FALSE;
		value1 = 3;
		
		}	// end catch

			// Value1 contains the previous value of the projection index before the 
			// present optimization. value contains the projection index value after
			// the present optimization. The while loop evaluates if the percentage of 
			// increment is greater than  .01. This could be changed. Also it could 
			// optimize the combination of group of bands only once in order to speed
			// the process.
  
	while ((value-value1)/value1 > delta_Optimization) 
  		{
  		sum = 0;
  		value1 = value;

				// For every group of adjacent bands perform an optimization.

		for (i=1; i<=global_final_features; ++i) 
			{
					// Value of global_Index to be used in the function minBhatt_function.
					 
    		global_Index = i;

					// The initial value of the vector a (initial for the numerical 
					// optimization) is taken from the matrix PAmatrix.

    		for (k=1; k<=global_group_bands[global_Index]; ++k)
      		a[k] = PAmatrix[(global_Index-1)*global_num_features+k+sum-1];

					// Initialize the xi matrix to identity matrix.
					
    		for (k=1; k<=global_group_bands[global_Index]; ++k)
    			{
    			for (j=1; j<k; ++j)
    				{
    				xi[k][j] = 0.0;
    				xi[j][k] = 0.0;
    				
    				}	// end "for (j=1; j<k; ++j)"
    				
      		xi[k][k] = 1.0;
    				
    			}	// end "for (k=1; k<=global_group_bands[global_Index]; ++k)"

					// Function Function that minimize a function of n variables. The 
					// initial starting point is a.
					
    		continueFlag = powell (a, 
    										xi, 
    										global_group_bands[global_Index], 
    										.001, 
    										&iter, 
    										&value2, 
    										minBhatt_function);
    																	
  			if (!continueFlag)
  				break;

					// Normalize the vector a.
					
    		norm = 0;
    		for (k=1; k<=global_group_bands[global_Index]; ++k)
      		norm += a[k] * a[k];

    		if (norm == 0)
      		norm = 1;

					// Return the values of a to the matrix PAmatrix.

			norm = sqrt (norm);
    		for (j=1; j<= global_group_bands[global_Index]; ++j)
      		PAmatrix[(global_Index-1)*global_num_features+j+sum-1] = a[j]/norm;

					// Calculate the value of the minimum Bhattacharyya distance after 
					// the projection produced by PAmatrix.

    		continueFlag = minBhatt (PAmatrix, 
												global_num_features, 
												global_final_features,
												global_num_classes,
												&value,
												&minBIndex);
			
			if (!continueFlag)
				break;

    		mBhatt[i] = value;
			
			if (TickCount () >= gNextTime && !CheckSomeEvents (
											osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				continueFlag = FALSE;
				break;
				
				}		/* end "if (!CheckSomeEvents (osMask..." */

      	sum += global_group_bands[i];

			}	// end "for (i=1; i<=global_final_features; ++i)"
			
		if (!continueFlag || gAlertReturnCode == 1)
			break;
 
		iter = FindMaxValueInVector (&mBhatt[1], global_final_features, &value) + 1;

		}	// end "while ((value-value1)/value1 > delta_Optimization)"
		
	mBhatt = free_dvector (mBhatt, 1, global_final_features);

	a = free_dvector (a, 1, global_num_features);

	xi = free_dmatrix (xi,
								1, 
								global_num_features, 
								1,
								global_num_features);
	         			
	g_f1dim_xt = free_dvector (g_f1dim_xt, 1, maxGroupBands);
		
	g_linmin_pcom = free_dvector (g_linmin_pcom, 1, maxGroupBands);
	g_linmin_xicom = free_dvector (g_linmin_xicom, 1, maxGroupBands);
	
	g_powell_pt = free_dvector (g_powell_pt, 1, maxGroupBands);
	g_powell_ptt = free_dvector (g_powell_ptt, 1, maxGroupBands);
	g_powell_xit = free_dvector (g_powell_xit, 1, maxGroupBands);
	         			
  return (continueFlag);

}	// end "Second_Stage_matrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetupGroupingVector
//
//	Software purpose:	The purpose of this routine is to initialize the grouping vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/03/1996
//	Revised By:			Larry L. Biehl			Date: 07/03/1996	

void SetupGroupingVector (
				SInt32*								group_of_bands,
				UInt32								numberOfInputBands,
				UInt32								numberOfOutputFeatures)

{		
	SInt32								inputBandsLeft = numberOfInputBands,
											outputFeaturesLeft = numberOfOutputFeatures;

	UInt32								i;
											
				
	for (i=1; i<=numberOfOutputFeatures; i++)
		{
		group_of_bands[i] = inputBandsLeft/outputFeaturesLeft;
		inputBandsLeft -= group_of_bands[i];
		outputFeaturesLeft--;
		
		}	// for (i=1; i<=numberOfOutputFeatures; i++)

}	// end "SetupGroupingVector"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void subtract_matrix
//
//	Software purpose:	The purpose of this routine is to substract two matrices.
//
//	Parameters in:		None
//
//	Parameters out:	None
// 
// Called By:
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

void subtract_matrix (
				double* 								inputMatrix1Ptr, 
				double* 								inputMatrix2Ptr, 
				double*								outputMatrixPtr, 
				UInt32								numberRows, 
				UInt32								numberColumns) 

{
	UInt32								count,
											i;
	
	
	count = numberRows * numberColumns;
	for (i=1; i<=count; i++) 
		{
		*outputMatrixPtr = *inputMatrix1Ptr - *inputMatrix2Ptr;
		outputMatrixPtr++;
		inputMatrix1Ptr++;
		inputMatrix2Ptr++;

		}	// end "for (i=1; i<=count; i++)"
			
	if (TickCount () >= gNextTime)
		CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask);

}	// end "subtract_matrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void sum_matrix
//
//	Software purpose:	The purpose of this routine is to sum two matrices.
//
//	Parameters in:		None
//
//	Parameters out:	None
// 
// Called By:
//
//	Coded By:			Luis Jiminez			Date: ??/??/1996

void sum_matrix (
				double* 								inputMatrix1Ptr, 
				double* 								inputMatrix2Ptr, 
				double*								outputMatrixPtr, 
				UInt32								numberRows, 
				UInt32								numberColumns) 

{
	UInt32								count,
											i;
	
	
	count = numberRows * numberColumns;
	for (i=1; i<=count; i++) 
		{
		*outputMatrixPtr = *inputMatrix1Ptr + *inputMatrix2Ptr;
		outputMatrixPtr++;
		inputMatrix1Ptr++;
		inputMatrix2Ptr++;

		}	// end "for (i=1; i<=count; i++)"
			
	if (TickCount () >= gNextTime)
		CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask);

}	// end "sum_matrix"
