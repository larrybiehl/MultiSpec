//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SWindowInfo.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The routines in this file handle retrieving information from
//								the window information structure.
//
//	Functions in file:	Boolean 				CheckIfEntireImage
//								Handle 				GetActiveDisplaySpecsHandle
//								Handle 				GetActiveImageFileInfoHandle
//								Handle 				GetActiveImageWindowInfoHandle
//								ListHandle 			GetActiveLegendListHandle
//								CMFileStream* 		GetActiveSupportFileStreamPointer
//								Handle 				GetDisplaySpecsHandle
//								SInt16 				GetDisplayClassGroupCode
//								Handle 				GetFileInfoHandle
//								CMFileStream* 		GetFileStreamPointer
//								Handle 				GetHistogramSpecsHandle
//								SInt16 				GetLegendWidth
//								Boolean 				GetProjectBaseImageFlag
//								Boolean 				GetProjectWindowFlag
//								Boolean 				GetShowLegendFlag
//								CMFileStream* 		GetSupportFileStreamPointer
//								SInt16 				GetTitleHeight
//								Handle 				GetWindowInfoHandle
//								Handle 				GetWindowInfoStructures
//								WindowPtr 			GetWindowPtr
//								SInt16 				GetWindowType
//								Handle 				InitializeWindowInfoStructure
//								void 					ReleaseOffscreenSupportMemory
//								void 					SetImageWindowIsUpToDateFlag
//								void 					UnlockActiveSupportFileStream
//								void 					UnlockFileStream
//								void 					UnlockSupportFileStream
//
//	Include files:			"MultiSpecHeaders"
//								"SMultiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"

	#include "wx/wx.h"
	#include "LImageView.h"
	#include "LImageDoc.h"
	#include "LImageFrame.h"
	#include "CImageWindow.h"
	#include "LTextView.h"
	#include "CDisplay.h"
#endif
	
#if defined multispec_mac
#endif	// defined multispec_mac    

#if defined multispec_win
	#include "WImageView.h"
	#include "CImageWindow.h"
	#include "CDisplay.h"
	#include "CHistogram.h"
	#include "WImageDoc.h"
	#include "WTextView.h"
#endif	// defined multispec_win

//#include "SExtGlob.h" 



extern void SetFileMapProjectionHandle2 (
				Handle								windowInfoHandle,
				Handle								mapProjectionHandle);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckIfDefaultHistogramInfoExists
//
//	Software purpose:	The purpose of this routine is to determine if the default
//							histogram information exists for the input window information
//							structure.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	True if histogram information does exist.
//							False - otherwise.
//
// Called By:			DisplayDialog in display.c
//
//	Coded By:			Larry L. Biehl			Date:	10/20/1999
//	Revised By:			Larry L. Biehl			Date: 10/20/1999

Boolean CheckIfDefaultHistogramInfoExists (
				WindowInfoPtr						windowInfoPtr)

{	
	SInt16								statFileFormat;
	
	Boolean								returnFlag = FALSE;
	
	
			// Verify that that the support histogram information exists.
	
	statFileFormat = GetHistogramSupportFileFormat (windowInfoPtr);
	
	if (statFileFormat == 0)
		returnFlag = FALSE;
		
	else if (statFileFormat == kImagineType)
		returnFlag = TRUE;
		
	else	// statFileFormat != kImagineType && statFileFormat != 0
		{
		CMFileStream*		supportFileStreamPtr = NULL;
		SignedByte			handleStatus;
		
		supportFileStreamPtr = GetActiveSupportFileStreamPointer (&handleStatus);
		if (FileExists (supportFileStreamPtr))
			returnFlag = TRUE;
			
		}	// end "else statFileFormat != kImagineType && statFileFormat != 0"
		
	return (returnFlag);

}	// end "CheckIfDefaultHistogramInfoExists"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckIfEntireImage
//
//	Software purpose:	The purpose of this routine is to determine if the
//							requested line and column numbers to be used
//							represent the first and last line and columns
//							that are available in the file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	True if input lines and columns represent the first and last
//							lines and columns in the file.
//							False - otherwise.
//
// Called By:			DisplayDialog in display.c
//
//	Coded By:			Larry L. Biehl			Date:	10/12/1989
//	Revised By:			Larry L. Biehl			Date: 10/20/1999

Boolean CheckIfEntireImage (
				WindowInfoPtr						windowInfoPtr,
				UInt32								lineStart,
				UInt32								lineEnd, 
				UInt32								columnStart, 
				UInt32								columnEnd)

{	
	Boolean								returnFlag = TRUE;
	
							
	if (windowInfoPtr != NULL)
		{																														
		if (lineStart != 1) 
			returnFlag = FALSE;
			
		else if (lineEnd != windowInfoPtr->maxNumberLines)
			returnFlag = FALSE;
													
		else if (columnStart != 1) 
			returnFlag = FALSE;
			
		else if (columnEnd != windowInfoPtr->maxNumberColumns)	
			returnFlag = FALSE;
																					
		}	// end "if (windowInfoPtr != NULL)"
		
	else
		returnFlag = FALSE; 
																					
	return (returnFlag);
		
}	// end "CheckIfEntireImage"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		WindowPtr DisposeOfImageWindowSupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the memory which 
//							supports the image window. This includes the memory for the 
//							histogram and display specification handles.  This also includes 
//							the memory associated with the supporting offscreen window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ModalFileSpecification in fileIO.c
//							CloseImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 12/24/1991
//	Revised By:			Larry L. Biehl			Date: 11/02/1999	

void DisposeOfImageWindowSupportMemory (
				WindowInfoPtr						windowInfoPtr)

{
		
	if (windowInfoPtr != NULL)
		{
				// Get handle to histogram structure and dispose of handle to it	
				// if there is one.	
				
		ReleaseHistogramSupportMemory (windowInfoPtr);
													
				// Dispose of memory for the image sums and sums of squares.
				
		ImageSecondOrderStatsPtr imageSecondOrderStatsPtr = 
															&windowInfoPtr->imageSecondOrderStats;
										
		imageSecondOrderStatsPtr->sumsHandle = 
								UnlockAndDispose (imageSecondOrderStatsPtr->sumsHandle);
		imageSecondOrderStatsPtr->sumSquaresHandle =
								UnlockAndDispose (imageSecondOrderStatsPtr->sumSquaresHandle);
						
				// Dispose of or initialize information in the display specs structure 
				// that relates to the image being disposed of in memory and prepare the 
				// structure for a new image.	
		
		ReleaseDisplaySupportMemory (windowInfoPtr);
				
				// Dispose of off screen bit/pix map and storage for offscreen image.											
		
		ReleaseOffscreenSupportMemory (windowInfoPtr);
			
		}	// end "if (windowInfoPtr)" 
			
}	// end "DisposeOfImageWindowSupportMemory" 


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle* GetActiveDisplaySpecsHandle
//
//	Software purpose:	This routine gets the handle containing
//							the display specifications structure.
//
//	Parameters in:		None			
//
//	Parameters out:	None				
//
// Value Returned:	Handle for the display specifications structure.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/17/1995
//	Revised By:			Larry L. Biehl			Date: 11/17/1995

Handle GetActiveDisplaySpecsHandle (void)

{
	#if defined multispec_mac 	
		if (gActiveImageWindowInfoH != NULL)
			return (((WindowInfoPtr)*gActiveImageWindowInfoH)->displaySpecsHandle);
	#endif	// defined multispec_mac 	
	              
	#if defined multispec_win || defined multispec_lin		
		if (gActiveImageViewCPtr != NULL)
			return (gActiveImageViewCPtr->m_displayMultiCPtr->mDisplaySpecsHandle);
	#endif	// defined multispec_win || defined multispec_lin
		
	return (NULL);
		
}	// end "GetActiveDisplaySpecsHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetActiveImageFileInfoHandle
//
//	Software purpose:	This routine returns the handle for the file
//							information structure for the active image window.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
// Value Returned:	Handle to a file information structure
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1995
//	Revised By:			Larry L. Biehl			Date: 03/01/1999

Handle GetActiveImageFileInfoHandle (void)

{
	#if defined multispec_mac || defined multispec_mac_swift
		return (gActiveImageFileInfoH);                       	
	#endif	// defined multispec_mac || defined multispec_mac_swift
		
	#if defined multispec_win || defined multispec_lin
		Handle		activeImageFileInfoHandle = NULL;
	
		Handle activeWindowInfoHandle = GetActiveImageWindowInfoHandle ();
		WindowInfoPtr activeWindowInfoPtr = 
									(WindowInfoPtr)GetHandlePointer (activeWindowInfoHandle);
		
		if (activeWindowInfoPtr != NULL)
			activeImageFileInfoHandle = activeWindowInfoPtr->fileInfoHandle;
			
		return (activeImageFileInfoHandle); 	
	#endif	// defined multispec_win | multispec_lin
	
}	// end "GetActiveImageFileInfoHandle"       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetActiveImageLayerInfoHandle
//
//	Software purpose:	This routine returns the handle for the layer
//							information structure for the active image window.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
// Value Returned:	Handle to a file information structure
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1997
//	Revised By:			Larry L. Biehl			Date: 03/01/1998

Handle GetActiveImageLayerInfoHandle (void)

{
	#if defined multispec_mac || defined multispec_mac_swift
		return (gActiveImageLayerInfoH);
	#endif	// defined multispec_mac || defined multispec_mac_swift
	
	#if defined multispec_win || defined multispec_lin	
		Handle			activeImageLayerInfoHandle = NULL;
	
		Handle activeWindowInfoHandle = GetActiveImageWindowInfoHandle ();
		WindowInfoPtr activeWindowInfoPtr = 
									(WindowInfoPtr)GetHandlePointer (activeWindowInfoHandle);
		
		if (activeWindowInfoPtr != NULL)
			activeImageLayerInfoHandle = activeWindowInfoPtr->layerInfoHandle;
			
		return (activeImageLayerInfoHandle); 
	#endif	// defined multispec_win
	
}	// end "GetActiveImageLayerInfoHandle"     



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetActiveImageWindowInfoHandle
//
//	Software purpose:	This routine returns the image window handle
//							for the active image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/27/1995
//	Revised By:			Larry L. Biehl			Date: 11/27/1995			

Handle GetActiveImageWindowInfoHandle (void)

{ 
	#if defined multispec_mac || defined multispec_mac_swift
  		return (gActiveImageWindowInfoH);	
	#endif // defined multispec_mac || defined multispec_mac_swift

	#if defined multispec_win || defined multispec_lin      
      Handle activeImageWindowInfoHandle = NULL;
      
   	if (gActiveImageViewCPtr != NULL)
   		{
			CMImageWindow* imageWindowCPtr = gActiveImageViewCPtr->GetImageWindowCPtr ();
			activeImageWindowInfoHandle = imageWindowCPtr->GetWindowInfoHandle ();
									
			}	// end "if (gActiveImageViewCPtr != NULL)"
								
		return (activeImageWindowInfoHandle);	
	#endif // defined multispec_win || multispec_lin
	
}	// end "GetActiveImageWindowInfoHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		LegendListHandle GetActiveLegendListHandle
//
//	Software purpose:	The purpose of this routine is to obtain the legend
//							list handle for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	Legend list handle of the active image window.
//							NULL if not available.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	04/04/1996
//	Revised By:			Larry L. Biehl			Date: 04/04/1996

LegendListHandle GetActiveLegendListHandle (void)

