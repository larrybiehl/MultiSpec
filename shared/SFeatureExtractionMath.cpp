// 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SFeatureExtractionMath.cpp
//
//	Author:					Chulhee Lee
//	Revised by:				Larry L. Biehl
//
//	Revision date:			02/07/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file are to do the 
//								math for the feature extraction algorithms.
//
//	Functions in file:	void			cal_h_X
//								void			FS_decision_boundary
//								void			FS_discriminant_analysis
//								void			FS_eigen_inverse_SwSb_gen
//								UInt32		FS_optimize_2_class
//								void			FS_quick_feature_extraction
//								void 			FS_sol_bnd_line
//								Boolean 		FS_sol_bnd_line_2
//								UInt32		FS_sub_find_edbfm_2_class
//								void			FS_sub_linear_transform
//								HPtr 			GetDecisionBoundaryMemoryBlock
//								void			normalize_rows
//
// FS_quick_feature_selection  (should be feature_extraction)
// 		Inputs: number of features
// 				number of classes
// 				class info structure
// 					includes pointer to data stored as BIS.
// 				feature selection type to be run.
// 				storage for output transformation matrix (no. features by no. features)
// 					first column is the best).
// 					
// 	FS_discriminant_analysis (feature extraction method)
// 		FS_eigen_inverse_SwSb_gen (of Sw-1Sb, where Sw is within class scatter matrix
// 				and Sb is between class scatter matrix.)
// 			ComputeEigenvectors
// 			normalize_rows (should not need.  Should already be normalized).
// 			
// 			Note: eigenvectors from non-symetric matix are not orthogal.
// 			 
// 	FS_decision_boundary (Chulhee's)
// 		FS_sub_find_edbfm_2_class (finds decision boundary feature matrix
// 				for each pair of classes.)
// 				threshold_prob is a threshold to get rid of outliers.
// 				threshold_prob2 is a threshold to find the effective decision boundary.
// 			FS_sol_bnd_line	 (find DBFM from the pairs of points classified differently).
// 				FS_sol_bnd_line_2 (find normal vector to the decision boundary from a 
// 						pair of points classified differently).
// 					cal_h_X 
// 			
// 		FS_optimize_2_class (reduces the number of features if can; i.e. the redundancy)
// 			This is time consuming.  This can be an option.
// 			There can also be a threshold setting for the classification accuracy
// 			relative to highest dimension.  Program now has a .97 setting.
// 		ComputeEigenvectors (finds eigenvectors of the sum of all the class pair
//				feature matrices)
// 		
// 	FS_steepest_ascent (do not include. FS_decision_boundary is always better for
// 			the parametric case).
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
  
#if defined multispec_lin
	#include	"SMultiSpec.h"
#endif	// defined multispec_lin

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDS_FeatureExtract74		74
	#define	IDS_FeatureExtract75		75
	#define	IDS_FeatureExtract76		76
#endif	// multispec_mac || defined multispec_mac_swift
  
#if defined multispec_lin
	#define SIGN(a,b) ((b) >= 0.0 ? fabs (a) : -fabs (a))
#endif	// defined multispec_lin    

//#include "SExtGlob.h"
//#include	"SFS.h"

#define SDOUBLE	sizeof (double)


/*
						// 'ListCovarianceMatrix' is for debug.

extern void ListCovarianceMatrix (
				HDoublePtr 							covariancePtr,
				SInt16								numberChannels,
				SInt16*								featurePtr,
				SInt16*								channelsPtr,
				SInt16								matrixType,
				Boolean								transformationFlag);
*/
		// Prototypes for routines in this file that are only called by
		// other routines in this file.

void AddBxSymMatrixToSymMatrix (
				HDoublePtr							inputMatrixPtr,
				HDoublePtr							outputMatrixPtr,
				UInt32								matrixSize,
				double								factor);			

extern void balbak (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh, 
				double*								scale,
				UInt32								m,
				double**								z,
				double*								zPtr);

void BalanceMatrix (
				UInt32								nm,
				UInt32								n,
				double**								a,
				UInt32*								lowPtr,
				UInt32*								ighPtr, 
				double*								scalePtr,
				double*								inputMatrixPtr);

void cal_h_X (
				HDoublePtr							h_X,
				HDoublePtr							normal, 
				HDoublePtr							icov_diff, 
				HDoublePtr		 					mean_icov_diff, 
				UInt32				 				dim, 
				double								c);

void cdiv (
				double								ar,
				double								ai, 
				double								br, 
				double								bi, 
				double*								cr, 
				double*								ci);	

double dpythag (
				double 								a,
				double 								b);	

void elmhes (
				UInt32								nm,
				UInt32								n, 
				UInt32								low, 
				UInt32								igh,
				double**								a,
				UInt32*								intPtr, 
				double*								inputMatrixPtr);
					
extern void eltran (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh, 
				double**								a,
				UInt32*								intPtr,
				double**								z,
				double*								inputMatrixPtr,
				double*								outputMatrixPtr);

void FindEigenvaluesOfUpperHessenbergMatrix (
				double*								inputMatrixPtr,
				UInt32								n, 
				double* 								wr, 
				double* 								wi,
				double**								a);

void FS_decision_boundary (
				struct class_info_str* 			class_info,
				UInt32								no_class, 
				UInt32								no_feature,
				HDoublePtr 							eigen_vectors, 
				HDoublePtr 							eigen_values, 
				SInt32*								ERROR_FLAG);
					
void FS_discriminant_analysis (
				struct class_info_str*			class_info,
				UInt32								no_class, 
				UInt32								no_feature,
				HDoublePtr	 						eigen_vectors, 
				HDoublePtr 							eigen_values, 
				SInt32*								ERROR_FLAG);
					
void FS_eigen_inverse_SwSb_gen (
				HDoublePtr 							Sw,
				HDoublePtr 							Sb,
				UInt32								no_new_channel,
				HDoublePtr 							eigenvalues,
				HDoublePtr 							eigenvectors,
				SInt32*								ERROR_FLAG);
					
UInt32 FS_optimize_2_class (
				struct class_info_str* 			class_info,
				SInt32*								class_index, 
				UInt32								no_class, 
				UInt32								no_new_channel,
				HDoublePtr	 						edbfm, 
				double 								accuracy, 
				SInt32*								ERROR_FLAG);

void FS_quick_feature_extraction (
				UInt32								no_feature,
				UInt32								no_class, 
				ClassInfoPtr				 		quick_class_info,
				SInt32								feature_extraction_type, 
				HDoublePtr		 					eigen_vectors, 
				HDoublePtr		 					eigen_values, 
				SInt32*								ERROR_FLAG);
					
void FS_sol_bnd_line (
				struct class_info_str* 			class_info,
				SInt32*								class_index,
				HDoublePtr*							point1_array,
				HDoublePtr*							point2_array, 
				Ptr									memoryBlockPtr, 
				UInt32								no_points, 
				UInt32								dim,
				HDoublePtr							edbfm, 
				SInt32*								ERROR_FLAG);
					
Boolean FS_sol_bnd_line_2 (
				HDoublePtr 							mean_icov_diff,
				HDoublePtr 							icov_diff, 
				double 								c,
				HDoublePtr							point1, 
				HDoublePtr							point2, 
				UInt32								dim, 
				HDoublePtr 							normal, 
				double 								threshold);
					
UInt32 FS_sub_find_edbfm_2_class (
				struct class_info_str* 			class_info,
				SInt32*								class_index, 
				UInt32								no_class, 
				UInt32								no_new_channel, 
				HDoublePtr 							edbfm, 
				HDoublePtr 							accuracy, 
				SInt32*								countMahDistancePtr, 
				SInt32*								ERROR_FLAG);
					
void FS_sub_linear_transform (
				HDoublePtr 							fdata_after,
				HDoublePtr							fdata_ori, 
				HDoublePtr 							matrixPtr, 
				UInt32								matarix_dim,
				UInt32								no_new_channel, 
				UInt32								no_sample, 
				UInt32								featureNumber);
					
HPtr GetDecisionBoundaryMemoryBlock (
				struct class_info_str* 			class_info,
				UInt32								numberClasses, 
				UInt32								numberFeatures);

SInt32 hqr2 (
				SInt32								nm,
				SInt32								n, 
				SInt32								low, 
				SInt32								igh, 
				double**								h, 
				double*								wr, 
				double*								wi, 
				double**								z,
				double*								hMatrixPtr,
				double*								zMatrixPtr);	

SInt32 MatlabEigFunction (
				double*								inputMatrixPtr,
				double*								eigenvectorPtr,
				double*								eigenvaluePtr,
				UInt32								numberFeatures);

SInt32 NWFE (
				struct class_info_str* 			class_info,
				UInt32								numberClasses, 
				UInt32								numberFeatures,
				HDoublePtr 							eigen_vectors, 
				HDoublePtr 							eigen_values);	

void NWFE_AddToScatterMatrixForClass_i (
				HDoublePtr							inputMatrixPtr,
				HDoublePtr							localMeanPtr,
				double								scatterMatrixWeight,
				HDoublePtr							data_i_ptr,
				UInt32								numberChannels);	

SInt32 NWFE_GetLocalMean (
				Boolean								withInClassFlag,
				UInt32								sample_i, 
				UInt32								numberSamplesClass_i,
				UInt32								numberSamplesClass_j,
				UInt32								numberChannels, 
				HDoublePtr							data_i_ptr,
				HDoublePtr							data_j_ptr, 
				HDoublePtr 							localMeanPtr,
				HDoublePtr							localMeanWeightPtr);

double NWFE_GetScatterMatrixWeight (
				UInt32								numberFeatures,
				HDoublePtr							data_i_ptr,
				double								sumInvDistXLocalMean, 
				HDoublePtr 							localMeanPtr);		

SInt32 NWFE_SumInvDistXLocalMean (
				Boolean								withInClassFlag,
				UInt32								numberSamplesClass_i,
				UInt32								numberSamplesClass_j,
				UInt32								numberChannels, 
				HDoublePtr							data_i_ptr,
				HDoublePtr							data_j_ptr,
				HDoublePtr 							sumInvDistXLocalMeanPtr,
				HDoublePtr							localMeanPtr,
				HDoublePtr							localMeanWeightPtr);

void orthes (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh,
				double**								a,
				double*								ort,
				double*								inputMatrixPtr);

void ortran (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh,
				double**								a,
				double*								ort,
				double**								z,
				double*								inputMatrixPtr,
				double*								outputMatrixPtr);

void RegularizeSymMatrix (
				HDoublePtr							matrixPtr,
				UInt32								matrixSize);

void TransformLineOfCovariance (
				HDoublePtr 							eigenvectorPtr,
				HDoublePtr 							meanPtr, 
				HDoublePtr 							covariancePtr, 
				HDoublePtr 							transformedMeanPtr, 
				HDoublePtr 							transformedCovPtr, 
				HDoublePtr 							workVectorPtr, 
				UInt32								matrixDimension, 
				UInt32								featureNumber);

void TransposeMatrix (
				HDoublePtr							squareMatrixPtr,
				HDoublePtr							squareOutputMatrixPtr,
				UInt32 								numberFeatures);

// UInt32		gSampleMultipleCount = 0;

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void cal_h_X (from db.c)
//
//	Software purpose:	
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 02/27/1999	
 
void cal_h_X (
				HDoublePtr							h_X, 
				HDoublePtr							normal, 
				HDoublePtr							icov_diff, 
				HDoublePtr		 					mean_icov_diff, 
				UInt32				 				dim, 
				double								c)

{         				
	double								localHX,
											t1,
											t3;
	
	HDoublePtr 							ficov;
	
	UInt32							 	i,
											j;
	
      
   t3 = c;
   localHX = 0.; 
    
	for (i=0; i<dim; i++)
   	{
		ficov = icov_diff;

		t1 = 0.;
		for (j=0; j<i; j++)
			{
			t1 += *normal * *ficov;
			ficov++;
			normal++;
			
			}	// end "for (j=0; j<i; j++)"

		localHX += *normal * (t1 + t1 + *normal * *ficov);
		t3 -= *mean_icov_diff * *normal;
		
		mean_icov_diff++;
		icov_diff += dim;
		normal -= i;
		
		}	// end "for (i=0; i<dim; i++)" 
		
	*h_X = localHX*.5 + t3;
	
}	// end "cal_h_X" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_decision_boundary
//
//	Software purpose:	
//
//	Parameters in:		no_feature (long):	 			number of features (=N)
//							no_class (long):	 				number of classes
//							class_info (struct): 			class informations
//
//	Parameters out:	eigenvectors (double, N by N)	eigenvectors
//							eigenvectors (double, N)		eigenvalues
//							ERROR_FLAG (*long)				error flag. must be zero.
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Chulhee Lee				Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

void FS_decision_boundary (
				struct class_info_str* 			class_info, 
				UInt32								no_class, 
				UInt32								no_feature,
				HDoublePtr 							eigen_vectors, 
				HDoublePtr 							eigen_values, 
				SInt32*								ERROR_FLAG)
				
{				
	const char							footNoteSymbol[2] = {' ', '*'};
	
	double								accuracy,
											det,
											weight,
											totalWeights;
											
	SInt64								numberBytes;
								
	HDoublePtr 							edbfm,
											edbfm_all,
			 								savedDoublePtr1,
											savedDoublePtr2;
								
	SInt16								*classPairWeightPtr,
											*classPtr;
								
	SInt32								class_index[2],
											classPairCount,
											countMahDistance[6], 
											numberClassPairs,
											numberIterations,
											pixelPairs;
											
	UInt32								i,
											j,
											k,
											matrixSize,
											numberFeatures,
											savedNumberClasses,
											statClassNumber,
											symbolIndex1,
											symbolIndex2;
								
	SInt16								numberPairReturnCode,
											stringNumber1,
											stringNumber2,
											stringNumber3,
											weightsIndex;
								
	Boolean								continueFlag,
											countOKFlag,
											determinantOKFlag,
											minimumNumberUsedFlag,	
											overallCountOKFlag;
	
	
	edbfm = NULL;
	edbfm_all = NULL;
	gInverseMatrixMemory.inversePtr = NULL;
	gInverseMatrixMemory.pivotPtr = NULL;
	gInverseMatrixMemory.indexRowPtr = NULL;
	gInverseMatrixMemory.indexColPtr = NULL;
	gInverseMatrixMemory.ipvotPtr = NULL;
	
	classPtr = (SInt16*)GetHandlePointer (gFeatureExtractionSpecsPtr->classHandle);
	
	continueFlag = TRUE;
	minimumNumberUsedFlag = FALSE;
	savedNumberClasses = no_class;
   classPairCount = 0;
   numberClassPairs = 0;

			// Assign memory. 
			
	numberBytes = no_feature*no_feature * FS_DOUBLE;
	
	edbfm = (HDoublePtr)MNewPointer (numberBytes);
	if (edbfm == NULL)
		*ERROR_FLAG=507;

	if (*ERROR_FLAG == 0)
		{
		edbfm_all = (HDoublePtr)MNewPointer (numberBytes);
		if (edbfm_all == NULL)
			*ERROR_FLAG=508;
		 
		}	// end "if (*ERROR_FLAG == 0)" 

	if (*ERROR_FLAG == 0)
		{
				// Get memory for matrix inversions and eigenvector 			
				// computation.															
	
		if (!SetupMatrixInversionMemory ((UInt16)no_feature,
														TRUE,
														NULL, 
														&gInverseMatrixMemory.pivotPtr, 
														&gInverseMatrixMemory.indexRowPtr, 
														&gInverseMatrixMemory.indexColPtr, 
														&gInverseMatrixMemory.ipvotPtr))
			*ERROR_FLAG = 551;
		 
		}	// end "if (*ERROR_FLAG == 0)" 

	if (*ERROR_FLAG == 0)
		{
		for (i=0; i<no_class; i++)
			{
			numberBytes = (SInt64)FS_FLOAT * (class_info+i)->no_sample;
			(class_info+i)->mah_dis = (float*)MNewPointer (numberBytes);
								
			if ((class_info+i)->mah_dis == NULL)
				{
				*ERROR_FLAG = 509;
				break;
				
				}	// end "if ((class_info+i)->mah_dis == NULL)" 
				
			(class_info+i)->mah_disLoadedFlag = FALSE;
			
			}	// end "for (i=0; i<no_class; i++)" 
		 
		}	// end "if (*ERROR_FLAG == 0)" 

	if (*ERROR_FLAG == 0)
		{
				// Get block of memory to be used later by routines call by 		
				// this routine.																	
				// Store in the unused matrix inversion memory structure 			
				// parameter (inversePtr).														
				
		gInverseMatrixMemory.inversePtr = (HDoublePtr)
				GetDecisionBoundaryMemoryBlock (class_info, no_class, no_feature);
								
		if (gInverseMatrixMemory.inversePtr == NULL)
			*ERROR_FLAG = 510;
		 
		}	// end "if (*ERROR_FLAG == 0)" 
		
	if (*ERROR_FLAG == 0)
		{
		matrixSize = no_feature * no_feature;
									
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		gNextStatusTime = gNextTime;
			
				// Calculate inverse of covariance for the classes.					
				// The inverse will be written over the covariance since the		
				// covariance is no longer needed after this.							
				
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract23,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
			
		for (i=0; i<no_class; i++)
			{
			savedDoublePtr1 = (class_info+i)->icov;
			(class_info+i)->icov = (class_info+i)->cov;
			(class_info+i)->cov = savedDoublePtr1;
	 				
			determinantOKFlag = InvertSymmetricMatrix ((class_info+i)->icov,
																		(UInt16)no_feature, 
																		gInverseMatrixMemory.pivotPtr, 
																		gInverseMatrixMemory.indexRowPtr, 
																		gInverseMatrixMemory.indexColPtr, 
																		gInverseMatrixMemory.ipvotPtr, 
																		&det,
																		&(class_info+i)->log_det,
																		kReturnMatrixInverse);
			
			if (gOperationCanceledFlag)
				{
				*ERROR_FLAG = 591;
				break;
				
				}	// end "if (gOperationCanceledFlag)" 
	
			if (!determinantOKFlag)
				{
				statClassNumber = classPtr[i] - 1;
				
				continueFlag = ListClassInformationMessage (kProjectStrID,
																			IDS_Project31,
																			NULL, 
																			gOutputForce1Code,
																			(SInt16)statClassNumber, 
																			continueFlag);
				
				*ERROR_FLAG = 600;
			 	
			 	}	// end "if (!determinantOKFlag)" 
	 		
			}	// end "for (i=0; i<no_class; i++)" 
			
				// find threshold to remove outliers (self). 							 
				
		gFeatureExtractionSpecsPtr->threshold1 = GetChiSquaredValue (
												(SInt32)no_feature,
												gFeatureExtractionSpecsPtr->outlierThreshold1);
		gFeatureExtractionSpecsPtr->threshold1 = -gFeatureExtractionSpecsPtr->threshold1;
		
				// Find threshold to remove outliers (the other class). 				
				
		gFeatureExtractionSpecsPtr->threshold2 = GetChiSquaredValue (
													(SInt32)no_feature,
													gFeatureExtractionSpecsPtr->outlierThreshold2);
		gFeatureExtractionSpecsPtr->threshold2 = -gFeatureExtractionSpecsPtr->threshold2;

		if (*ERROR_FLAG == 0)
			{
					//****** find edbfm of each pair of classes 							 
					
			savedDoublePtr1 = edbfm_all;
			for (i=0; i<matrixSize; i++)	// initialize 
				{
				*edbfm_all = 0.;
				edbfm_all++;
				
				}	// end "for (i=0; i<matrixSize; i++)" 
				
			edbfm_all = savedDoublePtr1;
		
			ShowStatusDialogItemSet (kStatusField);
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract24,
											gStatusDialogPtr, 
											IDC_Status7, 
											(Str255*)gTextString);

			numberClassPairs = (SInt32)GetNumberOfCombinations ((SInt32)no_class,
																					2, 
																					1, 
																					FALSE,
																					&numberPairReturnCode);
			
			LoadDItemValue (gStatusDialogPtr, IDC_Status10, numberClassPairs);
				
			DrawDialog (gStatusDialogPtr);
			classPairCount = 0;
		
					// Get the inter class weight list pointer.
					
			classPairWeightPtr = (SInt16*)GetHandlePointer (
															gProjectInfoPtr->classPairWeightsHandle);
			
					// Get the class weight list pointer.
					// Also get the sum of the class weights.
				
			weightsIndex = GetProjectClassWeightsIndex ();
				
			totalWeights = GetTotalProbability (
												(UInt16*)classPtr, (UInt32)no_class, weightsIndex);
				
					// Scale the total weights such that a weighting factor of one
					// will be used if each class is weighted equally.
					
			totalWeights /= no_class;
			
					// Create table that gives the number of features needed to 	
					// satisfy the percent of classification accuracy for each  	
					// class pair when using class pair optimization.					
		
			if (gFeatureExtractionSpecsPtr->optimizeClassFlag)
				{
				stringNumber1 = IDS_FeatureExtract68; 
				stringNumber2 = IDS_FeatureExtract72; 
				stringNumber3 = IDS_FeatureExtract69;
				//i = 129;
				
				}	// end "if (gFeatureExtractionSpecsPtr->optimizeClassFlag)"
				
			else	// !gFeatureExtractionSpecsPtr->optimizeClassFlag
				{        
				stringNumber1 = IDS_FeatureExtract70;
				stringNumber2 = -1;  
				stringNumber3 = IDS_FeatureExtract71;
				//i = 130;
				
				}	// end "else !gFeatureExtractionSpecsPtr->optimizeClassFlag"
				
			continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID,
																	stringNumber1, 
																	(unsigned char*)gTextString,
																	NULL, 
																	gOutputForce1Code,
																	continueFlag);  
  
			if (stringNumber2 != -1)
				continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID,
																		stringNumber2, 
																		(unsigned char*)gTextString,
																		NULL, 
																		gOutputForce1Code,
																		continueFlag);  

			continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID,
																	stringNumber3, 
																	(unsigned char*)gTextString,
																	NULL, 
																	gOutputForce1Code,
																	continueFlag);
			
			}	// end "if (*ERROR_FLAG == 0)" 

		overallCountOKFlag = TRUE;
		countOKFlag = TRUE;
		
		if (*ERROR_FLAG != 0)
			no_class = 0;

		if (numberClassPairs <= 0)
			no_class = 0;
			
		LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)classPairCount+1);
		 
		for (i=0; i<no_class; i++)
			{
			for (j=i+1; j<no_class; j++)
				{
				classPairCount++;
				if (TickCount () >= gNextStatusTime)
					{
					LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)classPairCount);
					gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
					}	// end "if (TickCount () >= gNextStatusTime)" 
				
				if (classPairWeightPtr[classPairCount-1] > 0)
					{
					class_index[0] = i;
					class_index[1] = j;
					countMahDistance[0] = 0;
					countMahDistance[1] = 0;
					countMahDistance[2] = 0;
					countMahDistance[3] = 0;
					countMahDistance[4] = 0;	// Count of closest values found for class 1
					countMahDistance[5] = 0;	// Count of closest values found for class 2
					
					countOKFlag = 
								(class_info[i].no_sample > 0 && class_info[j].no_sample > 0);
								
					if (countOKFlag && *ERROR_FLAG == 0)
						pixelPairs = FS_sub_find_edbfm_2_class (class_info,
																				class_index,
																				2,
																				no_feature,
																				edbfm,
																				&accuracy,
																				countMahDistance,
																				ERROR_FLAG);

							// Exit routine if user has "command period" down			
					
					if (TickCount () >= gNextTime)
						{
						if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
							{
							if (*ERROR_FLAG == 0)
								*ERROR_FLAG = 590;
							
							}	// end "if (!CheckSomeEvents (osMask..." 
							
						}	// end "if (TickCount () >= gNextTime)" 
						
					if (*ERROR_FLAG != 0)
						break;
					
					symbolIndex1 = 0;
					if (countMahDistance[2] <
									(SInt32)gFeatureExtractionSpecsPtr->minimumThresholdNumber)
						{
						countMahDistance[2] = countMahDistance[4];
						//(SInt32)gFeatureExtractionSpecsPtr->minimumThresholdNumber;
						minimumNumberUsedFlag = TRUE;
						symbolIndex1 = 1;
						
						}	// end "if (countMahDistance[2] < ..." 
				
					symbolIndex2 = 0;
					if (countMahDistance[3] <
									(SInt32)gFeatureExtractionSpecsPtr->minimumThresholdNumber)
						{
						countMahDistance[3] = countMahDistance[5];
						//(SInt32)gFeatureExtractionSpecsPtr->minimumThresholdNumber;
						minimumNumberUsedFlag = TRUE;
						symbolIndex2 = 1;
						
						}	// end "if (countMahDistance[3] < ..." 
								
		  			if (gFeatureExtractionSpecsPtr->optimizeClassFlag) 
		  				{
						LoadDItemStringNumber (kFeatureExtractStrID, 
														IDS_FeatureExtract25,
														gStatusDialogPtr, 
														IDC_Status21, 
														(Str255*)gTextString);
						numberFeatures = 0;
						
						if (countOKFlag && *ERROR_FLAG == 0)
		   				numberFeatures = FS_optimize_2_class (class_info,
																				class_index,
																				2,
																				no_feature,
																				edbfm,
																				accuracy,
																				ERROR_FLAG);
						
						sprintf ((char*)gTextString,
										#if defined multispec_mac || defined multispec_win
											"     %3d-%3d       %5lld   %5lld      %5d   %5d"
										#endif
										#if defined multispec_lin
											"     %3d-%3d       %5d   %5d      %5d   %5d"
										#endif
										"      %5d%c   %5d%c          %4d%s",
										classPtr[i],
										classPtr[j],
										class_info[i].no_sample,
										class_info[j].no_sample,
										(int)countMahDistance[0],
										(int)countMahDistance[1],
										(int)countMahDistance[2],
										footNoteSymbol[symbolIndex1],
										(int)countMahDistance[3],
										footNoteSymbol[symbolIndex2],
										(unsigned int)numberFeatures,
										gEndOfLine);
						
						LoadDItemString (gStatusDialogPtr, IDC_Status21, (Str255*)"\0");
		   								
		   			}	// end "if (...->optimizeClassFlag)" 
		   			
		   		else	// !gFeatureExtractionSpecsPtr->optimizeClassFlag 
						sprintf ((char*)gTextString,
										#if defined multispec_mac || defined multispec_win
											"     %3d-%3d       %5lld   %5lld      %5d   %5d"
										#endif
										#if defined multispec_lin
											"     %3d-%3d       %5d   %5d      %5d   %5d"
										#endif
										"      %5d%c   %5d%c%s",
										classPtr[i],
										classPtr[j],
										class_info[i].no_sample,
										class_info[j].no_sample,
										(int)countMahDistance[0],
										(int)countMahDistance[1],
										(int)countMahDistance[2],
										footNoteSymbol[symbolIndex1],
										(int)countMahDistance[3],
										footNoteSymbol[symbolIndex2],
										gEndOfLine);
					
					if (*ERROR_FLAG == 0)
						continueFlag = OutputString ((CMFileStream*)NULL, 
																(char*)gTextString, 
																0, 
																gOutputForce1Code, 
																continueFlag);
														
					if (*ERROR_FLAG == 0 && pixelPairs <= 0)
						{
						sprintf ((char*)gTextString,
									"     %3d-%3d       Boundary could not be determined for"
									" this pair.%s",
									classPtr[i],
									classPtr[j],
									gEndOfLine);
								 
						continueFlag = OutputString ((CMFileStream*)NULL, 
																(char*)gTextString, 
																0, 
																gOutputForce1Code, 
																continueFlag);
						
						}	// end "if (pixelPairs <= 0)" 
					
					if (countOKFlag && *ERROR_FLAG == 0)
						{
						statClassNumber = classPtr[i] - 1;
						weight = GetClassWeightValue (
											(UInt16)statClassNumber, weightsIndex, totalWeights);
										
						statClassNumber = classPtr[j] - 1;
						weight *= GetClassWeightValue (
											(UInt16)statClassNumber, weightsIndex, totalWeights);
						
						savedDoublePtr1 = edbfm_all;
						savedDoublePtr2 = edbfm;
		  				for (k=0; k<matrixSize; k++)
			  				{
			  						// edbfm_all[k] += edbfm[k]							
			  						
			  				*edbfm_all += weight * *edbfm;	// can be weighted here 
			  				edbfm_all++;
			  				edbfm++;
			  				
			  				}	// end "for (k=0; k<matrixSize; k++)" 
		  				
						edbfm_all = savedDoublePtr1;
						edbfm = savedDoublePtr2;
						
						}	// end "if (countOKFlag && *ERROR_FLAG == 0)	" 
						
					}	// end "if (classPairWeightPtr[classPairCount-1] > 0)" 
						
				else	// classPairWeightPtr[classPairCount-1] <= 0 
					{
					sprintf ((char*)gTextString,
								"     %3d-%3d    This class pair was not used.%s",
								classPtr[i],
								classPtr[j],
								gEndOfLine);
								 
					continueFlag = OutputString ((CMFileStream*)NULL, 
															(char*)gTextString, 
															0, 
															gOutputForce1Code, 
															continueFlag);
							
					}	// end "else classPairWeightPtr[classPairCount-1] <= 0" 
					
				overallCountOKFlag = (overallCountOKFlag & countOKFlag);
		 		
				if (*ERROR_FLAG != 0)
					break;	
		  			
		 		}	// end "for (j=0; j<i; j++)" 
		 		
			if (*ERROR_FLAG != 0)
				break;
		  			
		 	}	// end "for (i=0; i<no_class; i++)" 
	 	
		if (!overallCountOKFlag)
			{
			*ERROR_FLAG = 591;
			
			continueFlag = ListSpecifiedStringNumber (kFeatureExtractStrID,
																	IDS_FeatureExtract1, 
																	(unsigned char*)gTextString, 
																	NULL, 
																	gOutputForce1Code, 
																	continueFlag);
			
			}	// end "if (!overallCountOKFlag)" 
			
		if (minimumNumberUsedFlag)
			continueFlag = ListSpecifiedStringNumber (
									kFeatureExtractStrID,
									IDS_FeatureExtract2, 
									NULL, 
									gOutputForce1Code,
									(SInt32)gFeatureExtractionSpecsPtr->minimumThresholdNumber,
									continueFlag);
		
		if (*ERROR_FLAG == 0 || *ERROR_FLAG == 590)
			{
			HideStatusDialogItemSet (kStatusField);
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract26,
											gStatusDialogPtr, 
											IDC_Status21, 
											(Str255*)gTextString);
					
					// Calculate eigenvectors and eigenvalues of edbfm_all	
						
			if (!ComputeEigenvectors (edbfm_all, 
												(UInt16)no_feature,
												eigen_vectors,
												gInverseMatrixMemory.ipvotPtr,
												gInverseMatrixMemory.pivotPtr,
												3))
				*ERROR_FLAG = 590;
									
			numberIterations = (SInt32)gInverseMatrixMemory.pivotPtr[0];
			if ((*ERROR_FLAG == 0) && numberIterations <= 0)
				*ERROR_FLAG = 602;
	
			}	// end "if (*ERROR_FLAG == 0 || ...)" 
			
		if (*ERROR_FLAG == 0 || *ERROR_FLAG == 590)
			{
					// Copy eigen_vectors and eigen_value							 
			
			BlockMoveData (edbfm_all, eigen_values, (SInt32)no_feature*sizeof (double));
	
			}	// end "if (*ERROR_FLAG == 0 || ...)" 
		 
		}	// end "if (*ERROR_FLAG == 0)" 

			// Free memory		
	
	ReleaseMatrixInversionMemory ();															 
			
	edbfm_all = CheckAndDisposePtr (edbfm_all);
	edbfm = CheckAndDisposePtr (edbfm);
	
	no_class = savedNumberClasses;
	for (i=0; i<no_class; i++)
		(class_info+i)->mah_dis = CheckAndDisposePtr ((class_info+i)->mah_dis);
	
	return;
	
}	// end "FS_decision_boundary" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_discriminant_analysis { by SmSw } (35)
//
//	Software purpose:	
//
//	Parameters in:		no_feature (long):	 			number of features (=N)
//							no_class (long):	 				number of classes
//							class_info (struct): 			class informations
//
//	Parameters out:	eigenvectors (double, N by N)	eigenvectors
//							eigenvectors (double, N)		eigenvalues
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 08/27/2010	

