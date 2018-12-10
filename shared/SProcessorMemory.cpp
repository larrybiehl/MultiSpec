//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//									Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SProcessorMemory.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines that release memory assigned to 
//								the structure related to the various non-project processors.
//
//	Functions in file:	void			ReleaseClusterSpecsMemory
//								void			ReleaseListDataSpecsMemory
//								void			ReleaseListResultsSpecsMemory
//								void 			ReleaseNonProjProcessorStructures
//								void			ReleasePrincipalCompSpecsMemory
//								void 			ReleaseReformatSpecsMemory
//								void			ReleaseStatisticsImageSpecsMemory
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------
           
#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClearNonProjProcessorFileInfoHandleReferences
//
//	Software purpose:	The purpose of this routine is to clear any references
//							to the input fileInfoHandle. This is done when the 
//							fileInfoHandle is not valid any more.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			DisposeFileInfoHandle in SDatFile.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/01/2013
//	Revised By:			Larry L. Biehl			Date: 10/01/2013

void ClearNonProjProcessorFileInfoHandleReferences (
				Handle								fileInfoHandle)

{
	ClusterSpecsPtr					clusterSpecsPtr;
	ListResultsSpecsPtr				listResultsSpecsPtr;
						
	
	if (gNonProjProcessorSpecs.clusterSpecsH != NULL)
		{
				// Get pointer to cluster data specifications.							
				
		clusterSpecsPtr = (ClusterSpecsPtr)GetHandlePointer (
															gNonProjProcessorSpecs.clusterSpecsH);
		if (clusterSpecsPtr->fileInfoHandle == fileInfoHandle)
			clusterSpecsPtr->fileInfoHandle = NULL;
			
		}	// end "if (gNonProjProcessorSpecs.clusterSpecsH != NULL)"
		
	if (gNonProjProcessorSpecs.listResultsSpecsH != NULL)
		{
				// Get pointer to list results data specifications.							
				
		listResultsSpecsPtr = (ListResultsSpecsPtr)GetHandlePointer (
														gNonProjProcessorSpecs.listResultsSpecsH);
		if (listResultsSpecsPtr->fileInfoHandle == fileInfoHandle)
			listResultsSpecsPtr->fileInfoHandle = NULL;
			
		}	// end "if (gNonProjProcessorSpecs.listResultsSpecsH != NULL)"
		
}	// end "ClearNonProjProcessorFileInfoHandleReferences" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClearNonProjProcessorWindowInfoHandleReferences
//
//	Software purpose:	The purpose of this routine is to clear any references
//							to the input windowInfoHandle. This is done when the 
//							windowInfoHandle is not valid any more.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/01/2013
//	Revised By:			Larry L. Biehl			Date: 10/01/2013

void ClearNonProjProcessorWindowInfoHandleReferences (
				Handle								windowInfoHandle)