{	
			// Get pointer to the window information structure.
			
	Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
		
	if (windowInfoPtr != NULL)	
		return (windowInfoPtr->legendListHandle);
		
	return (NULL);
												
}	// end "GetActiveLegendListHandle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetActiveSelectionInfoHandle
//
//	Software purpose:	This routine returns the handle for the selection
//							information structure for the active image window.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
// Value Returned:	Handle to a file information structure
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/13/1998
//	Revised By:			Larry L. Biehl			Date: 09/25/1998

Handle GetActiveSelectionInfoHandle (void)

{
	Handle								activeSelectionInfoH = NULL;
	
	
	#if defined multispec_mac
		if (gActiveImageWindowInfoH != NULL)
			activeSelectionInfoH = 
							((WindowInfoPtr)*gActiveImageWindowInfoH)->selectionInfoHandle;                      
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_lin
		if (gActiveImageViewCPtr != NULL)
			activeSelectionInfoH = GetSelectionInfoHandle (gActiveImageViewCPtr);
	#endif	// defined multispec_win

	return (activeSelectionInfoH);
	
}	// end "GetActiveSelectionInfoHandle"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMFileStream* GetActiveSupportFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to obtain the file stream
//							pointer for the support file for the active image window.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	pointer to the support file stream pointer.
//							NULL if not available.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	10/26/1995
//	Revised By:			Larry L. Biehl			Date: 04/04/1996

CMFileStream* GetActiveSupportFileStreamPointer (
				SignedByte*							handleStatusPtr)

{	
			// Get active window information structure.
			
	Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();
	
	CMFileStream* supportFileStreamPtr = GetSupportFileStreamPointer (
																windowInfoHandle, handleStatusPtr);
		
	return (supportFileStreamPtr);
												
}	// end "GetActiveSupportFileStreamPointer"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetActiveWindowInfoHandle
//
//	Software purpose:	This routine returns the image window handle
//							for the active image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1996
//	Revised By:			Larry L. Biehl			Date: 06/10/2015		

Handle GetActiveWindowInfoHandle (void)

{ 
	#if defined multispec_mac || defined multispec_mac_swift
		return (gActiveWindowInfoH);	
	#endif // defined multispec_mac || defined multispec_mac_swift

	#if defined multispec_win
      Handle activeWindowInfoHandle = NULL;
      
   	if (gActiveImageViewCPtr != NULL)
   		{
   		if (gActiveImageViewCPtr->GetActiveWindowFlag ())
   			{
				CMImageWindow* imageWindowCPtr = 
									gActiveImageViewCPtr->GetImageWindowCPtr ();
				activeWindowInfoHandle = 
										imageWindowCPtr->GetWindowInfoHandle ();
										
				}	// end "if (gActiveImageViewCPtr->GetActiveWindowFlag ())"
									
			}	// end "if (gActiveImageViewCPtr != NULL)"
								
		return (activeWindowInfoHandle);
	#endif // defined multispec_win

	#if defined multispec_lin
      Handle activeWindowInfoHandle = NULL;
      
   	if (gActiveImageViewCPtr != NULL)
   		{
					// Need to not use the active window flag for linux because it gets
					// set to false when the menu bar is selected.
   		//if (gActiveImageViewCPtr->GetActiveWindowFlag ())
			CMImageWindow* imageWindowCPtr = gActiveImageViewCPtr->GetImageWindowCPtr ();
			activeWindowInfoHandle = imageWindowCPtr->GetWindowInfoHandle ();
																			
			}	// end "if (gActiveImageViewCPtr != NULL)"
								
		return (activeWindowInfoHandle);
	#endif // defined multispec_lin
	
}	// end "GetActiveWindowInfoHandle"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateHeight
//
//	Software purpose:	The purpose of this routine is to set the display map
//							coordinate flag for the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/02/2000
//	Revised By:			Larry L. Biehl			Date: 12/13/2000

UInt32 GetCoordinateHeight (
				Handle								windowInfoHandle)

{	
	UInt32 coordinateHeight = 0; 
	          
	#if defined multispec_mac
				// Get pointer to the support file information.
													
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
		
		if (windowInfoPtr != NULL)
			coordinateHeight = windowInfoPtr->coordinateHeight;			
	#endif	// defined multispec_mac   
	
	#if defined multispec_lin
		//CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
		//CMImageDoc* imageDocCPtr = imageViewCPtr->GetDocument ();
		//if (imageDocCPtr->GetDisplayCoordinatesFlag ())
		//	coordinateHeight = 1;
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
	
		if (windowInfoPtr != NULL)
			coordinateHeight = windowInfoPtr->coordinateHeight;	
	#endif	// defined multispec_lin
	
	#if defined multispec_win
		CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
		CMImageDoc* imageDocCPtr = imageViewCPtr->GetDocument ();
		if (imageDocCPtr->GetDisplayCoordinatesFlag ())
			coordinateHeight = 1;
	#endif	// defined multispec_win
		
	return (coordinateHeight);
												
}	// end "GetCoordinateHeight"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewAreaFactor
//
//	Software purpose:	The purpose of this routine is to return the factor to use to
//							convert a pixel to user specified units for the window 
//							represented by the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

double GetCoordinateViewAreaFactor (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewAreaFactor);
		
	return (1.);
												
}	// end "GetCoordinateViewAreaFactor"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewAreaUnits
//
//	Software purpose:	The purpose of this routine is to get the current area 
//							units code for the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

SInt16 GetCoordinateViewAreaUnits (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewAreaUnitsCode);
		
	return (0);
												
}	// end "GetCoordinateViewAreaUnits"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewAreaUnitsControl
//
//	Software purpose:	The purpose of this routine is to get the handle for the area
//							units control for the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

ControlHandle GetCoordinateViewAreaUnitsControl (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewAreaUnitsControl);
		
	return (NULL);
												
}	// end "GetCoordinateViewAreaUnitsControl"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewCursorStart
//
//	Software purpose:	The purpose of this routine is to get the start location for the
//							cursor information in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/07/2000

UInt32 GetCoordinateViewCursorStart (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewCursorStart);
		
	return (0);
												
}	// end "GetCoordinateViewCursorStart"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewFactor
//
//	Software purpose:	The purpose of this routine is to get the start location for the
//							cursor information in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/09/2000
//	Revised By:			Larry L. Biehl			Date: 11/09/2000

double GetCoordinateViewFactor (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewFactor);
		
	return (1.);
												
}	// end "GetCoordinateViewFactor"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewAreaPopupStart
//
//	Software purpose:	The purpose of this routine is to return the x left location
//							of the area popup menu control in the coordinate view.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

UInt32 GetCoordinateViewAreaPopupStart (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewAreaPopupStart);
		
	return (0);
												
}	// end "GetCoordinateViewAreaPopupStart"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewNumberChars
//
//	Software purpose:	The purpose of this routine is to return the maximum number
//							of characters to allow for when listing the coordinates
//							in the coordinate view.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/07/2000

UInt32 GetCoordinateViewNumberChars (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewNumberChars);
		
	return (0);
												
}	// end "GetCoordinateViewNumberChars" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewNumberPixelsChars
//
//	Software purpose:	The purpose of this routine is to return the maximum number of
//							characters to allow for when writing the number of pixel in the
//							coordinate view.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/09/2000
//	Revised By:			Larry L. Biehl			Date: 11/09/2000

UInt32 GetCoordinateViewNumberPixelsChars (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewNumberPixelsChars);
		
	return (0);
												
}	// end "GetCoordinateViewNumberPixelsChars" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewNumberPixelsStart
//
//	Software purpose:	The purpose of this routine is to return the start location for
//							writing the number of pixels in the coordinate view.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/09/2000
//	Revised By:			Larry L. Biehl			Date: 11/09/2000

UInt32 GetCoordinateViewNumberPixelsStart (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewNumberPixelsStart);
		
	return (0);
												
}	// end "GetCoordinateViewNumberPixelsStart"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewScaleStart
//
//	Software purpose:	The purpose of this routine is to return the x left location
//							of the area popup menu control in the coordinate view.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

UInt32 GetCoordinateViewScaleStart (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewScaleStart);
		
	return (0);
												
}	// end "GetCoordinateViewScaleStart"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewSelectionStart
//
//	Software purpose:	The purpose of this routine is to get the start location for the
//							selection information in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/07/2000

UInt32 GetCoordinateViewSelectionStart (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateViewSelectionStart);
		
	return (0);
												
}	// end "GetCoordinateViewSelectionStart"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewUnits
//
//	Software purpose:	The purpose of this routine is to get the current coordinate 
//							units code for the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/06/2000
//	Revised By:			Larry L. Biehl			Date: 11/06/2000

SInt16 GetCoordinateViewUnits (
				Handle								windowInfoHandle)

{	
	SInt16 coordinateViewUnitsCode = 0;
	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		coordinateViewUnitsCode = windowInfoPtr->coordinateViewUnitsCode;
		
	return (coordinateViewUnitsCode);
												
}	// end "GetCoordinateViewUnits"     



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCoordinateViewUnitsControl
//
//	Software purpose:	The purpose of this routine is to get the handle for the area
//							units control for the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	12/13/2000
//	Revised By:			Larry L. Biehl			Date: 12/13/2000

ControlHandle GetCoordinateViewUnitsControl (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->coordinateUnitsControl);
		
	return (NULL);
												
}	// end "GetCoordinateViewUnitsControl" 


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetDisplayClassGroupCode
//
//	Software purpose:	This routine gets the code for the thematic class and or
//							grouping that is currently shown for the window represented
//							by the input window information structure pointer.
//
//	Parameters in:		None			
//
//	Parameters out:	None				
//
// Value Returned:	Code for the thematic class and or grouping being shown.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1996
//	Revised By:			Larry L. Biehl			Date: 11/11/1996

SInt16 GetDisplayClassGroupCode (
				Handle								windowInfoHandle)

{
	Handle displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);
	
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					displaySpecsHandle);
								
	if (displaySpecsPtr != NULL)
		return (displaySpecsPtr->classGroupCode);
	
	return (0);
		
}	// end "GetDisplayClassGroupCode" 


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetDisplayedLineStart
//
//	Software purpose:	This routine returns the 'displayedLineStart' value stored
//							in the displaySpecs structure for the input window information
//							structure.
//
//	Parameters in:		Pointer to window information structure			
//
//	Parameters out:	None				
//
// Value Returned:	Displayed line start.	
//
// Called By:			OutlineFieldsControl in SOutlineFields
//
//	Coded By:			Larry L. Biehl			Date: 01/27/2001
//	Revised By:			Larry L. Biehl			Date: 01/27/2001

UInt32 GetDisplayedLineStart (
				WindowInfoPtr						windowInfoPtr)

{
	DisplaySpecsPtr					displaySpecsPtr;
	Handle								displaySpecsH;
	
	
			// Make certain that an image has been drawn in the window.				
			// If displayedLineStart is 0, then no image has been displayed in	
			// the window.  The user must have cancelled the display operation.	
	
	displaySpecsH = GetDisplaySpecsHandle (windowInfoPtr);
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);

	if (displaySpecsPtr != NULL)
		return (displaySpecsPtr->displayedLineStart); 
		
	else	// displaySpecsPtr == NULL
		return (0);
												
}	// end "GetDisplayedLineStart"  


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetDisplaySpecsHandle
//
//	Software purpose:	This routine gets the handle containing
//							the display specifications structure for the input
//							window information structure pointer.
//
//	Parameters in:		None			
//
//	Parameters out:	None				
//
// Value Returned:	Handle for the display specifications structure.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1996
//	Revised By:			Larry L. Biehl			Date: 02/17/1998

