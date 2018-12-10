//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CHistogram.cpp
//	Class Definition:		CHistogram.h
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to provide a
//								class for supporting the histogram structure.
//
//	Functions in file:								CMHistogram
//															~CMHistogram
//								HistogramSpecsPtr		GetHistogramSpecsPtr
//								void						ReleaseHistogramSpecsPtr
//								void						ReleaseHistogramSupportMemory
//								Boolean					SetUpHistogramSpecsPtr
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "CDisplay.h"
#include "CHistogram.h"
#include "CImageWindow.h"

#ifdef multispec_win
	#include "WHistogramDialog.h"
	#include "WImageDoc.h"
	#include "WImageView.h"
#endif

#if defined multispec_lin
	#include "LImageDoc.h"
	#include "LImageView.h"
#endif

extern void ReleaseHistogramSupportMemory (
				Handle*								histogramSpecsHandlePtr,
				Handle*								histogramSummaryHandlePtr);                 

// === Static Member Variable ===

HistogramSpecsPtr		CMHistogram::sHistogramSpecsPtr = NULL;

HistogramSummaryPtr	CMHistogram::sHistogramSummaryPtr = NULL;


                                                     					

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
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
//								 Copyright (1988-2018)
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

									
/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean Dialog
//
//	Software purpose:	The purpose of this routine is to handle the modal dialog
//							for confirming the histogram specification.  The modal
//							dialog template is in the resource file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:			HistogramControl in MHistgrm.c
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1995	
//	Revised By:			Larry L. Biehl			Date: 06/27/1995	

Boolean CMHistogram::Dialog (void)

{
	Boolean		continueFlag = FALSE;
	
	            
			// Make certain that we have an active image window class pointer.
			    		
	if (gActiveImageViewCPtr == NULL)
																					return (continueFlag);
	
	#if !defined _X86_
		continueFlag = TRUE;
	#endif // !defined _X86_ 
	
	#if defined _X86_
		CMHistogramSpecsDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMHistogramSpecsDlg ();
			                  
			continueFlag = dialogPtr->DoDialog (sHistogramSpecsPtr);
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif // defined _X86_ 
	
	return (continueFlag);
		     
}	// end "Dialog"
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32* GetHistogramArrayPtr
//
//	Software purpose:	The purpose of this routine is to return the
//							current static histogram array pointer.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	05/09/1995
//	Revised By:	Larry L. Biehl					Date: 05/09/1995

HUInt32Ptr CMHistogram::GetHistogramArrayPtr (void)

{
	return (sHistogramSpecsPtr->histogramArrayPtr);

}	// end "GetHistogramArrayPtr" 
*/

/*
//------------------------------------------------------------------------------------
/								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetHistogramChangedSinceDisplayFlag
//
//	Software purpose:	The purpose of this routine is to return the flag indicating
//							whether the histogram has been changed since the last
//							display of the image.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	HistogramChangedSinceDisplayFlag	
//
// Called By:
//
//	Coded By:			Larry L. Biehl					Date:	05/09/1995
//	Revised By:			Larry L. Biehl					Date: 05/09/1995

Boolean CMHistogram::GetHistogramChangedSinceDisplayFlag (void)

{
	return (sHistogramSpecsPtr->histogramChangedSinceDisplayFlag);
	
}	// end "GetHistogramChangedSinceDisplayFlag"
*/

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetHistogramLoadedFlag
//
//	Software purpose:	The purpose of this routine is to return the flag indicating
//							whether the histogram summary has been loaded into memory.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	HistogramChangedSinceDisplayFlag	
//
// Called By:
//
//	Coded By:			Larry L. Biehl					Date:	05/09/1995
//	Revised By:			Larry L. Biehl					Date: 05/09/1995

Boolean CMHistogram::GetHistogramLoadedFlag (void)

{
	return (sHistogramSpecsPtr->loadedFlag);
	
}	// end "GetHistogramLoadedFlag"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
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
//								 Copyright (1988-2018)
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
//								 Copyright (1988-2018)
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
// Called By:			~CMHistogram
//
//	Coded By:			Larry L. Biehl					Date:	06/12/1995
//	Revised By:			Larry L. Biehl					Date: 11/02/1999

void CMHistogram::ReleaseHistogramSupportMemory (void)

{  			
	::ReleaseHistogramSupportMemory (&mHistogramSpecsHandle, &mHistogramSummaryHandle);
	
}	// end "ReleaseHistogramSupportMemory" 


/*
///------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetHistogramChangedSinceDisplayFlag
//
//	Software purpose:	The purpose of this routine is to set the flag indicating
//							whether the histogram has been changed since the last
//							display of the image.
//
//	Parameters in:		setting value to be used (either TRUE or FALSE)	
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl					Date:	05/09/1995
//	Revised By:			Larry L. Biehl					Date: 05/09/1995

void CMHistogram::SetHistogramChangedSinceDisplayFlag (
				Boolean										setting)

{
	sHistogramSpecsPtr->histogramChangedSinceDisplayFlag = setting;
	
}	// end "SetHistogramChangedSinceDisplayFlag"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
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