{
	BiPlotDataSpecsPtr				biPlotDataSpecsPtr;
	ListDataSpecsPtr					listDataSpecsPtr;
	PrincipalCompSpecsPtr			principalCompSpecsPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	StatisticsImageSpecsPtr			statisticsImageSpecsPtr;
						
	
	if (gNonProjProcessorSpecs.biPlotDataSpecsH != NULL)
		{
				// Get pointer to biplot data specifications.							
				
		biPlotDataSpecsPtr = (BiPlotDataSpecsPtr)GetHandlePointer (
														gNonProjProcessorSpecs.biPlotDataSpecsH);
		if (biPlotDataSpecsPtr->windowInfoHandle == windowInfoHandle)
			biPlotDataSpecsPtr->windowInfoHandle = NULL;
			
		}	// end "if (gNonProjProcessorSpecs.biPlotDataSpecsH != NULL)"		
		
	if (gNonProjProcessorSpecs.listDataSpecsH != NULL)
		{
				// Get pointer to list data specifications.							
				
		listDataSpecsPtr = (ListDataSpecsPtr)GetHandlePointer (
															gNonProjProcessorSpecs.listDataSpecsH);
		if (listDataSpecsPtr->windowInfoHandle == windowInfoHandle)
			listDataSpecsPtr->windowInfoHandle = NULL;
			
		}	// end "if (gNonProjProcessorSpecs.listDataSpecsH != NULL)"
		
	if (gNonProjProcessorSpecs.principalCompSpecsH != NULL)
		{
				// Get pointer to principal components data specifications.							
				
		principalCompSpecsPtr = (PrincipalCompSpecsPtr)GetHandlePointer (
													gNonProjProcessorSpecs.principalCompSpecsH);
		if (principalCompSpecsPtr->windowInfoHandle == windowInfoHandle)
			principalCompSpecsPtr->windowInfoHandle = NULL;
			
		}	// end "if (gNonProjProcessorSpecs.principalCompSpecsH != NULL)"
		
	if (gNonProjProcessorSpecs.reformatOptionsSpecsH != NULL)
		{
				// Get pointer to reformat data specifications.							
				
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
												gNonProjProcessorSpecs.reformatOptionsSpecsH);
		if (reformatOptionsPtr->inputWindowInfoHandle == windowInfoHandle)
			reformatOptionsPtr->inputWindowInfoHandle = NULL;
			
		}	// end "if (gNonProjProcessorSpecs.reformatOptionsSpecsH != NULL)"
		
	if (gNonProjProcessorSpecs.statisticsImageSpecsH != NULL)
		{
				// Get pointer to statistics image data specifications.							
				
		statisticsImageSpecsPtr = (StatisticsImageSpecsPtr)GetHandlePointer (
												gNonProjProcessorSpecs.statisticsImageSpecsH);
		if (statisticsImageSpecsPtr->windowInfoHandle == windowInfoHandle)
			statisticsImageSpecsPtr->windowInfoHandle = NULL;
			
		}	// end "if (gNonProjProcessorSpecs.statisticsImageSpecsH != NULL)"
		
}	// end "ClearNonProjProcessorWindowInfoHandleReferences" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseBiPlotDataSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the biplot data
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			BiPlotDataControl in biPlotData.c
//							LoadBiPlotDataSpecs in biPlotData.c
//
//	Coded By:			Larry L. Biehl			Date: 03/09/1994
//	Revised By:			Larry L. Biehl			Date: 12/06/1995

void ReleaseBiPlotDataSpecsMemory (
				Handle*							biPlotDataSpecsHPtr)
			
{
	BiPlotDataSpecsPtr				biPlotDataSpecsPtr;
	
	
	if (*biPlotDataSpecsHPtr != NULL)
		{
				// Get pointer to biplot data specifications.							
				
		biPlotDataSpecsPtr = (BiPlotDataSpecsPtr)GetHandlePointer (
																					*biPlotDataSpecsHPtr,
																					kLock);
			
				// Release memory for classes vector if it has been allocated.		
			
		UnlockAndDispose (biPlotDataSpecsPtr->classHandle);
			
				// Release memory for fields vector if it	 has been allocated.		
				
		UnlockAndDispose (biPlotDataSpecsPtr->symbolsHandle);
				
				// Release memory for channels vector if it has been allocated.	
				
		UnlockAndDispose (biPlotDataSpecsPtr->channelsHandle);
															
		*biPlotDataSpecsHPtr = UnlockAndDispose (*biPlotDataSpecsHPtr);
		
		}	// end "if (*biPlotDataSpecsHPtr != NULL)" 
		
}	// end "ReleaseBiPlotDataSpecsMemory"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseClusterSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the cluster specification 
//							structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CloseProjectStructure in project.c
//
//	Coded By:			Larry L. Biehl			Date: 09/14/1989
//	Revised By:			Larry L. Biehl			Date: 07/23/1997

void ReleaseClusterSpecsMemory (
				Handle*								clusterSpecsHandlePtr)		