Handle GetDisplaySpecsHandle (
				Handle								windowInfoHandle)

{
	Handle								displaySpecsHandle;
	
	SignedByte							handleStatus;
	
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																	windowInfoHandle, &handleStatus);
									
	displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
				
			// Unlock the window information handle if needed.						
	
	MHSetState (windowInfoHandle, handleStatus);
									
	return (displaySpecsHandle);
		
}	// end "GetDisplaySpecsHandle" 



Handle GetDisplaySpecsHandle (
				WindowInfoPtr						windowInfoPtr)

{
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac 
			return (windowInfoPtr->displaySpecsHandle);
		#endif	// defined multispec_mac 
				              
		#if defined multispec_win || defined multispec_lin
			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
			
	   	if (imageWindowCPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = imageWindowCPtr->mImageViewCPtr;
	   		
	   		if (imageViewCPtr != NULL)
					return (imageViewCPtr->m_displayMultiCPtr->mDisplaySpecsHandle);
								
				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win | multispec_lin 
		
		}	// end "if (windowInfoPtr != NULL)"
		
	return (NULL);
		
}	// end "GetDisplaySpecsHandle"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetFileInfoHandle
//
//	Software purpose:	The purpose of this routine is to return the file information
//							structure handle for the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	Handle for the file information struction.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

Handle GetFileInfoHandle (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->fileInfoHandle); 
		
	return (NULL);
												
}	// end "GetFileInfoHandle" 

	

Handle GetFileInfoHandle (
				WindowInfoPtr						windowInfoPtr)

{ 
	if (windowInfoPtr != NULL)
	  	return (windowInfoPtr->fileInfoHandle);   
	     
	return (NULL);
	
}	// end "GetFileInfoHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetFileInfoVersion
//
//	Software purpose:	The purpose of this routine is to return the file info version
//							stored in the structure pointed to by the input window 
//							information pointer.
//
//	Parameters in:		Window Information structure pointer
//
//	Parameters out:	None
//
// Value Returned:  	file info version
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	02/26/2013
//	Revised By:			Larry L. Biehl			Date: 02/26/2013

SInt16 GetFileInfoVersion (
				WindowInfoPtr						windowInfoPtr)

{	
	SInt16								fileInfoVersion = 0;
	
	
	if (windowInfoPtr != NULL)
		fileInfoVersion = windowInfoPtr->fileInfoVersion;
				
	return (fileInfoVersion);
												
}	// end "GetFileInfoVersion"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetFileMapProjectionHandle2
//
//	Software purpose:	The purpose of this routine is to return the map projection
//							handle for the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	map projection handle
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	12/28/2000
//	Revised By:			Larry L. Biehl			Date: 12/28/2000

Handle GetFileMapProjectionHandle2 (
				Handle								windowInfoHandle)

{	
	Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		
	return (GetFileMapProjectionHandle (fileInfoHandle));
												
}	// end "GetFileMapProjectionHandle2"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMFileStream* GetFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to obtain the file stream
//							pointer for the support file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	pointer to the support file stream pointer.
//							NULL if not available.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	10/19/1995
//	Revised By:			Larry L. Biehl			Date: 06/04/1996

CMFileStream* GetFileStreamPointer (
				Handle								windowInfoHandle,
				SignedByte*							handleStatusPtr)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
												
	*handleStatusPtr = -1;
	                  
	#if defined multispec_mac 		
		if (windowInfoPtr != NULL)
			{									
			FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																		windowInfoPtr->fileInfoHandle,
																		handleStatusPtr);
			
			if (fileInfoPtr != NULL)				
																	return (&fileInfoPtr->fileStream);
													
			}	// end "if (windowInfoPtr != NULL)"                    
	#endif	// defined multispec_mac 
              
	#if defined multispec_win || defined multispec_lin	
		return (windowInfoPtr->cImageWindowPtr->GetImageFileStreamPointer ());
	#endif	// defined multispec_win 
		
	return (NULL);
												
}	// end "GetFileStreamPointer"  


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHasWavelengthValuesFlag
//
//	Software purpose:	The purpose of this routine is to get the hasWavelengthValueFlag
//							variable for this window object.
//
//	Parameters in:		window pointer
//
//	Parameters out:	None			
//
//	Value Returned:	value of the hasWavelengthValuesFlag
//							
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 03/02/2018
//	Revised By:			Larry L. Biehl			Date: 03/02/2018

Boolean GetHasWavelengthValuesFlag (
				WindowPtr 							windowPtr)

{
	Handle								windowInfoHandle;
	
	
	windowInfoHandle = GetWindowInfoHandle (windowPtr);
	
	return (GetHasWavelengthValuesFlag (windowInfoHandle));
	
}	// end "GetHasWavelengthValuesFlag"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHasWavelengthValuesFlag
//
//	Software purpose:	The purpose of this routine is to get the hasWavelengthValueFlag
//							variable for this window object.
//
//	Parameters in:		window info structure handle
//
//	Parameters out:	None			
//
//	Value Returned:	value of the hasWavelengthValuesFlag
//							
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 03/01/2018
//	Revised By:			Larry L. Biehl			Date: 03/01/2018

Boolean GetHasWavelengthValuesFlag (
				Handle 								windowInfoHandle)

{
	Boolean								hasWavelengthValuesFlag = FALSE;
	
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)                                                   
		hasWavelengthValuesFlag = windowInfoPtr->hasWavelengthValuesFlag;
	
	return (hasWavelengthValuesFlag);
	
}	// end "GetHasWavelengthValuesFlag"
*/

	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetHistogramSpecsHandle
//
//	Software purpose:	This routine gets the handle containing
//							the histogram specifications structure for the input
//							window information structure pointer.
//
//	Parameters in:		window information structure pointer			
//
//	Parameters out:	None				
//
// Value Returned:	Handle for the histogram specifications structure.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1995
//	Revised By:			Larry L. Biehl			Date: 11/29/1995

Handle GetHistogramSpecsHandle (
				WindowInfoPtr						windowInfoPtr)

{
	Handle								histogramSpecsHandle = NULL;
	
	
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac 		
			histogramSpecsHandle = windowInfoPtr->histogramSpecsHandle;		
		#endif	// defined multispec_mac 
				              
		#if defined multispec_win || defined multispec_lin
			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
			
	   	if (imageWindowCPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = imageWindowCPtr->mImageViewCPtr;
	   		
	   		if (imageViewCPtr != NULL)
					histogramSpecsHandle = 
										imageViewCPtr->m_histogramCPtr->mHistogramSpecsHandle;
								
				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win | multispec_lin 
		
		}	// end "if (windowInfoPtr != NULL)"
		
	return (histogramSpecsHandle);
		
}	// end "GetHistogramSpecsHandle"  



Handle GetHistogramSpecsHandle (
				Handle							windowInfoHandle)

{
	Handle								histogramSpecsHandle = NULL;
	
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac 
			histogramSpecsHandle = windowInfoPtr->histogramSpecsHandle;
		#endif	// defined multispec_mac 
		              
		#if defined multispec_win || defined multispec_lin
			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
			
	   	if (imageWindowCPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = imageWindowCPtr->mImageViewCPtr;
	   		
	   		if (imageViewCPtr != NULL)
					histogramSpecsHandle = 
										imageViewCPtr->m_histogramCPtr->mHistogramSpecsHandle;
								
				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win | multispec_lin
		
		}	// end "if (windowInfoPtr != NULL)"
		
	return (histogramSpecsHandle);
		
}	// end "GetHistogramSpecsHandle"  


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetHistogramSummaryHandle
//
//	Software purpose:	This routine gets the handle containing
//							the histogram specifications structure for the input
//							window information structure pointer.
//
//	Parameters in:		window information structure pointer			
//
//	Parameters out:	None				
//
// Value Returned:	Handle for the histogram specifications structure.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1999
//	Revised By:			Larry L. Biehl			Date: 11/02/1999

Handle GetHistogramSummaryHandle (
				WindowInfoPtr						windowInfoPtr)

{
	Handle								histogramSummaryHandle = NULL;
	
	
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac 		
			histogramSummaryHandle = windowInfoPtr->histogramSummaryHandle;		
		#endif	// defined multispec_mac 
				              
		#if defined multispec_win | defined multispec_lin			
			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
			
	   	if (imageWindowCPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = imageWindowCPtr->mImageViewCPtr;
	   		
	   		if (imageViewCPtr != NULL)
					histogramSummaryHandle = 
									imageViewCPtr->m_histogramCPtr->mHistogramSummaryHandle;
								
				}	// end "if (imageWindowCPtr != NULL)"		 
		#endif	// defined multispec_win | multispec_lin
		
		}	// end "if (windowInfoPtr != NULL)"
		
	return (histogramSummaryHandle);
		
}	// end "GetHistogramSummaryHandle" 



Handle GetHistogramSummaryHandle (
				Handle								windowInfoHandle)

{
	Handle								histogramSummaryHandle = NULL;
	
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac 
			histogramSummaryHandle = windowInfoPtr->histogramSummaryHandle;		
		#endif	// defined multispec_mac 
				              
		#if defined multispec_win | defined multispec_lin
			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
			
	   	if (imageWindowCPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = imageWindowCPtr->mImageViewCPtr;
	   		
	   		if (imageViewCPtr != NULL)
					histogramSummaryHandle = 
									imageViewCPtr->m_histogramCPtr->mHistogramSummaryHandle;
								
				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win | multispec_lin
		
		}	// end "if (windowInfoPtr != NULL)"
		
	return (histogramSummaryHandle);
		
}	// end "GetHistogramSummaryHandle"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHistogramSupportFileFormat
//
//	Software purpose:	This routine returns the histogram stat file format for the 
//							window refered to by the input window information structure 
//							pointer.
//
//	Parameters in:		Handle to window information structure		
//
//	Parameters out:				
//
// Value Returned:	histogram stat support file format
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/09/2009
//	Revised By:			Larry L. Biehl			Date: 07/09/2009		

SInt16 GetHistogramSupportFileFormat (
				Handle								windowInfoHandle)

{
	WindowInfoPtr		 				windowInfoPtr;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	return (GetHistogramSupportFileFormat (windowInfoPtr));
	
}	// end "GetHistogramSupportFileFormat"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHistogramSupportFileFormat
//
//	Software purpose:	This routine returns the histogram stat file format for the 
//							window refered to by the input window information structure 
//							pointer.
//
//	Parameters in:		Pointer to window information structure		
//
//	Parameters out:				
//
// Value Returned:	histogram stat support file format
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/20/1999
//	Revised By:			Larry L. Biehl			Date: 10/20/1999		

SInt16 GetHistogramSupportFileFormat (
				WindowInfoPtr						windowInfoPtr)

{
	HistogramSpecsPtr 				histogramSpecsPtr;
	
	Handle 								histogramSpecsHandle;
	
	SInt16								statFileFormat = 0;
	
	
	histogramSpecsHandle = GetHistogramSpecsHandle (windowInfoPtr);
	
	histogramSpecsPtr = (HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);
		
	if (histogramSpecsPtr != NULL)
		statFileFormat = histogramSpecsPtr->statFileFormat; 
		
	return (statFileFormat);
	
}	// end "GetHistogramSupportFileFormat"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetImageTopOffset
//
//	Software purpose:	This routine returns height for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/24/1996
//	Revised By:			Larry L. Biehl			Date: 04/19/2000			