void FS_discriminant_analysis (
				struct class_info_str*			class_info, 
				UInt32								no_class, 
				UInt32								no_feature,
				HDoublePtr	 						eigen_vectors, 
				HDoublePtr 							eigen_values, 
				SInt32*								ERROR_FLAG)
				

{					
	double								totalWeights,
											totalWeights2,
											weight;
						
	SInt64					 			total_no_sample;
										
	HDoublePtr							local_eigenvalues,
											mean0,
											Sb,
											Sw;
											
	SInt16								*classPtr;
											//*classWeightsPtr;
											
	UInt32								i,
											j,
											k,
											statClassNumber;

	SInt16								weightsIndex;
	
	
	*ERROR_FLAG = 0;
	
	mean0 = NULL;
	Sb = NULL;
	Sw = NULL;
	local_eigenvalues = NULL;
	gInverseMatrixMemory.pivotPtr = NULL;
	gInverseMatrixMemory.ipvotPtr = NULL;
	
			// Assign memory																
			
	mean0 = (double*)MNewPointer ((long)no_feature*FS_DOUBLE);
	if (mean0 == NULL)
		*ERROR_FLAG = 502;
	
	if (*ERROR_FLAG == 0)
		{
		Sb = (double*)MNewPointer ((long)no_feature*no_feature*FS_DOUBLE);
		if (Sb == NULL)
			*ERROR_FLAG = 503;
			
		}	// end "if (*ERROR_FLAG == 0)" 
	 
	if (*ERROR_FLAG == 0)
		{
		Sw = (double*)MNewPointer ((long)no_feature*no_feature*FS_DOUBLE);
		if (Sw == NULL)
			*ERROR_FLAG = 504;
			
		}	// end "if (*ERROR_FLAG == 0)" 
	 
	if (*ERROR_FLAG == 0)
		{
		local_eigenvalues =
				(double*)MNewPointer ((long)no_feature*no_feature*FS_DOUBLE);
		if (local_eigenvalues == NULL)
			*ERROR_FLAG = 505;
			
		}	// end "if (*ERROR_FLAG == 0)" 

	if (*ERROR_FLAG == 0)
		{
				// Get memory for eigenvector computation.							
	
		if (!SetupMatrixInversionMemory ((UInt16)no_feature,
														TRUE,
														NULL, 
														&gInverseMatrixMemory.pivotPtr, 
														NULL, 
														NULL, 
														&gInverseMatrixMemory.ipvotPtr))
			*ERROR_FLAG = 551;
		 
		}	// end "if (*ERROR_FLAG == 0)" 
	
			// Get the class weight list pointer.									
			// Also get the sum of the class weights.								
			
	classPtr = (SInt16*)GetHandlePointer (gFeatureExtractionSpecsPtr->classHandle);
		
	weightsIndex = GetProjectClassWeightsIndex ();
		
	totalWeights2 = GetTotalProbability (
											(UInt16*)classPtr, (UInt32)no_class, weightsIndex);
				
			// Scale the total weights such that a weighting factor of one 
			// will be used if each class is weighted equally.					
			
	totalWeights = totalWeights2/no_class;

	if (*ERROR_FLAG==0)
		{
				// Calculate Sw.																
	
		LoadDItemStringNumber (kFeatureExtractStrID,
										IDS_FeatureExtract15,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
					
				// Get total number of samples
				
		total_no_sample = 0;
		for (i=0; i<no_class; i++)
	 		total_no_sample += (class_info+i)->no_sample;
	 		
	 			// Initialize the Sw matrix.
	   	
		ZeroMatrix (Sw, no_feature, no_feature, TRUE);
	   	
	   		// Now compute the Sw matrix
	   
	   for (i=0; i<no_class; i++)
	 		{
			statClassNumber = classPtr[i] - 1;
			weight = GetClassWeightValue (
											(UInt16)statClassNumber, weightsIndex, totalWeights);
											
	 		for (j=0; j<no_feature; j++)
	 			{
	  			for (k=0; k<no_feature; k++)
	   			{
	    			*(Sw+j*no_feature+k) += *((class_info+i)->cov+j*no_feature+k) * weight;
	    							
	  				}	// end "for (k=0; k<no_feature; k++)" 
	  			
	  			}	// end "for (j=0; j<no_feature; j++)"
	    							
	    	}	// end "for (i=0; i<no_class; i++)"
	  
				// Calculate Sb 															 
	
		LoadDItemStringNumber (kFeatureExtractStrID,
										IDS_FeatureExtract16,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
					
				// Initialize Mo
				
		for (j=0; j<no_feature; j++)
			mean0[j] = 0.;
			
				// Now compute Mo
		
		for (j=0; j<no_class; j++)
			{
			statClassNumber = classPtr[j] - 1;
			weight = GetClassWeightValue (
											(UInt16)statClassNumber, weightsIndex, totalWeights2);
											
			for (k=0; k<no_feature; k++)
	 			mean0[k] += *((class_info+j)->mean+k) * weight;
	   		
	   	}	// end "for (j=0; j<no_class; j++)"
					
				// Initialize Sb
   		
		ZeroMatrix (Sb, no_feature, no_feature, TRUE);
   		
   			// Now compute Sb
	   	
	   for (i=0; i<no_class; i++)
	   	{
			statClassNumber = classPtr[i] - 1;
			weight = GetClassWeightValue (
											(UInt16)statClassNumber, weightsIndex, totalWeights);
	    								
	 		for (j=0; j<no_feature; j++)
	 			{
	  			for (k=0; k<no_feature; k++)
	    			*(Sb+j*no_feature+k) += (*((class_info+i)->mean+j) - mean0[j]) *
													(*((class_info+i)->mean+k) - mean0[k]) * weight;
	    								
	  			}	// end "for (j=0; j<no_feature; j++)" 
	  			
	  		}	// end "for (i=0; i<no_class; i++)"

				// Calculate EV_iSmSw.														
	
		LoadDItemStringNumber (kFeatureExtractStrID,
										IDS_FeatureExtract17,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
				 
		FS_eigen_inverse_SwSb_gen (Sw,
											Sb, 
											no_feature, 
											local_eigenvalues,
											eigen_vectors, 
											ERROR_FLAG);
	
				// Copy eigenvalues.															
				 
		for (i=0; i<no_feature; i++)
	 		eigen_values[i] = local_eigenvalues[i*no_feature+i];
	 		
		}	// end "if (*ERROR_FLAG==0)" 
		
			// Free memory.																	 
			
	local_eigenvalues = CheckAndDisposePtr (local_eigenvalues);
	Sw = CheckAndDisposePtr (Sw);
	Sb = CheckAndDisposePtr (Sb);
	mean0 = CheckAndDisposePtr (mean0);
	
	gInverseMatrixMemory.pivotPtr = CheckAndDisposePtr (gInverseMatrixMemory.pivotPtr);
	gInverseMatrixMemory.ipvotPtr = CheckAndDisposePtr (gInverseMatrixMemory.ipvotPtr);
	
	return;
	
}	// end "FS_discriminant_analysis" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_eigen_inverse_SwSb_gen
//
//	Software purpose:	
//							Note: First column vector: Best discriminating
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 02/27/1999	

void FS_eigen_inverse_SwSb_gen (
				HDoublePtr 							Sw,
				HDoublePtr 							Sb,
				UInt32								no_new_channel,
				HDoublePtr 							eigenvalues,
				HDoublePtr 							eigenvectors,
				SInt32*								ERROR_FLAG)
				
{		
	double								lEigenValue,
											lEigenVectorValue;
	
	HDoublePtr 							Sw1;
	
	SInt32	 							numberIterations;
											
	UInt32								i,
											j,
											k,
											numberBytes;
							 				
	
			// Assign memory.																	
			
	numberBytes = no_new_channel * no_new_channel * SDOUBLE;
	Sw1 = (HDoublePtr)MNewPointer (numberBytes);
	if (Sw1 == NULL)
		{
		*ERROR_FLAG=506;
		return;
		
		}	// end "if (Sw1 == NULL)"

 			// Calculate eigenvectors and eigenvalues of within-class
 			// scatter matrix (Sw)
	
	LoadDItemStringNumber (kFeatureExtractStrID, 
									IDS_FeatureExtract18,
									gStatusDialogPtr, 
									IDC_Status21, 
									(Str255*)gTextString);
 			
	if (!ComputeEigenvectors (Sw, 
										(UInt16)no_new_channel,
										eigenvectors,
										gInverseMatrixMemory.ipvotPtr,
										gInverseMatrixMemory.pivotPtr,
										1))
		*ERROR_FLAG = 590;
							
	numberIterations = (SInt32)gInverseMatrixMemory.pivotPtr[0];
	
	if ((*ERROR_FLAG == 0) && numberIterations <= 0)
		*ERROR_FLAG = 602;

	if (*ERROR_FLAG == 0)
		{
 			// Get Sw(-1/2){=Sw1}; This will be a symmetric matrix.
 			
		ZeroMatrix (Sw1, no_new_channel, no_new_channel, TRUE);
	 
		for (k=0; k<no_new_channel; k++)
			{
			lEigenValue = Sw[k*no_new_channel+k];
			if (lEigenValue > 0)
				{
				lEigenValue = sqrt (lEigenValue);
				
		 		for (i=0; i< no_new_channel; i++)
		 			{
		 			lEigenVectorValue = eigenvectors[no_new_channel*k+i];
					for (j=0; j<=i; j++)
						Sw1[i*no_new_channel+j] += 1./lEigenValue *
																lEigenVectorValue *
																	eigenvectors[no_new_channel*k+j];
		  								 				
		  			}	// end "for (i=0; i< no_new_channel; i++)" 
		  			
		  		}	// end "if (lEigenValue > 0)" 
		  		
		  	else	// lEigenValue <= 0 
		  		{
				*ERROR_FLAG = 602;
				break;
				
				}	// end "else lEigenValue <= 0" 
		  		
	 		}	// end "for (k=0; k<no_new_channel; k++)" 
	 		
		CopyLowerToUpperSquareMatrix ((UInt16)no_new_channel, Sw1);
			
		}	// end "*ERROR_FLAG == 0" 

	if (*ERROR_FLAG == 0)
		{
				// Calculate Sw(-1/2)SbSw(-1/2) into eigenvalues
				// Band weighting - second phase				
	
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract19,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);

		TransformSymmetricMatrix (Sw1, 
											Sb, 
											eigenvectors, 
											eigenvalues,
											(UInt16)no_new_channel, 
											(UInt16)no_new_channel,
											TRUE);
						
				// Calculate eigenvectors and eigenvalues of Sw(-1/2)SbSw(-1/2)
				// Band weighting - second phase
	
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract20,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
					
		if (!ComputeEigenvectors (eigenvalues, 
											(UInt16)no_new_channel,
											eigenvectors,
											gInverseMatrixMemory.ipvotPtr,
											gInverseMatrixMemory.pivotPtr,
											1))
			*ERROR_FLAG = 590;
								
		numberIterations = (SInt32)gInverseMatrixMemory.pivotPtr[0];
					
		if ((*ERROR_FLAG == 0) && numberIterations <= 0)
			*ERROR_FLAG = 602;
			
		}	// end "*ERROR_FLAG == 0" 

	if (*ERROR_FLAG == 0)
		{
				// Calculate Sw(-1/2)Ev. (Band Weighting - final phase)												
	
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract21,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
										
		MatrixMultiply (eigenvectors, 
								Sw1, 
								Sb,
								(UInt16)no_new_channel, 
								(UInt16)no_new_channel, 
								(UInt16)no_new_channel, 
								kMat1ByTransposeMat2);

		numberBytes = no_new_channel * no_new_channel * SDOUBLE;
		BlockMoveData (Sb, eigenvectors, numberBytes);
	
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract22,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
		
		normalize_rows (eigenvectors, no_new_channel, no_new_channel);
			
		}	// end "*ERROR_FLAG == 0" 
	
	Sw1 = CheckAndDisposePtr (Sw1);

	return;
	
}	// end "FS_eigen_inverse_SwSb_gen" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 FS_optimize_2_class
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
//	Coded By:			Chulhee Lee				Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

UInt32 FS_optimize_2_class (
				struct class_info_str* 			class_info, 
				SInt32*								class_index, 
				UInt32								no_class, 
				UInt32								no_new_channel,
				HDoublePtr	 						edbfm, 
				double 								accuracy, 
				SInt32*								ERROR_FLAG)
				
								
{
	double								accuracyThreshold,
											det,
											dValue,
											logDet[2],
											logDetDifference,
											mah[2],
											tacc,
											t1,
											t3;
	
	HDoublePtr							cov,
											EV,
											eval,
											fmean,
											icov_all,
											iCovPtr1,
											iCovPtr2,
											mahPtr,
											mean;
							
	HDoublePtr 							td,
											dDouble[2];
								
	HPtr									memoryBlockPtr;

	SInt64								total_sample;
								
	SInt32								error,
											numberIterations;
											
	UInt32								a,
											b,
											count2,
											i,
											i2,
											j,
											k,
											numberBytes,
											numberSamples;
											
	Boolean								determinantOKFlag;
	
	
	EV = NULL;
	eval = NULL;
	cov = NULL;
	icov_all = NULL;
	mean = NULL;
	dDouble[0] = NULL;
	dDouble[1] = NULL;
	
	memoryBlockPtr = (HPtr)gInverseMatrixMemory.inversePtr;
	
			// Init threshold																	 
			
	accuracyThreshold = accuracy * gFeatureExtractionSpecsPtr->optimizeThreshold/100.;
	
			// Assign memory & calculate eigenvalue of edbfm 						 
			
	count2 = no_new_channel * no_new_channel * FS_DOUBLE;
	
	EV = (HDoublePtr)memoryBlockPtr;
	memoryBlockPtr += count2;

	eval = (HDoublePtr)memoryBlockPtr;
	memoryBlockPtr += count2;
	
	BlockMoveData (edbfm, eval, count2);
					
	if (!ComputeEigenvectors (eval, 
										(UInt16)no_new_channel, 
										EV,
										gInverseMatrixMemory.ipvotPtr,
										gInverseMatrixMemory.pivotPtr,
										1))
		*ERROR_FLAG = 590;
								
	numberIterations = (SInt32)gInverseMatrixMemory.pivotPtr[0];
	if ((*ERROR_FLAG == 0) && numberIterations <= 0)
		*ERROR_FLAG = 702;
	
   i2 = 0;
	if (*ERROR_FLAG==0)
		{
		icov_all=(HDoublePtr)memoryBlockPtr;
		memoryBlockPtr += no_class * count2;
		
		cov = (HDoublePtr)memoryBlockPtr;
		memoryBlockPtr += no_class * count2;
		
		mean = (HDoublePtr)memoryBlockPtr;
		memoryBlockPtr += no_class * no_new_channel * FS_DOUBLE;
	
				// Assign memory & copy data. 											 
				
		dDouble[0] = (HDoublePtr)memoryBlockPtr;
		memoryBlockPtr += (long)FS_DOUBLE * 
						no_new_channel * (class_info+class_index[0])->no_sample;
				
		dDouble[1] = (HDoublePtr)memoryBlockPtr;
		memoryBlockPtr += (long)FS_DOUBLE * 
						no_new_channel * (class_info+class_index[1])->no_sample;
			
				// Get the total number of samples in the classes.					
				
		for (total_sample=0,i=0; i<no_class; i++)
			total_sample += (class_info+class_index[i])->no_sample;
			
		count2 = no_new_channel * no_new_channel;
		for (i2=1; i2<=no_new_channel; i2++)
			{
					// Transform the data values for new feature to be used.		
				
			for (i=0; i<no_class; i++)
				{
				if (*ERROR_FLAG == 0)
			 		FS_sub_linear_transform (
			 					dDouble[i],
			 					(class_info+class_index[i])->data_values,
								EV,
								no_new_channel,
								no_new_channel,
								(UInt32)(class_info+class_index[i])->no_sample,
								i2);	
			
						// Exit routine if user has "command period" down			
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						{
						if (*ERROR_FLAG == 0)
							*ERROR_FLAG = 590;
						
						}	// end "if (!CheckSomeEvents (osMask..." 
						
					}	// end "if (TickCount () >= gNextTime)" 
					
						// Transform the covariance matrix for the new feature	
						// to be used.															
		 
		 		if (*ERROR_FLAG == 0)
					TransformLineOfCovariance (EV,
														(class_info+class_index[i])->mean,
														(class_info+class_index[i])->cov,
														mean+i*no_new_channel,
														cov+i*count2,
														gInverseMatrixMemory.pivotPtr, 
														no_new_channel, 
														i2);
				
						// Exit routine if user has "command period" down			
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						{
						if (*ERROR_FLAG == 0)
							*ERROR_FLAG = 590;
						
						}	// end "if (!CheckSomeEvents (osMask..." 
						
					}	// end "if (TickCount () >= gNextTime)" 
		 	
				}	// end "for (i=0; i<no_class; i++)" 
				
			if (*ERROR_FLAG != 0)
			 	break;
			 	
					// Classsify																	 
					// Load a matrix with the subset of channels to be used for		
					// each class.																	
		
			numberBytes	= i2 * FS_DOUBLE;
			iCovPtr1 = icov_all;
			for (i=0; i<no_class; i++)
				{
				iCovPtr2 = &cov[count2 * i];
				for (j=0; j<i2; j++)
					{
					BlockMoveData (iCovPtr2, iCovPtr1, numberBytes);
					iCovPtr1 += i2;
					iCovPtr2 += no_new_channel;
					
	 				}	// end "for (j=0; j<i2; j++)" 
	 				
	 			}	// end "for (i=0; i<no_class; i++)" 
	 			
			for (i=0; i<no_class; i++)
	 			{			
				determinantOKFlag = InvertSymmetricMatrix (
																	icov_all+i2*i2*i,
																	(UInt16)i2, 
																	gInverseMatrixMemory.pivotPtr, 
																	gInverseMatrixMemory.indexRowPtr, 
																	gInverseMatrixMemory.indexColPtr, 
																	gInverseMatrixMemory.ipvotPtr,
																	&det, 
																	&logDet[i],
																	kReturnMatrixInverse);
					
				if (gOperationCanceledFlag)
					{
					*ERROR_FLAG = 591;
					break;
					
					}	// end "if (gOperationCanceledFlag)" 

				if (!determinantOKFlag)
					{
					*ERROR_FLAG = 601;
					break;
				 	
					}	// end "if (!determinantOKFlag)" 
	 			
				}	// end "for (i=0; i<no_class; i++)" 
	 
			if (*ERROR_FLAG != 0)
			 	break;
	 
			error = 0;
			for (i=0; i<no_class; i++)
				{
						// Do classification													
				
				logDetDifference = logDet[i] - logDet[1-i];	 
	  			td = dDouble[i];
	  			numberSamples = (UInt32)(class_info+class_index[i])->no_sample;
	 			for (j=0; j<numberSamples; j++)
	 				{
	 				fmean = mean;
	  				iCovPtr1 = icov_all;
	  				for (k=0; k<no_class; k++)
	  					{
	  					mahPtr = &mah[k];
	  					for (*mahPtr=0.,a=0; a<i2; a++)
	  						{
	         			t3 = *(td+a)- *(fmean+a);

	         			for (t1=0.,b=0; b<a; b++)
	          				t1 -= (*(td+b)- *(fmean+b)) * *(iCovPtr1+b);
	         			*mahPtr += t3 * (t1 + t1 - t3 * *(iCovPtr1+a));
	         			
	         			iCovPtr1 += i2;
	         			
	    					}	// end "for (mah[k]=0.,a=0; a<i2; a++)" 
	    					
	 					fmean += no_new_channel;
	    					   
	  					}	// for k 
	  					
							// Check error 													 
							
	    			if (mah[i]-logDetDifference < mah[1-i])
	     				error++;
			
							// Exit routine if user has "command period" down		
					
					if (TickCount () >= gNextTime)
						{
						if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
							{
							*ERROR_FLAG = 590;
							break;
							
							}	// end "if (!CheckSomeEvents (osMask..." 
							
						}	// end "if (TickCount () >= gNextTime)" 
						
					td += no_new_channel;
	     				
	 				}	// for  j 
	 
	 			if (*ERROR_FLAG != 0)
	 				break;
	 				
				}	// for  i 
		
			if (*ERROR_FLAG != 0)
	 			break;
			
			tacc = 100. * (total_sample-error)/total_sample;
			if (tacc >= accuracyThreshold)
				break;
		
			}	// for i2 
			
		}	// end "if (*ERROR_FLAG == 0)" 
		
			// i2 now represents the number of features required to meet the		
			// classification percent threshold.  Make certain that it is not		
			// larger than 'no_new_channel'.													
			
	if (i2 > no_new_channel)
		i2 = no_new_channel;
	
			// Making new EDBFM
	
	if ((*ERROR_FLAG == 0) && (i2 < no_new_channel))
		{
		ZeroMatrix (edbfm, no_new_channel, no_new_channel, TRUE);
	 
	 	a = 0;
		for (i=0; i<i2; i++)
			{
					// Make new EDBFM based on just the I2 number of channels 
					// needed to meet the threshold.
			
			b = 0;
			iCovPtr1 = edbfm;
			for (j=0; j<no_new_channel; j++)
				{
				dValue = eval[a] * EV[i*no_new_channel+j];
				for (k=0; k<no_new_channel; k++)
					{
					*iCovPtr1 += dValue * EV[i*no_new_channel+k];
					iCovPtr1++;
					
					}	// end "for (k=0; k<no_new_channel; k++)" 
	         
	         }	// end "for (j=0; j<no_new_channel; j++)" 
	         
	      a += no_new_channel + 1;
	                             
			}	// end "for (i=0; i<i2; i++)" 
			
		}	// end "if ((*ERROR_FLAG == 0) && ..." 
		
	if (*ERROR_FLAG == 702)
		{
				// error code of 702 indicates that the classes could not be		
				// optimized.  Change error back to 0 and continue with no			
				// optimization.																	
				
		*ERROR_FLAG = 0;
		i2 = no_new_channel;
		
		}	// end "if (*ERROR_FLAG == 702)" 
	
	return (i2);
	
}	// end "FS_optimize_2_class" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_quick_feature_extraction
//
//	Software purpose:	
//
//	Parameters in:		no_feature (long):	 			number of features (=N)
//							no_class (long):	 			number of classes
//							quick_class_info (struct): 	class informations
//															 		no_sample, data_values, cov, mean
//							feature_extraction_type (long):	
//																2:discriminant analysis
//																3:decision boundary feature extraction 
//
//	Parameters out:	eigenvectors (double, N by N)	eigenvectors
//							eigenvectors (double, N)			eigenvalues
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 02/07/2002	