{
	if (*clusterSpecsHandlePtr != NULL)
		{
				// Get pointer to cluster specifications.									
				
		gClusterSpecsPtr = (ClusterSpecsPtr)GetHandlePointer (*clusterSpecsHandlePtr,
																				kLock);
				
				// Release memory for cluster feature pointer vector if it has been 
				// allocated.																
				
		UnlockAndDispose (gClusterSpecsPtr->featureHandle);
				
				// Release memory for cluster channels vector if it has been allocated.																
				
		UnlockAndDispose (gClusterSpecsPtr->channelsHandle);
				
				// Release memory for cluster symbols vector if it has been allocated.																		
				
		UnlockAndDispose (gClusterSpecsPtr->symbolsHandle);
				
				// Release memory for cluster classes vector if it has been allocated.																		
				
		UnlockAndDispose (gClusterSpecsPtr->clusterClassHandle);
				
				// Release memory for the inialization class vector if it has been 		
				// allocated.																		
				
		UnlockAndDispose (gClusterSpecsPtr->initializationClassHandle);
				
				// Release handles for total covariance information.					
			
		UnlockAndDispose (gClusterSpecsPtr->totalChanStatsHandle);
		UnlockAndDispose (gClusterSpecsPtr->totalSumSquaresStatsHandle);
		
		UnlockAndDispose (gClusterSpecsPtr->probabilityFileInfoHandle);
															
		*clusterSpecsHandlePtr = UnlockAndDispose (*clusterSpecsHandlePtr);
		
		}	// end "if (*clusterSpecsHandlePtr != NULL)" 
		
	gClusterSpecsPtr = NULL;
		
}	// end "ReleaseClusterSpecsMemory"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseEvalCovarianceHandleMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the evalution covariance
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:						LoadEvaluateCovariancesSpecs in other.c
//
//	Coded By:			Larry L. Biehl			Date: 10/29/1990
//	Revised By:			Larry L. Biehl			Date: 05/13/1998

void ReleaseEvalCovarianceHandleMemory (
				Handle*								evaluateCovarianceSpecsHPtr)		

{
	EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr;
	
	
	if (*evaluateCovarianceSpecsHPtr != NULL)
		{
				// Get pointer to classification specifications.						
				
		evaluateCovarianceSpecsPtr = (EvaluateCovarianceSpecsPtr)GetHandlePointer (
																		*evaluateCovarianceSpecsHPtr,
																		kLock);
			
				// Release memory for classes handle if it has been allocated.																
				
		UnlockAndDispose (evaluateCovarianceSpecsPtr->classHandle);
				
				// Release memory for feature handle if it 		
				// has been allocated.															
				
		UnlockAndDispose (evaluateCovarianceSpecsPtr->featureHandle);
				
				// Release memory for channels handle if it has			
				// been allocated.																
				
		UnlockAndDispose (evaluateCovarianceSpecsPtr->channelsHandle);
												
		*evaluateCovarianceSpecsHPtr = 
								UnlockAndDispose (*evaluateCovarianceSpecsHPtr);
		
		}	// end "if (*evaluateCovarianceSpecsHPtr != NULL)" 
		
}	// end "ReleaseEvalCovarianceHandleMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseEvalTransformHandleMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the evaluation transform
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:						LoadEvaluateCovariancesSpecs in other.c
//
//	Coded By:			Larry L. Biehl			Date: 10/29/1990
//	Revised By:			Larry L. Biehl			Date: 05/13/1998

void ReleaseEvalTransformHandleMemory (
				Handle*								evaluateTransformSpecsHPtr)		

{
	*evaluateTransformSpecsHPtr = UnlockAndDispose (*evaluateTransformSpecsHPtr);
		
}	// end "ReleaseEvalTransformHandleMemory" 




//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseListDataSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the list data
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ListDataControl in listData.c
//							LoadListDataSpecs in listData.c
//
//	Coded By:			Larry L. Biehl			Date: 05/25/1990
//	Revised By:			Larry L. Biehl			Date: 03/01/1991

void ReleaseListDataSpecsMemory (
				Handle*								listDataSpecsHPtr)
			