UInt32 GetImageTopOffset (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  	return (windowInfoPtr->imageTopOffset);    
	     
	return (0);
	
}	// end "GetImageTopOffset" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetImageType
//
//	Software purpose:	The purpose of this routine is to return the image type
//							variable for this window object.
//
//	Parameters in:		window info structure handle			
//
//	Parameters out:	None			
//
//	Value Returned:	0 if no window structure exists
//							image type such as kMultispectralImageType, or kThematicImageType.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 09/11/2006
//	Revised By:			Larry L. Biehl			Date: 09/11/2006	

SInt16 GetImageType (
				Handle 								windowInfoHandle)

{  															
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)                                                   
		return (windowInfoPtr->imageType);

	return (0);
	
}	// end "GetImageType" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetLayerInfoHandle
//
//	Software purpose:	This routine returns the layer information handle
//							for the input window information structure pointer.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1995
//	Revised By:			Larry L. Biehl			Date: 03/15/1999			

Handle GetLayerInfoHandle (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr						windowInfoPtr;
	
	Handle								layerInfoHandle = NULL;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL) 
	  		layerInfoHandle = GetLayerInfoHandle (windowInfoPtr); 
	     
	return (layerInfoHandle);
	
}	// end "GetLayerInfoHandle"

       		

Handle GetLayerInfoHandle (
				WindowInfoPtr						windowInfoPtr)

{ 
	Handle								layerInfoHandle = NULL;
	
	
	if (windowInfoPtr != NULL) 
	  	layerInfoHandle = windowInfoPtr->layerInfoHandle;
	     
	return (layerInfoHandle);
	
}	// end "GetLayerInfoHandle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetActiveLegendListType
//
//	Software purpose:	This routine returns legend width for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2004
//	Revised By:			Larry L. Biehl			Date: 12/01/2017			

SInt16 GetActiveLegendListType ()

{ 
	LegendListHandle 					legendListHandle;
	
	SInt16 								legendListType;
	
	
	legendListType = kClassDisplay;
	
	legendListHandle = GetActiveLegendListHandle ();
	if (legendListHandle != NULL)
		{
				// Get the current list type.
				                      
		#if defined multispec_mac 
			legendListType = (((ListPtr)*legendListHandle)->refCon & 0x00000006)/2; 
		#endif	// defined multispec_mac 

		#if defined multispec_win || multispec_lin
			legendListType = (SInt16)((CMLegendList*)legendListHandle)->m_listType;
		#endif	// defined multispec_win 
		
		}	// end "if (legendListHandle != NULL)"
		
	return (legendListType);
	
}	// end "GetActiveLegendListType"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLegendWidth
//
//	Software purpose:	This routine returns legend width for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1996
//	Revised By:			Larry L. Biehl			Date: 06/10/2015			

SInt16 GetLegendWidth (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac          	     
	  		return (windowInfoPtr->legendWidth);		
		#endif // defined multispec_mac
		
		#if defined multispec_win		
	  		return (windowInfoPtr->legendWidth);
			/*
	   	if (windowInfoPtr->cImageWindowPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = 
												windowInfoPtr->cImageWindowPtr->mImageViewCPtr;
	   							
	   		if (imageViewCPtr != NULL)
					return (imageViewCPtr->m_legendWidth);
							
				}	// end "if (windowInfoPtr->cImageWindowPtr != NULL)"
			*/
		#endif // defined multispec_win 
		
		#if defined multispec_lin
			wxRect				legendArea;
			CMImageFrame*		imageFrameCPtr;
			CMImageView*		imageViewCPtr;
	   	SInt16				legendWidth = 0;
			
			if (windowInfoPtr->cImageWindowPtr != NULL)
	   		{
	   		imageViewCPtr = windowInfoPtr->cImageWindowPtr->mImageViewCPtr;
				imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
	   							
	   		if (imageFrameCPtr != NULL)
					{
					//if (imageFrameCPtr->m_leftWindow->IsShown ())
					//	{
						legendArea = (imageFrameCPtr->m_leftWindow)->GetClientRect ();
						legendWidth = (SInt16)legendArea.GetRight ();
					
					//	}	// end "if (imageFrameCPtr->m_leftWindow->IsShown ())"
					
					}	// end "if (imageFrameCPtr != NULL)"
							
				}	// end "if (windowInfoPtr->cImageWindowPtr != NULL)"
				
			return (legendWidth);
		#endif // defined multispec_lin  
				
		}	// end "if (windowInfoPtr != NULL)"    
	     
	return (0);
	
}	// end "GetLegendWidth" 


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetMapProjectionHandle
//
//	Software purpose:	The purpose of this routine is to return the map projection
//							handle for the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	map projection handle
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/06/2000
//	Revised By:			Larry L. Biehl			Date: 11/06/2000

Handle GetMapProjectionHandle (
				Handle								windowInfoHandle)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->mapProjectionHandle); 
		
	return (NULL);
												
}	// end "GetMapProjectionHandle" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetMaxNumberColumns
//
//	Software purpose:	The purpose of this routine is to return the window the maximum
//							number of lines in the input window information structure.
//
//	Parameters in:		window info structure handle			
//
//	Parameters out:	None		
//
//	Value Returned:	0 if now window structure exists
//							maximum number lines	
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 03/03/1999
//	Revised By:			Larry L. Biehl			Date: 03/03/1999	

UInt32 GetMaxNumberColumns (
				Handle 								windowInfoHandle)

{  
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL)                                                   
		return (windowInfoPtr->maxNumberColumns);
		
	else	// windowInfoPtr == NULL
		return (0);
	
}	// end "GetMaxNumberColumns" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetMaxNumberLines
//
//	Software purpose:	The purpose of this routine is to return the window the maximum
//							number of lines in the input window information structure.
//
//	Parameters in:		window info structure handle			
//
//	Parameters out:	None		
//
//	Value Returned:	0 if now window structure exists
//							maximum number lines	
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 03/03/1999
//	Revised By:			Larry L. Biehl			Date: 03/03/1999	

UInt32 GetMaxNumberLines (
				Handle 								windowInfoHandle)

{  
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL)                                                   
		return (windowInfoPtr->maxNumberLines);
		
	else	// windowInfoPtr == NULL
		return (0);
	
}	// end "GetMaxNumberLines" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetNumberImageFiles
//
//	Software purpose:	This routine returns the number of image files for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:			GetDefaultSupportFile in SOpnImag.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 10/18/1999			

UInt32 GetNumberImageFiles (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
		return (windowInfoPtr->numberImageFiles);    
	     
	return (0);
	
}	// end "GetNumberImageFiles" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetNumberImageWindowOverlays
//
//	Software purpose:	This routine returns the number of image window overlays for the 
//							input window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/20/2003
//	Revised By:			Larry L. Biehl			Date: 01/20/2003			

UInt32 GetNumberImageWindowOverlays (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
		return (windowInfoPtr->numberImageOverlays);    
	     
	return (0);
	
}	// end "GetNumberImageWindowOverlays" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetNumberWindowOverlays
//
//	Software purpose:	This routine returns the number of window overlays (or vector
//							overlay) for the input window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2005
//	Revised By:			Larry L. Biehl			Date: 03/25/2005			

UInt32 GetNumberWindowOverlays (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
		return (windowInfoPtr->numberOverlays);    
	     
	return (0);
	
}	// end "GetNumberWindowOverlays" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		PMOrientation GetPrintPageOrientation
//
//	Software purpose:	This routine returns the code for the printer page orientation.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/18/2011
//	Revised By:			Larry L. Biehl			Date: 08/18/2011			

PMOrientation GetPrintPageOrientation (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  return (windowInfoPtr->printPageOrientation);    
	     
	#if defined multispec_mac || defined multispec_mac_swift
		return (kPMPortrait);
	#endif	// defined multispec_mac || defined multispec_mac_swift
		
	#if defined multispec_win 
		return (DMORIENT_PORTRAIT);
	#endif	// defined multispec_win
	
}	// end "GetPrintPageOrientation"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetPrintPageScale
//
//	Software purpose:	This routine returns the scale to be used for printing the page
//							on the printer.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/18/2011
//	Revised By:			Larry L. Biehl			Date: 08/18/2011			

double GetPrintPageScale (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  return (windowInfoPtr->printPageScale);    
	     
	return (100);
	
}	// end "GetPrintPageScale"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetProjectBaseImageFlag
//
//	Software purpose:	This routine returns the project base image flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/01/1998
//	Revised By:			Larry L. Biehl			Date: 10/01/1998			

Boolean GetProjectBaseImageFlag (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  return (windowInfoPtr->projectBaseImageFlag);    
	     
	return (FALSE);
	
}	// end "GetProjectBaseImageFlag"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetProjectWindowFlag
//
//	Software purpose:	This routine returns the project window flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/01/1998
//	Revised By:			Larry L. Biehl			Date: 10/01/1998			

Boolean GetProjectWindowFlag (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  return (windowInfoPtr->projectWindowFlag);    
	     
	return (FALSE);
	
}	// end "GetProjectWindowFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetOutputWindowChangedFlag
//
//	Software purpose:	The purpose of this routine is to set the window changed
//							flag.
//
//	Parameters in:		window info structure handle			
//
//	Parameters out:	None			
//
//	Value Returned:	0 if now window structure exists
//							window type if others such as kThematicWindowType, or kImageWindowType.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1998
//	Revised By:			Larry L. Biehl			Date: 06/22/2015	

Boolean GetOutputWindowChangedFlag (void)

{ 
	Boolean								windowChangedFlag = FALSE;
	
	
	#if defined multispec_lin 
		if (gOutputViewCPtr != NULL)
			windowChangedFlag = gOutputViewCPtr->m_textsw->IsModified ();
	#endif	// defined multispec_lin 
	
	#if defined multispec_mac 
		Handle windowInfoHandle = (Handle)GetWRefCon (gOutputWindow); 
																
		WindowInfoPtr windowInfoPtr = 
											(WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
		if (windowInfoPtr != NULL)                                                   
			windowChangedFlag = windowInfoPtr->windowChangedFlag;           
	#endif	// defined multispec_mac 
	
	#if defined multispec_win 
		if (gOutputViewCPtr != NULL)
			windowChangedFlag = gOutputViewCPtr->GetEditCtrl ().GetModify ();
	#endif	// defined multispec_win 
		
	return (windowChangedFlag);
	
}	// end "GetOutputWindowChangedFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetSelectionInfoHandle
//
//	Software purpose:	The purpose of this routine is to return the file information
//							structure handle for the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	Handle for the file information struction.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	02/16/1998
//	Revised By:			Larry L. Biehl			Date: 09/25/1998

Handle GetSelectionInfoHandle (
				Handle								windowInfoHandle)

{		
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
												
	return (GetSelectionInfoHandle (windowInfoPtr));
												
}	// end "GetSelectionInfoHandle"  



Handle GetSelectionInfoHandle (
				WindowInfoPtr						windowInfoPtr)

{                                                            
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->selectionInfoHandle);  

	return (NULL);
	
}	// end "GetSelectionInfoHandle" 

 
      
Handle GetSelectionInfoHandle (
				WindowPtr							windowPtr)

{	
	Handle								imageWindowInfoHandle;
	
	                                                                               
	imageWindowInfoHandle = GetWindowInfoHandle (windowPtr);
	  
	return (GetSelectionInfoHandle (imageWindowInfoHandle)); 
												
}	// end "GetSelectionInfoHandle"

 
      