void FS_quick_feature_extraction (
				UInt32								no_feature, 
				UInt32								no_class, 
				ClassInfoPtr				 		quick_class_info,
				SInt32								feature_extraction_type, 
				HDoublePtr		 					eigen_vectors, 
				HDoublePtr		 					eigen_values, 
				SInt32*								ERROR_FLAG)
				

{
	struct class_info_str* 			class_info;
	
	UInt32								i;
	

	*ERROR_FLAG = 0;
	
			// assign memory to class_info & 
			// copy information from quick_class_info to class_info

	class_info=(CLASS_INFO_STR *)MNewPointer (sizeof (CLASS_INFO_STR)*no_class);
	if (class_info == NULL)
		*ERROR_FLAG = 500;

	else	// class_info != NULL 
		{
		for (i=0; i<no_class; i++)
			{
			(class_info+i)->log_det = 0.;
			(class_info+i)->wave_length_default = 0;
			(class_info+i)->no_sample = (quick_class_info+i)->no_samples;
			(class_info+i)->data_values = (quick_class_info+i)->dataValuesPtr;
			(class_info+i)->mean = (quick_class_info+i)->meanPtr;
			(class_info+i)->cov = (quick_class_info+i)->covariancePtr;
			(class_info+i)->icov = (quick_class_info+i)->inversePtr;
			(class_info+i)->half_image = NULL;
			(class_info+i)->fmean = NULL;
			(class_info+i)->fvar = NULL;
			(class_info+i)->mah_dis = NULL;
			(class_info+i)->stat_name_storage = 0;
			(class_info+i)->mah_disLoadedFlag = FALSE;
			
			}	// end "for (i=0; i<no_class; i++)" 
			
		}	// end "else class_info != NULL" 
	
			// feature selection

	if (*ERROR_FLAG==0)
		{
		ShowStatusDialogItemSet (kStatusCommand);
	
		if (feature_extraction_type == kDiscriminantAnalysis)
			{
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract13,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
					
			FS_discriminant_analysis (class_info,
												no_class,
												no_feature,
												eigen_vectors,
												eigen_values,
												ERROR_FLAG);
			
	 		}	// end "if (feature_extraction_type == kDiscriminantAnalysis)" 
	 									
		else if (feature_extraction_type == kDecisionBoundary)
			{
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract14,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
							
	 		FS_decision_boundary (class_info,
											no_class,
											no_feature,
											eigen_vectors,
											eigen_values,
											ERROR_FLAG);
	 									
	 		}	// end "else if (feature_extraction_type == kDecisionBoundary)" 
	 									
		else if (feature_extraction_type == kNonparametricWeighted)
			{
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract75,
											gStatusDialogPtr, 
											IDC_Status11, 
											(Str255*)gTextString);
			
			*ERROR_FLAG = NWFE (class_info, 
										no_class,
										no_feature,
										eigen_vectors, 
										eigen_values);
	 									
	 		}	// end "else if (feature_extraction_type == kNonparametricWeighted)" 

		}	// end "if (*ERROR_FLAG==0)" 
	
			// free memory
			
	if (class_info != NULL)
		class_info = (class_info_str*)CheckAndDisposePtr ((Ptr)class_info);
	
	LoadDItemString (gStatusDialogPtr, IDC_Status21, (Str255*)"\0");
	
	return;
	
}	// end "FS_quick_feature_extraction" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_sol_bnd_line
//
//	Software purpose:	
//
//	Parameters in:			
//
//	Parameters out:	
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

void FS_sol_bnd_line (
				struct class_info_str* 			class_info, 
				SInt32*								class_index,
				HDoublePtr*							point1_array,
				HDoublePtr*							point2_array, 
				Ptr									memoryBlockPtr, 
				UInt32								no_points, 
				UInt32								dim,
				HDoublePtr							edbfm, 
				SInt32*								ERROR_FLAG)
				
{
	double								c,
											doubleValue,
											msm1,
											msm2,
			 								logdif,
			 								threshold=0.;
	
	HDoublePtr		 					icov_diff,
											normal,
		 									icov1,
											icov2,
											mean1,
											mean2,
				 							mean_icov_diff,
											tempDoublePtr1,
											tempDoublePtr2,
											tempDoublePtr3;
											
	UInt32								i,
											j,
											k,
											lowerLeftIndexSkip,
											pointsRejected;
	
	Boolean								rejectedFlag;
	
		
			// Initialize & check no of points											
	  	
	ZeroMatrix (edbfm, dim, dim, TRUE);

	if (no_points > 0)
		{  	
				// Assign memory																	
		
		j = 2 * dim * FS_DOUBLE;
		normal = (double*)memoryBlockPtr;
		memoryBlockPtr += j;
		
		j = (unsigned long)dim * dim * FS_DOUBLE;
		icov_diff = (double*)memoryBlockPtr;
		memoryBlockPtr += j;
		
		j = (unsigned long)dim * FS_DOUBLE;
		mean_icov_diff = (double*)memoryBlockPtr;
		
		mean1 = (class_info+class_index[0])->mean;
		mean2 = (class_info+class_index[1])->mean;
		icov1 = (class_info+class_index[0])->icov;
		icov2 = (class_info+class_index[1])->icov;
		logdif = (class_info+class_index[0])->log_det -
												(class_info+class_index[1])->log_det;
									
				// Calculate icov_diff														
				
		tempDoublePtr1 = icov1;
 		tempDoublePtr2 = icov2;
 		tempDoublePtr3 = icov_diff;
		lowerLeftIndexSkip = dim - 1;
 		for (i=0; i<dim; i++)
 			{
			for (j=0; j<=i; j++)
				{
				*icov_diff = *icov1 - *icov2;
				icov1++;
				icov2++;
				icov_diff++;
			
				}	// end "for (j=0; j<=i; j++)" 
				
			icov_diff += lowerLeftIndexSkip;
			icov1 += lowerLeftIndexSkip;
			icov2 += lowerLeftIndexSkip;
			lowerLeftIndexSkip--;
			
			}	// end "for (i=0; i<dim; i++)" 
			
		icov1 = tempDoublePtr1;
 		icov2 = tempDoublePtr2;
 		icov_diff = tempDoublePtr3;
 		
		CopyLowerToUpperSquareMatrix ((UInt16)dim, icov_diff);
		
				// Calculate mean_icov_diff												
				
 		tempDoublePtr3 = mean_icov_diff;
		for (i=0; i<dim; i++)
			{
			for (*mean_icov_diff=0.,j=0; j<dim; j++)
				{
				*mean_icov_diff += *(mean1+j) * *icov1 - *(mean2+j) * *icov2;
				
				icov1++;
				icov2++;
															
				}	// end "for (j=0; j<dim; j++)" 
				
			mean_icov_diff++;
			
			}	// end "for (i=0; i<dim; i++)" 
															
		icov1 = tempDoublePtr1;
 		icov2 = tempDoublePtr2;
 		mean_icov_diff = tempDoublePtr3;
																
				// Calculate c																	 
		
		tempDoublePtr1 = icov1;
		for (msm1=0.,i=0; i<dim; i++)
			{
			doubleValue = *(mean1+i);
			for (j=0; j<dim; j++)
				{
				msm1 += doubleValue * *icov1 * *(mean1+j);
				icov1++;
				
		 		}	// end "for (j=0; j<dim; j++)" 
				
		 	}	// end "for (i=0; i<dim; i++)" 
		 		
		icov1 = tempDoublePtr1;
	 	
		tempDoublePtr2 = icov2;
		for (msm2=0.,i=0; i<dim; i++)
			{
			doubleValue = *(mean2+i);
			for (j=0; j<dim; j++)
				{
				msm2 += doubleValue * *icov2 * *(mean2+j);
				icov2++;
				
		 		}	// end "for (j=0; j<dim; j++)" 
		 		
		 	}	// end "for (msm1=0.,msm2=0.,i=0; i<dim; i++)" 
		 		
		icov2 = tempDoublePtr2;
	 	
		c = 0.5 * (msm1 - msm2 + logdif);
	
				// Calculate effective decision boudary feature matrix.			 
	
		pointsRejected = 0;
		for (i=0; i<no_points; i++)
			{
			rejectedFlag = FS_sol_bnd_line_2 (mean_icov_diff,
															icov_diff,
															c,
															*(point1_array+i),
															*(point2_array+i),
															dim,
															normal,
															threshold);
			
			if (rejectedFlag)
				pointsRejected++;
	
			else	// !rejectedFlag 
				{								
				tempDoublePtr1 = edbfm;
		 		tempDoublePtr2 = normal;
				lowerLeftIndexSkip = dim - 1;
		 		for (j=0; j<dim; j++)
		 			{
		 			doubleValue = *(normal+j);
					for (k=0; k<=j; k++)
						{
						*edbfm += doubleValue * *normal;
						normal++;
						edbfm++;
					
						}	// end "for (k=0; k<dim; k++)" 
						
					edbfm += lowerLeftIndexSkip;
					lowerLeftIndexSkip--;
					
		 			normal = tempDoublePtr2;
					
					}	// end "for (j=0; j<dim; j++)" 
				
				edbfm = tempDoublePtr1;
				
				}	// end "else !rejectedFlag" 
				
					// Exit routine if user has "command period" down.				
			
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					{
					if (*ERROR_FLAG == 0)
						*ERROR_FLAG = 590;
					break;
					
					}	// end "if (!CheckSomeEvents (..."
					
				}	// end "if (TickCount () >= gNextTime)" 
					
			}	// end "for (i=0; i<no_points; i++)" 
			
				// Normalize edbfm															 
	
		if (*ERROR_FLAG == 0)
			{
			no_points -= pointsRejected;
			
			if (no_points > 0)
				{
				tempDoublePtr1 = edbfm;
				lowerLeftIndexSkip = dim - 1;
				doubleValue = 1./no_points;
				for (j=0; j<dim; j++)
					{
					for (k=0; k<=j; k++)
						{
						*edbfm *= doubleValue;
						edbfm++;
						
						}	// end "for (k=0; k<=j; k++)" 
						
					edbfm += lowerLeftIndexSkip;
					lowerLeftIndexSkip--;
					
					}	// end "for (j=0; j<dim; j++)" 
		  	
				edbfm = tempDoublePtr1;
				CopyLowerToUpperSquareMatrix ((UInt16)dim, edbfm);
					
				}	// end "if (no_points > 0)" 
				
			}	// end "if (*ERROR_FLAG == 0)" 
				
		}	// end "if (no_points > 0)" 
	
	return;
	
}	// end "FS_sol_bnd_line" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean FS_sol_bnd_line_2
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
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 02/27/1999	

Boolean FS_sol_bnd_line_2 (
				HDoublePtr 							mean_icov_diff, 
				HDoublePtr 							icov_diff, 
				double 								c,
				HDoublePtr							point1, 
				HDoublePtr							point2, 
				UInt32								dim, 
				HDoublePtr 							normal, 
				double 								threshold)


{
	double								a,
											b,
											c2,
											tempPoint1,
											t1,
											u,
											u1,
											u2;
								
	HDoublePtr							icovDiffPtr1,
											icovDiffPtr2,
											V;
	
	UInt32								i,
											j;
	
	
			// Get address for memory for difference vector V and calcuate V	
			// The memory for vector v is at the end of the memory for the		
			// normal vector.																	
	
	V = &normal[dim];
	
	for (i=0; i<dim; i++)
		{
		*(V+i) = (double)*point2 - (double)*(point1+i);
		point2++;
		
		}	// end "for (i=0; i<dim; i++)" 

			// Calculate a, b, c2 															 
			
	icovDiffPtr1 = icov_diff;
	for (a=0.,b=0.,c2=0.,i=0; i<dim; i++)
		{
		tempPoint1 = (double)*(point1+i);
		icovDiffPtr2 = icovDiffPtr1;
		for (t1=0.,j=0; j<dim; j++)
			{
			t1 += *icovDiffPtr2 * *(V+j);
			icovDiffPtr2++;
			
			}	// end "for (t1=0.,j=0; j<dim; j++)" 
			
		a += *(V+i) * t1;
		b += tempPoint1 * t1;
		
		icovDiffPtr2 = icovDiffPtr1;
		for (t1=0.,j=0; j<i; j++)
			{
	 		t1 += (double)*(point1+j) * *icovDiffPtr2;
	 		icovDiffPtr2++;
	 		
			}	// end "for (t1=0.,j=0; j<i; j++)" 

		c2 += tempPoint1 * (t1 + t1 + tempPoint1 * *(icovDiffPtr1+i));
		
		icovDiffPtr1 += dim;
	  
		}	// end "for (i=0; i<dim; i++)" 
		
	a *= .5;
	c2 *= .5;
	
	for (i=0; i<dim; i++)
		{
		b -= *(mean_icov_diff+i) * *(V+i);
		c2 -= *(mean_icov_diff+i) * (double)*(point1+i);
		
		}	// end "for (i=0; i<dim; i++)" 
		
	c2 += c;
	
			// Calculate u
			 
	if (fabs (a) < 1e-6*fabs (b))
		{
		if (b==0)
			u = 0;
		
		else	// b!=0  
	 		u = threshold - c2/b;
	 		
		}	// end "if (fabs (a) < 1e-6*fabs (b))" 
		
	else	// fabs (a) >= 1e-6*fabs (b) 
		{
		t1 = b*b - 4*a*(c2-threshold);
		if (t1 < 0)
	 																					return (TRUE);
	 
		 t1 = sqrt (t1);
		 a = 1./(2 * a);
		 u1 = (-b+t1) * a;
		 u2 = (-b-t1) * a;
		 if ((u1>=0) && (u1<=1))
		 	u = u1;
		 	
		 else	// (u1<0) || (u1>1) 
		 	{
		 	if ((u2>=0) && (u2<=1))
		 		u = u2;
		 		
		 	else	// (u2<0) || (u2>1) 
	
		  																				return (TRUE);
		 		
		 	}	// end "else (u1<0) || (u1>1)" 
	 		
		}	// end "else fabs (a) >= 1e-6*fabs (b)" 
		
			// Find intersection point (normal[i]) and calculate h (X)
	
	a = u;		
	for (i=0; i<dim; i++)
		*(normal+i) = a * *(V+i) + (double)*(point1+i);
		
	cal_h_X (&u2, normal, icov_diff, mean_icov_diff, dim, c);

	if (fabs (u2) > 1e-7)
	  																					return (TRUE);
			
			// Find normal vector & normalize.											
	
	icovDiffPtr1 = icov_diff;
	for (t1=0.,i=0; i<dim; i++)
		{
		a = 0;
		
		for (j=0; j<dim; j++)
			{
	 		a += *icovDiffPtr1 * normal[j];
	 		icovDiffPtr1++;
	 		
	 		}	// end "for (j=0; j<dim; j++)" 
	 		
		a -= mean_icov_diff[i];
		t1 += a * a;
		
		*V = a;
		V++;
		
		}	// end "for (i=0; i<dim; i++)" 
	
	t1 = sqrt (1./t1);
	V = &normal[dim];
	for (i=0; i<dim; i++)
		{
		*normal = *V * t1;
		normal++;
		V++;
		
		}	// end "for (i=0; i<dim; i++)" 
	
	return (FALSE);
	
}	// end "FS_sol_bnd_line_2" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 FS_sub_find_edbfm_2_class
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
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 05/13/2016	

typedef struct cl_res_info_str 
	{
	double		threshold2;
	SInt16*		classified_as;
	UInt32		no_passed_mah_dis;
	HFloatPtr	mah_dis;
	UInt32		no_passed_mah_dis_the_other;
	HFloatPtr	mah_dis_the_other;
	
	} CL_RES_INFO_STR;

UInt32 FS_sub_find_edbfm_2_class (
				struct class_info_str* 			class_info, 
				SInt32*								class_index, 
				UInt32								no_class, 
				UInt32								no_new_channel, 
				HDoublePtr 							edbfm, 
				HDoublePtr 							accuracy, 
				SInt32*								countMahDistancePtr, 
				SInt32*								ERROR_FLAG)				
									