{
	ListDataSpecsPtr					listDataSpecsPtr;
	
	
	if (*listDataSpecsHPtr != NULL)
		{
				// Get pointer to list data specifications.								
				
		listDataSpecsPtr = (ListDataSpecsPtr)GetHandlePointer (*listDataSpecsHPtr,
																					kLock);
			
				// Release memory for classes vector if it has been allocated.		
			
		UnlockAndDispose (listDataSpecsPtr->classHandle);
			
				// Release memory for fields vector if it has been allocated.		
				
		UnlockAndDispose (listDataSpecsPtr->fieldHandle);
				
				// Release memory for feature pointer vector if it has been 		
				// allocated.																		
				
		UnlockAndDispose (listDataSpecsPtr->featureHandle);
				
				// Release memory for channels vector if it has been allocated.	
				
		UnlockAndDispose (listDataSpecsPtr->channelsHandle);
															
		*listDataSpecsHPtr = UnlockAndDispose (*listDataSpecsHPtr);
		
		}	// end "if (*listDataSpecsHPtr != NULL)" 
		
}	// end "ReleaseListDataSpecsMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseListResultsSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the list results
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			LoadListResultsSpecs in ListResults.c
//
//	Coded By:			Larry L. Biehl			Date: 02/18/1991
//	Revised By:			Larry L. Biehl			Date: 06/09/1993

void ReleaseListResultsSpecsMemory (
				Handle*								listResultsSpecsHandlePtr)
			
{
	ListResultsSpecsPtr				listResultsSpecsPtr;
	
	
	if (*listResultsSpecsHandlePtr != NULL)
		{
				// Get pointer to classification specifications.						
				
		listResultsSpecsPtr = (ListResultsSpecsPtr)GetHandlePointer (
																			*listResultsSpecsHandlePtr,
																			kLock);
			
				// Release memory for list results class area vector if it has		
				// been allocated.																
				
		UnlockAndDispose (listResultsSpecsPtr->classAreaHandle);
			
				// Release memory for list results classes vector if it has			
				// been allocated.																
				
		UnlockAndDispose (listResultsSpecsPtr->classHandle);
														
		*listResultsSpecsHandlePtr = UnlockAndDispose (*listResultsSpecsHandlePtr);
		
		}	// end "if (*listResultsSpecsHandlePtr != NULL)" 
		
}	// end "ReleaseListResultsSpecsMemory" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseNonProjProcessorStructures
//
//	Software purpose:	The purpose of this routine is to release the
//							structure memory for non-project processors.  A check
//							will be made to be sure that the structure is not
//							being used before releasing.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			ExitMultiSpec in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 02/20/1991
//	Revised By:			Larry L. Biehl			Date: 01/31/2013

void ReleaseNonProjProcessorStructures (void)

{
			// Dispose of biplot specifications if they exist.						
	
	if (gProcessorCode != kBiPlotDataProcessor)		
		ReleaseBiPlotDataSpecsMemory (&gNonProjProcessorSpecs.biPlotDataSpecsH);
		
			// Dispose of cluster specifications if they exist.						
	
	if (gProcessorCode != kClusterProcessor)		
		ReleaseClusterSpecsMemory (&gNonProjProcessorSpecs.clusterSpecsH);
		
			// Dispose of list data specifications if they exist.						
	
	if (gProcessorCode != kListDataProcessor)		
		ReleaseListDataSpecsMemory (&gNonProjProcessorSpecs.listDataSpecsH);
		
			// Dispose of list results specifications if they exist.					
				
	if (gProcessorCode != kListResultsProcessor)	
		ReleaseListResultsSpecsMemory (&gNonProjProcessorSpecs.listResultsSpecsH);
		
			// Dispose of principal components specifications if they exist.		
				
	if (gProcessorCode != kPrincipalComponentsProcessor)
		ReleasePrincipalCompSpecsMemory (
											&gNonProjProcessorSpecs.principalCompSpecsH);
		
			// Dispose of reformat processor specifications if they exist.		
				
	if (kReformatProcessor != 0 &&
			gProcessorCode < kRefChangeHeaderProcessor && 
					gProcessorCode > kRecodeThematicImageProcessor)		
		ReleaseReformatSpecsMemory (&gNonProjProcessorSpecs.reformatOptionsSpecsH);
		
			// Dispose of statistics image specifications if they exist.		
				
	if (gProcessorCode != kStatisticsImageProcessor)	
		ReleaseStatisticsImageSpecsMemory (
											&gNonProjProcessorSpecs.statisticsImageSpecsH);
		
			// Dispose of utility specifications if they exist.		
											
	if (gProcessorCode != kTransformCheckProcessor)	
		ReleaseEvalTransformHandleMemory (
											&gNonProjProcessorSpecs.evalTransformationSpecsH);		
		
}	// end "ReleaseNonProjProcessorStructures" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleasePrincipalCompSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the princpal component
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			PrincipalComponentControl in principalComponent.c
//							LoadPrincipalComponentSpecs in principalComponent.c
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1990
//	Revised By:			Larry L. Biehl			Date: 03/02/1991

