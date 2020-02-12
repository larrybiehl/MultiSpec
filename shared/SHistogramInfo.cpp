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
//	File:						SHistogramInfo.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide routines to support the Histogram Specs structure.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"  

#if defined multispec_wx
#endif
  
#if defined multispec_win
#endif	// defined multispec_win 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
		
		}	// end "if (windowInfoPtr != NULL)"

}	// end "ReleaseHistogramSupportMemory"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
			GetHandleStatusAndPointer (*histogramSpecsHandlePtr, &handleStatus);
	
	if (histogramSpecsPtr != NULL)
		{                                              
		histogramSpecsPtr->histogramArrayH = 
											UnlockAndDispose (histogramSpecsPtr->histogramArrayH);
								
		histogramSpecsPtr->channelsHandle =
											UnlockAndDispose (histogramSpecsPtr->channelsHandle); 
								
		histogramSpecsPtr->initializeStructureFlag = TRUE;
		
		MHSetState (*histogramSpecsHandlePtr, handleStatus);
		
		}	// end "if (histogramSpecsPtr != NULL)"
		
	*histogramSummaryHandlePtr = UnlockAndDispose (*histogramSummaryHandlePtr);
			
	#if defined multispec_mac 
		*histogramSpecsHandlePtr = UnlockAndDispose (*histogramSpecsHandlePtr);
	#endif	// defined multispec_win 

}	// end "ReleaseHistogramSupportMemory"   