{
	struct cl_res_info_str 			cl_res[2];
	
	double								dmin,
											fmax,
											logDet[2],
											threshold2,
											threshold,
											tmp,
											t1,
											t3;
													
	Ptr									memoryBlockPtr;
	
	DoublePtr							ficov,
											fmean,
											icov,
											icov_all[2];
										
	DoublePtr							td,
											tp1,
											tp2,
											tp2MinPtr;
										
	DoublePtr							*point1_array,
											*point2_array,
											*savedPoint1_array,
											*savedPoint2_array;
	
	HSInt32Ptr							array_index[2],
											array_indexPtr;

	SInt64								numberSamples,
											samplesInClass,
											samplesInOtherClass,
											total_sample;
												
	SInt32								error,
											index,
											otherClass;
												
	UInt32								i,
											j,
											k,
											l,
											m, 
											minimum,
											minimumLimit,
											numberBytes,
											point_array_cnt;
			
			
	if (no_class != 2)
		*ERROR_FLAG = 1;
	
	savedPoint1_array = NULL;
	savedPoint2_array = NULL;
	(cl_res+0)->classified_as = NULL;
	(cl_res+1)->classified_as = NULL;
	(cl_res+0)->mah_dis = (class_info+class_index[0])->mah_dis;
	(cl_res+1)->mah_dis = (class_info+class_index[1])->mah_dis;
	(cl_res+0)->mah_dis_the_other = NULL;
	(cl_res+1)->mah_dis_the_other = NULL;
	array_index[0] = NULL;
	array_index[1] = NULL;
	point_array_cnt = 0;
	
	memoryBlockPtr = (Ptr)gInverseMatrixMemory.inversePtr;
	
			// Assign memory
			
	samplesInClass = (class_info+class_index[0])->no_sample;
	samplesInOtherClass = (class_info+class_index[1])->no_sample;
	total_sample = samplesInClass + samplesInOtherClass;

	if (*ERROR_FLAG == 0)
		{
				// Assign memory for savedPoint1_array.								 
	
		numberBytes = sizeof (HDoublePtr*) * (UInt32)total_sample;
		
		savedPoint1_array = (HDoublePtr*)memoryBlockPtr;
		memoryBlockPtr += numberBytes;
					
		savedPoint2_array = (HDoublePtr*)memoryBlockPtr;
		memoryBlockPtr += numberBytes;
		
		numberBytes = (UInt32)(FS_FLOAT*samplesInClass);
		(cl_res+0)->mah_dis_the_other = (float*)memoryBlockPtr;
		memoryBlockPtr += numberBytes;
		
		numberBytes = (UInt32)(FS_FLOAT*samplesInOtherClass);
		(cl_res+1)->mah_dis_the_other=(float*)memoryBlockPtr;
		memoryBlockPtr += numberBytes;
		
		#ifndef multispec_lin
			numberBytes = (UInt32)(sizeof (long)*samplesInClass);
			array_index[0] = (SInt32*)memoryBlockPtr;
			memoryBlockPtr += numberBytes;
			numberBytes = (UInt32)(sizeof (long)*samplesInOtherClass);
			array_index[1] = (SInt32*)memoryBlockPtr;
			memoryBlockPtr += numberBytes;
		#else
			numberBytes = (UInt32)(sizeof (int)*samplesInClass);
			array_index[0] = (int*)memoryBlockPtr;
			memoryBlockPtr += numberBytes;
			numberBytes = (UInt32)(sizeof (int)*samplesInOtherClass);
			array_index[1] = (int*)memoryBlockPtr;
			memoryBlockPtr += numberBytes;
		#endif
		
		numberBytes = (UInt32)(sizeof (short)*samplesInClass);
		numberBytes = 4 * (numberBytes + 3)/4;
		(cl_res+0)->classified_as = (short*)memoryBlockPtr;
		memoryBlockPtr += numberBytes;
		
		numberBytes = (UInt32)(sizeof (short)*samplesInOtherClass);
		numberBytes = 4 * (numberBytes + 3)/4;
		(cl_res+1)->classified_as = (short*)memoryBlockPtr;
		memoryBlockPtr += numberBytes;
		
		for (i=0; i<no_class; i++)
			{
	 		icov_all[i] = (class_info+class_index[i])->icov;
	 		logDet[i] = (class_info+class_index[i])->log_det;
	 		
			}	// end "for (i=0; i<no_class; i++)" 
			
				// Copy threshold values to local variables.								
				
		threshold = gFeatureExtractionSpecsPtr->threshold1;
		threshold2 = gFeatureExtractionSpecsPtr->threshold2;

				// Classsify
		
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract27,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
		
				// Do classification
										
		for (i=0; i<no_class; i++)
			{	   
			numberSamples = (class_info+class_index[i])->no_sample;
			
			for (j=0; j<no_class; j++)
				{
				if (!(class_info+class_index[i])->mah_disLoadedFlag || (i != j))
					{
					fmean = (class_info+class_index[j])->mean;
					icov = icov_all[j];
					td = (class_info+class_index[i])->data_values;
						
					for (k=0; k<numberSamples; k++)
						{
					  	ficov = icov;
					  	
					  	for (tmp=0.,l=0; l<no_new_channel; l++)
					  		{
					      t3 = (double)*(td+l) - *(fmean+l);
				
					      for (t1=0.,m=0; m<l; m++)
					      	t1 -= ((double)*(td+m) - *(fmean+m))* *(ficov+m);
					      	
					      tmp += t3 * (t1 + t1 - t3 * *(ficov+l));
					      
					  		ficov += no_new_channel;
					      
							}	// end "for (l=0; l<no_new_channel; l++)" 
							
						if (i == j)
							(cl_res+i)->mah_dis[k] = (float)tmp;
							
						else	// i != j 
				   		(cl_res+i)->mah_dis_the_other[k] = (float)tmp;
			
								// Exit routine if user has "command period" down		
					
						if (TickCount () >= gNextTime)
							{
							if (!CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
								{
								*ERROR_FLAG = 590;
								break;
								
								}	// end "if (!CheckSomeEvents (..." 
							
							}	// end "if (TickCount () >= gNextTime)" 
							
						td += no_new_channel;
					
						}	// end "for (k=0; k<numberSamples; k++)" 
						
					}	// end "if (!(...->mah_disLoadedFlag || (i != j))" 
				
				if (*ERROR_FLAG == 590)
					break;
				   		
				}	// end "for (j=0; j<no_class; j++)" 
				
			if (*ERROR_FLAG == 590)
				break;
	
					// Now check errors for this class. 								
						
			error = 0;
			tmp = logDet[i] - logDet[1-i];
			for (k=0; k<numberSamples; k++)
				{		 
				if ((cl_res+i)->mah_dis[k]-tmp < (cl_res+i)->mah_dis_the_other[k])
		  			{
		  			error++;
					(cl_res+i)->classified_as[k] = (SInt16)(1 - i);
					
					}	// end "if ((cl_res+i)->mah_dis[k]-tmp < ..."
					
		 		else	// ...mah_dis[k]-tmp >= ...mah_dis_the_other[k] 
		 			{
		 			if ((cl_res+i)->mah_dis[k] > threshold)
						(cl_res+i)->classified_as[k] = (SInt16)i;
						
					else	// (cl_res+i)->mah_dis[k] <= threshold 
						(cl_res+i)->classified_as[k] = -1;
						
					}	// end "else ...mah_dis[k]-tmp >= ..." 
								
				}	// end "for (k=0; k<numberSamples; k++)" 
				
			(class_info+class_index[i])->mah_disLoadedFlag = TRUE;
				
			}	// end "for (i=0; i<no_class; i++)" 
	
		*accuracy = 100. * (total_sample-error)/total_sample;
		
				// Count number of samples passing the threshold test.			 
						
		minimum = gFeatureExtractionSpecsPtr->minimumThresholdNumber;
		
		if (*ERROR_FLAG == 590)
			no_class = 0;
		
		for (i=0; i<no_class; i++)
			{
			array_indexPtr = array_index[i];
			(cl_res+i)->no_passed_mah_dis_the_other = (cl_res+i)->no_passed_mah_dis = 0;
		 	numberSamples = (class_info+class_index[i])->no_sample;
		 	
			for (j=0; j<numberSamples; j++)	// 333 
				if ((cl_res+i)->classified_as[j] == (SInt32)i)
					{
					(cl_res+i)->no_passed_mah_dis++;
						
					if ((cl_res+i)->mah_dis_the_other[j]>threshold2)
						{
						(cl_res+i)->no_passed_mah_dis_the_other++;
						*array_indexPtr = j * (SInt32)no_new_channel;
						array_indexPtr++;
	   				
	   				}	// end "if (...->mah_dis_the_other[j]>threshold2)" 
	   				
		 			}	// end "if ((cl_res+i)->classified_as[j]==i && ..." 
	
					// Too Few Samples ? 													 
					
		 	if ((cl_res+i)->no_passed_mah_dis_the_other < minimum)
				{
				array_indexPtr = array_index[i];
				minimumLimit = (UInt32)MIN (minimum, numberSamples);
				for (k=0; k<minimumLimit; k++)
					{
					index = -1;
					for (fmax=-1e38,m=0; m<numberSamples; m++)
						{
						if ((cl_res+i)->mah_dis_the_other[m] > fmax && 
												(cl_res+i)->classified_as[m] == (SInt32)i)
		    				{
		     				fmax = (cl_res+i)->mah_dis_the_other[m];
		 					index = (SInt32)m;
			 				
		    				}	// end "if ((cl_res+i)->mah_dis_the_other[m]>..."
							
						}	// end "for (fmax=-1e38,m=0; m<numberSamples; m++)"
		    				
	  				*array_indexPtr = index * (SInt32)no_new_channel;
	  				array_indexPtr++;
	  				
					if (index >= 0)
						{
						(cl_res+i)->mah_dis_the_other[index] = (float)(-FLT_MAX);
						countMahDistancePtr[i+4]++;
						
						}	// end "if (index >= 0)"
	  				
		   		}	// end "for (k=0; k<minimumLimit; k++)" 
		  		
		 		}	// end "if ((cl_res+i)->no_passed_mah_dis_the_other<minimum)" 
		 		
		 	countMahDistancePtr[i] = (cl_res+i)->no_passed_mah_dis;
		 	countMahDistancePtr[i+2] = (cl_res+i)->no_passed_mah_dis_the_other;
		 		
		 	}	// end "for (i=0; i<no_class; i++)" 
			
				// Exit routine if user has "command period" down					
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				*ERROR_FLAG = 590;
				no_class = 0;
				
				}	// end "if (!CheckSomeEvents (..." 
				
			}	// end "if (TickCount () >= gNextTime)" 
		  
				// Find the closest sample in the other group.						 
			
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract28,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
	
		point1_array = savedPoint1_array;
		point2_array = savedPoint2_array;
		
		for (i=0; i<no_class; i++)
			{
			otherClass = 1 - i;
			samplesInClass = (class_info+class_index[i])->no_sample;
			samplesInOtherClass = (class_info+class_index[otherClass])->no_sample;
			minimumLimit = (UInt32)MIN (minimum, samplesInOtherClass);
			minimumLimit = MAX (minimumLimit, cl_res[1-i].no_passed_mah_dis_the_other); 
			if (*(array_index[otherClass]) < 0)
				minimumLimit = 0;
				
		   td = (class_info+class_index[i])->data_values;
			
		 	for (j=0; j<samplesInClass; j++)
		 		{
		  		if ((cl_res+i)->classified_as[j] == (SInt32)i)
		  			{
		   		dmin = DBL_MAX;
		    		tp2MinPtr = NULL;
					array_indexPtr = array_index[otherClass];
	     			for (l=0; l<minimumLimit; l++)
	     				{
	      			tp2 = (class_info+class_index[otherClass])->data_values +
																							*array_indexPtr;
	     				tp1 = td;

							  // Get the Euclidean distance. 								
							   
				      for (tmp=0.,m=0; m<no_new_channel; m++)
				      	{
				      	t1 = (SInt32)*tp1 - (SInt32)*tp2;
				      	tmp += t1 * t1;
				      	tp1++;
				      	tp2++;
				      	
				      	}	// end "for (tmp=0.,m=0; m<no_new_channel; m++)" 
			
				      if (tmp < dmin)
				      	{
				       	dmin = tmp;
				       	tp2MinPtr = tp2 - no_new_channel;
				       	
				      	}	// end "if (tmp < dmin)" 
				      	
						array_indexPtr++;
								
						}	// end "for (l=0; l<minimumLimit; l++)" 
		     
	    			if (tp2MinPtr)	// save pair addresses 
	    				{
		     			*point1_array = td;
					   *point2_array = tp2MinPtr;
					      
					   point1_array++;
					   point2_array++;
	     					
	     				point_array_cnt++;
	     				
						}	// if (tp2MinPtr != NULL) 
						
					}	// if ((cl_res+i)->... 
					
		    	td += no_new_channel;
			
						// Exit routine if user has "command period" down			
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						{
						*ERROR_FLAG = 590;
						break;
						
						}	// end "if (!CheckSomeEvents (..." 
						
					}	// end "if (TickCount () >= gNextTime)" 
							
		   	}	// for j 
		   	
		   if (*ERROR_FLAG != 0)
		   	break;
							
		   }	// for i 
		
		}	// end "if (*ERROR_FLAG==0)" 
		   
			// Find the point on decision boundary and calculate edbfm
			
	LoadDItemStringNumber (kFeatureExtractStrID, 
									IDS_FeatureExtract29,
									gStatusDialogPtr, 
									IDC_Status21, 
									(Str255*)gTextString);
	
	if (*ERROR_FLAG==0)
		FS_sol_bnd_line (class_info, 
									class_index,
									savedPoint1_array,
									savedPoint2_array,
									(Ptr)(cl_res+0)->mah_dis_the_other,
									point_array_cnt,
									no_new_channel,
									edbfm,
									ERROR_FLAG);

	return (point_array_cnt);
	
}	// end "FS_sub_find_edbfm_2_class" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_sub_linear_transform
//
//	Software purpose:	Transform the input data values for the requested feature.
//
//	Parameters in:			
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:
//
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

void FS_sub_linear_transform (
				HDoublePtr 							fdata_after, 
				HDoublePtr							fdata_ori, 
				HDoublePtr 							matrixPtr, 
				UInt32								matarix_dim,
				UInt32								no_new_channel, 
				UInt32								no_sample, 
				UInt32								featureNumber)

{
	HDoublePtr 							localMatrixPtr;
	
	HDoublePtr 							localDataAfterPtr,
											localDataOrigPtr;

	UInt32								j,
											k;
	
	
	localDataAfterPtr = fdata_after + featureNumber - 1;
	localMatrixPtr = &matrixPtr[(featureNumber-1)*matarix_dim];
	for (j=0; j<no_sample; j++)
		{
		localDataOrigPtr = fdata_ori;
		*localDataAfterPtr = (double)0.;
		for (k=0; k<no_new_channel; k++)
			{
			*localDataAfterPtr += (double)(*localDataOrigPtr * *localMatrixPtr);
			localMatrixPtr++;
			localDataOrigPtr++;
				
			}	// end "for (k=0; k<no_new_channel; k++)" 
						
		localDataAfterPtr += no_new_channel;
		fdata_ori += no_new_channel;
		localMatrixPtr -= no_new_channel;
			
		}	// end "for (j=0; j<no_sample; j++)" 
	
}	// end "FS_sub_linear_transform"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr GetDecisionBoundaryMemoryBlock
//
//	Software purpose:	The purpose of this routine is to get a block of
//							memory that will be shared by the routines that
//							'FS_decision_boundary' calls.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 07/02/1993
//	Revised By:			Larry L. Biehl			Date: 02/27/1999	

HPtr GetDecisionBoundaryMemoryBlock (
				struct class_info_str* 			class_info, 
				UInt32								numberClasses, 
				UInt32								numberFeatures)
				
{
	UInt32								bytesNeeded,
											i,
											j,
											matrixSize,
											maxBytesNeeded,
											maxNumberSamplesInTwoClasses,
											totalSamples;
											
											

			// Intialize local variables.														
			
	matrixSize = numberFeatures * numberFeatures * FS_DOUBLE;
	
			// Find the maximum number of total samples in two classes.				
	
	maxNumberSamplesInTwoClasses = 0;
	for (i=0; i<numberClasses; i++)
		{
		for (j=i+1; j<numberClasses; j++)
			{
			totalSamples = (UInt32)(class_info[i].no_sample + class_info[j].no_sample);
			maxNumberSamplesInTwoClasses = 
										MAX (maxNumberSamplesInTwoClasses, totalSamples);
										
			}	// end "for (j=i+1; j<numberClasses; j++)" 
			
		}	// end "for (i=0; i<numberClasses; i++)"

			// Determine size of memory block needed for 'Find_edfm_2_class'		
			// before the call to 'FS_sol_bnd_line'.										
			// point1_array vector.																
			
	bytesNeeded = sizeof (HDoublePtr*) * maxNumberSamplesInTwoClasses;
	
			// point2_array vector.																
			
	bytesNeeded += bytesNeeded;
	
			// classified_as vectors.															
			// Make certain that memory can start on 4-byte boundary for			
			// each of two sizeof (short) arrays.											
			
	bytesNeeded += sizeof (short) * maxNumberSamplesInTwoClasses + 4 + 4;
	
			// mah_dis_the_other vectors.														
			
	bytesNeeded += FS_FLOAT * maxNumberSamplesInTwoClasses;
	
			// array_index vectors.																
			
	bytesNeeded += sizeof (SInt32) * maxNumberSamplesInTwoClasses;
	
	maxBytesNeeded = bytesNeeded;
		
			// Determine size of memory block needed for 'FS_sol_bnd_line'.		
			// point1_array vector.																
			
	bytesNeeded = sizeof (HDoublePtr*) * maxNumberSamplesInTwoClasses;
	
			// point2_array vector.																
			
	bytesNeeded += bytesNeeded;
	
			// normal vector.																		
			
	bytesNeeded += 2 * numberFeatures * FS_DOUBLE;
	
			// icov_diff matrix.																	
			
	bytesNeeded += matrixSize;
	
			// mean_icov_diff matrix.															
			
	bytesNeeded += matrixSize;
	
	maxBytesNeeded = MAX (maxBytesNeeded, bytesNeeded);
	
	
	if (gFeatureExtractionSpecsPtr->optimizeClassFlag)	
		{
				// Determine size of memory block needed for 							
				// 'FS_optimize_2_class'.														
				// EV matrix.																		
			
		bytesNeeded = matrixSize;
		
				// eval matrix.																	
				
		bytesNeeded += matrixSize;
		
				// icov_all matrix (2 classes).												
				
		bytesNeeded += 2 * matrixSize;
		
				// cov matrix (2 classes).														
				
		bytesNeeded += 2 * matrixSize;
		
				// mean vector (2 classes).													
				
		bytesNeeded += 2 * numberFeatures * FS_DOUBLE;
		
				// transformed data values vector (2 classes).							
				
		bytesNeeded += maxNumberSamplesInTwoClasses * numberFeatures * FS_DOUBLE;
		
		maxBytesNeeded = MAX (maxBytesNeeded, bytesNeeded);
		
		}	// end "if (gFeatureExtractionSpecsPtr->optimizeClassFlag)" 
	
			// Get block of memory to satisfy the maximum block needed.				
			// Make certain the requested block is a multiple of the length of	
			// a double
	
	if (maxBytesNeeded > 0)
		{
		maxBytesNeeded = FS_DOUBLE * ((maxBytesNeeded + FS_DOUBLE - 1)/FS_DOUBLE);
		return ((HPtr)MNewPointer (maxBytesNeeded));
		
		}	// end "if (maxBytesNeeded > 0)" 
		
	else	// maxBytesNeeded <= 0 
		return (NULL);

}	// end "GetDecisionBoundaryMemoryBlock" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void normalize_rows
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
//	Coded By:			Chulhee Lee				Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 02/27/1999	

void normalize_rows (
				HDoublePtr 							eigenvectorPtr,
				UInt32 								numberComponents,
				UInt32 								numberFeatures)
						
{
	double								norm,
											temp;
	
	UInt32		 						i,
											j;
	
	
	for (i=0; i<numberComponents; i++)
		{
	 	norm = 0.;
	 	for (j=0; j<numberFeatures; j++)
	 		{
	 		temp = *eigenvectorPtr;
	  		norm += temp * temp;
	  		eigenvectorPtr++;
	  		
	  		}	// end "for (j=0; j<numberFeatures; j++)"
	  	
	  	if (norm > 0)
	 		norm = sqrt (norm);
	 	
	 	else	// norm <= 0
	 		norm = 1.;
	  
	  	norm = 1./norm;
	  	eigenvectorPtr -= numberFeatures;
	 	for (j=0; j<numberFeatures; j++)
	 		{
	 		*eigenvectorPtr *= norm;
	 		eigenvectorPtr++;
	 		
	 		}	// end "for (j=0; j<numberFeatures; j++)"
	 	
		}	// end "for (i=0; i<numberComponents; i++)"
	
}	// end "normalize_rows"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void NWFE
//
//	Software purpose:	
//
//	Parameters in:		no_feature (long):	 			number of features (=N)
//							no_class (long):	 				number of classes
//							class_info (struct): 			class informations
//
//	Parameters out:	eigenvectors (double, N by N)	eigenvectors
//							eigenvectors (double, N)		eigenvalues
//							ERROR_FLAG (*long)				error flag. must be zero.
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Bor-Chen Kuo			Date: 07/24/2001
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

SInt32 NWFE (
				struct class_info_str* 			class_info, 
				UInt32								numberClasses, 
				UInt32								numberChannels,
				HDoublePtr 							eigen_vectors, 
				HDoublePtr 							eigen_values)
				