void ReleasePrincipalCompSpecsMemory (
				Handle*								principalCompSpecsHPtr)
			
{
	PrincipalCompSpecsPtr			principalCompSpecsPtr;
	
	
	if (*principalCompSpecsHPtr != NULL)
		{
				// Get pointer to principal component specifications.					
				
		principalCompSpecsPtr = (PrincipalCompSpecsPtr)GetHandlePointer (
																				*principalCompSpecsHPtr,
																				kLock);
			
				// Release memory for principal component classes vector if it has been 
				// allocated.															
		
		UnlockAndDispose (principalCompSpecsPtr->classHandle);
				
				// Release memory for principal component feature pointer vector if it 
				// has been allocated.											
				
		UnlockAndDispose (principalCompSpecsPtr->featureHandle);
				
				// Release memory for principal component channels vector if it has been 
				// allocated.														
				
		UnlockAndDispose (principalCompSpecsPtr->channelsHandle);
			
				// Make certain that memory for the covariance matrix and mean vector 
				// have been released.										
				
		UnlockAndDispose (principalCompSpecsPtr->totalSumSquaresStatsHandle);
		UnlockAndDispose (principalCompSpecsPtr->totalChanStatsHandle);
															
		*principalCompSpecsHPtr = UnlockAndDispose (*principalCompSpecsHPtr);
		
		}	// end "if (*principalCompSpecsHPtr != NULL)" 
		
}	// end "ReleasePrincipalCompSpecsMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseReformatSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the reformat
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			AreasToThematicFileControl in fieldsToThematicFile.c
//							LoadAreasToThematicSpecs in fieldsToThematicFile.c
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1990
//	Revised By:			Larry L. Biehl			Date: 04/10/2013

void ReleaseReformatSpecsMemory (
				Handle*								reformatOptionsHPtr)
			