SInt16 GetSelectionInfoDisplayUnits (
				WindowInfoPtr							windowInfoPtr)

{	
	SelectionInfoPtr					selectionInfoPtr;	
	
	Handle								selectionInfoHandle = NULL;
											
	SInt16								unitsDisplayCode = kLineColumnUnits;
	
	                                                                               
	if (windowInfoPtr != NULL)
		selectionInfoHandle = windowInfoPtr->selectionInfoHandle;								
		
	selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle);
													
	if (selectionInfoPtr != NULL)
		unitsDisplayCode = selectionInfoPtr->unitsDisplayCode;
		
	return (unitsDisplayCode);
												
}	// end "GetSelectionInfoDisplayUnits"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetShowLegendFlag
//
//	Software purpose:	This routine returns the show legend flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/24/1998
//	Revised By:			Larry L. Biehl			Date: 10/01/1998			

Boolean GetShowLegendFlag (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  	return (windowInfoPtr->showLegend);    
	     
	return (FALSE);
	
}	// end "GetShowLegendFlag"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMFileStream* GetSupportFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to obtain the file stream
//							pointer for the support file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	pointer to the support file stream pointer.
//							NULL if not available.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	10/19/1995
//	Revised By:			Larry L. Biehl			Date: 06/04/1996

CMFileStream* GetSupportFileStreamPointer (
				Handle								windowInfoHandle,
				SignedByte*							handleStatusPtr)

{	
	CMFileStream*						supportFileStreamPtr = NULL;
	
	
			// Get pointer to the window information structure.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
												
	if (windowInfoPtr != NULL)
		{	            
		#if defined multispec_mac  
			supportFileStreamPtr = (CMFileStream*)GetHandleStatusAndPointer (
															windowInfoPtr->supportFileStreamHandle,
															handleStatusPtr);
		#endif	// defined multispec_mac
	
		#if defined multispec_win | defined multispec_lin
			supportFileStreamPtr = 
							windowInfoPtr->cImageWindowPtr->GetSupportFileStreamPointer ();
		#endif	// defined multispec_win | multispec_lin
														
		}	// end "if (windowInfoPtr != NULL)"                    
		
	return (supportFileStreamPtr);
												
}	// end "GetSupportFileStreamPointer"

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMFileStream* GetSupportFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to obtain the file stream
//							pointer for the support file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:  	pointer to the support file stream pointer.
//							NULL if not available.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/1995
//	Revised By:			Larry L. Biehl			Date: 11/07/1995

CMFileStream* GetSupportFileStreamPointer (
				WindowInfoPtr						windowInfoPtr)

{	
	CMFileStream*						supportFileStreamPtr = NULL;
		
	if (windowInfoPtr != NULL)
		{								
		#if defined multispec_mac  
			supportFileStreamPtr = windowInfoPtr->supportFileStreamPtr;
		#endif	// defined multispec_mac 
              
		#if defined multispec_win 
			supportFileStreamPtr = 
							windowInfoPtr->cImageWindowPtr->GetSupportFileStreamPointer ();
		#endif	// defined multispec_win 
													
		}	// end "if (windowInfoPtr != NULL)"                    
		
	return (supportFileStreamPtr);
												
}	// end "GetSupportFileStreamPointer"
*/                 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetTitleHeight
//
//	Software purpose:	This routine returns height for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/24/1996
//	Revised By:			Larry L. Biehl			Date: 04/30/1996			

SInt16 GetTitleHeight (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  	return (windowInfoPtr->titleHeight);    
	     
	return (0);
	
}	// end "GetTitleHeight" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetTotalNumberOfChannels
//
//	Software purpose:	This routine returns height for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/19/1999
//	Revised By:			Larry L. Biehl			Date: 03/19/1999			

UInt32 GetTotalNumberOfChannels (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  	return ((UInt32)windowInfoPtr->totalNumberChannels);    
	     
	return (0);
	
}	// end "GetTotalNumberOfChannels"     



#if defined multispec_win || defined multispec_lin
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMImageWindow* GetWindowClassPointer
//
//	Software purpose:	The purpose of this routine is to return the window information
//							structure handle for the input window pointer.
//
//	Parameters in:		Window pointer
//
//	Parameters out:	None
//
// Value Returned:  	Handle for the window information struction.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	04/07/1998
//	Revised By:			Larry L. Biehl			Date: 04/07/1998

CMImageWindow* GetWindowClassPointer (
				Handle								windowInfoHandle)

{	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
													
	if (windowInfoPtr != NULL)
		return (windowInfoPtr->cImageWindowPtr);	
		
	return (NULL);
												
}	// end "GetWindowClassPointer"
#endif	// defined multispec_win || multispec_lin



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetWindowInfoHandle
//
//	Software purpose:	The purpose of this routine is to return the window information
//							structure handle for the input window pointer.
//
//	Parameters in:		Window pointer
//
//	Parameters out:	None
//
// Value Returned:  	Handle for the window information struction.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	01/25/1996
//	Revised By:			Larry L. Biehl			Date: 01/25/1996

Handle GetWindowInfoHandle (
				WindowPtr							windowPtr)

{	
	if (windowPtr != NULL)
		{				
		#if defined multispec_mac 				
			return ((Handle)GetWRefCon (windowPtr));            	
		#endif	// defined multispec_mac 		
		
		#if defined multispec_win | defined multispec_lin
			CMImageWindow* imageWindowCPtr = windowPtr->GetImageWindowCPtr ();
			if (imageWindowCPtr != NULL)
				return (imageWindowCPtr->GetWindowInfoHandle ());	 
		#endif	// defined multispec_win | multispec_lin
		
		}	// end "if (windowPtr != NULL)"
		
	return (NULL);
												
}	// end "GetWindowInfoHandle"


#if defined multispec_lin
Handle GetWindowInfoHandle (wxView* windowPtr)
	{
	if (windowPtr != NULL) 
		{
		CMImageWindow* imageWindowCPtr = 
												((CMImageView*)windowPtr)->GetImageWindowCPtr ();
		if (imageWindowCPtr != NULL)
			return (imageWindowCPtr->GetWindowInfoHandle ());

		} // end "if (windowPtr != NULL)"

	 return (NULL);

} // end "GetWindowInfoHandle"
#endif	//  defined multispec_lin


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetWindowInfoHandle
//
//	Software purpose:	The purpose of this routine is to return the window information
//							structure handle for the input window pointer.
//
//	Parameters in:		Window pointer
//
//	Parameters out:	None
//
// Value Returned:  	Handle for the window information struction.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	02/26/2013
//	Revised By:			Larry L. Biehl			Date: 02/26/2013

Handle GetWindowInfoHandle (
				WindowInfoPtr						windowInfoPtr)

{	
	Handle								windowInfoHandle = NULL;
	
	
	if (windowInfoPtr != NULL)
		windowInfoHandle = windowInfoPtr->windowInfoHandle;
				
	return (windowInfoHandle);
												
}	// end "GetWindowInfoHandle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetWindowInfoStructures
//
//	Software purpose:	The purpose of this routine is to obtain the memory
//							for and initialize the window information structures.
//
//	Parameters in:		Handle to file information.
//							Variable indicating whether thematic or multispectral
//									type image.
//
//	Parameters out:	None
//
// Value Returned:	Handle to window information structure.
// 
// Called By:			LoadMask in SMask.cpp
//							ChangeProjectBaseImage in SProject.cpp
//							OpenProjectImageWindow in pSProject.cpp
//							OpenImageFile in openImageRoutines.c
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1991
//	Revised By:			Larry L. Biehl			Date: 09/08/2006	

Handle GetWindowInfoStructures (
				Handle								fileInfoHandle, 
				SInt16								fileImageType,
				SInt16								windowType)
				
{
	#if defined multispec_win | defined multispec_lin
		CMImageWindow* 					imageWindowCPtr = NULL;
	#endif	// defined multispec_win | multispec_lin
	
	WindowInfoPtr						windowInfoPtr;
	
	Handle								windowInfoHandle = NULL;
	
	Boolean								continueFlag;
	
	
			// Check input variables.																
			
	if (fileInfoHandle != NULL)
		{
		continueFlag = TRUE;
		
				// Initialize the window information record.	
		
		#if defined multispec_mac
			windowInfoHandle = InitializeWindowInfoStructure (
								NULL, kNotPointer, fileInfoHandle, fileImageType, windowType);
		#endif	// defined multispec_mac
	                  
		#if defined multispec_win | defined multispec_lin
			imageWindowCPtr = new CMImageWindow (fileInfoHandle);
			if (imageWindowCPtr != NULL)
				{
				imageWindowCPtr->SetImageWindowCPtrInWindowInfo (imageWindowCPtr);
				windowInfoHandle = imageWindowCPtr->GetWindowInfoHandle ();
				
				}	// end "if (maskWindowCPtr != NULL)" 
		#endif	// defined multispec_win | multispec_lin
		
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
												windowInfoHandle, kLock, kMoveHi);
													
		if (windowInfoPtr != NULL)
			{			
			if (windowType == kImageWindowType || 
					windowType == kThematicWindowType || 
						windowType == kProjectWindowType)
						// Initialize the "window" file parameters.							
				continueFlag = UpdateLayerInfoStructure (windowInfoPtr, fileInfoHandle);
			
			if (continueFlag)
				CheckAndUnlockHandle (windowInfoHandle);
				
			else	// !continueFlag 
				{
				#if defined multispec_mac
					UnlockAndDispose (windowInfoPtr->layerInfoHandle);
					UnlockAndDispose (windowInfoHandle);
				#endif	// defined multispec_mac
	                  
				#if defined multispec_win || defined multispec_lin
					delete imageWindowCPtr;
				#endif	// defined multispec_win || multispec_lin
				
				windowInfoHandle = NULL;
				
				}	// end "!continueFlag" 
			
			}	// end "if (windowInfoPtr != NULL)" 
			
		}	// end "if (fileInfoHandle)" 
		
	return (windowInfoHandle);

}	// end "GetWindowInfoStructures"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		WindowPtr GetWindowPtr
//
//	Software purpose:	The purpose of this routine is to return the window pointer
//							for the input window structure.
//
//	Parameters in:		window info structure handle			
//
//	Parameters out:	None			
//
//	Value Returned:	0 if now window structure exists
//							window type if others such as kThematicWindowType, or kImageWindowType.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 03/19/1999
//	Revised By:			Larry L. Biehl			Date: 11/03/1999	

WindowPtr GetWindowPtr (
				Handle 								windowInfoHandle)

{  
			// Verify that an offscreen images exists.
																
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	return (GetWindowPtr (windowInfoPtr));
	
}	// end "GetWindowPtr" 



WindowPtr GetWindowPtr (
				WindowInfoPtr 						windowInfoPtr)

{  
	WindowPtr							windowPtr = NULL;
	
	
	if (windowInfoPtr != NULL) 
		{                                     
		#if defined multispec_mac
			return (windowInfoPtr->windowPtr);
		#endif	// defined multispec_mac
               
		#if defined multispec_win | defined multispec_lin
			return (windowInfoPtr->cImageWindowPtr->mImageViewCPtr);
		#endif	// defined multispec_win | multispec_lin
			
		}	// end "if (windowInfoPtr != NULL)"

	return (windowPtr);
	
}	// end "GetWindowPtr" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetWindowType
//
//	Software purpose:	The purpose of this routine is to return the window type
//							variable for this window object.
//
//	Parameters in:		window info structure handle			
//
//	Parameters out:	None			
//
//	Value Returned:	0 if now window structure exists
//							window type if others such as kThematicWindowType, or 
//								kImageWindowType.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 04/03/1996
//	Revised By:			Larry L. Biehl			Date: 03/19/1999	