{							
	double								det,
											factor,
											log_det,
											sumInvDistXLocalMean,
											scatterMatrixWeight,
											totalWeights,
											weight;
								
	HDoublePtr 							localMeanWeightPtr,
											sb_nwfe_ptr,
											sw_nwfe_ptr,
			 								tempMatrixPtr;
			 								
	HDoublePtr							data_i_ptr,
											localMeanPtr,
											savedLocalMeanPtr;

	SInt64								numberSamplesClass_i;
								
	SInt16								*classPairWeightPtr,
											*classPtr;
								
	SInt32								classPairCount,
											numberClassPairs,
											returnCode;
											
	UInt32								classPairIndex,
											i,
											j,
											k,
											maxNumberSamplesInOneClass,
											numberBytes,
											statClassNumber;
								
	SInt16								classPairWeight,
											weightsIndex;
								
	Boolean								continueFlag,
											countOKFlag,
											determinantOKFlag,
											withInClassFlag;
	
	
	returnCode = noErr;
	
	sb_nwfe_ptr = NULL;
	sw_nwfe_ptr = NULL;
	tempMatrixPtr = NULL;
	localMeanWeightPtr = NULL;
	localMeanPtr = NULL;
	
	gInverseMatrixMemory.inversePtr = NULL;
	gInverseMatrixMemory.pivotPtr = NULL;
	gInverseMatrixMemory.indexRowPtr = NULL;
	gInverseMatrixMemory.indexColPtr = NULL;
	gInverseMatrixMemory.ipvotPtr = NULL;
	
	classPtr = (SInt16*)GetHandlePointer (gFeatureExtractionSpecsPtr->classHandle);
	
	continueFlag = TRUE;

			// Assign memory. 
			
	numberBytes = numberChannels*numberChannels * FS_DOUBLE;
	
	sb_nwfe_ptr = (HDoublePtr)MNewPointer (numberBytes);
	if (sb_nwfe_ptr == NULL)
		returnCode = 507;

	if (returnCode == 0)
		{
		sw_nwfe_ptr = (HDoublePtr)MNewPointer (numberBytes);
		if (sw_nwfe_ptr == NULL)
			returnCode = 508;
		 
		}	// end "if (returnCode == 0)" 

	if (returnCode == 0)
		{
				// Get memory for eigenvector computation.															
	
		if (!SetupMatrixInversionMemory ((UInt16)numberChannels,
														TRUE,
														&gInverseMatrixMemory.inversePtr, 
														&gInverseMatrixMemory.pivotPtr, 
														&gInverseMatrixMemory.indexRowPtr, 
														&gInverseMatrixMemory.indexColPtr, 
														&gInverseMatrixMemory.ipvotPtr))
			returnCode = 551;
		 
		}	// end "if (returnCode == 0)" 
		
			// Find the maximum number of samples in the classes.				
	
	maxNumberSamplesInOneClass = 0;
	for (i=0; i<numberClasses; i++)
		{
		maxNumberSamplesInOneClass = MAX (maxNumberSamplesInOneClass,
														(UInt32)class_info[i].no_sample);
		
		}	// end "for (i=0; i<numberClasses; i++)"

	if (returnCode == 0)
		{
				// Get storage for weights to compute the local mean for a point
				// in class_j
			
		numberBytes = maxNumberSamplesInOneClass * FS_DOUBLE;
		localMeanWeightPtr = (HDoublePtr)MNewPointer (numberBytes);
		
		if (localMeanWeightPtr == NULL)
			returnCode = 509;
		 
		}	// end "if (returnCode == 0)" 

	if (returnCode == 0)
		{
				// Get storage for local means for all points in class_i
			
		numberBytes = maxNumberSamplesInOneClass * numberChannels * FS_DOUBLE;
		localMeanPtr = (HDoublePtr)MNewPointer (numberBytes);
		
		if (localMeanPtr == NULL)
			returnCode = 509;
			
		savedLocalMeanPtr = localMeanPtr;
		 
		}	// end "if (returnCode == 0)" 
		
	if (returnCode == 0)
		{
		tempMatrixPtr = gInverseMatrixMemory.inversePtr;
		
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
			
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract74,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
										
		ShowStatusDialogItemSet (kStatusClassB);
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract76,
										gStatusDialogPtr, 
										IDC_Status4, 
										(Str255*)gTextString);
		
		ShowStatusDialogItemSet (kStatusField);
		LoadDItemStringNumber (kFeatureExtractStrID, 
										IDS_FeatureExtract24,
										gStatusDialogPtr, 
										IDC_Status7, 
										(Str255*)gTextString);

		numberClassPairs = (SInt32)numberClasses * numberClasses;
																	
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, numberClassPairs);
			
		DrawDialog (gStatusDialogPtr);
		classPairCount = 0;
	
				// Get the inter class weight list pointer.
				
		classPairWeightPtr = (SInt16*)GetHandlePointer (
															gProjectInfoPtr->classPairWeightsHandle);
		
				// Get the class weight list pointer.
				// Also get the sum of the class weights.
			
		weightsIndex = GetProjectClassWeightsIndex ();
			
		totalWeights = GetTotalProbability (
										(UInt16*)classPtr, (UInt32)numberClasses, weightsIndex);
			
				// Scale the total weights such that a weighting factor of one
				// will be used if each class is weighted equally.
				
		totalWeights /= numberClasses;
		
				// Initialize the matrices
				
		ZeroMatrix (sb_nwfe_ptr, numberChannels, numberChannels, kSquareInputMatrix);
		ZeroMatrix (sw_nwfe_ptr, numberChannels, numberChannels, kSquareInputMatrix);
		
		if (returnCode != 0)
			numberClasses = 0;

		if (numberClassPairs <= 0)
			numberClasses = 0;
		
		//gSampleMultipleCount = 0;
		for (i=0; i<numberClasses; i++)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)i+1);
					
			numberSamplesClass_i = class_info[i].no_sample;
			
			statClassNumber = classPtr[i] - 1;
			weight = GetClassWeightValue (
								(UInt16)statClassNumber, weightsIndex, totalWeights);
								
			if ((weight > 0) & (numberSamplesClass_i > 0))
				{
				for (j=0; j<numberClasses; j++)
					{
					LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)j+1);
			
					classPairCount++;
					LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)classPairCount);
					
					countOKFlag = (class_info[j].no_sample > 0);
					
					if (i == j)
						{
						withInClassFlag = TRUE;
						classPairWeight = 10;
						
						}	// end "if (i == j)"
						
					else	// if (i != j)
						{
						withInClassFlag = FALSE;
						
						if (i < j)
							classPairIndex = i*(numberClasses-1) - i*(i+1)/2 + j - 1;
							
						else	// (i > j)
							classPairIndex = j*(numberClasses-1) - j*(j+1)/2 + i - 1;
							
						classPairWeight = classPairWeightPtr[classPairIndex];
						//classPairWeight = 10;
						
						}	// end "else if (i != j)"
					
					if (classPairWeight > 0 && countOKFlag)
						{
						localMeanPtr = savedLocalMeanPtr;
						data_i_ptr = class_info[i].data_values;
			
								// Initialize the temp matrix
								
						ZeroMatrix (tempMatrixPtr, 
											numberChannels, 
											numberChannels, 
											kSquareInputMatrix);
							
						returnCode = NWFE_SumInvDistXLocalMean (
																		withInClassFlag,
																		(UInt32)numberSamplesClass_i,
																		(UInt32)class_info[j].no_sample,
																		numberChannels, 
																		class_info[i].data_values,
																		class_info[j].data_values,
																		&sumInvDistXLocalMean,
																		localMeanPtr,
																		localMeanWeightPtr);	
						
						if (returnCode != noErr)
							break;					
													  						
						for (k=0; k<numberSamplesClass_i; k++)
							{
							/*
							returnCode = NWFE_GetLocalMean (withInClassFlag,
																		k,
																		numberSamplesClass_i,
																		class_info[j].no_sample,
																		numberChannels, 
																		data_i_ptr,
																		class_info[j].data_values,
																		localMeanPtr,
																		localMeanWeightPtr);
							
							if (returnCode != noErr)
								break;
							*/
							scatterMatrixWeight = NWFE_GetScatterMatrixWeight (
																			numberChannels, 
																			data_i_ptr,
																			sumInvDistXLocalMean,
																			localMeanPtr);
																			
									// Update scatter matrix for class i.
									
							NWFE_AddToScatterMatrixForClass_i (tempMatrixPtr,
																			localMeanPtr,
																			scatterMatrixWeight,
																			data_i_ptr,
																			numberChannels);
																			
							data_i_ptr += numberChannels;
							localMeanPtr += numberChannels;
								
							}	// end "for (k=0; k<numberSamplesClass_i; k++)"
						
						if (returnCode != noErr)
							break;
						
								// Now add to the appropriate scatter matrix.
							
						if (i == j)
							{	
							factor = weight;
							
							AddBxSymMatrixToSymMatrix (tempMatrixPtr,
																	sw_nwfe_ptr,
																	numberChannels,
																	factor);	
																	
							}	// end "if (i == j)"
							
						else	// i != j
							{
							//factor = weight/(numberClasses-1);
							factor = weight;
							
							AddBxSymMatrixToSymMatrix (tempMatrixPtr,
																	sb_nwfe_ptr,
																	numberChannels,
																	factor);
																	
							}	// end "else i != j"
							
						}	// end "if (classPairWeight > 0 && countOKFlag)"
			  			
			 		}	// end "for (j=0; j<numberClasses; j++)" 
						
				if (returnCode != noErr)
					break;
			 		
			 	}	// end "if (weight > 0)"
		 		
			if (returnCode != noErr)
				break;
		  			
		 	}	// end "for (i=0; i<numberClasses; i++)" 
		/*
		sprintf ((char*)gTextString,  
					"%s    Number sample pairs multiplied was %ld.%s",
					gEndOfLine,
					gSampleMultipleCount, 
					gEndOfLine);
		continueFlag = OutputString (NULL, 
												(char*)gTextString, 
												0, 
												gOutputCode, 
												TRUE);
		*/
		if (returnCode == noErr)
			{ 	
			 		// Regularize the sw matrix
			 		
			RegularizeSymMatrix (sw_nwfe_ptr, numberChannels);
										
					// Invert the sw matrix.
		 				
			determinantOKFlag = InvertSymmetricMatrix (sw_nwfe_ptr,
																		(UInt16)numberChannels, 
																		gInverseMatrixMemory.pivotPtr, 
																		gInverseMatrixMemory.indexRowPtr, 
																		gInverseMatrixMemory.indexColPtr, 
																		gInverseMatrixMemory.ipvotPtr, 
																		&det,
																		&log_det,
																		kReturnMatrixInverse);
				
			if (gOperationCanceledFlag)
				returnCode = 591;

			if (returnCode == noErr && !determinantOKFlag)
				returnCode = 601;
				
			}	// end "if (returnCode == noErr)"
									
		if (returnCode == noErr)
			MatrixMultiply (sw_nwfe_ptr, 
									sb_nwfe_ptr, 
									tempMatrixPtr,
									(UInt16)numberChannels, 
									(UInt16)numberChannels, 
									(UInt16)numberChannels, 
									1);
				
				// Exit routine if user has "command period" down			
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				if (returnCode == noErr)
					returnCode = 591;
				
				}	// end "if (!CheckSomeEvents (osMask..." 
				
			}	// end "if (TickCount () >= gNextTime)" 
		
		if (returnCode == 0)
			{
			HideStatusDialogItemSet (kStatusClassB);
			HideStatusDialogItemSet (kStatusField);
			LoadDItemStringNumber (kFeatureExtractStrID, 
											IDS_FeatureExtract26,
											gStatusDialogPtr, 
											IDC_Status21, 
											(Str255*)gTextString);
			/*
			returnCode = ComputeEigenvectorsSVD (
										tempMatrixPtr, 
										numberChannels, 
										numberChannels, 
										eigen_values, 
										eigen_vectors,
										gInverseMatrixMemory.pivotPtr,
										(double**)gInverseMatrixMemory.indexRowPtr,
										(double**)gInverseMatrixMemory.indexColPtr);
			*/
			returnCode = MatlabEigFunction (tempMatrixPtr,
														eigen_vectors,
														eigen_values,
														numberChannels);
	
			}	// end "if (returnCode == 0)" 
		 
		}	// end "if (returnCode == 0)" 

			// Free memory																	 
			
	sb_nwfe_ptr = CheckAndDisposePtr (sb_nwfe_ptr);
	sw_nwfe_ptr = CheckAndDisposePtr (sw_nwfe_ptr);
	localMeanWeightPtr = CheckAndDisposePtr (localMeanWeightPtr);
	localMeanPtr = CheckAndDisposePtr (savedLocalMeanPtr);
	
	ReleaseMatrixInversionMemory ();
	
	return (returnCode);
	
}	// end "NWFE" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 AddBxSymMatrixToSymMatrix
//
//	Software purpose:	The purpose of this routine is to add a factor times a symmetric
//							matrix to a symmetrix matrix.
//
//	Parameters in:			
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/07/2002
//	Revised By:			Larry L. Biehl			Date: 02/07/2002	

void AddBxSymMatrixToSymMatrix (
				HDoublePtr							inputMatrixPtr,
				HDoublePtr							outputMatrixPtr,
				UInt32								matrixSize,
				double								factor)	
				
{
   HDoublePtr							savedOutputMatrixPtr;
								
	UInt32								i,
											j,
											lowerLeftIndexSkip;
   							
	
	savedOutputMatrixPtr = outputMatrixPtr;
			
	lowerLeftIndexSkip = matrixSize - 1;
   
   for (i=0; i<matrixSize; i++)
   	{
      for (j=0; j<=i; j++)
      	{
	   	*outputMatrixPtr += factor * *inputMatrixPtr;
	   	inputMatrixPtr++;
	    	outputMatrixPtr++;
			
      	}	// end "for (j=0; j<=i; j++)"
			
		inputMatrixPtr += lowerLeftIndexSkip;
		outputMatrixPtr += lowerLeftIndexSkip;
		lowerLeftIndexSkip--;
      
   	}	// end "for (i=0; i<matrixSize; i++)"
	
			// Now copy the lower left part of the matrix to the upper right part.							
	
	outputMatrixPtr = savedOutputMatrixPtr;	
	CopyLowerToUpperSquareMatrix ((UInt16)matrixSize, outputMatrixPtr);
	
}	// end "AddBxSymMatrixToSymMatrix" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RegularizeSymMatrix
//
//	Software purpose:	The purpose of this routine is to regularize a symmetric matrix.
//							Output = 0.5*Input + 0.5*diag (Input)
//
//	Parameters in:			
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/07/2002
//	Revised By:			Larry L. Biehl			Date: 02/07/2002	

void RegularizeSymMatrix (
				HDoublePtr							matrixPtr,
				UInt32								matrixSize)	
				
{
   HDoublePtr							savedMatrixPtr;
								
	UInt32								i,
											j,
											lowerLeftIndexSkip;
   							
	
	savedMatrixPtr = matrixPtr;
			
	lowerLeftIndexSkip = matrixSize;
   
   for (i=0; i<matrixSize; i++)
   	{
      for (j=0; j<i; j++)
      	{
	   	*matrixPtr *= 0.5;
	   	matrixPtr++;
			
      	}	// end "for (j=0; j<=i; j++)"
			
		matrixPtr += lowerLeftIndexSkip;
		lowerLeftIndexSkip--;
      
   	}	// end "for (i=0; i<matrixSize; i++)"
	
			// Now copy the lower left part of the matrix to the upper right part.							
	
	matrixPtr = savedMatrixPtr;	
	CopyLowerToUpperSquareMatrix ((UInt16)matrixSize, matrixPtr);
	
}	// end "RegularizeSymMatrix" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 NWFE_AddToScatterMatrixForClass_i
//
//	Software purpose:	The purpose of this routine is to add the scatter for the input
//							sample to the scatter matrix for the class. Note that the input
//							matrix is a square matrix. Only the lower triangular portion will
//							be computed for now. Since the matrix is symmetric, the upper
//							portion will be copied later.
//
//	Parameters in:			
//
//	Parameters out:	
//
// Value Returned:					
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/07/2002
//	Revised By:			Larry L. Biehl			Date: 02/10/2002	

void NWFE_AddToScatterMatrixForClass_i (
				HDoublePtr							inputMatrixPtr,
				HDoublePtr							localMeanPtr,
				double								scatterMatrixWeight,
				HDoublePtr							data_i_ptr,
				UInt32								numberChannels)
				
{
	double								distance1,
											distance2;
	
	UInt32								i,
											j,
											lowerLeftIndexSkip;
   							
	
	lowerLeftIndexSkip = numberChannels - 1;
   
   for (i=0; i<numberChannels; i++)
   	{
      distance1 = data_i_ptr[i] - localMeanPtr[i];
	   	
      for (j=0; j<i; j++)
      	{
      	distance2 = (double)data_i_ptr[j] - (double)localMeanPtr[j];
	   	*inputMatrixPtr += scatterMatrixWeight * distance1 * distance2;
	   	inputMatrixPtr++;
			
      	}	// end "for (j=0; j<=i; j++)"
      	
	   *inputMatrixPtr += scatterMatrixWeight * distance1 * distance1;
	   inputMatrixPtr++;
			
		inputMatrixPtr += lowerLeftIndexSkip;
		lowerLeftIndexSkip--;
      
   	}	// end "for (i=0; i<numberChannels; i++)"
	
}	// end "NWFE_AddToScatterMatrixForClass_i" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 NWFE_GetLocalMean
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
//	Coded By:			Bor-Chen Kuo			Date: 07/24/2001
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

SInt32 NWFE_GetLocalMean (
				Boolean								withInClassFlag, 
				UInt32								sample_i, 
				UInt32								numberSamplesClass_i,
				UInt32								numberSamplesClass_j,
				UInt32								numberChannels, 
				HDoublePtr							data_i_ptr,
				HDoublePtr							data_j_ptr, 
				HDoublePtr 							localMeanPtr,
				HDoublePtr							localMeanWeightPtr)				
									
{
	double									difference,
												distance,
												smallValue,
												sumInvDistance,
												w;
												
	HDoublePtr								localDataJPtr;
												
	SInt32									returnCode = noErr;
												
	UInt32									distanceIndex,
												k2,
												l;
			
	
	smallValue = 10.0;
	
			// Get inverse of pixel distances.
			
	distanceIndex = 0;	
	sumInvDistance = 0.;
	localDataJPtr = data_j_ptr;
	for (k2=0; k2<numberSamplesClass_j; k2++)
		{
		if (!withInClassFlag || sample_i != k2)
			{
			//gSampleMultipleCount++;
			distance = 0.;
			
			for (l=0; l<numberChannels; l++)	
				{
				difference = (double)*data_i_ptr - (double)*localDataJPtr;
				distance += difference * difference;
				data_i_ptr++;
				localDataJPtr++;
			      
				}	// end "for (l=0; l<numberChannels; l++)"
				
			if (distance == 0)
				distance = smallValue;
				
			else	// distance != 0
				distance = 1. / sqrt (distance);
				
			localMeanWeightPtr[k2] = distance; 
			sumInvDistance += distance;
						
			data_i_ptr -= numberChannels;
				
			}	// end "if (!withInClassFlag || sample_i != k2)"
				
		else	// withInClassFlag && sample_i == k2
			{
			localMeanWeightPtr[k2] = 0.; 
			localDataJPtr += numberChannels;
			
			}	// end "else withInClassFlag && sample_i == k2"
	
				// Exit routine if user has "command period" down		
	
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = 591;
				break;
				
				}	// end "if (!CheckSomeEvents (..." 
					
			}	// end "if (TickCount () >= gNextTime)"
	
		}	// end "for (k2=0; k2<numberSamplesClass_j; k2++)"
	
	if (returnCode == noErr)
		{	
				// Get the local mean of data value Xk of class i in class j 
		
		for (l=0; l<numberChannels; l++)	
			localMeanPtr[l] = 0.;
			
		sumInvDistance = 1. / sumInvDistance;
		
		localDataJPtr = data_j_ptr;
		for (k2=0; k2<numberSamplesClass_j; k2++)
			{
					// Get weight for local means.
					
			w = localMeanWeightPtr[k2] * sumInvDistance;
			
			for (l=0; l<numberChannels; l++)	
				{
				localMeanPtr[l] += (double)(w * *localDataJPtr);
				localDataJPtr++;
			      
				}	// end "for (l=0; l<numberChannels; l++)"
		
			}	// end "for (k2=0; k2<numberSamplesClass_j; k2++)"
			
		}	// end "if (returnCode == noErr)"

	return (returnCode);
	
}	// end "NWFE_GetLocalMean" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 NWFE_GetScatterMatrixWeight
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
//	Coded By:			Bor-Chen Kuo			Date: 07/24/2001
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

double NWFE_GetScatterMatrixWeight (
				UInt32								numberChannels, 
				HDoublePtr							data_i_ptr,
				double								sumInvDistXLocalMean, 
				HDoublePtr 							localMeanPtr)		
									
{
	double									difference,
												distance,
												scatterMatrixWeight,
												smallValue;
												
	UInt32									l;
			
	
	smallValue = 10.;

	distance = 0;
	for (l=0; l<numberChannels; l++)	
		{
		difference = (double)*data_i_ptr - (double)*localMeanPtr;
		distance += difference * difference;
		data_i_ptr++;
		localMeanPtr++;
	      
		}	// end "for (l=0; l<numberChannels; l++)"
			
				
	if (distance == 0)
		distance = smallValue;
		
	else	// distance != 0
		distance = 1. / sqrt (distance);
		
			// Get scatter matrix weight.
			
	scatterMatrixWeight = distance * sumInvDistXLocalMean;
	
	return (scatterMatrixWeight);
	
}	// end "NWFE_GetScatterMatrixWeight" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 NWFE_SumInvDistXLocalMean
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
//	Coded By:			Bor-Chen Kuo			Date: 07/24/2001
//	Revised By:			Larry L. Biehl			Date: 06/17/2006	

SInt32 NWFE_SumInvDistXLocalMean (
				Boolean								withInClassFlag, 
				UInt32								numberSamplesClass_i,
				UInt32								numberSamplesClass_j,
				UInt32								numberChannels, 
				HDoublePtr							data_i_ptr,
				HDoublePtr							data_j_ptr,
				HDoublePtr 							sumInvDistXLocalMeanPtr,
				HDoublePtr							localMeanPtr,
				HDoublePtr							localMeanWeightPtr)		
									
{
	double									difference,
												distance,
												smallValue;
												
	SInt32									returnCode = noErr;
												
	UInt32									k,
												l;
			
	
	smallValue = 10.;
	
			// Get the distance between each point
									
	*sumInvDistXLocalMeanPtr = 0;
												  						
	for (k=0; k<numberSamplesClass_i; k++)
		{
		returnCode = NWFE_GetLocalMean (withInClassFlag,
													k,
													numberSamplesClass_i,
													numberSamplesClass_j,
													numberChannels, 
													data_i_ptr,
													data_j_ptr, 
						 							localMeanPtr,
													localMeanWeightPtr);		
													
		if (returnCode != noErr)
			break;
		
				// Get inverse of pixel and local mean distance.
		
		distance = 0;	
		for (l=0; l<numberChannels; l++)	
			{
			difference = (double)*data_i_ptr - (double)localMeanPtr[l];
			distance += difference * difference;
			data_i_ptr++;
		      
			}	// end "for (l=0; l<numberChannels; l++)"
				
				
		if (distance == 0)
			distance = smallValue;
			
		else	// distance != 0
			distance = 1. / sqrt (distance);
			
		*sumInvDistXLocalMeanPtr += distance;
		
		localMeanPtr += numberChannels;
		
				// Exit routine if user has "command period" down		
	
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = 591;
				break;
				
				}	// end "if (!CheckSomeEvents (..." 
					
			}	// end "if (TickCount () >= gNextTime)"
			
		}	// end "for (k=0; k<numberSamplesClass_i; k++)"
		
	*sumInvDistXLocalMeanPtr = 1. / *sumInvDistXLocalMeanPtr;

	return (returnCode);
	
}	// end "NWFE_SumInvDistXLocalMean" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void TransformLineOfCovariance
//
//	Software purpose:	The purpose of this routine is to transform a line
//							of the covariance matrix using the input
//							eigenvector matrix and work vector.
//
//	Parameters in:					.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/07/1993
//	Revised By:			Larry L. Biehl			Date: 02/27/1999	

void TransformLineOfCovariance (
				HDoublePtr 							eigenvectorPtr, 
				HDoublePtr 							meanPtr, 
				HDoublePtr 							covariancePtr, 
				HDoublePtr 							transformedMeanPtr, 
				HDoublePtr 							transformedCovPtr, 
				HDoublePtr 							workVectorPtr, 
				UInt32								matrixDimension, 
				UInt32								featureNumber)