{
	ReformatOptionsPtr				reformatOptionsPtr;
	
	
	if (*reformatOptionsHPtr != NULL)
		{
				// Get pointer to reformat data specifications.							
				
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (*reformatOptionsHPtr, 
																						kLock);
		
		if (reformatOptionsPtr->rectifyImageOptionsPtr)
			reformatOptionsPtr->rectifyImageOptionsPtr->rectifyChannelPtr = 
						CheckAndDisposePtr (reformatOptionsPtr->
										rectifyImageOptionsPtr->rectifyChannelPtr);
			
				// Release memory for rectification parameters if it has been allocated.															
				
		reformatOptionsPtr->rectifyImageOptionsPtr = (RectifyImageOptionsPtr)
			CheckAndDisposePtr ((Ptr)reformatOptionsPtr->rectifyImageOptionsPtr);
				
				// Release memory for reformat feature pointer vector if it has been 
				// allocated.											
				
		//reformatOptionsPtr->channelPtr = CheckAndDisposePtr (
		//															reformatOptionsPtr->channelPtr);
		UnlockAndDispose (reformatOptionsPtr->channelHandle);
				
				// Release memory for transform coeffiecents vector if it has been 
				// allocated.														
				
		//reformatOptionsPtr->coefficientsPtr = CheckAndDisposePtr (
		//													reformatOptionsPtr->coefficientsPtr);
		UnlockAndDispose (reformatOptionsPtr->coefficientsHandle);
				
				// Release memory for reformat principal component pointer vector if it 
				// has been allocated.											
				
		//reformatOptionsPtr->pcChannelPtr = CheckAndDisposePtr (
		//													reformatOptionsPtr->pcChannelPtr);
		UnlockAndDispose (reformatOptionsPtr->pcChannelHandle);
				
				// Release memory for transform channels vector if it has been allocated.														
				
		//reformatOptionsPtr->transformChannelPtr = CheckAndDisposePtr (
		//												reformatOptionsPtr->transformChannelPtr);
		UnlockAndDispose (reformatOptionsPtr->transformChannelHandle);
				
				// Release memory for transform channels vector if it has been allocated.														
				
		//reformatOptionsPtr->transformOperatorPtr = CheckAndDisposePtr (
		//												reformatOptionsPtr->transformOperatorPtr);
		UnlockAndDispose (reformatOptionsPtr->transformOperatorHandle);
			
				// Release memory for reformat classes vector if it has been allocated.															
				
		reformatOptionsPtr->classPtr = CheckAndDisposePtr (
													reformatOptionsPtr->classPtr);
			
				// Release memory for the output IO buffer if it has been			
				// allocated.																		
				
		reformatOptionsPtr->ioOutBufferPtr = (HUCharPtr)CheckAndDisposePtr (
											(Ptr)reformatOptionsPtr->ioOutBufferPtr);
		reformatOptionsPtr->ioOutAdjustBufferPtr = NULL;
		/*									
				// If the input file information pointer is given make sure that
				// the channel description handle or class description handle 
				// is not disposed of if it is that same as that for the input
				// file information structure.
				
		if (inputFileInfoPtr != NULL)
			{										
			outputFileInfoPtr = (FileInfoPtr)GetHandlePointer (
														reformatOptionsPtr->outFileInfoHandle);
			
			if (outputFileInfoPtr != NULL)
				{
				if (outputFileInfoPtr->channelDescriptionH == 
															inputFileInfoPtr->channelDescriptionH)
					outputFileInfoPtr->channelDescriptionH = NULL;
				
				if (outputFileInfoPtr->classDescriptionH == 
															inputFileInfoPtr->classDescriptionH)
					outputFileInfoPtr->classDescriptionH = NULL;	
			
				}	// end "if (outputFileInfoPtr != NULL)"
			
			}	// end "if (inputFileInfoPtr != NULL)"
		*/	
				// Release memory that was allocated for the output file information.	
		 
		DisposeFileInfoHandle (reformatOptionsPtr->outFileInfoHandle);
		reformatOptionsPtr->outFileInfoHandle = NULL;
			
		*reformatOptionsHPtr = UnlockAndDispose (*reformatOptionsHPtr);
		
		}	// end "if (reformatOptionsHPtr != NULL)" 
		
}	// end "ReleaseReformatSpecsMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseStatisticsImageSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the statistics image
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			StatisticsImageControl in statisticsImage.c
//							LoadStatisticsImageSpecs in statisticsImage.c
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1991
//	Revised By:			Larry L. Biehl			Date: 11/19/1991

void ReleaseStatisticsImageSpecsMemory (
				Handle*								statisticsImageSpecsHPtr)
			
{
	StatisticsImageSpecsPtr			statisticsImageSpecsPtr;
	
	
	if (*statisticsImageSpecsHPtr != NULL)
		{
				// Get pointer to statistics image specifications.						
				
		statisticsImageSpecsPtr = (StatisticsImageSpecsPtr)GetHandlePointer (
																			*statisticsImageSpecsHPtr,
																			kLock);
			
				// Release memory for statistics image classes vector if it has been 
				// allocated.															
		
		UnlockAndDispose (statisticsImageSpecsPtr->classHandle);
			
				// Release memory for statistics image field vector if it has been 
				// allocated.															
		
		UnlockAndDispose (statisticsImageSpecsPtr->fieldHandle);
				
				// Release memory for statistics image feature pointer vector if it has 
				// been allocated.											
				
		UnlockAndDispose (statisticsImageSpecsPtr->featureHandle);
				
				// Release memory for statistics image channels vector if it has been 
				// allocated.														
				
		UnlockAndDispose (statisticsImageSpecsPtr->channelsHandle);
															
		*statisticsImageSpecsHPtr = UnlockAndDispose (*statisticsImageSpecsHPtr);
		
		}	// end "if (*statisticsImageSpecsHPtr != NULL)" 
		
}	// end "ReleaseStatisticsImageSpecsMemory" 
