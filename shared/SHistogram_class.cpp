//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SHistogram_class.cpp
//	Class Definition:		SHistogram_class.h
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux, MacOS and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to provide a
//								class for supporting the histogram structure.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SDisplay_class.h"
#include "SHistogram_class.h"
#include "SImageWindow_class.h"

#ifdef multispec_win
	#include "WHistogramDialog.h"
	#include "WImageDoc.h"
	#include "WImageView.h"
#endif

#if defined multispec_wx
	#include "xImageDoc.h"
	#include "xImageView.h"
#endif


// === Static Member Variable ===

HistogramSpecsPtr		CMHistogram::sHistogramSpecsPtr = NULL;

HistogramSummaryPtr	CMHistogram::sHistogramSummaryPtr = NULL;


                                                     					

//------------------------------------------------------------------------------------
//								 Copyright (1988-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMDisplay
//
//	Software purpose:	Constructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMHistogram::CMHistogram ()

{
	mHistogramSummaryHandle = NULL;
	m_initializedFlag = FALSE;
			
			// Get memory for the histogram specification structure and indicate
			// that it will need to be intialized.
		
	mHistogramSpecsHandle = MNewHandle (sizeof (HistogramSpecs));
	
	if (mHistogramSpecsHandle != NULL)
		{
		HistogramSpecsPtr histogramSpecsPtr =
								(HistogramSpecsPtr)GetHandlePointer (mHistogramSpecsHandle);
		histogramSpecsPtr->initializeStructureFlag = TRUE;
		histogramSpecsPtr->histogramArrayH = NULL;
		histogramSpecsPtr->channelsHandle = NULL;
		
		m_initializedFlag = TRUE;
		
		}	// end "if (mHistogramSpecsHandle != NULL)"

}	// end "CMHistogram"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		~CMDisplay
//
//	Software purpose:	Destructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMHistogram::~CMHistogram ()

{  
	::ReleaseHistogramSupportMemory (&mHistogramSpecsHandle, &mHistogramSummaryHandle);
		
	UnlockAndDispose (mHistogramSpecsHandle); 

}	// end "~CMHistogram"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		HistogramSpecsPtr GetHistogramSpecsPtr
//
//	Software purpose:	The purpose of this routine is to return the
//							current static histogram specs pointer.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	04/26/1995
//	Revised By:	Larry L. Biehl					Date: 04/26/1995

HistogramSpecsPtr CMHistogram::GetHistogramSpecsPtr (void)

{
	return (sHistogramSpecsPtr);

}	// end "GetHistogramSpecsPtr" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseHistogramSpecsPtr
//
//	Software purpose:	The purpose of this routine is to unlock the
//							histogram specs handle and set the static pointer
//							to the block of memory to NULL.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	04/26/1995
//	Revised By:	Larry L. Biehl					Date: 05/09/1995

void CMHistogram::ReleaseHistogramSpecsPtr (void)

{
	if (sHistogramSpecsPtr != NULL)
		CheckAndUnlockHandle (sHistogramSpecsPtr->channelsHandle);
		
	CheckAndUnlockHandle (mHistogramSpecsHandle);
	sHistogramSpecsPtr = NULL;
	
	CheckAndUnlockHandle (mHistogramSummaryHandle);
	sHistogramSummaryPtr = NULL;

}	// end "ReleaseHistogramSpecsPtr" 
									


//------------------------------------------------------------------------------------
//								 Copyright (1988-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseHistogramSupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the histogram
//							support memory - handles for channel vector and histogram
//							summary vector.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			DisposeImageWindowSupportMemory in xImageView.cpp
//
//	Coded By:			Larry L. Biehl					Date:	06/12/1995
//	Revised By:			Larry L. Biehl					Date: 11/02/1999

void CMHistogram::ReleaseHistogramSupportMemory (void)

{  			
	::ReleaseHistogramSupportMemory (&mHistogramSpecsHandle, &mHistogramSummaryHandle);
	
}	// end "ReleaseHistogramSupportMemory"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpHistogramSpecsPtr
//
//	Software purpose:	The purpose of this routine is to move the
//							histogram specs handle block high in memory,
//							lock the handle and set the static pointer
//							to the block of memory.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl					Date:	04/26/1995
//	Revised By:			Larry L. Biehl					Date: 04/26/1995

Boolean CMHistogram::SetUpHistogramSpecsPtr (void)

{
	Boolean		continueFlag = FALSE;
	
	
	sHistogramSpecsPtr =
			(HistogramSpecsPtr)GetHandlePointer (mHistogramSpecsHandle, kLock, kMoveHi);
	
	sHistogramSummaryPtr = (HistogramSummaryPtr)
									GetHandlePointer (mHistogramSummaryHandle, kLock, kMoveHi);
						
	continueFlag = (sHistogramSummaryPtr != NULL);
	
	return (continueFlag); 
	
}	// end "SetUpHistogramSpecsPtr"