{
   HDoublePtr							lEigenvectorPtr,
   										lInputVectorPtr,
   										resultVectorPtr,
											savedCovariancePtr;
	
	UInt32								covarianceOffset1,
											covarianceOffset2,
   										featureNumberLessOne,
   										i,
   										j;
											
   
   		// Initialize local variables.												
   		
   featureNumberLessOne = featureNumber - 1;
   		
   		// Transform the request feature for the mean vector.					
   		
	lEigenvectorPtr = &eigenvectorPtr[featureNumberLessOne*matrixDimension];
   resultVectorPtr = transformedMeanPtr + featureNumberLessOne;
   lInputVectorPtr = meanPtr;
   
   *resultVectorPtr = 0.;
   for (i=0; i<matrixDimension; i++)
   	{
   	*resultVectorPtr += *lEigenvectorPtr * *lInputVectorPtr;
   	lEigenvectorPtr++;
    	lInputVectorPtr++;
      
   	}	// end "for (i=0; i<matrixDimension; i++)"
   	
   		
   resultVectorPtr = workVectorPtr;
   covarianceOffset1 = 0;
   savedCovariancePtr = covariancePtr;
   
   for (i=0; i<matrixDimension; i++)
   	{
		lEigenvectorPtr = &eigenvectorPtr[featureNumberLessOne*matrixDimension];
		
		savedCovariancePtr += covarianceOffset1;
		lInputVectorPtr = savedCovariancePtr;
		
		*resultVectorPtr = 0.;
      for (j=0; j<=i; j++)
      	{
	   	*resultVectorPtr += *lEigenvectorPtr * *lInputVectorPtr;
	   	lEigenvectorPtr++;
	    	lInputVectorPtr++;
	    		
			}	// end "for (j=0; j<matrixDimension; j++)"
			
		lInputVectorPtr += i;
 		covarianceOffset2 = i + 2;
      for (j=i+1; j<matrixDimension; j++)
      	{
	   	*resultVectorPtr += *lEigenvectorPtr * *lInputVectorPtr;
	   	lEigenvectorPtr++;
	    	lInputVectorPtr += covarianceOffset2;
	    	covarianceOffset2++;
	    		
			}	// end "for (j=0; j<matrixDimension; j++)"
				
		resultVectorPtr++;
		covarianceOffset1++;
      
   	}	// end "for (i=0; i<matrixDimension; i++)"
   
   resultVectorPtr = &transformedCovPtr[featureNumberLessOne*matrixDimension];
   transformedCovPtr += featureNumberLessOne;
   
	lEigenvectorPtr = eigenvectorPtr;
   for (i=0; i<featureNumber; i++)
   	{
   	lInputVectorPtr = workVectorPtr;
   	
   	*resultVectorPtr = 0.;
      for (j=0; j<matrixDimension; j++)
      	{
	   	*resultVectorPtr += *lInputVectorPtr * *lEigenvectorPtr;
	   	lEigenvectorPtr++;
	    	lInputVectorPtr++;
	    		
			}	// end "for (j=0; j<matrixDimension; j++)"
			
		if (i != featureNumberLessOne)
			{
			*transformedCovPtr = *resultVectorPtr;
			transformedCovPtr += matrixDimension;
			
			}	// end "if (i != featureNumberLessOne)" 
			
		resultVectorPtr++;
      
   	}	// end "for (i=0; i<featureNumber; i++)"
		
}	// end "TransformLineOfCovariance" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void BalanceMatrix
//
//	Software purpose:	Given a matrix a[1..n][1..n], this routine replaces it 
//							by a balanced matrix with identical eigenvalues. A 
//							symmetric matrix is already balanced and is unaffected 
//							by this procedure. The parameter RADIX should be the 
//							machine's floating-point radix.
//
//	Parameters in:					.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Revised By:			Larry L. Biehl			Date: 02/15/2002	

void BalanceMatrix (
				UInt32								nm,
				UInt32								n,
				double**								a,
				UInt32*								lowPtr,
				UInt32*								ighPtr, 
				double*								scalePtr,
				double*								inputMatrixPtr)
				

{
	UInt32 								i,
											iexc,
											j,
											jj,
											k,
											l,
											last,
											m;
								
	double 								s,
											r,
											g,
											f,
											c,
											RADIX = 2.,
											sqrdx;
							
	
			// Set pointers the rows for a. Allow for indexing from
			// 1 not 0.	
	
	scalePtr--;																		
	a[1] = inputMatrixPtr - 1;
	
	for (i=2; i<=n; i++) 
		a[i] = a[i-1] + n;		
							
	sqrdx = RADIX*RADIX;
	
	k = 1;
	l = n;
	goto L100;

			// in-line procedure for row and column exchange
			
L20: 
   	scalePtr[m] = j;
      if (j == m) 
      	goto L50;

		for (i=1; i<=l; i++)
			{
         f = a[i][j];
         a[i][j] = a[i][m];
         a[i][m] = f;
         
         }	// end "for (i=1; i<=l; i++)" 30

      for (i=k; i<=n; i++)
      	{
         f = a[j][i];
         a[j][i] = a[m][i];
         a[m][i] = f;
         
			}	// end "for (i=k; i<=n; i++)"  40 continue

L50:
		switch (iexc)
			{
			case 1: goto L80;
			case 2: goto L130;
			
			}	// end "switch (iexc)"
			
				// search for rows isolating an eigenvalue and push them down
				
L80:
		if (l == 1) 
			goto L280;
			
      l--;
      
				// for j=l step -1 until 1 do -- 
				
L100:
		for (jj=1; jj<=l; jj++)
			{
         j = l + 1 - jj;

         for (i=1; i<=l; i++)
         	{
            if (i == j) 
            	goto L110;
            	
            if (a[j][i] != 0) 
            	goto L120;
            	
L110:
				;
				}	// end "for (i=1; i<=l; i++)"

         m = l;
         iexc = 1;
         goto L20;
         
L120:
			;
			}	// end "for (jj=1; jj<=l; jj++)"
			
      goto L140;

				// search for columns isolating an eigenvalue and push them left
				
L130:
		k++;

L140:
		for (j=k; j<=l; j++)
			{
         for (i=k; i<=l; i++)
         	{
            if (i == j) 
            	goto L150;
            	
            if (a[i][j] != 0) 
            	goto L170;
            	
L150:
				;
				}	// end "for (i=k; i<=l; i++)"	150    continue

         m = k;
         iexc = 2;
         goto L20;
         
L170:
			;
         
         }	// end "for (j=k; j<=l; j++)"	 170 continue
	
			// Now balance the submatrix in rows k to l
	
	for (i=k; i<=l; i++)
		scalePtr[i] = 1;
	
			// Interative loop for norm reduction.
				
	last = 0;
	while (last == 0) 
		{
		last = 1;
		for (i=k; i<=l; i++) 
			{ 
					// Calculate row and column norms.
					
			r = c = 0.0;
			for (j=k; j<=l; j++)
				if (j != i) 
					{
					c += fabs (a[j][i]);
					r += fabs (a[i][j]);
					
					}	// end "if (j != i)"
					
			if (c && r) 
				{ 
						// If both are nonzero,
						
				g = r/RADIX;
				f = 1.0;
				s = c + r;
				while (c < g) 
					{ 
							// find the integer power of the machine radix that
							// comes closest to balancing the matrix. 
					
					f *= RADIX;
					c *= sqrdx;
					
					}	// end "while (c<g)"
					
				g = r*RADIX;
				while (c >= g) 
					{
					f /= RADIX;
					c /= sqrdx;
					
					}	// end "while (c >= g)"
	
				if ((c+r)/f < 0.95*s) 
					{
					last = 0;
					g = 1.0/f;
					scalePtr[i] *= f;
					
							// Apply similarity transformation.
							
					for (j=k; j<=n; j++) 
						a[i][j] *= g;  
						
					for (j=1; j<=l; j++) 
						a[j][i] *= f;
	
					}	// end "if ((c+r)/f < 0.95*s)"
	
				}	// end "if (c && r)"
				
			}	// end "for (i=1; i<=n; i++)"
			
		}	// end "for (i=1; i<=n; i++)"

L280:
		
	*lowPtr = k;
	*ighPtr = l;
	
}	// end "BalanceMatrix"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void balbak
//
//	Software purpose:	This subroutine forms the eigenvectors of a real general matrix 
//							by back transforming those of the corresponding balanced matrix 
//							determined by balanc.
//
//							this subroutine is a translation of the algol procedure balbak, 
//							num. math. 13, 293-304 (1969) by parlett and reinsch. handbook 
//							for auto. comp., vol.ii-linear algebra, 315-326 (1971).
//
//							questions and comments should be directed to burton s. garbow,
//							mathematics and computer science div, argonne national 
//							laboratory
//
//							this version dated august 1983.
//
//	Parameters in:		nm must be set to the row dimension of two-dimensional array 
//								parameters as declared in the calling program dimension 
//								statement.
//
//        				n is the order of the matrix.
//
//							low and igh are integers determined by balanc.
//
//							scale contains information determining the permutations and 
//								scaling factors used by balanc.
//
//							m is the number of columns of z to be back transformed.
//
//							z contains the real and imaginary parts of the eigenvectors to 
//								be back transformed in its first m columns.
//
//	Parameters out:	z contains the real and imaginary parts of the transformed 
//								eigenvectors in its first m columns.
//
// Value Returned:	None	
// 
// Called By:
//
//	Revised By:			Larry L. Biehl			Date: 02/15/2002	

void balbak (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh, 
				double*								scale,
				UInt32								m,
				double**								z,
				double*								zPtr)
				
{
	double								s;
      
	UInt32								i,
											j,
											k,
											ii;
							
	
			// Set pointers the rows for a. Allow for indexing from
			// 1 not 0.	
	
	scale--;																		
	z[1] = zPtr - 1;
	
	for (i=2; i<=n; i++) 
		z[i] = z[i-1] + n;		
      
   if (m != 0)
   	{
   	if (igh != low)
			{
   		for (i=low; i<=igh; i++)
      		{
      		s = scale[i];
				//s = 1/scale[i];

						// left hand eigenvectors are back transformed if the foregoing 
						// statement is replaced by s = 1.0d0/scale (i).

      		for (j=1; j<=m; j++)
					z[i][j] *= s;

				}	// end "for (i=low; i<=igh; i++)"  110 continue
				
			}	// end "if (igh != low)"
				
					// for i=low-1 step -1 until 1, igh+1 step 1 until n do --
						
		for (ii=1; ii<=n; ii++)
			{
         i = ii;
         
         if (i < low || i > igh)
         	{
	         if (i < low) 
	         	i = low - ii;
	         	
	         k = (UInt32)scale[i];
	         if (k != i)
	         	{
		         for (j=1; j<=m; j++)
		         	{
		            s = z[i][j];
		            z[i][j] = z[k][j];
		            z[k][j] = s;
		            
						}	// end "for (j=1; j<=m; j++)"  130 continue
						
					}	// end "if (k != i)"
					
				}	// end "if (i < low || i > igh)"

			}	// end "for (ii=1; ii<=n; ii++)"  140 continue

		}	// end "if (m != 0)"
		
	return;
	
}	// end "balbak"



#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void elmhes
//							ReduceToHessenbergFormEliminationMethod
//
//	Software purpose:	Reduction to Hessenberg form by the elimination method. 
//							The real, nonsymmetric matrix a[1..n][1..n] is replaced 
//							by an upper Hessenberg matrix with identical eigenvalues. 
//							Recommended, but not required, is that this routine be 
//							preceded by balance. On output, the Hessenberg matrix is 
//							in elements a[i][j] with i <= j+1. Elements with i > j+1 
//							are to 
//							be thought of as zero, but are returned with random values.
//
//							this subroutine is a translation of the algol procedure elmhes, 
//							num. math. 12, 349-368 (1968) by martin and wilkinson. handbook 
//							for auto. comp., vol.ii-linear algebra, 339-358 (1971).
//
//							given a real general matrix, this subroutine reduces a submatrix 
//							situated in rows and columns low through igh to upper hessenberg 
//							form by stabilized elementary similarity transformations.
//
//							questions and comments should be directed to burton s. garbow, 
//							mathematics and computer science div, argonne national laboratory
//
//							this version dated august 1983.
//
//	Parameters in:		nm must be set to the row dimension of two-dimensional array 
//								parameters as declared in the calling program dimension 
//								statement.
//
//							n is the order of the matrix.
//
//							low and igh are integers determined by the balancing subroutine  
//								balanc.  if  balanc  has not been used, set low=1, igh=n.
//
//					    	a (inputMatrixPtr) contains the input matrix.
//
//	Parameters out:	a contains the hessenberg matrix.  the multipliers which were 
//								used in the reduction are stored in the remaining triangle 
//								under the hessenberg matrix.
//
//							intPtr contains information on the rows and columns interchanged 
//								in the reduction. Only elements low through igh are used.
//
// Value Returned:	None	
// 
// Called By:
//
//	Revised By:			Larry L. Biehl			Date: 02/12/2002	

void elmhes (
				UInt32								nm, 
				UInt32								n, 
				UInt32								low, 
				UInt32								igh,
				double**								a,
				UInt32*								intPtr, 
				double*								inputMatrixPtr)
				
{
	UInt32								m,
											mm1,
											j,
											i;
											
	double 								y,
											x;
							
	
			// Set pointers the rows for a. Allow for indexing from
			// 1 not 0.	
	
	intPtr--;																		
	a[1] = inputMatrixPtr - 1;
	
	for (i=2; i<=n; i++) 
		a[i] = a[i-1] + n;	
		
			// Initialize intPtr vector	
						
	for (m=1; m<=n; m++) 
		intPtr[m] = 0;	
											
	for (m=2; m<n; m++) 
		{ 
				//  m is called r + 1in the text.
		
		mm1 = m - 1;		
		x = 0.0;
		i = m;
		for (j=m; j<=n; j++) 
			{ 
					// Find the pivot.
					
			if (fabs (a[j][mm1]) > fabs (x)) 
				{
				x = a[j][mm1];
				i = j;
				
				}	// end "if (fabs (a[j][m-1]) > fabs (x))"
				
			}	// end "for (j=m; j<=n; j++)"
			
		intPtr[m] = i;
			
		if (i != m) 
			{ 
					// Interchange rows and columns.
					
			for (j=mm1;j<=n;j++) 
				SWAP (a[i][j],a[m][j])
				
			for (j=1;j<=n;j++) 
				SWAP (a[j][i],a[j][m])
				
			}	// end "if (i != m)"
			
		if (x) 
			{ 
					// Carry out the elimination.
					
			for (i=m+1; i<=n; i++) 
				{
				if ((y=a[i][mm1]) != 0.0) 
					{
					y /= x;
					a[i][mm1] =  y;
					
					for (j=m;j<=n;j++)
						a[i][j] -= y*a[m][j];
						
					for (j=1;j<=n;j++)
						a[j][m] += y*a[j][i];
						
					}	// end "if ((y=a[i][m-1]) != 0.0)"
	
				}	// end "for (i=m+1; i<=n; i++)"
	
			}	// end "if (x)"
	
		}	// end "for (m=2; m<n; m++)"
	
}	// end "elmhes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void eltran
//
//	Software purpose:	this subroutine is a translation of the algol procedure elmtrans, 
//							num. math. 16, 181-204 (1970) by peters and wilkinson. handbook 
//							for auto. comp., vol.ii-linear algebra, 372-395 (1971).
//
//							this subroutine accumulates the stabilized elementary similarity 
//							transformations used in the reduction of a real general matrix 
//							to upper hessenberg form by elmhes.
//
//							questions and comments should be directed to burton s. garbow 
//							mathematics and computer science div, argonne national laboratory.
//
//					  		this version dated august 1983.
//
//	Parameters in:		nm must be set to the row dimension of two-dimensional array 
//								parameters as declared in the calling program dimension 
//								statement.
//
//							n is the order of the matrix.
//
//							low and igh are integers determined by the balancing subroutine  
//								balanc. If  balanc  has not been used, set low=1, igh=n.
//
//							a contains the multipliers which were used in the reduction by 
//								elmhes in its lower triangle below the subdiagonal.
//
//							intPtr contains information on the rows and columns interchanged  
//								in the reduction by  elmhes. only elements low through igh are 
//								used.
//
//	Parameters out:	z contains the transformation matrix produced in the reduction 
//								by  elmhes.
//
// Value Returned:	None	
// 
// Called By:
//
//	Translated from Fortran to C By:			
//							Larry L. Biehl			Date: 02/12/2002
//	Revised By:			Larry L. Biehl			Date: 02/12/2002	

void eltran (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh, 
				double**								a,
				UInt32*								intPtr,
				double**								z,
				double*								inputMatrixPtr,
				double*								outputMatrixPtr)

{
 	UInt32								i,
 											j,
 											kl,
 											mm,
 											mp,
 											mp1;
									

			// Initialize z (outputMatrixPtr) to identity matrix.
			
	SetIdentityMatrix (outputMatrixPtr, 
								n,
								kSquareOutputMatrix);
	
			// Set pointers to the rows for a. Allow for indexing from
			// 1 not 0.	
	
	intPtr--;																		
	a[1] = inputMatrixPtr - 1;												
	z[1] = outputMatrixPtr - 1;
	
	for (i=2; i<=n; i++)
		{ 
		a[i] = a[i-1] + n;
		z[i] = z[i-1] + n;
		
		}	// end "for (i=2; i<=n; i++)"	

	kl = igh - low - 1;
	if (kl > 0)
		{
		for (mm=1; mm<=kl; mm++)
			{
			mp = igh - mm;
			mp1 = mp + 1;
			
			for (i=mp1; i<=igh; i++)
				z[i][mp] = a[i][mp-1];         
				
			i = intPtr[mp];
			if (i != mp)
				{
	         for (j=mp; j<=igh; j++)
	         	{
	            z[mp][j] = z[i][j];
	            z[i][j] = 0;
	            
	  				}	// end "for (j=mp; j<=igh; j++)"
	  				
				z[i][mp] = 1;
	  			
	  			}	// end "if (i != mp)"
	  			
	  		}	// end "for (mm=1; mm=<kl; mm++)"
	  		
	  	}	// end "if (kl > 0)"

}	// end "eltran"



static double c_b50 = 0.;
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void hqr2
//
//	Software purpose:	this subroutine is a translation of the algol procedure hqr2,
//							num. math. 16, 181-204 (1970) by peters and wilkinson. handbook 
//							for auto. comp., vol.ii-linear algebra, 372-395 (1971).
//
//							this subroutine finds the eigenvalues and eigenvectors of a real 
//							upper hessenberg matrix by the qr method. The eigenvectors of 
//							a real general matrix can also be found if  elmhes  and  eltran  
//							or  orthes  and  ortran  have been used to reduce this general 
//							matrix to hessenberg form and to accumulate the similarity 
//							transformations.
//
//							calls cdiv for complex division.
//
//							questions and comments should be directed to burton s. garbow 
//							mathematics and computer science div, argonne national laboratory.
//
//					  		this version dated august 1983.
//
//	Parameters in:		nm must be set to the row dimension of two-dimensional array 
//								parameters as declared in the calling program dimension 
//								statement.
//
//					    	n is the order of the matrix.
//
//							low and igh are integers determined by the balancing subroutine  
//								balanc. If balanc  has not been used, set low=1, igh=n.
//
//				     		h contains the upper hessenberg matrix.
//
//							z contains the transformation matrix produced by  eltran after 
//								the reduction by  elmhes, or by  ortran  after the reduction 
//								by  orthes, if performed. If the eigenvectors of the 
//								hessenberg matrix are desired, z must contain the identity 
//								matrix.
//
//	Parameters out: 	h has been destroyed.
//
//							wr and wi contain the real and imaginary parts, respectively, 
//								of the eigenvalues.  the eigenvalues are unordered except 
//								that complex conjugate pairs of values appear consecutively 
//								with the eigenvalue having the positive imaginary part first.  if an
//					      	error exit is made, the eigenvalues should be correct for 
//								indices ierr+1,...,n.
//
//							z contains the real and imaginary parts of the eigenvectors.
//								if the i-th eigenvalue is real, the i-th column of z contains 
//								its eigenvector.  if the i-th eigenvalue is complex with 
//								positive imaginary part, the i-th and (i+1)-th columns of 
//								z contain the real and imaginary parts of its eigenvector. 
//								The eigenvectors are unnormalized. If an error exit is made, 
//								none of the eigenvectors has been found.
//
//							ierr is set to 
//								zero for normal return, 
//								j if the limit of 30*n iterations is exhausted while the 
//									j-th eigenvalue is being sought.
//
// Value Returned:	None	
// 
// Called By:
//
//	Translated from Fortran to C By:			
//							Larry L. Biehl			Date: 02/12/2002
//	Revised By:			Larry L. Biehl			Date: 02/15/2002	

SInt32 hqr2(
				SInt32								nm, 
				SInt32								n, 
				SInt32								low, 
				SInt32								igh, 
				double**								h, 
				double*								wr, 
				double*								wi, 
				double**								z,
				double*								hMatrixPtr,
				double*								zMatrixPtr)