SInt16 GetWindowType (
				Handle 								windowInfoHandle)

{  															
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)                                                   
		return (windowInfoPtr->windowType);

	return (0);
	
}	// end "GetWindowType" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeWindowInfoStructure
//
//	Software purpose:	The purpose of this routine is to initialize memory for
//							a new Window Information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			CreateGraphicsWindow in graphicsUtilities.c
//							CreateOutputWindow in initialize.c
//							CreateProjectWindow in statistics.c
//							CreatePaletteWindow in window.c
//							GetWindowInfoStructures in window.c
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1991
//	Revised By:			Larry L. Biehl			Date: 07/10/2018

Handle InitializeWindowInfoStructure (
				Handle								windowInfoHandle,
				Boolean								inputPtrFlag,
				Handle								fileInfoHandle, 
				SInt16								imageType,
				SInt16								windowType)

{
	WindowInfoPtr						windowInfoPtr;
	SignedByte							handleStatus;
								
	
	if (windowInfoHandle == NULL)
		{                               
		windowInfoHandle = MNewHandle (sizeof (WindowInfo));
		inputPtrFlag = FALSE;
		
		}	// end "if (windowInfoHandle == NULL)"
	
	if (windowInfoHandle != NULL)
		{
		if (inputPtrFlag)
			windowInfoPtr = (WindowInfoPtr)windowInfoHandle;
			
		else	// !inputPtrFlag
			windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																	windowInfoHandle, &handleStatus);
							
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		
		windowInfoPtr->cImageWindowPtr = NULL;
		windowInfoPtr->windowPtr = NULL;
		
		#if defined multispec_mac
			windowInfoPtr->cgInfo.contextRef = NULL;
			windowInfoPtr->cgInfo.imageRef = NULL;
			windowInfoPtr->cgInfo.contextRowBytes = 0;
		#endif	// defined multispec_mac
		
		windowInfoPtr->boundingLatLongRectangle.top = DBL_MAX;
		windowInfoPtr->boundingLatLongRectangle.bottom = -DBL_MAX;
		windowInfoPtr->boundingLatLongRectangle.left = DBL_MAX;
		windowInfoPtr->boundingLatLongRectangle.right = -DBL_MAX;
		
		windowInfoPtr->boundingMapRectangle.top = DBL_MAX;
		windowInfoPtr->boundingMapRectangle.bottom = -DBL_MAX;
		windowInfoPtr->boundingMapRectangle.left = DBL_MAX;
		windowInfoPtr->boundingMapRectangle.right = -DBL_MAX;
		
		ImageSecondOrderStatsPtr imageSecondOrderStatsPtr = 
															&windowInfoPtr->imageSecondOrderStats;
													
		imageSecondOrderStatsPtr->sumsHandle = NULL;
		imageSecondOrderStatsPtr->sumSquaresHandle = NULL;					
		imageSecondOrderStatsPtr->lineStart = 1; 						
		imageSecondOrderStatsPtr->lineEnd = 1;				
		imageSecondOrderStatsPtr->lineInterval = 1; 					
		imageSecondOrderStatsPtr->columnStart = 1; 					
		imageSecondOrderStatsPtr->columnEnd = 1;				
		imageSecondOrderStatsPtr->columnInterval = 1;							
		imageSecondOrderStatsPtr->totalPixels = 0;												
		imageSecondOrderStatsPtr->numberChannels = 0;
		
		InitializeMaskStructure (&windowInfoPtr->mask);
		
		windowInfoPtr->overlayList[0].index = 0;
		
		windowInfoPtr->coordinateViewAreaFactor = 1.;
		windowInfoPtr->coordinateViewFactor = 1.;
		windowInfoPtr->maxUsableDataValue = 255;
		windowInfoPtr->minUsableDataValue = 0;
		
		windowInfoPtr->printPageOrientation = kPMPortrait;
		windowInfoPtr->printPageScale = 100;
				
		windowInfoPtr->coordinateViewAreaUnitsControl = NULL;
		windowInfoPtr->coordinateViewControl = NULL;
		windowInfoPtr->coordinateUnitsControl = NULL;
		windowInfoPtr->horizontalControl = NULL;
		windowInfoPtr->legendPaletteControl = NULL;
		windowInfoPtr->legendScrollDownControl = NULL;
		windowInfoPtr->legendScrollUpControl = NULL;
		windowInfoPtr->magnificationControl = NULL;
		windowInfoPtr->overlayControl = NULL;
		windowInfoPtr->verticalControl = NULL;
		windowInfoPtr->zoomInControl = NULL;
		windowInfoPtr->zoomOutControl = NULL;
		
		windowInfoPtr->graphViewCPtr = NULL;
		windowInfoPtr->supportFileStreamPtr = NULL;
		windowInfoPtr->maskFileStreamPtr = NULL;
		//windowInfoPtr->descriptionH = NULL;
		windowInfoPtr->displayLevelHandle = NULL;
		windowInfoPtr->displaySpecsHandle = NULL;
		windowInfoPtr->fileInfoHandle = fileInfoHandle;
		windowInfoPtr->histogramSpecsHandle = NULL;
		windowInfoPtr->histogramSummaryHandle = NULL;
		windowInfoPtr->imageBaseAddressH = NULL;
		windowInfoPtr->layerInfoHandle = NULL;
		windowInfoPtr->mapProjectionHandle = NULL;
		windowInfoPtr->offScreenMapHandle = NULL;
		windowInfoPtr->selectionInfoHandle = NULL;
		windowInfoPtr->supportFileStreamHandle = NULL; 
		windowInfoPtr->windowInfoHandle = windowInfoHandle; 
		windowInfoPtr->windowRecordHandle = NULL;
		windowInfoPtr->legendListHandle = NULL;
		windowInfoPtr->offscreenGWorld = NULL;
		windowInfoPtr->textH = NULL;
		windowInfoPtr->maxNumberColumns = 0;
		windowInfoPtr->maxNumberLines = 0;
		windowInfoPtr->numberBins = 0;
		windowInfoPtr->numberOverlays = 0;
		windowInfoPtr->numberImageOverlays = 0;
		windowInfoPtr->offscreenMapSize = 0;
		windowInfoPtr->coordinateViewAreaUnitsCode = kNumberPixelsUnitsMenuItem;
		windowInfoPtr->bandInterleave = 0;
		windowInfoPtr->changed = 0;
		windowInfoPtr->channelsInWavelengthOrderCode = kNotApplicable;
		windowInfoPtr->coordinateViewUnitsCode = kLineColumnUnitsMenuItem;
		
		windowInfoPtr->coordinateHeight = 0;
		windowInfoPtr->coordinateViewAreaPopupStart = 0;
		windowInfoPtr->coordinateViewCursorStart = 0;
		windowInfoPtr->coordinateViewNumberChars = 0;
		windowInfoPtr->coordinateViewSelectionStart = 0;
		windowInfoPtr->coordinateViewNumberPixelsStart = 0;
		windowInfoPtr->coordinateViewNumberPixelsChars = 0;
		windowInfoPtr->coordinateViewScaleStart = 0;
		windowInfoPtr->imageTopOffset = 0;
		
		windowInfoPtr->descriptionCode = 0;
		if (fileInfoPtr != NULL) 
			windowInfoPtr->descriptionCode = fileInfoPtr->descriptionCode;			
			
		windowInfoPtr->fileInfoVersion = 0;
		windowInfoPtr->grafPortType = 0;	
		windowInfoPtr->legendLeftStart = 0;
		windowInfoPtr->legendTitleWidth = 0;
		windowInfoPtr->legendTopStart = 0;			
		windowInfoPtr->legendWidth = 0;
		windowInfoPtr->dataTypeCode = kIntegerType;
		windowInfoPtr->localMaxNumberBytes = 0;
		windowInfoPtr->imageType = imageType;
		windowInfoPtr->maskFileLayer = 0;
		
		windowInfoPtr->maxNumberDescriptionCharacters = 0;
		if (fileInfoPtr != NULL)
			windowInfoPtr->maxNumberDescriptionCharacters = 
													fileInfoPtr->maxNumberDescriptionCharacters;
					
		windowInfoPtr->supportFileCode = 0;
		windowInfoPtr->titleHeight = 0;
		windowInfoPtr->windowMenuItem = 0;
		windowInfoPtr->windowNumber = 0;
		windowInfoPtr->windowType = windowType;
		windowInfoPtr->numberBits = 0;
		windowInfoPtr->numberBytes = 0;
		windowInfoPtr->numberImageFiles = 0;
		windowInfoPtr->totalNumberChannels = 0;
		
		windowInfoPtr->drawBaseImageFlag = TRUE;
		windowInfoPtr->drawVectorOverlaysFlag = TRUE;
		//windowInfoPtr->hasWavelengthValuesFlag = FALSE;
		windowInfoPtr->windowChangedFlag = FALSE;
		windowInfoPtr->localBytesDifferFlag = FALSE;
		windowInfoPtr->projectWindowFlag = FALSE;
		windowInfoPtr->projectBaseImageFlag = FALSE;
		windowInfoPtr->showLegend = FALSE;
		windowInfoPtr->signedDataFlag = FALSE;
		
		if (!inputPtrFlag)
			MHSetState (windowInfoHandle, handleStatus);
		
		}	// end "if (windowInfoHandle != NULL)"
		
	return (windowInfoHandle);
			
}	// end "InitializeWindowInfoStructure"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean OffscreenImageMapExists
//
//	Software purpose:	This routine returns a flag indicating whether an offscreen
//							image map exists for the input window information structure
//							handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1999
//	Revised By:			Larry L. Biehl			Date: 03/14/2015			

Boolean OffscreenImageMapExists (
				Handle								windowInfoHandle)

{ 
	WindowInfoPtr						windowInfoPtr;
	
	Boolean								offscreenImageMapExistsFlag = FALSE;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL) 
		{   
		#ifndef multispec_lin
			if (windowInfoPtr->offScreenMapHandle != NULL)
				offscreenImageMapExistsFlag = TRUE;
		#endif	// not multispec_lin
		
		#if defined multispec_lin
			WindowPtr windowPtr = GetWindowPtr (windowInfoPtr);
			if (windowPtr != NULL)
				offscreenImageMapExistsFlag = windowPtr->CheckIfOffscreenImageExists ();
		#endif	// defined multispec_lin

		}	// end "if (windowInfoPtr != NULL) "
	     
	return (offscreenImageMapExistsFlag);
		
}	// end "OffscreenImageMapExists"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseOffscreenSupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the offscreen
//							memory for the window.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 12/24/1991
//	Revised By:			Larry L. Biehl			Date: 11/02/2001	

void ReleaseOffscreenSupportMemory (
				WindowInfoPtr						windowInfoPtr)

