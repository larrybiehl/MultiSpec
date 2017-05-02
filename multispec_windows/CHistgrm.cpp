//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	File:						MHistgrm.cpp
//
//	Class Definition:		MHistgrm.h	<- double-click + Command-D to open
//
//	Functions in file:	Boolean 					CheckHistogramTextWindowSpaceNeeded
//								void 						CloseUpHistogramArrayReadParameters
//								Boolean 					ComputeHistogram
//								long int 				CountTotalNumberHistogramPixels
//								void 						CreateSTASupportFile
//								void 						DecodeSTASupportFile
//								long int					DetermineBytesForHistogramText
//								pascal void 			DrawHistogramMethodPopUp
//								Boolean 					DoHistogramRequests
//								void						ForceHistogramCodeResourceLoad
//								char* 					GetSTASupportFileBuffer
//								Boolean 					GetSTASupportFile
//								unsigned long int*	GetHistogramValuesMemory
//								void 						HistogramControl
//								Boolean 					HistogramDialog
//								void 						HistogramDialogStatisticsFile
//								HistogramBlockPtr 	InitializeHistogramInfoStructure
//								Boolean 					ListHistogramSummary
//								Boolean 					ListHistogramTitle
//								Boolean 					ListHistogramValues
//								Boolean 					ListHistogramValuesInColumns
//								Boolean 					ListHistogramValuesInLines
//								void 						LoadSTASupportFile
//								void 						LoadHistogramSpecs	
//								Boolean 					ReadSTASupportFile
//								void 						SetListAndEmptyBinsDialogBoxes
//								Boolean 					SetUpToReadHistogramArray
//								Boolean 					UpdateAllChannelsAtOnceFlag
//
//	Brief description:	The purpose of the routines in this file is to
//								determine the user specifications for the image 
//								histogram, compute and 
//								manage the use of the image histograms.  A diagram of
//								the use of the functions follows:
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		HistogramControl
//			LoadHistogramSpecs
//				SetSelectedAreaInfo (in selectionArea.c)
//			HistogramDialog
//				LoadDItemValue (in multiSpecUtilities.c)
//				CheckSomeEvents (in multiSpec.c)
//				HiliteOKButton2 (in multiSpecUtilities.c)
//				GetERDAS_staFile
//					GetFile (in fileIO.c)
//					IOCheck (fileIO.c)
//			CheckSomeEvents (in multiSpec.c)
//			ReadERDAS_staFile
//				ConvertATRealtoReal (in fileIO.c)
//				ConvertATRealtoInt (in fileIO.c)
//				MoveBytes (in multiSpecUtilities.c)
//				CloseFile (in fileIO.c)
//				UnlockAndDispose (in multiSpecUtilities.c)
//			LoadDItemString (in multiSpecUtilities.c)
//			ComputeHistogram
//				UnlockAndDispose (in multiSpecUtilities.c)
//			CreateERDAS_staFile
//				CloseFile (in fileIO.c)
//				CopyPToP (in multiSpecUtilities.c)
//				AppendPascaltoPascal (in multiSpecUtilities.c)
//				PutFile (in fileIO.c)
//				IOCheck (in fileIO.c)
//				MoveBytes (in multiSpecUtilities.c)
//				ConvertRealAT (in fileIO.c)
//				UnlockAndDispose (in multiSpecUtilities.c)
//			ListHistogramSummary
//				OutputString (in multiSpecUtilities.c)
//			ListHistogramValues
//				OutputString (in multiSpecUtilities.c)
//				UnlockAndDispose (in multiSpecUtilities.c)
//			UpdateOutputWScrolls
//			UnlockAndDispose (in multiSpecUtilities.c)
//
//	Language:				C++
//
//	System:					Macintosh Operating System
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision number:		2.7
//
//	Revision date:			09/03/2013

#include "SMulSpec.h"

#include "CImagWin.h"
#include "CHistgrm.h"
#ifndef multispec_lin
//#include "CFileDlg.h"
#include "WImagDoc.h"
#include "CImagVew.h"
#endif
#include "CDisplay.h"

#if defined multispec_lin
#include "LImageDoc.h"
#include "LImageView.h"
#endif

// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN

#if defined _X86_ || defined _AMD64_
	#include "WHistDlg.h"
#endif   //	!defined __MWERKS__

#include	"SExtGlob.h"   

extern void 		ReleaseHistogramSupportMemory(
							Handle*								histogramSpecsHandlePtr,
							Handle*								histogramSummaryHandlePtr);                 

// === Static Member Variable ===

HistogramSpecsPtr		CMHistogram::sHistogramSpecsPtr = NULL;

HistogramSummaryPtr	CMHistogram::sHistogramSummaryPtr = NULL;


                                                     					

// ---------------------------------------------------------------------------
//		CMHistogram 
// ---------------------------------------------------------------------------
//	Constructor

CMHistogram::CMHistogram()
{
	mHistogramSummaryHandle = NULL;
	m_initializedFlag = FALSE;
			
			// Get memory for the histogram specification structure and indicate
			// that it will need to be intialized.
		
	mHistogramSpecsHandle = MNewHandle( sizeof(HistogramSpecs) );
	
	if (mHistogramSpecsHandle != NULL)
		{
		HistogramSpecsPtr histogramSpecsPtr = (HistogramSpecsPtr)
					GetHandlePointer(mHistogramSpecsHandle, kNoLock, kNoMoveHi);
		histogramSpecsPtr->initializeStructureFlag = TRUE;
		histogramSpecsPtr->histogramArrayH = NULL;
		histogramSpecsPtr->channelsHandle = NULL;
		
		m_initializedFlag = TRUE;
		
		}		// end "if (mHistogramSpecsHandle != NULL)"

}		// end "CMHistogram"