{
	double 								d__1, 
											d__2;

	double 								norm,
											p, 
											q, 
											r,
											ra,
											s,
											sa, 
											t,
											tst1, 
											tst2,
											vi,
											vr, 
											w, 
											x, 
											y,
											zz;
	
	SInt32 								en,
											enm2,
											i, 
											ii,
											ierr,
											itn,
											its,
											j, 
											jj,
											k, 
											l,
											ll, 
											m,
											mm,
											mp2,
											na,
											nn;
										
	Boolean 								notlas;
    

			// Set pointers to the rows for a. Allow for indexing from
			// 1 not 0.	
	
	--wi;
	--wr;
																			
	h[1] = hMatrixPtr - 1;												
	z[1] = zMatrixPtr - 1;
	
	for (i=2; i<=n; i++)
		{ 
		h[i] = h[i-1] + n;
		z[i] = z[i-1] + n;
		
		}	// end "for (i=2; i<=n; i++)"

			// Function Body
			
	ierr = 0;
	norm = 0.;
	k = 1;
	
			// Store roots isolated by balance and compute matrix norm ..........
			
	for (i=1; i<=n; i++) 
    	{
		for (j=k; j<=n; j++) 
	    	norm += fabs (h[i][j]);

		k = i;
		if (i<low || i>igh) 
			{
			wr[i] = h[i][i];
			wi[i] = 0.;
			
			}	// end "if (i<low || i>igh)"
		
   	}	// end "for (i=1; i<=n; ++i)"

	en = igh;
	t = 0.;
	itn = n * 30;
    
			// search for next eigenvalues ..........
			
L60:
	if (en < low)
		goto L340;
    
	its = 0;
	na = en - 1;
	enm2 = na - 1;
	
			// look for single small sub-diagonal element
			// for l=en step -1 until low do -- ..........
L70:
	for (ll=low; ll<=en; ll++) 
		{
		l = en + low - ll;
		if (l == low)
			goto L100;
		
		s = fabs (h[l-1][l-1]) + fabs (h[l][l]);
		if (s == 0.)
			s = norm;
		    
		tst1 = s;
		tst2 = tst1 + fabs (h[l][l-1]);
		if (tst2 == tst1)
			goto L100;
		    
		}	// end "for (ll=low; ll<=en; ++ll)"	L80:
    
			// Form shift ..........
			
L100:
	x = h[en][en];
	if (l == en)
		goto L270;
	
	y = h[na][na];
	w = h[en][na] * h[na][en];
	if (l == na) 
		goto L280;
	
	if (itn == 0)
		goto L1000;
	
	if (its != 10 && its != 20)
		goto L130;
    
			// form exceptional shift
			
	t += x;

	for (i=low; i<=en; i++) 
		h[i][i] -= x;		// L120:

	s = fabs (h[en][na]) + fabs (h[na][enm2]);
	x = s * .75;
	y = x;
	w = s * -.4375 * s;
    
L130:
	its++;
	itn--;
			
			// look for two consecutive small
			// sub-diagonal elements.
			// for m=en-2 step -1 until l do -- 

	for (mm=l; mm<=enm2; mm++) 
		{
		m = enm2 + l - mm;
		zz = h[m][m];
		r = x - zz;
		s = y - zz;
		p = (r * s - w) / h[m+1][m] + h[m][m+1];
		q = h[m+1][m+1] - zz - r - s;
		r = h[m+2][m+1];
		s = fabs (p) + fabs (q) + fabs (r);
		p /= s;
		q /= s;
		r /= s;
		if (m == l)
			goto L150;
		    
		tst1 = fabs (p) * (fabs (h[m-1][m-1]) + fabs (zz) + fabs (h[m+1][m+1]));
		tst2 = tst1 + fabs (h[m][m-1]) * (fabs (q) + fabs (r));
		if (tst2 == tst1)
			goto L150;
		
		}	// end "for (mm=l; mm<=enm2; mm++)"		L140:

L150:
	mp2 = m + 2;

	for (i=mp2; i<=en; i++) 
		{
		h[i][i-2] = 0.;
		if (i != mp2)
			h[i][i-3] = 0.;
			
		}	// end "for (i=mp2; i<=en; i++)"		L160:
		
			// double qr step involving rows l to en and
			// columns m to en ..........

	for (k=m; k<=na; k++)		// L260:
		{
		notlas = k != na;
		if (k == m)
			goto L170;
			
		p = h[k][k-1];
		q = h[k+1][k-1];
		r = 0.;
		if (notlas)
			r = h[k+2][k-1];
			
		x = fabs (p) + fabs (q) + fabs (r);
		if (x == 0.)
			goto L260;
		    
		p /= x;
		q /= x;
		r /= x;
		
L170:
		d__1 = sqrt (p*p + q*q + r*r);
		s = SIGN(d__1, p);
		if (k == m)
			goto L180;
		
		h[k][k-1] = -s * x;
		goto L190;
		
L180:
		if (l != m)
			h[k][k-1] = -h[k][k-1];
		
L190:
		p += s;
		x = p / s;
		y = q / s;
		zz = r / s;
		q /= p;
		r /= p;
		if (notlas)
			goto L225;
		
				// row modification

		for (j=k; j<=n; j++) 
			{
			p = h[k][j] + q * h[k+1][j];
			h[k][j] -= p * x;
			h[k+1][j] -= p * y;
			
			}	// end "for (j=k; j<=n; ++j)"		L200:

				// Computing MIN */

		j = MIN (en,k+3);
		
				// column modification

		for (i=1; i <=j; i++) 
			{
			p = x * h[i][k] + y * h[i][k+1];
			h[i][k] -= p;
			h[i][k+1] -= p * q;
			
			}	// end "for (i=1; i <=j; i++)"	 L210:
	
				// accumulate transformations

		for (i=low; i<=igh; i++) 
			{
			p = x * z[i][k] + y * z[i][k+1];
			z[i][k] -= p;
			z[i][k+1] -= p * q;
			
			}	// end "for (i=low; i<=igh; i++)"	L220:
			
		goto L255;
		
L225:
				// row modification

		for (j=k; j<=n; j++) 
			{
			p = h[k][j] + q * h[k+1][j] + r * h[k+2][j];
			h[k][j] -= p * x;
			h[k+1][j] -= p * y;
			h[k+2][j] -= p * zz;
			
			}	// end "for (j=k; j<=n; j++)"	L230:

				// Computing MIN

		j = MIN (en,k+3);
		
				// column modification

		for (i=1; i<=j; i++) 
			{
			p = x * h[i][k] + y * h[i][k+1] + zz * h[i][k+2];
			h[i][k] -= p;
			h[i][k+1] -= p * q;
			h[i][k+2] -= p * r;
	    
			}	// end "for (i=1; i<=j; i++)"	L240:
	
				// accumulate transformations

		for (i=low; i<=igh; i++) 
			{
			p = x*z[i][k] + y*z[i][k+1] + zz*z[i][k+2];
			z[i][k] -= p;
			z[i][k+1] -= p * q;
			z[i][k+2] -= p * r;
			
			}	// end "for (i=low; i<=igh; i++)"	L250:
			
L255:

L260:	;
		}	// end "for (k=m; k<=na; k++)"

	goto L70;
    
			// one root found
			
L270:
	h[en][en] = x + t;
	wr[en] = h[en][en];
	wi[en] = 0.;
	en = na;
	goto L60;
	
			// two roots found

L280:
	p = (y - x) / 2.;
	q = p * p + w;
	zz = sqrt ((fabs (q)));
	h[en][en] = x + t;
	x = h[en][en];
	h[na][na] = y + t;
	if (q < 0.)
		goto L320;
		
			// real pair
			
	zz = p + SIGN(zz, p);
	wr[na] = x + zz;
	wr[en] = wr[na];
	if (zz != 0.)
		wr[en] = x - w / zz;
		
	wi[na] = 0.;
	wi[en] = 0.;
	x = h[en][na];
	s = fabs (x) + fabs (zz);
	p = x / s;
	q = zz / s;
	r = sqrt (p * p + q * q);
	p /= r;
	q /= r;
	
			// row modification
 
	for (j=na; j<=n; j++) 
		{
		zz = h[na][j];
		h[na][j] = q * zz + p * h[en][j];
		h[en][j] = q * h[en][j] - p * zz;
		
		}	// end "for (j=na; j<=n; j++)"	L290:
		
			// column modification

	for (i=1; i<=en; i++) 
		{
		zz = h[i][na];
		h[i][na] = q * zz + p * h[i][en];
		h[i][en] = q * h[i][en] - p * zz;
	
		}	// end "for (i=1; i<=en; i++)"	L300:
    
			// accumulate transformations

	for (i=low; i<=igh; i++) 
		{
		zz = z[i][na];
		z[i][na] = q * zz + p * z[i][en];
		z[i][en] = q * z[i][en] - p * zz;
		
		}	// end "for (i=low; i<=igh; i++)"	L310:

	goto L330;
	
			// complex pair
			
L320:
	wr[na] = x + p;
	wr[en] = x + p;
	wi[na] = zz;
	wi[en] = -zz;
	
L330:
	en = enm2;
	goto L60;
	
			// all roots found.  backsubstitute to find
			// vectors of upper triangular form

L340:
	if (norm == 0.)
		goto L1001;
	
			// for en=n step -1 until 1 do --

	for (nn=1; nn <=n; nn++) 	// to L800:
		{
		en = n + 1 - nn;
		p = wr[en];
		q = wi[en];
		na = en - 1;
		if (q < 0.)
			goto L710;
			
		else if (q == 0)
		 	goto L600;
		 	
		else
			goto L800;
			
				// real vector
L600:
		m = en;
		h[en][en] = 1.;
		if (na == 0)
		    goto L800;
		    
				// for i=en-1 step -1 until 1 do --

		for (ii=1; ii<=na; ii++) 
			{
			i = en - ii;
			w = h[i][i] - p;
			r = 0.;

			for (j=m; j<=en; j++)
				r += h[i][j] * h[j][en];	//  L610:

			if (wi[i] >= 0.)
				goto L630;
				
			zz = w;
			s = r;
			goto L700;
			
L630:
			m = i;
			if (wi[i] != 0.)
				goto L640;
			
			t = w;
			if (t != 0.)
				goto L635;
			
			tst1 = norm;
			t = tst1;
			
L632:
			t *= .01;
			tst2 = norm + t;
			if (tst2 > tst1)
				goto L632;
			
L635:
			h[i][en] = -r / t;
			goto L680;
			
					// solve real equations
L640:
			x = h[i][i+1];
			y = h[i+1][i];
			q = (wr[i] - p) * (wr[i] - p) + wi[i] * wi[i];
			t = (x * s - zz * r) / q;
			h[i][en] = t;
			if (fabs (x) <= fabs (zz))
				goto L650;
			
			h[i+1][en] = (-r - w * t) / x;
			goto L680;
			
L650:
			h[i+1][en] = (-s - y * t) / zz;

					// overflow control
					
L680:
			t = fabs (h[i][en]);
			if (t == 0.)
				goto L700;
				
			tst1 = t;
			tst2 = tst1 + 1. / tst1;
			if (tst2 > tst1)
				goto L700;

			for (j=i; j<=en; j++)
				h[j][en] /= t;		// L690:

L700: ;
			}	// end "for (ii=1; ii<=na; ii++)"
			
				// end real vector
				
		goto L800;
		
				// complex vector
				
L710:
		m = na;
		
				// last vector component chosen imaginary so that
				// eigenvector matrix is triangular
				
		if (fabs (h[en][na]) <= fabs (h[na][en]))
			goto L720;
	
		h[na][na] = q / h[en][na];
		h[na][en] = -(h[en][en] - p) / h[en][na];
		goto L730;
		
L720:
		d__2 = h[na][na] - p;
		cdiv (c_b50, -h[na][en], d__2, q, &h[na][na], &h[na][en]);
	
L730:
		h[en][na] = 0.;
		h[en][en] = 1.;
		enm2 = na - 1;
		if (enm2 == 0)
			goto L800;
		    
				// for i=en-2 step -1 until 1 do --

		for (ii=1; ii<=enm2; ii++)		// to L795:
			{
			i = na - ii;
			w = h[i][i] - p;
			ra = 0.;
			sa = 0.;

			for (j=m; j<=en; j++) 
				{
				ra += h[i][j] * h[j][na];
				sa += h[i][j] * h[j][en];

				}	// end "for (j=m; j<=en; j++)"	L760:

			if (wi[i] >= 0.)
				goto L770;
				
			zz = w;
			r = ra;
			s = sa;
			goto L795;
			
L770:
			m = i;
			if (wi[i] != 0.)
				goto L780;
			
			d__1 = -ra;
			d__2 = -sa;
			cdiv (d__1, d__2, w, q, &h[i][na], &h[i][en]);
			goto L790;
			
					// solve complex equations
					
L780:
			x = h[i][i+1];
			y = h[i+1][i];
			vr = (wr[i] - p) * (wr[i] - p) + wi[i] * wi[i] - q * q;
			vi = (wr[i] - p) * 2. * q;
			if (vr != 0. || vi != 0.)
				goto L784;
			
			tst1 = norm * (fabs (w) + fabs (q) + fabs (x) + fabs (y) + fabs (zz));
			vr = tst1;
			
L783:
			vr *= .01;
			tst2 = tst1 + vr;
			if (tst2 > tst1)
				goto L783;
			
L784:
			d__1 = x * r - zz * ra + q * sa;
			d__2 = x * s - zz * sa - q * ra;
			cdiv (d__1, d__2, vr, vi, &h[i][na], &h[i][en]);
			if (fabs (x) <= fabs (zz) + fabs (q))
				goto L785;
			
			h[i+1][na] = (-ra - w * h[i][na] + q * h[i][en]) / x;
			h[i+1][en] = (-sa - w * h[i][en] - q * h[i][na]) / x;
			goto L790;
			
L785:
			d__1 = -r - y * h[i][na];
			d__2 = -s - y * h[i][en];
			cdiv (d__1, d__2, zz, q, &h[i+1][na], &h[i+1][en]);

					// overflow control
L790:
					// Computing MAX
					
			t = MAX (fabs (h[i][na]), fabs (h[i][en]));
			if (t == 0.)
				goto L795;
			
			tst1 = t;
			tst2 = tst1 + 1. / tst1;
			if (tst2 > tst1)
				goto L795;

			for (j=i; j<=en; j++) 
				{
				h[j][na] /= t;
				h[j][en] /= t;
				
	    		}	// end "for (j=i; j<=en; j++)"	L792:

L795: ;
			}	// end "for (ii=1; ii<=enm2; ii++)"
			
				// end complex vector
				
L800: ;
		}	// end "for (nn=1; nn <=n; nn++)"	
		
			// end back substitution.
			// vectors of isolated roots

	for (i=1; i <=n; i++) 
		{
		if (i >= low && i <= igh)
	   	goto L840;
	

		for (j=i; j<=n; j++)
			z[i][j] = h[i][j];	// L820:

L840: ;
		}	// end "for (i=1; i <=n; i++)"
		
			// multiply by transformation matrix to give
			// vectors of original full matrix.
			// for j=n step -1 until low do --
			
	for (jj=low; jj<=n; jj++) 
		{
		j = n + low - jj;
		m = MIN (j,igh);

		for (i=low; i<=igh; i++) 
			{
	   	zz = 0.;

			for (k=low; k<=m; k++)
				zz += z[i][k] * h[k][j];	// L860:

	 		z[i][j] = zz;
	 		
			}	// end "for (i=low; i<=igh; i++)"
			
		}	// end "for (jj=low; jj<=n; ++jj)"	L880:

	goto L1001;
    
			// set error -- all eigenvalues have not
			// converged after 30*n iterations
L1000:
	ierr = en;
    
L1001:
	return (ierr);
    
}	// end "hqr2" 



void cdiv (
				double 						ar, 
				double 						ai, 
				double 						br, 
				double 						bi, 
				double*						cr, 
				double*						ci)
			
{
	double 								ais,
											ars,
											bis,
											brs,
											s;
											

			// complex division, (cr,ci) = (ar,ai)/(br,bi)

	s = fabs (br) + fabs (bi);
	ars = ar / s;
	ais = ai / s;
	brs = br / s;
	bis = bi / s;

		// Computing 2nd power
		
	s = brs*brs + bis*bis;

	*cr = (ars * brs + ais * bis) / s;
	*ci = (ais * brs - ars * bis) / s;

	return;
    
}	// end "cdiv" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void orthes
//
//	Software purpose:	Given a real general matrix, this subroutine reduces a submatrix 
//							situated in rows and columns low through igh to upper hessenberg 
//							form by orthogonal similarity transformations.
//
//							this subroutine is a translation of the algol procedure orthes,
//							num. math. 12, 349-368 (1968) by martin and wilkinson. handbook 
//							for auto. comp., vol.ii-linear algebra, 339-358 (1971).
//
//							questions and comments should be directed to burton s. garbow, 
//							mathematics and computer science div, argonne national laboratory
//
//							this version dated august 1983.
//
//	Parameters in:		nm must be set to the row dimension of two-dimensional array 
//								parameters as declared in the calling program dimension 
//								statement.
//
//							n is the order of the matrix.
//
//							low and igh are integers determined by the balancing subroutine  
//								balanc.  if  balanc  has not been used, set low=1, igh=n.
//
//					    	a (inputMatrixPtr) contains the input matrix.
//
//	Parameters out:	a contains the hessenberg matrix.  information about the 
//								orthogonal transformations used in the reduction is stored 
//								in the remaining triangle under the hessenberg matrix.
//
//							ort contains further information about the transformations.
//								only elements low through igh are used.
//
// Value Returned:	None	
// 
// Called By:
//
//	Revised By:			Larry L. Biehl			Date: 02/15/2002
	
void orthes (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh,
				double**								a,
				double*								ort,
				double*								inputMatrixPtr)
										
{
	SInt32								mp;

	UInt32								i,
											ii,
											j,
											jj,
											kp1,
											la,
											m;
											
	double								f,
											g,
											h,
											scale;
							
	
			// Set pointers the rows for a. Allow for indexing from
			// 1 not 0.	
	
	ort--;																		
	a[1] = inputMatrixPtr - 1;
	
	for (i=2; i<=n; i++) 
		a[i] = a[i-1] + n;	
		
			// Initialize ort vector	
						
	for (m=1; m<=n; m++) 
		ort[m] = 0;	

	la = igh - 1;
	kp1 = low + 1;

	if (la >= kp1)
		{
		for (m=kp1; m<=la; m++)
			{
         h = 0;
         ort[m] = 0;
         scale = 0;
         
					// Scale column (algol tol then not needed)
					
         for (i=m; i<=igh; i++)
				scale += fabs (a[i][m-1]);

         if (scale != 0)
         	{
	         mp = m + igh;
	         
						// for i=igh step -1 until m do --
						
	        for (ii=m; ii<=igh; ii++)
	        		{
	            i = mp - ii;
	            ort[i] = a[i][m-1] / scale;
	            h += ort[i] * ort[i];
	            
					}	// end "for (ii=m; ii<=igh; II++)"

	         g = -SIGN(sqrt (h),ort[m]);
	         h -= ort[m] * g;
	         ort[m] -= g;
	         
						// form (i-(u*ut)/h) * a
						
	         for (j=m; j<=n; j++)
	         	{
	            f = 0;
	            
							// for i=igh step -1 until m do --
							
	            for (ii=m; ii<=igh; ii++)
	            	{
	               i = mp - ii;
	               f += ort[i] * a[i][j];
	               
						}	// end "for (ii=m; ii<=igh; ii++)"

	            f /= h;

	            for (i=m; i<=igh; i++)
						a[i][j] -= f * ort[i];

					}	// end "for (j=m; j<=n; j++)"
	  
						// form (i-(u*ut)/h)*a*(i-(u*ut)/h)
						
	         for (i=1; i<=igh; i++)
	         	{
	            f = 0;
	            
							// for j=igh step -1 until m do --
							 
	            for (jj=m; jj<=igh; jj++)
	            	{
	               j = mp - jj;
	               f += ort[j] * a[i][j];
	               
						}	// end "for (jj=m; jj<=igh; j++)"

	            f /= h;

	            for (j=m; j<=igh; j++)
						a[i][j] -= f * ort[j];

					}	// end "for (i=1; i<=igh; i++)"

	         ort[m] *= scale;
	         a[m][m-1] = scale * g;
         
         	}	// end "if (scale != 0)"
  
			}	// end "for (m=kp1; m<=la; m++)"
			
		}	// end "if (la >= kp1)"

	return;

}	// end "orthes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ortran
//
//	Software purpose:	This subroutine accumulates the orthogonal similarity 
//							transformations used in the reduction of a real general matrix 
//							to upper hessenberg form by orthes.
//
//							this subroutine is a translation of the algol procedure ortrans,
//							num. math. 16, 181-204 (1970) by peters and wilkinson. handbook 
//							for auto. comp., vol.ii-linear algebra, 372-395 (1971).
//
//							questions and comments should be directed to burton s. garbow, 
//							mathematics and computer science div, argonne national laboratory
//
//							this version dated august 1983.
//
//	Parameters in:		nm must be set to the row dimension of two-dimensional array 
//								parameters as declared in the calling program dimension 
//								statement.
//
//							n is the order of the matrix.
//
//							low and igh are integers determined by the balancing subroutine  
//								balanc.  if balanc has not been used, set low=1, igh=n.
//
//					    	a (inputMatrixPtr) contains information about the orthogonal 
//								transformations used in the reduction by  orthes in its strict 
//								lower triangle.
//
//							ort contains further information about the transformations used in 
//								the reduction by orthes. only elements low through igh are used.
//
//	Parameters out:	z contains the transformation matrix produced in the reduction by  
//								orthes.
//
//							ort has been altered.
//
// Value Returned:	None	
// 
// Called By:
//
//	Revised By:			Larry L. Biehl			Date: 02/15/2002
	
void ortran (
				UInt32								nm,
				UInt32								n,
				UInt32								low,
				UInt32								igh,
				double**								a,
				double*								ort,
				double**								z,
				double*								inputMatrixPtr,
				double*								outputMatrixPtr)
										
{
	SInt32								kl,
											mm,
											mp,
											mp1;

	UInt32								i,
											j;
											
	double 								g;
									

			// Initialize z (outputMatrixPtr) to identity matrix.
			
	SetIdentityMatrix (outputMatrixPtr, 
								n,
								kSquareOutputMatrix);
	
			// Set pointers to the rows for a. Allow for indexing from
			// 1 not 0.	
	
	ort--;																		
	a[1] = inputMatrixPtr - 1;												
	z[1] = outputMatrixPtr - 1;
	
	for (i=2; i<=n; i++)
		{ 
		a[i] = a[i-1] + n;
		z[i] = z[i-1] + n;
		
		}	// end "for (i=2; i<=n; i++)"	

	kl = igh - low - 1;
	if (kl >= 1)
		{
				// for mp=igh-1 step -1 until low+1 do --
			
		for (mm=1; mm<=kl; mm++)
			{
			mp = igh - mm;
		
			if (a[mp][mp-1] != 0)
				{
				mp1 = mp + 1;

				for (i=mp1; i<=igh; i++)
					ort[i] = a[i][mp-1];

				for (j=mp; j<=igh; j++)
					{
            	g = 0;

            	for (i=mp; i<=igh; i++)
						g += ort[i] * z[i][j];
  
							// divisor below is negative of h formed in orthes.
							// double division avoids possible underflow
							
            	g = (g / ort[mp]) / a[mp][mp-1];

					for (i=mp; i<=igh; i++)
						z[i][j] += g * ort[i];

					}	// end "for (j=mp; j<=igh; j++)"

				}	// end "if (a[mp][mp-1] != 0)"
				
			}	// end "for (mm=1; mm<=kl; m++)"

		}	// end "if (kl >= 1)"

	return;

}	// end "ortran"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 MatlabEigFunction
//
//	Software purpose:	
//
//	Parameters in:					.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/14/2002
//	Revised By:			Larry L. Biehl			Date: 02/14/2002	

SInt32 MatlabEigFunction (
				double*								inputMatrixPtr,
				double*								eigenvectorPtr,
				double*								eigenvaluePtr,
				UInt32								numberFeatures)
										
{
	SInt32								returnCode;
	
	UInt32								igh,
											low;
	
					
			// Calculate eigenvectors and eigenvalues of tempMatrixPtr
			// This is the beginning of the eig method in Matlab.
			
	double*	scalePtr = gInverseMatrixMemory.pivotPtr;
	double**	a_indexPtr = (double**)gInverseMatrixMemory.indexRowPtr;
	double** z_indexPtr = (double**)gInverseMatrixMemory.ipvotPtr;
	//UInt32*	intPtr = (UInt32*)gInverseMatrixMemory.indexColPtr;
	double*	zMatrixPtr = eigenvectorPtr;
	double*	ortPtr = eigenvaluePtr;
	double*	imaginaryEigenvaluePtr = &eigenvaluePtr[numberFeatures];
	/*
	ListTransformationInformation (
									eigenvaluePtr, 
									inputMatrixPtr, 
									0, 
									0,
									NULL,
									(UInt16)numberFeatures,
									(UInt16)numberFeatures,
									FALSE,
									TRUE,
									2);
	*/
	BalanceMatrix (numberFeatures,
							numberFeatures, 
							a_indexPtr,
							&low,
							&igh,
							scalePtr,
							inputMatrixPtr);
	/*
	ListTransformationInformation (eigenvaluePtr,
												inputMatrixPtr, 
												0, 
												0,
												NULL,
												(UInt16)numberFeatures,
												(UInt16)numberFeatures,
												FALSE,
												TRUE,
												2);
	 
	balbak (numberFeatures,
				numberFeatures,
				low,
				igh, 
				scalePtr,
				numberFeatures,
				z_indexPtr,
				inputMatrixPtr);
	
	ListTransformationInformation (eigenvaluePtr,
												inputMatrixPtr, 
												0, 
												0,
												NULL,
												(UInt16)numberFeatures,
												(UInt16)numberFeatures,
												FALSE,
												TRUE,
												2);
	*/
	orthes (numberFeatures,
				numberFeatures,
				low,
				igh,
				a_indexPtr,
				ortPtr,
				inputMatrixPtr);
	/*
	ListTransformationInformation (eigenvaluePtr,
												inputMatrixPtr, 
												0, 
												0,
												NULL,
												(UInt16)numberFeatures,
												(UInt16)numberFeatures,
												FALSE,
												TRUE,
												2);
	*/
	ortran (numberFeatures,
				numberFeatures,
				low,
				igh,
				a_indexPtr,
				ortPtr,
				z_indexPtr,
				inputMatrixPtr,
				zMatrixPtr);

	/*
	elmhes (numberFeatures,
				numberFeatures,
				low,
				igh,
				a_indexPtr,
				intPtr,
				inputMatrixPtr);
	
	ListTransformationInformation (eigenvaluePtr,
												inputMatrixPtr, 
												0, 
												0,
												NULL,
												(UInt16)numberFeatures,
												(UInt16)numberFeatures,
												FALSE,
												TRUE,
												2);
	 
	eltran (numberFeatures,
				numberFeatures,
				low,
				igh, 
				a_indexPtr,
				intPtr,
				z_indexPtr,
				inputMatrixPtr,
				zMatrixPtr);
	
	ListTransformationInformation (eigenvaluePtr,
												inputMatrixPtr, 
												0, 
												0,
												NULL,
												(UInt16)numberFeatures,
												(UInt16)numberFeatures,
												FALSE,
												TRUE,
												2);
	
	ListTransformationInformation (eigenvaluePtr,
												zMatrixPtr, 
												0, 
												0,
												NULL,
												(UInt16)numberFeatures,
												(UInt16)numberFeatures,
												FALSE,
												TRUE,
												2);	
	*/
	returnCode = hqr2 (numberFeatures,
								numberFeatures, 
								low, 
								igh, 
								a_indexPtr, 
								eigenvaluePtr, 
								imaginaryEigenvaluePtr, 
								z_indexPtr, 
								inputMatrixPtr, 
								zMatrixPtr);
	
	if (returnCode == noErr)
		{					
		balbak (numberFeatures,
					numberFeatures,
					low,
					igh, 
					scalePtr,
					numberFeatures,
					z_indexPtr,
					zMatrixPtr);
				
				// The eigenvectors are in column format. Change to row format. MultiSpec
				// expects them to be that way for later use.
					
		TransposeMatrix (eigenvectorPtr, eigenvectorPtr, numberFeatures);

		OrderEigenvaluesAndEigenvectors (eigenvaluePtr, 
														eigenvectorPtr,
														gInverseMatrixMemory.pivotPtr,	// tempVector
														numberFeatures,
														0x0003);

		normalize_rows (eigenvectorPtr, numberFeatures, numberFeatures);
		
		}	// end "if (returnCode == noErr)"

	/*
	FindEigenvaluesOfUpperHessenbergMatrix (
							inputMatrixPtr, 
							numberFeatures, 
							eigenvaluePtr, 
							gInverseMatrixMemory.pivotPtr,
							a_indexPtr);

	returnCode = 591;
	*/
	return (returnCode);

}	// end "MatlabEigFunction"



