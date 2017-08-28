//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SHisInfo.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		2.9
//
//	Revision date:			11/02/1999
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The purpose of the routines in this file is to
//								provide routines to support the Histogram Specs structure.
//
//	Functions in file:	

#include	"SMulSpec.h" 
  
#if defined multispec_win
#endif	// defined multispec_win    

#include "SExtGlob.h"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseHistogramSupportMemory
//
//	Software purpose:	
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	06/12/1995
//	Revised By:			Larry L. Biehl			Date: 11/02/1999

void ReleaseHistogramSupportMemory(
				WindowInfoPtr						windowInfoPtr)

{  
	Handle								histogramSpecsHandle,
											histogramSummaryHandle;
	
	
	if (windowInfoPtr != NULL)
		{                          
		histogramSpecsHandle = GetHistogramSpecsHandle (windowInfoPtr);
		histogramSummaryHandle = GetHistogramSummaryHandle (windowInfoPtr);
		
		ReleaseHistogramSupportMemory (&histogramSpecsHandle, &histogramSummaryHandle);
		
		SetHistogramSpecsHandle (windowInfoPtr, histogramSpecsHandle);
		SetHistogramSummaryHandle (windowInfoPtr, histogramSummaryHandle);
		
		}		// end "if (windowInfoPtr != NULL)"

}		// end "ReleaseHistogramSupportMemory"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseHistogramSupportMemory
//
//	Software purpose:	
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	06/12/1995
//	Revised By:			Larry L. Biehl			Date: 11/02/1999

void ReleaseHistogramSupportMemory (
				Handle*								histogramSpecsHandlePtr,
				Handle*								histogramSummaryHandlePtr)

{  
	SignedByte							handleStatus;
	
	                            
	HistogramSpecsPtr histogramSpecsPtr = (HistogramSpecsPtr)             
			GetHandleStatusAndPointer (*histogramSpecsHandlePtr, &handleStatus, kNoMoveHi);
	
	if (histogramSpecsPtr != NULL)
		{                                              
		histogramSpecsPtr->histogramArrayH = 
											UnlockAndDispose (histogramSpecsPtr->histogramArrayH);
								
		histogramSpecsPtr->channelsHandle =
											UnlockAndDispose (histogramSpecsPtr->channelsHandle); 
								
		histogramSpecsPtr->initializeStructureFlag = TRUE;
		
		MHSetState (*histogramSpecsHandlePtr, handleStatus);
		
		}		// end "if (histogramSpecsPtr != NULL)"
		
	*histogramSummaryHandlePtr = UnlockAndDispose (*histogramSummaryHandlePtr);
			
	#if defined multispec_mac 
		*histogramSpecsHandlePtr = UnlockAndDispose (*histogramSpecsHandlePtr);
	#endif	// defined multispec_win 

}		// end "ReleaseHistogramSupportMemory"   