{  
	#if defined multispec_mac 
		GWorldPtr							offscreenGWorld;
		PixMapHandle						pixMapHandle;
	
		if (windowInfoPtr->offscreenGWorld != NULL)
			{
			offscreenGWorld = windowInfoPtr->offscreenGWorld;
			pixMapHandle = GetPortPixMap (offscreenGWorld);
			
			UnlockPixels (pixMapHandle);
			AllowPurgePixels (pixMapHandle);
			DisposeGWorld (offscreenGWorld);
			
			windowInfoPtr->offscreenGWorld = NULL;
			windowInfoPtr->offScreenMapHandle = NULL;
			windowInfoPtr->offscreenMapSize = 0;
			
			}	// end "if (windowInfoPtr->offscreenGWorld != NULL)" 
			
		if (gOSXCoreGraphicsFlag)
			{
					// Release the core graphics image.
			
			if (windowInfoPtr->cgInfo.imageRef != NULL)		
				gCGImageReleasePtr (windowInfoPtr->cgInfo.imageRef);
			
			//if (windowInfoPtr->cgInfo.contextRef != NULL)
			//	gCGContextReleasePtr (windowInfoPtr->cgInfo.contextRef);
				
			UnlockAndDispose (windowInfoPtr->imageBaseAddressH);
			
			if (windowInfoPtr->offScreenMapHandle != NULL)	
				DisposePixMap ((PixMapHandle)windowInfoPtr->offScreenMapHandle);
				
			windowInfoPtr->cgInfo.imageRef = NULL;
			//windowInfoPtr->cgInfo.contextRef = NULL;
			windowInfoPtr->cgInfo.contextRowBytes = 0;
			
			windowInfoPtr->imageBaseAddressH = NULL;
			windowInfoPtr->offScreenMapHandle = NULL;
			windowInfoPtr->offscreenMapSize = 0;
		
			}	// end "if (gOSXCoreGraphicsFlag)"
	#endif // defined multispec_mac 
			                               
	#if defined multispec_win
		windowInfoPtr->offScreenMapHandle =  
										UnlockAndDispose (windowInfoPtr->offScreenMapHandle);
		
		windowInfoPtr->imageBaseAddressH = 							
										UnlockAndDispose (windowInfoPtr->imageBaseAddressH);
		
		windowInfoPtr->offscreenMapSize = 0;
		
		CMImageView* imageViewCPtr = GetWindowPtr (windowInfoPtr);
		imageViewCPtr->SetLegendBitMapInfoHeaderHandle (NULL);
	#endif // defined multispec_win
	
	#if defined multispec_lin
		//windowInfoPtr->offScreenMapHandle =
		//								UnlockAndDispose (windowInfoPtr->offScreenMapHandle);
	
		windowInfoPtr->offscreenMapSize = 0;
	
		CMImageView* imageViewCPtr = GetWindowPtr (windowInfoPtr);
		imageViewCPtr->SetLegendBitMapInfoHeaderHandle (NULL);
	#endif // defined multispec_lin
	
}	// end "ReleaseOffscreenSupportMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateHeight
//
//	Software purpose:	The purpose of this routine is to set the display map
//							coordinate flag for the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/02/2000
//	Revised By:			Larry L. Biehl			Date: 01/04/2019

void SetCoordinateHeight (
				Handle								windowInfoHandle,
				UInt32								coordinateHeight)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		{
		windowInfoPtr->coordinateHeight = coordinateHeight;
		
		#if defined multispec_mac || defined multispec_win
					// Note that for wxWidgets version, the coordinate bar is not a
					// part of the image panel
			windowInfoPtr->imageTopOffset =
								windowInfoPtr->titleHeight + windowInfoPtr->coordinateHeight;
		 					
			windowInfoPtr->legendTopStart = (SInt16)windowInfoPtr->coordinateHeight + 1;
		#endif
		
		}	// end "if (windowInfoPtr != NULL)"
												
}	// end "SetCoordinateHeight"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewAreaFactor
//
//	Software purpose:	The purpose of this routine is to set the factor to use to
//							convert number of pixels to user selected area for the window
//							represented by the input window information structure.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

void SetCoordinateViewAreaFactor (
				Handle								windowInfoHandle,
				double								coordinateViewAreaFactor)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewAreaFactor = coordinateViewAreaFactor;
												
}	// end "SetCoordinateViewAreaFactor"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewAreaPopupStart
//
//	Software purpose:	The purpose of this routine is to set the start location for the
//							area popup menu in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

void SetCoordinateViewAreaPopupStart (
				Handle								windowInfoHandle,
				UInt32								coordinateViewAreaPopupStart)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewAreaPopupStart = coordinateViewAreaPopupStart;
												
}	// end "SetCoordinateViewAreaPopupStart" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewAreaUnits
//
//	Software purpose:	The purpose of this routine is to set the coordinate view
//							units setting.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

void SetCoordinateViewAreaUnits (
				Handle								windowInfoHandle,
				SInt16								coordinateViewAreaUnits)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewAreaUnitsCode = coordinateViewAreaUnits;
												
}	// end "SetCoordinateViewAreaUnits"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewAreaUnits
//
//	Software purpose:	The purpose of this routine is to set the coordinate view
//							control pointer. It will be needed later for making setting
//							changes.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	07/31/2017
//	Revised By:			Larry L. Biehl			Date: 07/31/2017

void SetCoordinateViewAreaUnitsControl (
				Handle								windowInfoHandle,
				ControlHandle						coordinateViewAreaUnitsControl)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewAreaUnitsControl = coordinateViewAreaUnitsControl;
												
}	// end "SetCoordinateViewAreaUnitsControl"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewCursorStart
//
//	Software purpose:	The purpose of this routine is to set the start location for the
//							cursor information in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/07/2000

void SetCoordinateViewCursorStart (
				Handle								windowInfoHandle,
				UInt32								coordinateViewCursorStart)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewCursorStart = coordinateViewCursorStart;
												
}	// end "SetCoordinateViewCursorStart"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewFactor
//
//	Software purpose:	The purpose of this routine is to set the coordinate view factor
//							in the window information structure.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/09/2000
//	Revised By:			Larry L. Biehl			Date: 11/09/2000

void SetCoordinateViewFactor (
				Handle								windowInfoHandle,
				double								coordinateViewFactor)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewFactor = coordinateViewFactor;
												
}	// end "SetCoordinateViewFactor"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewNumberChars
//
//	Software purpose:	The purpose of this routine is to set the start location for the
//							selection information in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/07/2000

void SetCoordinateViewNumberChars (
				Handle								windowInfoHandle,
				UInt32								coordinateViewNumberChars)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewNumberChars = coordinateViewNumberChars;
												
}	// end "SetCoordinateViewNumberChars" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewNumberPixelsChars
//
//	Software purpose:	The purpose of this routine is to set the maximum number of
//							characters to allow for when writing the number of pixel in the
//							coordinate view in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/09/2000
//	Revised By:			Larry L. Biehl			Date: 11/09/2000

void SetCoordinateViewNumberPixelsChars (
				Handle								windowInfoHandle,
				UInt32								coordinateViewNumberPixelsChars)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewNumberPixelsChars = 
																	coordinateViewNumberPixelsChars;
												
}	// end "SetCoordinateViewNumberPixelsChars" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewNumberPixelsStart
//
//	Software purpose:	The purpose of this routine is to set the start location for
//							writing the number of pixels in the coordinate view in the input 
//							window information structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/09/2000
//	Revised By:			Larry L. Biehl			Date: 11/09/2000

void SetCoordinateViewNumberPixelsStart (
				Handle								windowInfoHandle,
				UInt32								coordinateViewNumberPixelsStart)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewNumberPixelsStart = 
																	coordinateViewNumberPixelsStart;
												
}	// end "SetCoordinateViewNumberPixelsStart"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewScaleStart
//
//	Software purpose:	The purpose of this routine is to set the start location for the
//							scale information in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/22/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000

void SetCoordinateViewScaleStart (
				Handle								windowInfoHandle,
				UInt32								coordinateViewScaleStart)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewScaleStart = coordinateViewScaleStart;
												
}	// end "SetCoordinateViewScaleStart"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewSelectionStart
//
//	Software purpose:	The purpose of this routine is to set the start location for the
//							selection information in the input window information 
//							structure handle.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/07/2000

void SetCoordinateViewSelectionStart (
				Handle								windowInfoHandle,
				UInt32								coordinateViewSelectionStart)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewSelectionStart = coordinateViewSelectionStart;
												
}	// end "SetCoordinateViewSelectionStart" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewUnits
//
//	Software purpose:	The purpose of this routine is to set the coordinate view
//							units setting.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/07/2000

void SetCoordinateViewUnits (
				Handle								windowInfoHandle,
				SInt16								coordinateViewUnits)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateViewUnitsCode = coordinateViewUnits;
												
}	// end "SetCoordinateViewUnits"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCoordinateViewUnitsControl
//
//	Software purpose:	The purpose of this routine is to set the coordinate view
//							units setting.
//
//	Parameters in:		Window information structure handle
//							setting to be used
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	07/31/2017
//	Revised By:			Larry L. Biehl			Date: 07/31/2017

void SetCoordinateViewUnitsControl (
				Handle								windowInfoHandle,
				ControlHandle						coordinateUnitsControl)

{	
			// Get pointer to the support file information.
												
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)
		windowInfoPtr->coordinateUnitsControl = coordinateUnitsControl;
												
}	// end "SetCoordinateViewUnitsControl"  	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetFileMapProjectionHandle2
//
//	Software purpose:	The purpose of this routine is to return the map projection
//							handle for the input window information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	map projection handle
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	08/02/2010
//	Revised By:			Larry L. Biehl			Date: 08/02/2010

void SetFileMapProjectionHandle2 (
				Handle								windowInfoHandle,
				Handle								mapProjectionHandle)

{	
	Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
	
	SetFileMapProjectionHandle (fileInfoHandle, mapProjectionHandle);
	
}	// end "SetFileMapProjectionHandle2" 


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetHasWavelengthValuesFlag
//
//	Software purpose:	The purpose of this routine is to set the hasWavelengthValueFlag
//							variable for this window object.
//
//	Parameters in:		window info structure handle	
//							value to set the hasWavelengthValuesFlag to
//
//	Parameters out:	None			
//
//	Value Returned:	None
//							
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 03/01/2018
//	Revised By:			Larry L. Biehl			Date: 03/01/2018

void SetHasWavelengthValuesFlag (
				Handle 								windowInfoHandle,
				Boolean								hasWavelengthValuesFlag)

{  															
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)                                                   
		windowInfoPtr->hasWavelengthValuesFlag = hasWavelengthValuesFlag;
	
}	// end "SetHasWavelengthValuesFlag"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetHistogramSpecsHandle
//
//	Software purpose:	This routine sets the histogram specs handle
//							for the input window information structure pointer.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1999
//	Revised By:			Larry L. Biehl			Date: 11/02/1999			

void SetHistogramSpecsHandle (
				WindowInfoPtr						windowInfoPtr,
				Handle								histogramSpecsHandle)

{ 
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac 
			 windowInfoPtr->histogramSpecsHandle = histogramSpecsHandle;
		#endif	// defined multispec_mac 
		              
		#if defined multispec_win | defined multispec_lin			
			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
			
	   	if (imageWindowCPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = imageWindowCPtr->mImageViewCPtr;
	   		
	   		if (imageViewCPtr != NULL)
					 imageViewCPtr->m_histogramCPtr->mHistogramSpecsHandle = 
					 																histogramSpecsHandle;
								
				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win | multispec_lin
		
		}	// end "if (windowInfoPtr != NULL)"
	
}	// end "SetHistogramSpecsHandle"     



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetHistogramSummaryHandle
//
//	Software purpose:	This routine sets the histogram summary handle
//							for the input window information structure pointer.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1999
//	Revised By:			Larry L. Biehl			Date: 11/02/1999			
// TODO for linux
void SetHistogramSummaryHandle (
				WindowInfoPtr						windowInfoPtr,
				Handle								histogramSummaryHandle)