// ---------------------------------------------------------------------------
//		~CMHistogram
// ---------------------------------------------------------------------------
//	Destructor
//

CMHistogram::~CMHistogram()
{  
	::ReleaseHistogramSupportMemory (&mHistogramSpecsHandle, 
													&mHistogramSummaryHandle);
		
	UnlockAndDispose (mHistogramSpecsHandle); 

}		// end "~CMHistogram" 

									
/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl			Date: 06/27/95	
//	Revised By:			Larry L. Biehl			Date: 06/27/95	

Boolean 
CMHistogram::Dialog (void)

{
	Boolean		continueFlag = FALSE;
	
	            
			// Make certain that we have an active image window class pointer.
			    		
	if (gActiveImageViewCPtr == NULL)
																			return (continueFlag);
	
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
	
	#if !defined _X86_ && !defined _AMD64_
		
		continueFlag = TRUE; 
	
	#endif // !defined _X86_ && !defined _AMD64_
	
	#if defined _X86_ || defined _AMD64_
	
		CMHistogramSpecsDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMHistogramSpecsDlg(); 
			                  
			continueFlag = dialogPtr->DoDialog (sHistogramSpecsPtr);
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL(e)
			{
			MemoryMessage(0, kObjectMessage);
			}
		END_CATCH_ALL 
	
	#endif // defined _X86_ || defined _AMD64_
	
	return (continueFlag);
		     
}		// end "Dialog"
*/

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	05/09/95
//	Revised By:	Larry L. Biehl					Date: 05/09/95

HUInt32Ptr 
CMHistogram::GetHistogramArrayPtr (void)

{
	return (sHistogramSpecsPtr->histogramArrayPtr);

}		// end "GetHistogramArrayPtr" 
*/

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl					Date:	05/09/95
//	Revised By:			Larry L. Biehl					Date: 05/09/95

Boolean 
CMHistogram::GetHistogramChangedSinceDisplayFlag (void)

{
	return (sHistogramSpecsPtr->histogramChangedSinceDisplayFlag);
	
}		// end "GetHistogramChangedSinceDisplayFlag"
*/

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl					Date:	05/09/95
//	Revised By:			Larry L. Biehl					Date: 05/09/95

Boolean 
CMHistogram::GetHistogramLoadedFlag (void)

{
	return (sHistogramSpecsPtr->loadedFlag);
	
}		// end "GetHistogramLoadedFlag"
*/


//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	04/26/95
//	Revised By:	Larry L. Biehl					Date: 04/26/95

HistogramSpecsPtr 
CMHistogram::GetHistogramSpecsPtr (void)

{
	return (sHistogramSpecsPtr);

}		// end "GetHistogramSpecsPtr" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseHistogramSpecsPtr
//
//	Software purpose:	The purpose of this routine is to unlock the
//							histogram specs handlee and set the static pointer
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
//	Coded By:	Larry L. Biehl					Date:	04/26/95
//	Revised By:	Larry L. Biehl					Date: 05/09/95

void 
CMHistogram::ReleaseHistogramSpecsPtr (void)

{
	if (sHistogramSpecsPtr != NULL)
		CheckAndUnlockHandle (sHistogramSpecsPtr->channelsHandle);
		
	CheckAndUnlockHandle (mHistogramSpecsHandle);
	sHistogramSpecsPtr = NULL;
	
	CheckAndUnlockHandle (mHistogramSummaryHandle);
	sHistogramSummaryPtr = NULL;

}		// end "ReleaseHistogramSpecsPtr" 
									


//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
// Called By:			~CMHistogram in MHistGrm.cpp
//
//	Coded By:			Larry L. Biehl					Date:	06/12/95
//	Revised By:			Larry L. Biehl					Date: 11/02/99

void
CMHistogram::ReleaseHistogramSupportMemory(void)

{  			
	::ReleaseHistogramSupportMemory (&mHistogramSpecsHandle, &mHistogramSummaryHandle);
	
}		// end "ReleaseHistogramSupportMemory" 


/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl					Date:	05/09/95
//	Revised By:			Larry L. Biehl					Date: 05/09/95

void 
CMHistogram::SetHistogramChangedSinceDisplayFlag (
				Boolean					setting)

{
	sHistogramSpecsPtr->histogramChangedSinceDisplayFlag = setting;
	
}		// end "SetHistogramChangedSinceDisplayFlag"
*/


//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl					Date:	04/26/95
//	Revised By:			Larry L. Biehl					Date: 04/26/95

Boolean 
CMHistogram::SetUpHistogramSpecsPtr (void)

{
	Boolean		continueFlag = FALSE;
	
	
	sHistogramSpecsPtr = (HistogramSpecsPtr)
						GetHandlePointer (mHistogramSpecsHandle, kLock, kMoveHi);
	
//			// Make certain that the historgram structure is valid.
//			
//	continueFlag = InitializeHistogramInfoStructure (kNoSetUpHistogramArray);
//	
//	if (continueFlag)
//		{
		sHistogramSummaryPtr = (HistogramSummaryPtr)
						GetHandlePointer (mHistogramSummaryHandle, kLock, kMoveHi);
						
//		sHistogramSpecsPtr->histogramSummaryPtr = sHistogramSummaryPtr; 
//						
		continueFlag = (sHistogramSummaryPtr != NULL);
						
//		}		// end "if (continueFlag)"
	
	return (continueFlag); 
	
}		// end "SetUpHistogramSpecsPtr"