//#include <math.h>
//#include "nrutil.h"
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FindEigenvaluesOfUpperHessenbergMatrix
//							hqr.c in Numerical Recipes in C
//
//	Software purpose:	Finds all eigenvalues of an upper Hessenberg matrix 
//							a[1..n][1..n]. On input a can be exactly as output from 
//							elmhes x 11.5; on output it is destroyed. The real and 
//							imaginary parts of the eigenvalues are returned in wr[1..n] 
//							and wi[1..n], respectively.
//
//	Parameters in:					.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/07/1993
//	Revised By:			Larry L. Biehl			Date: 02/09/2002	

void FindEigenvaluesOfUpperHessenbergMatrix (
				double*								inputMatrixPtr, 
				UInt32								n, 
				double* 								wr, 
				double* 								wi,
				double**								a)

{
	double 								z,
											y,
											x,
											w,
											v,
											u,
											t,
											s,
											r,
											q,
											p,
											anorm;
	
	SInt32 								nn,
											m,
											l,
											k,
											j,
											its,
											i,
											mmin;
							
	
			// Allow indexing of rv1Ptr and w from 1 not 0;
		
	wr--;
	wi--;
	
			// Set pointers the rows for a. Allow for indexing from
			// 1 not 0.	
																			
	a[1] = inputMatrixPtr - 1;
	
	for (i=2; i<=(SInt32)n; i++) 
		a[i] = a[i-1] + n;	
	
			// Compute matrix norm for possible use in locating single small 
			// subdiagonal element. 
			
	anorm=0.0; 
	for (i=1; i<=(SInt32)n; i++)
		for (j=MAX (i-1,1); j<=(SInt32)n; j++)
			anorm += fabs (a[i][j]);
			
	nn = n;
	t = 0.0;		// Gets changed only by an exceptional shift.
	while (nn >= 1) 
		{ 
				// Begin search for next eigenvalue.
				
		its = 0;
		do 
			{
			for (l=nn; l>=2; l--) 
				{ 
						// Begin iteration: look for single small subdiagonal 
						// element. s=fabs (a[l-1][l-1])+fabs (a[l][l]);
						
				if (s == 0.0) 
					s = anorm;
					
				if ((double)(fabs (a[l][l-1]) + s) == s) 
					break;
					
				}	// end "for (l=nn; l>=2; l--)"
	
			x = a[nn][nn];
			if (l == nn) 
				{ 
						// One root found.
	
				wr[nn]=x+t;
				wi[nn--]=0.0;
				
				}	// end "if (l == nn)"
				
			else	// l != nn
				{
				y = a[nn-1][nn-1];
				w = a[nn][nn-1]*a[nn-1][nn];
				if (l == (nn-1)) 
					{ 
							// Two roots found...
							
					p = 0.5*(y-x);
					q = p*p + w;
					z = sqrt (fabs (q));
					x += t;
					if (q >= 0.0) 
						{ 
								// ...a real pair.
	
						z = p + SIGN(z,p);
						wr[nn-1] = wr[nn] = x + z;
						if (z) 
							wr[nn] = x - w/z;
							
						wi[nn-1] = wi[nn] = 0.0;
						
						} 
						
					else	// q < 0.0
						{ 
								// ...a complex pair.
	
						wr[nn-1] = wr[nn] = x + p;
						wi[nn-1] = -(wi[nn] = z);
	
						}	// end "else q < 0.0"
	
					nn -= 2;
	
					}	// end "if (l == (nn-1))"
					
				else	// l != (nn-1)
					{ 
							// No roots found. Continue iteration.
	
					//if (its == 30)
					//	nrerror ("Too many iterations in hqr");
	
					if (its == 10 || its == 20) 
						{ 
								// Form exceptional shift.
	
						t += x;
						for (i=1;i<=nn;i++) 
							a[i][i] -= x;
							
						s = fabs (a[nn][nn-1]) + fabs (a[nn-1][nn-2]);
						y = x = 0.75*s;
						w = -0.4375*s*s;
						
						}	// end "if (its == 10 || its == 20)"
	
					++its;
					for (m=(nn-2); m>=l; m--) 
						{ 
								// Form shift and then look for 2 consecutive 
								// small subdiagonal elements.
	
						z = a[m][m];
						r = x - z;
						s = y - z;
						p = (r*s-w)/a[m+1][m] + a[m][m+1];
						
								// Equation (11.6.23).
						
						q = a[m+1][m+1]-z-r-s;
						r = a[m+2][m+1];
						s = fabs (p) + fabs (q) + fabs (r);
						
								// Scale to prevent over flow or under flow. 
								
						p /= s;
						q /= s;
						r /= s;
						if (m == l) 
							break;
							
						u = fabs (a[m][m-1])*(fabs (q)+fabs (r));
						v = fabs (p)*(fabs (a[m-1][m-1]) + fabs (z) + fabs (a[m+1][m+1]));
						if ((double)(u+v) == v) 
							break; 
							
								// Equation (11.6.26).
						
						}	// end "for (m=(nn-2); m>=l; m--)"
						
					for (i=m+2; i<=nn; i++)
						{
						a[i][i-2] = 0.0;
						if (i != (m+2)) 
							a[i][i-3] = 0.0;
							
						}	// end "for (i=m+2; i<=nn; i++)"
						
					for (k=m; k<=nn-1; k++)
						{
								// Double QR step on rows l to nn and columns m to nn.
	
						if (k != m) 
							{
							p = a[k][k-1];
							
									// Begin setup of Householder vector. 
									
							q = a[k+1][k-1];
							r = 0.0;
							if (k != (nn-1)) 
								r = a[k+2][k-1];
								
							if ((x = fabs (p)+fabs (q)+fabs (r)) != 0.0)
								{
								p /= x;
								
										// Scale to prevent over flow or under flow.
										 
								q /= x;
								r /= x;
							
								}	// end "if ((x = fabs (p)+fabs (q)+fabs (r)) != 0.0)"
							
							}	// end "if (k != m)"
							
						if ((s = SIGN(sqrt (p*p+q*q+r*r), p)) != 0.0)
							{
							if (k == m) 
								{
								if (l != m)
									a[k][k-1] = -a[k][k-1];
								
								}	// end "if (k == m)"
						
							else
								a[k][k-1] = -s*x;
								
							p += s; 
							
									// Equations (11.6.24).
									
							x = p/s;
							y = q/s;
							z = r/s;
							q /= p;
							r /= p;
							
							for (j=k; j<=nn; j++)
								{ 
										// Row modification.
	
								p = a[k][j] + q*a[k+1][j];
								
								if (k != (nn-1)) 
									{
									p += r*a[k+2][j];
									a[k+2][j] -= p*z;
									
									}	// end "if (k != (nn-1))"
									
								a[k+1][j] -= p*y;
								a[k][j] -= p*x;
								
								}	// end "for (j=k; j<=nn; j++)"
								
							mmin = nn<k+3 ? nn : k+3;
							for (i=l; i<=mmin; i++)
								{ 
										// Column modification.
										
								p=x*a[i][k]+y*a[i][k+1];
								if (k != (nn-1)) 
									{
									p += z*a[i][k+2];
									a[i][k+2] -= p*r;
								
									}	// end "if (k != (nn-1))"
								
								a[i][k+1] -= p*q;
								a[i][k] -= p;
								
								}	// end "for (i=l; i<=mmin; i++)"
								
							}	// end "if ((s = SIGN(sqrt (p*p+q*q+r*r), p)) != 0.0)"
		
						}	// end "for (k=m; k<=nn-1; k++)"
		
					}	// end "else l != (nn-1)"
		
				}	// end "else l != nn"
		
			} while (l < nn-1);
		
		}	// end "while (nn >= 1)"
	
}	// end "FindEigenvaluesOfUpperHessenbergMatrix"



/* (C) Copr. 1986-92 Numerical Recipes Software w5. */
//#define NRANSI
//#include "nrutil.h"
//#include "nr.h"
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeEigenvectorsSVD
//							dsvdcmp.c in Numerical Recipes in C
//
//	Software purpose:	Given a matrix a[1..m][1..n], this routine computes its 
//							singular value decomposition, A = U*W*VT. The matrix U 
//							replaces a on output. The diagonal matrix of singular 
//							values W is output as a vector w[1..n]. The matrix V 
//							(not the transpose VT) is output as v[1..n][1..n].
//
//							The eigenvectors in U are in column form. They are
//							transformed to row form at the end of the program. If the
//							input matrix is symmetric, then V is the same as U.
//
//	Parameters in:					.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/07/1993
//	Revised By:			Larry L. Biehl			Date: 02/09/2002	

SInt32 ComputeEigenvectorsSVD (
				double*								inputMatrixPtr, 
				UInt32 								m, 
				UInt32 								n, 
				double* 								w, 
				double*								outputEigenvectorsPtr,
				double*								rv1Ptr,
				double**								a,
				double**								v)
				
{
	double 								anorm,
											c,
											f,
											g,
											h,
											s,
											scale,
											x,
											y,
											z;
											
	UInt32 								flag,
											i,
											its,
											j,
											jj,
											k,
											l,
											nm;


			// Allow indexing of rv1Ptr and w from 1 not 0;
		
	rv1Ptr--;
	w--;
	
			// Set pointers the rows for a and v. Allow for indexing from
			// 1 not 0.	
																			
	a[1] = inputMatrixPtr - 1;
	v[1] = outputEigenvectorsPtr - 1;
	
	for (i=2; i<=m; i++) 
		{
		a[i] = a[i-1] + n;
		v[i] = v[i-1] + n;
		
		}	// end "for (i=2; i<=m; i++)"			

	g = scale = anorm = 0.0;
	for (i=1; i<=n; i++) 
		{
		l = i + 1;
		rv1Ptr[i] = scale * g;
		g = s = scale = 0.0;
		
		if (i <= m) 
			{
			for (k=i; k<=m; k++) 
				scale += fabs (a[k][i]);
				
			if (scale) 
				{
				for (k=i; k<=m; k++) 
					{
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
					
					}	// end "for (k=i; k<=m; k++)"
					
				f = a[i][i];
				g = -SIGN(sqrt (s), f);
				h = f*g - s;
				a[i][i] = f - g;
				for (j=l;j<=n;j++) 
					{
					for (s=0.0,k=i;k<=m;k++) 
						s += a[k][i]*a[k][j];
						
					f = s/h;
					for (k=i; k<=m; k++)
						a[k][j] += f*a[k][i];
						
					}	// end "or (j=l;j<=n;j++)"
					
				for (k=i; k<=m; k++)
					a[k][i] *= scale;
					
				}	// end "if (scale)"
			
			}	// end "if (i <= m)"
			
		w[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m && i != n) 
			{
			for (k=l; k<=n; k++)
				scale += fabs (a[i][k]);
			
			if (scale) 
				{
				for (k=l; k<=n; k++)
					{
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
					
					}	// end "for (k=l; k<=n; k++)"
					
				f = a[i][l];
				g = -SIGN(sqrt (s), f);
				h = f*g - s;
				a[i][l] = f - g;
				for (k=l; k<=n; k++)
					rv1Ptr[k] = a[i][k]/h;
					
				for (j=l; j<=m; j++)
					{
					for (s=0.0,k=l;k<=n;k++)
						s += a[j][k]*a[i][k];
					
					for (k=l;k<=n;k++)
						a[j][k] += s*rv1Ptr[k];
					
					}	// end "for (j=l; j<=m; j++)"
					
				for (k=l;k<=n;k++) 
					a[i][k] *= scale;
			
				}	// end "if (scale)"
			
			}	// end "if (i <= m && i != n)"
		
		anorm = MAX (anorm,(fabs (w[i])+fabs (rv1Ptr[i])));
		
		}	// end "for (i=1; i<=n; i++)"
		
	for (i=n; i>=1; i--) 
		{
		if (i < n) 
			{
			if (g) 
				{
				for (j=l; j<=n; j++)
					v[j][i] = (a[i][j]/a[i][l])/g;
					
				for (j=l; j<=n; j++)
					{
					for (s=0.0,k=l; k<=n; k++)
						s += a[i][k]*v[k][j];
						
					for (k=l; k<=n; k++)
						v[k][j] += s*v[k][i];
					
					}	// end "for (j=l; j<=n; j++)"
				
				}	// end "if (g)"
			
			for (j=l; j<=n; j++)
				v[i][j] = v[j][i] = 0.0;
	   	
			if (TickCount () >= gNextTime)
				{
		   			// This code checks to see if the user has requested 				
		   			// computation to stop; i.e. the user has entered command-.		
		   			// on the keyboard.	
	   			
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (591);
					
				}	// end "if (TickCount () >= nextTime)"
		
			}	// end "if (i < n)"
		
		v[i][i] = 1.0;
		g = rv1Ptr[i];
		l = i;
		
		}	// end "for (i=n; i>=1; i--)"
	
	for (i=MIN (m,n); i>=1; i--)
		{
		l = i + 1;
		g = w[i];
		for (j=l; j<=n; j++)
			a[i][j] = 0.0;
		
		if (g) 
			{
			g = 1.0/g;
			for (j=l; j<=n; j++)
				{
				for (s=0.0,k=l; k<=m; k++)
					s += a[k][i]*a[k][j];
					
				f = (s/a[i][i])*g;
				for (k=i; k<=m; k++)
					a[k][j] += f*a[k][i];
				
				}	// end "for (j=l; j<=n; j++)"
			
			for (j=i; j<=m; j++)
				a[j][i] *= g;
		
			}	// end "if (g)"
			
		else	// !g
			for (j=i; j<=m; j++)
				a[j][i] = 0.0;
		
		++a[i][i];
	   	
		if (TickCount () >= gNextTime)
			{
	   			// This code checks to see if the user has requested 				
	   			// computation to stop; i.e. the user has entered command-.		
	   			// on the keyboard.	
   			
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																							return (591);
				
			}	// end "if (TickCount () >= nextTime)"
		
		}	// end "for (i=MIN (m,n); i>=1; i--)"
	
	for (k=n; k>=1; k--) 
		{
		for (its=1; its<=30; its++)
			{
			flag = 1;
			for (l=k; l>=1; l--)
				{
				nm = l - 1;
				if ((double)(fabs (rv1Ptr[l])+anorm) == anorm) 
					{
					flag = 0;
					break;
					
					}	// end "if ((double)(fabs (rv1Ptr[l])+anorm) == anorm)"
				
				if ((double)(fabs (w[nm]) + anorm) == anorm)
					break;
				
				}	// end "for (l=k; l>=1; l--)"
			
			if (flag) 
				{
				c = 0.0;
				s = 1.0;
				for (i=l; i<=k; i++)
					{
					f = s*rv1Ptr[i];
					rv1Ptr[i] = c*rv1Ptr[i];
					if ((double)(fabs (f)+anorm) == anorm) 
						break;
						
					g = w[i];
					h = dpythag (f,g);
					w[i] = h;
					h = 1.0/h;
					c = g*h;
					s = -f*h;
					for (j=1; j<=m; j++)
						{
						y = a[j][nm];
						z = a[j][i];
						a[j][nm] = y*c + z*s;
						a[j][i] = z*c - y*s;
						
						}	// end "for (j=1; j<=m; j++)"
					
					}	// end "for (i=l; i<=k; i++)"
				
				}	// end "if (flag)"
			
			z = w[k];
			if (l == k) 
				{
				if (z < 0.0) 
					{
					w[k] = -z;
					for (j=1; j<=n; j++)
						v[j][k] = -v[j][k];
					
					}	// end "if (z < 0.0)"
				
				break;
				
				}	// end "if (l == k)"
	   	
			if (TickCount () >= gNextTime)
				{
		   			// This code checks to see if the user has requested 				
		   			// computation to stop; i.e. the user has entered command-.		
		   			// on the keyboard.	
	   			
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (591);
					
				}	// end "if (TickCount () >= nextTime)"
			
			if (its == 30) 
				{
				//nrerror ("no convergence in 30 dsvdcmp iterations");
																								return (-2);
				}
			
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1Ptr[nm];
			h = rv1Ptr[k];
			f = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g = dpythag (f, 1.0);
			f = ((x-z)*(x+z) + h*((y/(f + SIGN(g,f)))-h))/x;
			c = s = 1.0;
			for (j=l; j<=nm; j++)
				{
				i = j+1;
				g = rv1Ptr[i];
				y = w[i];
				h = s*g;
				g = c*g;
				z = dpythag (f, h);
				rv1Ptr[j] = z;
				c = f/z;
				s = h/z;
				f = x*c + g*s;
				g = g*c - x*s;
				h = y*s;
				y *= c;
				for (jj=1; jj<=n; jj++)
					{
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x*c + z*s;
					v[jj][i] = z*c - x*s;
					
					}	// end "for (jj=1; jj<=n; jj++)"
				
				z = dpythag (f, h);
				w[j] = z;
				if (z) 
					{
					z = 1.0/z;
					c = f*z;
					s = h*z;
					
					}	// end "if (z)"
				
				f = c*g + s*y;
				x = c*y - s*g;
				for (jj=1; jj<=m; jj++)
					{
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y*c + z*s;
					a[jj][i] = z*c - y*s;
					
					}	// end "for (jj=1; jj<=m; jj++)"
				
				}	// end "for (j=l; j<=nm; j++)"
			
			rv1Ptr[l] = 0.0;
			rv1Ptr[k] = f;
			w[k] = x;
		
			}	// end "for (its=1; its<=30; its++)"
	
		}	// end "for (k=n; k>=1; k--)"
	
			// The eigenvectors are in column format. Change to row format. MultiSpec
			// expects them to be that way for later use.
				
	TransposeMatrix (inputMatrixPtr, outputEigenvectorsPtr, m);
		
			// Order the eigenvectors and eigenvalues. Use the 'rv1Ptr' vector
			// for temporary holding.

	w++;
	rv1Ptr++;
	OrderEigenvaluesAndEigenvectors (w, 
												outputEigenvectorsPtr,
												rv1Ptr,	// tempVector
												n,
												0x0003);
													
	if (TickCount () >= gNextTime)
		{
   			// This code checks to see if the user has requested 				
   			// computation to stop; i.e. the user has entered command-.		
   			// on the keyboard.	
			
		if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																							return (591);
			
		}	// end "if (TickCount () >= nextTime)"
	
	return (noErr);
	
}	// end "ComputeEigenvectorsSVD"


static double dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)
double dpythag (
				double								a,
				double								b)

{
	double								absa,
											absb;
							
							
	absa = fabs (a);
	absb = fabs (b);
	if (absa > absb) 
		return absa*sqrt (1.0+DSQR (absb/absa));
		
	else 
		return (absb == 0.0 ? 0.0 : absb*sqrt (1.0+DSQR (absa/absb)));
}
/* (C) Copr. 1986-92 Numerical Recipes Software w5. */



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void TransposeMatrix
//
//	Software purpose:	Transpose the input matrix
//
//	Parameters in:					.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/09/2002
//	Revised By:			Larry L. Biehl			Date: 02/09/2002	

void TransposeMatrix (
				HDoublePtr							squareInputMatrixPtr, 
				HDoublePtr							squareOutputMatrixPtr,
				UInt32 								numberFeatures)
				
{
	double								tempHolder;
	
	HDoublePtr							inputLowerLeftPtr,
											inputUpperRightPtr,
											outputLowerLeftPtr,
											outputUpperRightPtr;
	
	SInt32								lowerLeftIndexSkip,
											upperRightIndexSkip;
											
	UInt32								row,
											column;

	
	if (squareInputMatrixPtr == squareOutputMatrixPtr)
		{	
		lowerLeftIndexSkip = (SInt32)numberFeatures * (2 - (SInt32)numberFeatures) + 1;
		upperRightIndexSkip = 2;
	
		inputLowerLeftPtr = squareInputMatrixPtr + numberFeatures;
		inputUpperRightPtr = squareInputMatrixPtr + 1;
	
		for (row=2; row<=numberFeatures; row++)
			{
			for (column=row; column<=numberFeatures; column++)
				{
				tempHolder = *inputLowerLeftPtr;
				*inputLowerLeftPtr = *inputUpperRightPtr;
				*inputUpperRightPtr = tempHolder;
			 					
			 	inputUpperRightPtr++;
			 	inputLowerLeftPtr += numberFeatures;
			
				}	// end "for (column=row; column<=numberFeatures; column++)" 
						
			inputLowerLeftPtr += lowerLeftIndexSkip;
			lowerLeftIndexSkip += numberFeatures; 
			
			inputUpperRightPtr += upperRightIndexSkip;
			upperRightIndexSkip++;
				
			}	// end "for (row=2; row<=numberFeatures; row++)"
		
		}	// end "if (squareInputMatrixPtr == squareOutputMatrixPtr)"
		
	else 	// squareInputMatrixPtr != squareOutputMatrixPtr
		{
		lowerLeftIndexSkip = (SInt32)numberFeatures * (2 - (SInt32)numberFeatures) + 1;
		upperRightIndexSkip = 1;
		
		inputLowerLeftPtr = squareInputMatrixPtr + numberFeatures;
		inputUpperRightPtr = squareInputMatrixPtr;
		
		outputLowerLeftPtr = squareOutputMatrixPtr + numberFeatures;
		outputUpperRightPtr = squareOutputMatrixPtr;
		
		for (row=1; row<=numberFeatures; row++)
			{
			*outputUpperRightPtr = *inputUpperRightPtr;
			outputUpperRightPtr++;
			inputUpperRightPtr++;
			
			//outputLowerLeftPtr++;
			//inputLowerLeftPtr++;
			
			for (column=row+1; column<=numberFeatures; column++)
				{
				*outputLowerLeftPtr = *inputUpperRightPtr;
				*outputUpperRightPtr = *inputLowerLeftPtr;
			 					
			 	inputUpperRightPtr++;
			 	outputUpperRightPtr++;
			 	inputLowerLeftPtr += numberFeatures;
			 	outputLowerLeftPtr += numberFeatures;
			
				}	// end "for (column=row+1; column<=numberFeatures; column++)" 
			
			inputLowerLeftPtr += lowerLeftIndexSkip;		
			outputLowerLeftPtr += lowerLeftIndexSkip;
			lowerLeftIndexSkip += numberFeatures; 
			
			inputUpperRightPtr += upperRightIndexSkip;
			outputUpperRightPtr += upperRightIndexSkip;
			upperRightIndexSkip++;
				
			}	// end "for (row=2; row<=numberFeatures; row++)"
			
		}	// end "else squareInputMatrixPtr != squareOutputMatrixPtr" 
			
}	// end "TransposeMatrix"