{ 
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac 
			 windowInfoPtr->histogramSummaryHandle = histogramSummaryHandle;
		#endif	// defined multispec_mac 
		
		#if defined multispec_win || defined multispec_lin			
			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
			
	   	if (imageWindowCPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = imageWindowCPtr->mImageViewCPtr;
	   		
	   		if (imageViewCPtr != NULL)
					 imageViewCPtr->m_histogramCPtr->mHistogramSummaryHandle = 
																				histogramSummaryHandle;
								
				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win || defined multispec_lin
		
		}	// end "if (windowInfoPtr != NULL)"
	
}	// end "SetHistogramSummaryHandle"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetHistogramSupportFileFormat
//
//	Software purpose:	This routine set the histogram stat file format for the 
//							window refered to by the input window information structure 
//							handle.
//
//	Parameters in:		Handle to window information structure		
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/08/2009
//	Revised By:			Larry L. Biehl			Date: 07/08/2009		

void SetHistogramSupportFileFormat (
				Handle								windowInfoHandle,
				SInt16								statFileFormat)

{
	HistogramSpecsPtr 				histogramSpecsPtr;
	
	Handle 								histogramSpecsHandle;
	
	
	histogramSpecsHandle = GetHistogramSpecsHandle (windowInfoHandle);
	
	histogramSpecsPtr = (HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);
		
	if (histogramSpecsPtr != NULL)
		histogramSpecsPtr->statFileFormat = statFileFormat;
	
}	// end "SetHistogramSupportFileFormat" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetImageType
//
//	Software purpose:	The purpose of this routine is to set the image type
//							variable for this window object.
//
//	Parameters in:		window info structure handle	
//							image type such as kMultispectralImageType, or kThematicImageType.
//
//	Parameters out:	None			
//
//	Value Returned:	None
//							
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 11/21/2006
//	Revised By:			Larry L. Biehl			Date: 11/21/2006	

void SetImageType (
				Handle 								windowInfoHandle,
				SInt16								imageType)

{  															
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)                                                   
		windowInfoPtr->imageType = imageType;
	
}	// end "SetImageType" 

	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetImageWindowIsUpToDateFlag
//
//	Software purpose:	This routine gets the code for the thematic class and or
//							grouping that is currently shown for the window represented
//							by the input window information structure pointer.
//
//	Parameters in:		None			
//
//	Parameters out:	None				
//
// Value Returned:	Code for the thematic class and or grouping being shown.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/19/1999
//	Revised By:			Larry L. Biehl			Date: 07/19/1999

void SetImageWindowIsUpToDateFlag (
				WindowInfoPtr						windowInfoPtr,
				Boolean								imageWindowIsUpToDateFlag)

{
	Handle displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
	
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					displaySpecsHandle);
								
	if (displaySpecsPtr != NULL)
		displaySpecsPtr->imageWindowIsUpToDateFlag = imageWindowIsUpToDateFlag;
		
}	// end "SetImageWindowIsUpToDateFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetLayerInfoHandle
//
//	Software purpose:	This routine sets the layer information handle
//							for the input window information structure pointer.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1995
//	Revised By:			Larry L. Biehl			Date: 03/01/1999			

void SetLayerInfoHandle (
				WindowInfoPtr						windowInfoPtr,
				Handle								layerInfoHandle)

{ 
	if (windowInfoPtr != NULL)
	  	windowInfoPtr->layerInfoHandle = layerInfoHandle;
	
}	// end "SetLayerInfoHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetOutputWindowChangedFlag
//
//	Software purpose:	The purpose of this routine is to set the window changed
//							flag.
//
//	Parameters in:		window info structure handle			
//
//	Parameters out:	None			
//
//	Value Returned:	0 if now window structure exists
//							window type if others such as kThematicWindowType, or 
//								kImageWindowType.
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1998
//	Revised By:			Larry L. Biehl			Date: 01/16/1998	

void SetOutputWindowChangedFlag (
				Boolean								windowChangedFlag)

{ 
	Handle								windowInfoHandle;
	
	
	#if defined multispec_mac 
		windowInfoHandle = (Handle)GetWRefCon (gOutputWindow);            
	#endif	// defined multispec_mac 
	
	
	#if defined multispec_win || defined multispec_lin
		//CMImageWindow* imageWindowCPtr = windowPtr->GetImageWindowCPtr ();
		//if (imageWindowCPtr != NULL)
		//	windowInfoHandle = imageWindowCPtr->GetWindowInfoHandle ();

		windowInfoHandle = NULL;
	#endif	// defined multispec_win || multispec_lin
																
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)                                                   
		windowInfoPtr->windowChangedFlag = windowChangedFlag;
	
}	// end "SetOutputWindowChangedFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetPrintPageOrientation
//
//	Software purpose:	This routine sets the code for the printer page orientation in
//							the input window information structure.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/18/2011
//	Revised By:			Larry L. Biehl			Date: 08/18/2011			

void SetPrintPageOrientation (
				Handle								windowInfoHandle,
				PMOrientation						printPageOrientation)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  windowInfoPtr->printPageOrientation = printPageOrientation; 
	
}	// end "SetPrintPageOrientation"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetPrintPageScale
//
//	Software purpose:	This routine set the scale for the printer page scale in
//							the input window information structure.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/18/2011
//	Revised By:			Larry L. Biehl			Date: 08/18/2011			

void SetPrintPageScale (
				Handle								windowInfoHandle,
				double								printPageScale)

{ 
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL) 
	  windowInfoPtr->printPageScale = printPageScale;
	  	
}	// end "SetPrintPageScale"    


 
      
void SetSelectionInfoDisplayUnits (
				WindowInfoPtr							windowInfoPtr,
				SInt16									unitsDisplayCode)

{	
	SelectionInfoPtr					selectionInfoPtr;	
	
	Handle								selectionInfoHandle = NULL;
	
	                                                                               
	if (windowInfoPtr != NULL)
		selectionInfoHandle = windowInfoPtr->selectionInfoHandle;								
		
	selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle);
													
	if (selectionInfoPtr != NULL)
		selectionInfoPtr->unitsDisplayCode = unitsDisplayCode;
												
}	// end "SetSelectionInfoDisplayUnits"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetSupportFileStreamHandle
//
//	Software purpose:	The purpose of this routine is to set the support file stream
//							handle into the input window information structure.
//
//	Parameters in:		Window information handle
//							Support file stream handle
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	01/10/1999
//	Revised By:			Larry L. Biehl			Date: 01/10/1999

void SetSupportFileStreamHandle (
				Handle								windowInfoHandle)

{	
	WindowInfoPtr 						windowInfoPtr;
	
	SignedByte							handleStatus;
	
	
			// Get pointer to the window information structure.
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																	windowInfoHandle, &handleStatus);
												
	if (windowInfoPtr != NULL)
		{								
		#if defined multispec_mac 
			if (windowInfoPtr->supportFileStreamHandle == NULL)
				{	
						// Get support file stream structure.			
				
				Handle supportFileStreamHandle = InitializeFileStream ((Handle)NULL);
				
				windowInfoPtr->supportFileStreamHandle = supportFileStreamHandle;
				
				}	// end "if (windowInfoPtr->supportFileStreamHandle == NULL)"
		#endif	// defined multispec_mac 
	              
		#if defined multispec_win || multispec_lin
			windowInfoPtr->cImageWindowPtr->SetSupportFileStreamPointer ();
		#endif	// defined multispec_win
				
				// Unlock the window information handle if needed.						
		
		MHSetState (windowInfoHandle, handleStatus);
		
		}	// end "if (windowInfoPtr != NULL)"
												
}	// end "SetSupportFileStreamHandle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetWindowType
//
//	Software purpose:	The purpose of this routine is to set the window type
//							variable for this window object.
//
//	Parameters in:		window info structure handle		
//							window type if others such as kThematicWindowType, or 
//								kImageWindowType
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 11/21/2006
//	Revised By:			Larry L. Biehl			Date: 11/21/2006	

void SetWindowType (
				Handle 								windowInfoHandle,
				SInt16								windowType)

{  															
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	if (windowInfoPtr != NULL)                                                   
		windowInfoPtr->windowType = windowType;
	
}	// end "SetWindowType" 




//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnlockActiveSupportFileStream
//
//	Software purpose:	The purpose of this routine is to unlock the input
//							file stream for the active image window.
//
//	Parameters in:		File Stream pointer
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

void UnlockActiveSupportFileStream (
				SignedByte							handleStatus)

{	
	#if defined multispec_mac 
		if (handleStatus >= 0)
			{
					// Get pointer to the support file information.
												
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																				gActiveImageWindowInfoH);
		
			if (windowInfoPtr != NULL)
				CheckAndUnlockHandle (windowInfoPtr->supportFileStreamHandle);
				
			}	// end "if (handleStatus >= 0)"      
	#endif	// defined multispec_mac 

}	// end "UnlockActiveSupportFileStream"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnlockAndDisposeSupportFileStream
//
//	Software purpose:	The purpose of this routine is to unlock and dispose of the
//							support file stream handle for the input window handle.
//
//	Parameters in:		File Stream pointer
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	01/10/1999
//	Revised By:			Larry L. Biehl			Date: 01/10/1999

void UnlockAndDisposeSupportFileStream (
				Handle								windowInfoHandle)

{	
	#if defined multispec_mac 
		Handle							supportFileStreamHandle;
		
	
				// Get pointer to the window information structure.
											
		WindowInfoPtr windowInfoPtr = 
											(WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
		if (windowInfoPtr != NULL)
			{
			supportFileStreamHandle = windowInfoPtr->supportFileStreamHandle;
			windowInfoPtr->supportFileStreamHandle = NULL;
					
					// Make sure that the file is closed.
					
			CMFileStream* supportFileStreamPtr = (CMFileStream*)GetHandlePointer (
																				supportFileStreamHandle);
			CloseFile (supportFileStreamPtr);
		
			UnlockAndDispose (supportFileStreamHandle);
			
			}	// end "if (windowInfoPtr != NULL)"
	#endif	// defined multispec_mac 
              
}	// end "UnlockAndDisposeSupportFileStream"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnlockFileStream
//
//	Software purpose:	The purpose of this routine is to unlock the input
//							file stream.
//
//	Parameters in:		File Stream pointer
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

void UnlockFileStream (
				Handle								windowInfoHandle,
				SignedByte							handleStatus)

{	
	#if defined multispec_mac 
		if (handleStatus >= 0)
			{
					// Get pointer to the support file information.
												
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
		
			if (windowInfoPtr != NULL)
				CheckAndUnlockHandle (windowInfoPtr->fileInfoHandle); 
				
			}	// end "if (handleStatus >=0)"      
	#endif	// defined multispec_mac 
              
}	// end "UnlockFileStream"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnlockSupportFileStream
//
//	Software purpose:	The purpose of this routine is to unlock the input
//							file stream.
//
//	Parameters in:		File Stream pointer
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

void UnlockSupportFileStream (
				Handle								windowInfoHandle,
				SignedByte							handleStatus)

{	
	#if defined multispec_mac 
		if (handleStatus >= 0)
			{
					// Get pointer to the support file information.
												
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
		
			if (windowInfoPtr != NULL)
				CheckAndUnlockHandle (windowInfoPtr->supportFileStreamHandle);
				
			}	// end "if (handleStatus >=0)"      
	#endif	// defined multispec_mac 
              
}	// end "UnlockSupportFileStream"

