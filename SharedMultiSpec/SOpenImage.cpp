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
//	File:						SOpenImage.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/29/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains routines which are used to control and 
//								access various image file formats.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		.
//		.
//		etc.
//
//		GetDefaultSupportFile
//			CopyPToP (in SUtilities.cpp)
//			RemoveCharsNoCase (in SUtilities.cpp)
//			ConcatPStrings (in SUtility.cpp)
//			IOCheck (in SFileIO.cpp)
//			CloseFile (in SFileIO.cpp)
//
/* Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
													" SOpenImage: (): %s",
													gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx
	Rect rect;
	#include "wx/app.h"
	#include "wx/display.h"
	#include "wx/docview.h"
	
	#include "SFileStream_class.h"
	#include "SImageWindow_class.h"
	
	#include "xImageDoc.h"	
	#include "xImageFrame.h" 
	#include "xImageView.h"
	#include "xMultiSpec.h"

	#if include_gdal_capability
			// Set this to 1 to force MultiSpec to use built in geotiff/tiff reader
			// instead of the one in gdal.
		#define use_multispec_tiffcode	0
	#else	// not defined include_gdal_capability
		#define use_multispec_tiffcode	1
	#endif
	
	#define	kChanDescriptionStrID			0
#endif	// defined multispec_wx

#if defined multispec_mac || defined multispec_mac_swift
	#define	kChanDescriptionStrID			173
	#define	IDS_ChanDescription1				1
	#define	IDS_ChanDescription9				9
	#define	IDS_ChanDescription14			14	
	#define	IDS_ChanDescription28			28	
	#define	IDS_ChanDescription32			32
	#define	IDS_ChanDescription36			36
	#define	IDS_ChanDescription37			37
	#define	IDS_ChanDescription48			48
	#define	IDS_ChanDescription53			53
	#define	IDS_ChanDescription66			66
	#define	IDS_ChanDescription79			79
	#define	IDS_ChanDescription83			83
	#define	IDS_Dialog33						33
	#define	IDS_FileIO158						158
	#define	IDS_FileIO160						160

	extern GDHandle GetDevicePointIsIn (
			  Point* locationPtr);

	#if include_gdal_capability
		#define use_multispec_tiffcode	0
	#else	// not defined include_gdal_capability
		#define use_multispec_tiffcode	1
	#endif
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	#include "SImageWindow_class.h"
	#include "WImageDoc.h"
	#include "WFileFormatDialog.h"

	#define	kChanDescriptionStrID			0	 

	#if include_gdal_capability
		#define use_multispec_tiffcode	0
	#else	// not defined include_gdal_capability
		#define use_multispec_tiffcode	1
	#endif
#endif	// defined multispec_win

#if include_gdal_capability
			// oul: added definition of SIZE_UNSIGNED_LONG and SIZEOF_VOIDP
			// which are not defined in cpl_config.h

	#if defined multispec_wx
		#if defined NetBeansProject
			/* The size of a 'unsigned long', as computed by sizeof. */
			#define SIZEOF_UNSIGNED_LONG 8
			/* The size of a 'void p', as computed by sizeof. */
			#define SIZEOF_VOIDP 8
		#endif
	#endif
	
	#include "gdal.h"
#endif	// include_gdal_capability

#include "errno.h"


#define	kDoNotCloseOverlayFileIfNoRecordsRead		0
#define	kCloseOverlayFileIfNoRecordsRead				1

extern SInt16 ReadVectorWithGDALLibrary (
				Handle								windowInfoHandle,
				FileInfoPtr 						fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);


// Prototypes for routines in this file that are only called by		
// other routines in this file.

void AdjustImageWSize (
				Handle								windowInfoHandle);

float* CheckChannelValuesHandleSize (
				FileInfoPtr							fileInfoPtr,
				Boolean*								continueFlagPtr);

SInt16 CheckForENVIHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr);

SInt16 CheckForPDS_LBL_File (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr);

SInt16 DetermineIfFileSpecsDialogNeedsCalled (
				Handle								windowInfoHandle,
				FileInfoPtr							fileInfoPtr,
				SInt16								linkFileNumber);

SInt16 DeterminePeruSatBandOrder (
				FileInfoPtr							fileInfoPtr);

void FinishMapInformationSetUp (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr);

SInt32 GetFileHeaderValues (
				SInt16								stringID,
				SInt16								stringNumber,
				CharPtr								bufferPtr,
				SInt16								numberValues,
				SInt32*								valuesPtr,
				Boolean								skipEqualFlag,
				SInt16*								returnCodePtr);

void GetInstrumentChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr);

void GetInstrumentCode (
				FileInfoPtr							fileInfoPtr);
				
void GetMultiSpecChannelWidths (
				FileInfoPtr							fileInfoPtr);

//SInt16 GetChannelWavelengthOrder (
//				FileInfoPtr							fileInfoPtr);

SInt16 GetChannelWavelengthOrder (
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr);

Boolean LoadSelectedDataSetInformation (
				Handle								fileInfoHandle);

Boolean ModalFileFormat (
				Handle								fileInfoHandle);

SInt16 ReadENVIHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadENVIHeaderMapInfo (
				FileInfoPtr							fileInfoPtr,
				char*									stringPtr);

void ReadENVIChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr);

SInt16 ReadFastL7AHeader (
				FileInfoPtr							fileInfoPtr,
				SInt16*								versionPtr,
				UInt32								fileNumber,
				SInt16								formatOnlyCode);

void ReadLARSYSChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr);

SInt16 ReadLARSYSMISTHeader (
				FileInfoPtr							fileInfoPtr,
				SInt32*								headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadLASHeader (
				FileInfoPtr							fileInfoPtr,
				UCharPtr								headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadLGSOWGHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadMacSADIEHeader (
				FileInfoPtr							fileInfoPtr,
				SInt16*								headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadMultiSpecChannelDescriptions (
				FileInfoPtr							fileInfoPtr,
				SInt16								versionNumber);

void ReadMultiSpecChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr);

SInt16 ReadMultiSpecChannelValues (
				FileInfoPtr							fileInfoPtr);

SInt16 ReadMultiSpecClassificationHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);

Boolean ReadMultiSpecClassNames (
				FileInfoPtr							fileInfoPtr,
				Handle								windowInfoHandle,
				UCharPtr								classNamePtr);

SInt16 ReadPDSHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadSunScreenDumpHeader (
				FileInfoPtr							fileInfoPtr,
				SInt32*								headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadTGAHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);

Boolean ReadThematicGroupsFromImageFile (
				FileInfoPtr							inputFileInfoPtr,
				FileInfoPtr							outputFileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr);

SInt16 ReadVICARHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16 ReadWindowsBitMapHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		WindowPtr AddToImageWindowFile
//
//	Software purpose:	The purpose of this routine is to add the selected
// 						image file to the active image window "file".
//
//	Parameters in:		Handle to file information
//
//	Parameters out:	None
//
//	Value Returned:	FALSE means that the image file could not be added to 
//							the image window "file".
// 
// Called By:			LinkNDFFiles in SNDFFormat.cpp
//							AddSelectedFilesToWindow in SOpenFileDialog.cpp
//							LinkFastL7AFiles in SOpenImag.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/21/1991
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

Boolean AddToImageWindowFile (
				Handle								windowInfoHandle,
				Handle								fileInfoHandle)

{
	WindowInfoPtr						windowInfoPtr;

	Boolean continueFlag = FALSE;


			// Check input variables.														

	if (windowInfoHandle != NULL)
		{
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle, kLock);

		if (UpdateLayerInfoStructure (windowInfoPtr, fileInfoHandle))
			{
					// If histogram information currently exists for this			
					// window,  indicate that it is now not up-to-date.
					// Also indicate that an Imagine type of stat file is not
					// expected.		

			Handle histogramSpecsHandle = GetHistogramSpecsHandle (windowInfoPtr);
			HistogramSpecsPtr histogramSpecsPtr =
								(HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);
			if (histogramSpecsPtr != NULL)
				histogramSpecsPtr->initializeStructureFlag = TRUE;

					// Indicate that the display specs will need to be 			
					// reinitialized.	

			Handle displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
			DisplaySpecsPtr displaySpecsPtr =
									(DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
			if (displaySpecsPtr != NULL)
				displaySpecsPtr->initializeStructureFlag = TRUE;

					// Indicate that the supporting statistics file needs to be	
					// defined.								

			SignedByte supportHandleStatus;

			CMFileStream* supportFileStreamPtr = GetSupportFileStreamPointer (
															windowInfoHandle, &supportHandleStatus);

			SetFileDoesNotExist (supportFileStreamPtr, kDoNotKeepUTF8CharName);

			UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);

			continueFlag = TRUE;

			}	// end "if (UpdateLayerInfoStructure (..."

		CheckAndUnlockHandle (windowInfoHandle);

		}	// end "if (fileInfoHandle != NULL && ..."

	return (continueFlag);

}	// end "AddToImageWindowFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AdjustImageWSize
//
//	Software purpose:	The purpose of this routine is to adjust the size
//							of the image window before showing the window on the
//							screen.  The size of the window will be adjusted so
//							that the image will just fit within the window.  The
//							window, however, will not be smaller than the
//							minimum grow area which is set in MInitialize.c.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			SetUpImageWindow in SOpenImage.cpp
//							SetUpThematicImageWindow in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1988
//	Revised By:			Larry L. Biehl			Date: 08/16/2019

void AdjustImageWSize (
				Handle								windowInfoHandle)

{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	#if defined multispec_wx
		CMImageFrame*						imageFramePtr;
	#endif	// defined multispec_wx

	Handle								fileInfoHandle;

	UInt32								columnInterval,
											lineInterval,
											numberColumns,
											numberLines;

	UInt16								amountToAllowForHStuff,
											amountToAllowForVStuff,
											legendWidth,
											windowHeight,
											windowWidth;

	Boolean								latLongPossibleFlag;

	SignedByte							fHandleStatus,
											wHandleStatus;


	fileInfoHandle = GetFileInfoHandle (windowInfoHandle);

	if (fileInfoHandle == NULL)
																									return;

	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																windowInfoHandle, &wHandleStatus);

	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																	fileInfoHandle, &fHandleStatus);

	latLongPossibleFlag = DetermineIfLatLongPossible (windowInfoHandle);
	//latLongPossibleFlag = FALSE;

	#if defined multispec_mac || defined multispec_mac_swift
		BitMap								bitMap;
		Rect									rect;
		//SInt16								cellHeight; 
		WindowPtr							windowPtr;

		windowPtr = windowInfoPtr->windowPtr;

				// Move window to upper left position.											

		rect.top = 20 + GetMBarHeight () + 4;
		rect.left = 4 + 4;

		MoveWindow (windowPtr, rect.left, rect.top, FALSE);

				// Get the area of the image window												

		GetQDGlobalsScreenBits (&bitMap);
		rect = bitMap.bounds;
		rect.left = 4;
		rect.top = 46;
		rect.right -= 30;
		rect.bottom -= 4;

				// Allow for dock in OSX.

		GDHandle currentDevice;
		OSStatus status;


		currentDevice = GetDevicePointIsIn (&gEventRecord.where);

		if (currentDevice == NULL)
			currentDevice = GetGDevice ();

		if (currentDevice != NULL)
			{
			status = GetAvailableWindowPositioningBounds (currentDevice, &rect);

			if (status == noErr)
				{
						// Allow for window title bar.

				rect.top += 20;

						// Allow for a few pixels around the window.

				InsetRect (&rect, 6, 4);

				MoveWindow (windowPtr, rect.left, rect.top, FALSE);

				}	// end "if (status == noErr)"

			}	// end "if (currentDevice != NULL)"

				// Allow for the scroll bars

		rect.bottom -= kSBarWidth;
		rect.right -= kSBarWidth;

				// Amount to allow for scroll bars in window sizing. Macintosh version
				// needs to take the scroll bars into account. Windows version does not.

		amountToAllowForHStuff = kSBarWidth + 1;
		amountToAllowForVStuff = kSBarWidth + 1;

				// Allow for Coordinate Height if needed.

		if (latLongPossibleFlag)
			{
			rect.top += 25;
			SetCoordinateHeight (windowInfoHandle, 25);
			amountToAllowForVStuff += 25;

			}	// end "if (latLongPossibleFlag)"
	#endif	// defined multispec_mac || defined multispec_mac_swift

	#if defined multispec_win
		RECT rect;

		CWinApp* winAppPtr = AfxGetApp ();
		CFrameWnd* pMainFrame = (CFrameWnd*)winAppPtr->m_pMainWnd;
		pMainFrame->GetClientRect (&rect);

				// Allow for the status bar, tool bar, and scroll bars

		rect.bottom -= 4 * (kSBarWidth - 1);
		rect.right -= (kSBarWidth - 1);

		rect.bottom -= 2 * (kSBarWidth - 1);

				// Amount to allow for scroll bars in window sizing. Macintosh version
				// needs to take the scroll bars into account. Windows version does not.
				// Allow space for 1 extra horizontal pixel to allow for possible
				// truncation.

		amountToAllowForHStuff = 1;
		amountToAllowForVStuff = 1;

		if (latLongPossibleFlag)
			{
			rect.bottom -= 33;
			amountToAllowForVStuff += 33;

			}	// end "if (latLongPossibleFlag)"
	#endif	// defined multispec_win

	#if defined multispec_wx
		RECT rect, frameRect;
		wxRect client_rect, frameClient_rect;

		#if defined multispec_wxlin
			wxFrame* pMainFrame = (wxFrame*)GetMainFrame ();
			client_rect = pMainFrame->GetClientRect ();
			//client_rect = pMainFrame->GetRect ();
			rect.top = client_rect.GetTop ();
			rect.left = client_rect.GetLeft ();
			rect.bottom = client_rect.GetBottom ();
			rect.right = client_rect.GetRight ();
   	#endif
		#if defined multispec_wxmac
			//int			menuWidth,
			//				menuHeight;
	
			client_rect = wxDisplay().GetClientArea ();
	
   		//GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
			rect.top = client_rect.GetTop ();
			rect.left = client_rect.GetLeft ();
			rect.bottom = client_rect.GetBottom ();
			rect.right = client_rect.GetRight ();
   	#endif
		
		CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
		CMImageView* imageViewCPtr = (CMImageView*)imageWindowCPtr->mImageViewCPtr;
		imageFramePtr = imageViewCPtr->GetImageFrameCPtr ();
		//client_rect = imageFramePtr->GetClientRect ();
		client_rect = imageFramePtr->GetRect ();		
		frameRect.top = client_rect.GetTop ();		
		frameRect.left = client_rect.GetLeft ();
		frameRect.bottom = client_rect.GetBottom ();
		frameRect.right = client_rect.GetRight ();

				// Get the total of the title and toolbar height
	
		wxRect wxFrameRect = imageFramePtr->GetRect ();
		wxRect wxFrameClientRect = imageFramePtr->GetClientRect ();
		int titleToolBarHeight = wxFrameRect.height - wxFrameClientRect.height;

				// Amount to allow for window border.
		#ifdef NetBeansProject
			rect.right -= 2 * 5;
			rect.bottom -= 2 * 5 + 33;

			amountToAllowForHStuff = 7;
			amountToAllowForVStuff = 33;
		#else	// mygeohub
			#if defined multispec_wxmac
				//wxSize toolBarSize = imageFramePtr->GetToolBar()->GetToolBitmapSize ();
						// Allow for 2 pixels top and bottom and 4 pixels top and bottom for
						// border.
						
				amountToAllowForHStuff =
								2 + 4 + wxFrameRect.width - wxFrameClientRect.width;	// 9
				amountToAllowForVStuff = 2 + 4 + titleToolBarHeight;		// 2 * 5 + 24
	
				rect.right -= amountToAllowForHStuff;
				rect.bottom -= amountToAllowForVStuff;		// 2 * 5 + 24
			#else
				rect.right -= 2 * 5;
				rect.bottom -= 2 * 5 + 21;

				amountToAllowForHStuff = 6;
				amountToAllowForVStuff = 6;	// Allows for border
			#endif
		#endif
		
		if (latLongPossibleFlag)
			{
					// Allow for coordinate view.
			#if defined multispec_wxmac
				rect.bottom -= 40;
				amountToAllowForVStuff += 40;
			#else
				rect.bottom -= 39;
				amountToAllowForVStuff += 39;
			#endif

			}	// end "if (latLongPossibleFlag)"
	#endif   // defined multispec_wx
	
			// Get legend width.																	

	legendWidth = 0;
	if (GetWindowType (windowInfoHandle) == kThematicWindowType)
				// Allow for extra pixel
		legendWidth = GetLegendWidth (windowInfoHandle) + 1;

			// Initialize resolutionElement depending upon whether display			
			// elements are color or B&W patterns and then number of display		
			// levels																				

	numberLines = windowInfoPtr->maxNumberLines;
	numberColumns = windowInfoPtr->maxNumberColumns;

	lineInterval = (numberLines / (rect.bottom - rect.top + 1)) + 1;

	columnInterval = (numberColumns / (rect.right - rect.left - legendWidth + 1)) + 1;

	if (lineInterval > columnInterval)
		columnInterval = lineInterval;

	if (columnInterval > lineInterval)
		lineInterval = columnInterval;
	
	double magnification = 1;
	if (lineInterval > 1)
		{
				// Use the magnification value that will be used later when displaying
				// the image
	
		magnification = (double)1/lineInterval;
		magnification = roundf (magnification * 10000) / 10000;
		
		}	// end "if (lineInterval > 1)"
	/*
	windowHeight = (UInt16)((numberLines - 1)/lineInterval + 1) +
																		amountToAllowForVStuff;
	
	windowWidth = (UInt16)((numberColumns - 1)/columnInterval + 1) +
														legendWidth + amountToAllowForHStuff;
	*/
	windowHeight = (UInt16)(magnification * numberLines +
																		amountToAllowForVStuff);

	windowWidth = (UInt16)(magnification * numberColumns +
														legendWidth + amountToAllowForHStuff);
	
	windowHeight = MAX (windowHeight, (UInt16)gGrowAreaMinimum);

	windowWidth = MAX (windowWidth, gGrowAreaMinimum + legendWidth);
	 
	#if defined multispec_mac
		SizeWindow (windowPtr, windowWidth, windowHeight, TRUE);

		if (windowInfoPtr->windowType == kThematicWindowType)
			{
					// Adjust the size of the legend list for Thematic type image
					// windows.	Will need to set the global active legend width parameter
					// since it has not been set yet. It will be needed for the 
					// AdjustLegendListLength routine.

			if (windowInfoPtr->showLegend)
				gActiveLegendWidth = windowInfoPtr->legendWidth;
			
			AdjustLegendListLength (windowPtr, FALSE);

			}	// end "if (windowInfoPtr->..."

				// Set font for window so that if popup box text is written it is the 
				// correct size.

		TextFont (gWindowTextFont); // monaco  
		TextSize (9);

		ShowWindow (windowPtr);

		SelectWindow (windowPtr);

		if (latLongPossibleFlag)
			{
			SetCoordinateViewUnits (windowInfoHandle, kDecimalLatLongUnitsMenuItem);
			SetControlValue (windowInfoPtr->coordinateUnitsControl,
									kDecimalLatLongUnitsMenuItem);
			SetCoordinateViewLocationParameters (windowInfoHandle);
			SetControlTitle (windowInfoPtr->coordinateViewControl, "\p");

			}	// end "if (latLongPossibleFlag)"

		DrawScrollBars (windowPtr);

		CheckSomeEvents (activMask + updateMask);
	#endif	// defined multispec_mac

	#if defined multispec_win
		CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
		CMImageDoc* imageDocCPtr = (CMImageDoc*)imageWindowCPtr->GetImageDocCPtr ();
		imageDocCPtr->SetDocSize (windowHeight, windowWidth);

				// Show the coordinate view if it is to be shown when the window is first
				// displayed.

		if (latLongPossibleFlag)
			{
			SetCoordinateViewUnits (windowInfoHandle, kDecimalLatLongUnitsMenuItem);
			imageDocCPtr->ShowCoordinateView (TRUE);

			}	// end "if (imageWindowCPtr->mImageViewCPtr != NULL && ..."
	#endif	// defined multispec_win

	#if defined multispec_wx
		int	lWindowWidth, lWindowHeight;
		lWindowWidth = windowWidth;
		lWindowHeight = windowHeight;
		imageFramePtr->SetSize (frameRect.left,
											frameRect.top,
											windowWidth,
											windowHeight);	
		(imageViewCPtr->m_Canvas)->SetVirtualSize (windowWidth, windowHeight);
		
				// Show the coordinate view if it is to be shown when the window is first
				// displayed.

		if (imageViewCPtr != NULL && latLongPossibleFlag)
			{
			SetCoordinateViewUnits (windowInfoHandle, kDecimalLatLongUnitsMenuItem);
			SetCoordinateViewLocationParameters (windowInfoHandle);
			imageFramePtr->ShowCoordinateView (1);

			}	// end "if (imageViewCPtr != NULL && ..."
			
		imageFramePtr->Layout ();
	#endif   // defined multispec_wx

	MHSetState (fileInfoHandle, fHandleStatus);
	MHSetState (windowInfoHandle, wHandleStatus);

}	// end "AdjustImageWSize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		float* CheckChannelValuesHandleSize
//
//	Software purpose:	The purpose of this routine is to get the channel values
//							handle for the input file information structure and very that
//							the memory allocated to it is sufficient for the number of
//							channels in the image.
//
//	Parameters in:		pointer to file information structure.
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/2018
//	Revised By:			Larry L. Biehl			Date: 07/10/2018

float* CheckChannelValuesHandleSize (
				FileInfoPtr							fileInfoPtr,
				Boolean*								continueFlagPtr)

{
	float*								channelValuesPtr;
	UInt32								count;
	
	Boolean								changedFlag;
	
	
	count = 2 * fileInfoPtr->numberChannels * sizeof (float);
	channelValuesPtr = (float*)CheckHandleSize (&fileInfoPtr->channelValuesHandle,
																continueFlagPtr,
																&changedFlag,
																count);
	
	return (channelValuesPtr);

}	// end "CheckChannelValuesHandleSize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckFileInfoParameters
//
//	Software purpose:	The purpose of this routine is to check the file
//							information structure parameters to make sure that
//							they are within limits.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			CheckImageHeader in SOpenImage.cpp
//							ReadMultiSpecClassificationHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1992
//	Revised By:			Larry L. Biehl			Date: 12/11/2012

SInt16 CheckFileInfoParameters (
				FileInfoPtr							fileInfoPtr)

{
	double								sizeDifference;

	Boolean								returnCode;


			// Intialize local variables.														

	returnCode = 1;

	if (fileInfoPtr->numberBytes != 1 &&
            fileInfoPtr->numberBytes != 2 &&
            fileInfoPtr->numberBytes != 4 &&
            fileInfoPtr->numberBytes != 8)
		returnCode = 0;

	if (fileInfoPtr->numberBits < 1 || fileInfoPtr->numberBits > 64)
		returnCode = 0;

	if (fileInfoPtr->numberChannels > kMaxNumberChannels)
		{
		fileInfoPtr->numberChannels = kMaxNumberChannels;
		returnCode = 0;

		}	// end "if (fileInfoPtr->numberChannels > kMaxNumberChannels"

	if (fileInfoPtr->numberColumns < 1 ||
            fileInfoPtr->numberColumns > kMaxNumberColumns)
		returnCode = 0;

	if (fileInfoPtr->numberLines < 1 ||
            fileInfoPtr->numberLines > kMaxNumberLines)
		returnCode = 0;

	if (fileInfoPtr->thematicType)
		{
		if (fileInfoPtr->numberClasses <= 0)
			{
			fileInfoPtr->numberClasses = fileInfoPtr->numberBins;
			returnCode = 0;

			}	// end "if (fileInfoPtr->numberClasses == 0)"

		if (fileInfoPtr->numberClasses > kMaxNumberClasses)
			{
			fileInfoPtr->numberClasses = kMaxNumberClasses;
			returnCode = 0;

			}	// end "if (fileInfoPtr->numberClasses > kMaxNumberClasses)"

		if (fileInfoPtr->numberClasses > 0)
			fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;

		}	// end "if (fileInfoPtr->thematicType)"

			// Force start line and column to be larger than 0.

	if (fileInfoPtr->startColumn < 1)
		fileInfoPtr->startColumn = 1;

	if (fileInfoPtr->startLine < 1)
		fileInfoPtr->startLine = 1;

			// Indicate an error if the start line and column is larger
			// than the max number.

	if (fileInfoPtr->startColumn > kMaxStartColumn)
		returnCode = 0;

	if (fileInfoPtr->startLine > kMaxStartLine)
		returnCode = 0;

			// If okay to here, make certain the the parameters make sense		
			// relative to the size of the file.										

	if (returnCode)
		{
		sizeDifference = CompareFileInfoAndFileSize (
																fileInfoPtr,
																fileInfoPtr->numberLines,
																fileInfoPtr->numberColumns,
																fileInfoPtr->numberChannels,
																fileInfoPtr->numberBytes,
																fileInfoPtr->numberPreLineBytes,
																fileInfoPtr->numberPostLineBytes,
																fileInfoPtr->numberPreChannelBytes,
																fileInfoPtr->numberPostChannelBytes,
																fileInfoPtr->numberHeaderBytes,
																fileInfoPtr->numberTrailerBytes,
																fileInfoPtr->bandInterleave,
																(fileInfoPtr->numberBits == 4));

		if (sizeDifference > 0)
			returnCode = 0;
		  
				// Need to find way to determine compression for Grib files.
				// Also need to find a way to know if the Imagine ige file is
				// present.
				// Also it is very possible for the ascii file sizes to be less than
				// what is computed based on parameters because of the way that gdal
				// treats the data value ... always real even though data may be 
				// ascii integers.

		if (!SizeOfImageFileCanBeCalculated (fileInfoPtr))
			returnCode = 1;

		}	// end "if (returnFlag)"

	return (returnCode);

}	// end "CheckFileInfoParameters"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckForENVIHeader
//
//	Software purpose:	This routine checks for the existence of an ENVI Header file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	kENVIType:	indicates ENVI header does exist
//							0:				indicates that ENVI header does not exist
//
// Called By:			OpenImageFile in SOpenFileDialog.cpp
//							GetProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/12/1999
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

SInt16 CheckForENVIHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr)
 
{
	CMFileStream						headerFileStream;

	FileStringPtr						headerFileNamePtr;

	CMFileStream						*fileStreamPtr,
											*headerFileStreamPtr;

	UInt32								count;

	SInt16								errCode,
											fileType = 0,
											stringCompare;


	if (fileInfoPtr != NULL && headerRecordPtr != NULL)
		{
				// Assume that an image file was selected and check if an ENVI header
				// file exists.

		errCode = noErr;

		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

		headerFileStreamPtr = &headerFileStream;
		InitializeFileStream (headerFileStreamPtr, fileStreamPtr);

		headerFileNamePtr =
				(FileStringPtr)GetFilePathPPointerFromFileStream (headerFileStreamPtr);

				// First add ".hdr" to the file name to see if that file exists.

		ConcatFilenameSuffix (headerFileNamePtr, (StringPtr)"\0.hdr\0");
		errCode = OpenFileReadOnly (headerFileStreamPtr,
											 kResolveAliasChains,
											 kLockFile,
											 kVerifyFileStream);

		if (errCode != noErr)
			{
						// Remove .hdr from the file name and remove any existing
						// 3 character suffix from the file name and then add ".hdr".	

			RemoveCharsNoCase ((char*)"\0.hdr\0", headerFileNamePtr);
			RemoveCharsNoCase ((char*)"\0.???\0", headerFileNamePtr);
			ConcatFilenameSuffix (headerFileNamePtr, (StringPtr) "\0.hdr\0");

			errCode = OpenFileReadOnly (headerFileStreamPtr,
												  kResolveAliasChains,
												  kLockFile,
												  kVerifyFileStream);

			}	// end "if (errCode != noErr)"

				// Make certain that we are at the beginning of the file.

		if (errCode == noErr)
			errCode = MSetMarker (headerFileStreamPtr, fsFromStart, 0, kNoErrorMessages);

				// Read no more than the first 4995 bytes of data. headerRecordPtr
				// represents 5000 bytes of memory

		count = 4995;
		if (errCode == noErr)
			{
			errCode = MReadData (headerFileStreamPtr,
										  &count,
										  headerRecordPtr,
										  kNoErrorMessages);

			if (errCode == eofErr && count > 10)
				errCode = noErr;

			}	// end "if (errCode == noErr)"

		CloseFile (headerFileStreamPtr);

		if (errCode == noErr)
			{
					// Make sure that there is a c string terminator in the record.

			headerRecordPtr[count] = 0;

					// Now check if this is an ENVI header file.

			stringCompare = strncmp (headerRecordPtr, "ENVI", 4);
			if (stringCompare == 0)
				fileType = kENVIType;

			else	// stringCompare != 0
				{
				stringCompare = strncmp (headerRecordPtr, "description", 11);
				if (stringCompare == 0)
					fileType = kENVIType;

				}	// end "else stringCompare != 0"

			}	// end "if (errCode == noErr)"

		}	// end "if (fileInfoPtr != NULL && headerRecordPtr != NULL)"

	return (fileType);

}	// end "CheckForENVIHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckForPDS_LBL_File
//
//	Software purpose:	This routine checks for the existence of a PDS LBL file
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	kPDSType:	indicates PDS LBL file does exist
//							0:				indicates PDS LBL file does not exist
//
// Called By:			
//							
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2016
//	Revised By:			Larry L. Biehl			Date: 02/23/2017

SInt16 CheckForPDS_LBL_File (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr)
 
{
	CMFileStream						headerFileStream;

	FileStringPtr						headerFileNamePtr;

	CMFileStream						*fileStreamPtr,
											*headerFileStreamPtr;

	UInt32								count;

	SInt16								errCode,
											fileType = 0;


	if (fileInfoPtr != NULL && headerRecordPtr != NULL) 
		{
				// Assume that an image file was selected and check if an LBL file
				// file exists.

		errCode = noErr;

		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

		headerFileStreamPtr = &headerFileStream;
		InitializeFileStream (headerFileStreamPtr, fileStreamPtr);

		headerFileNamePtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (headerFileStreamPtr);

				// First add ".LBL" to the file name to see if that file exists.

		ConcatFilenameSuffix (headerFileNamePtr, (StringPtr) "\0.LBL\0");
		errCode = OpenFileReadOnly (headerFileStreamPtr,
											 kResolveAliasChains,
											 kLockFile,
											 kVerifyFileStream);

		if (errCode != noErr) 
			{
					// Remove .LBL from the file name and remove any existing
					// 3 character suffix from the file name and then add ".LBL".	

			RemoveCharsNoCase ((char*)"\0.LBL\0", headerFileNamePtr);
			RemoveCharsNoCase ((char*)"\0.???\0", headerFileNamePtr);
			ConcatFilenameSuffix (headerFileNamePtr, (StringPtr)"\0.LBL\0");

			errCode = OpenFileReadOnly (headerFileStreamPtr,
												  kResolveAliasChains,
												  kLockFile,
												  kVerifyFileStream);

			}	// end "if (errCode != noErr)"

				// Make certain that we are at the beginning of the file.

		if (errCode == noErr)
			errCode = MSetMarker (headerFileStreamPtr, fsFromStart, 0, kNoErrorMessages);

				// Read no more than the first 4995 bytes of data. headerRecordPtr
				// represents 5000 bytes of memory

		count = 4995;
		if (errCode == noErr) 
			{
			errCode = MReadData (headerFileStreamPtr,
										  &count,
										  headerRecordPtr,
										  kNoErrorMessages);

			if (errCode == eofErr && count > 10)
				errCode = noErr;

			}	// end "if (errCode == noErr)"

		CloseFile (headerFileStreamPtr);

		if (errCode == noErr) 
			{
					
					// Make sure that there is a c string terminator in the record.

			headerRecordPtr[count] = 0;

					// The LBL file does exist. 

			fileType = kPDSType;

			}	// end "if (errCode == noErr)"

		}	// end "if (fileInfoPtr != NULL && headerRecordPtr != NULL)"

	return (fileType);

}	// end "CheckForPDS_LBL_File"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 CheckIfProjectFile
//
//	Software purpose:	This routine determines if the input file is a project
//							file.  The routines returns a code which indicates 
//							whether it is, 1; it is not, -1; or it is but the version
//							is wrong, -2;
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			OpenImageFile in SOpenFileDialog.cpp
//							GetProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/04/1991
//	Revised By:			Larry L. Biehl			Date: 03/16/2006

SInt32 CheckIfProjectFile (
				CMFileStream*						fileStreamPtr,
				SInt16								formatOnlyCode)

{
	SInt32								returnCode;

	UInt32								count,
											i,
											lastVersionIndex;

	SInt16								errCode,
											stringCompare;

	Boolean								continueFlag;


			// Initialize local variables.

	returnCode = 0;
	stringCompare = -1;

			// Read assumed project header information.

	count = 31;
	errCode = MReadData (fileStreamPtr, &count, gTextString, kNoErrorMessages);

	if (errCode != noErr)
		{
		if (formatOnlyCode != kLoadHeader)
			return (-1);

		IOCheck (errCode, fileStreamPtr);

		}	// end "if (errCode != noErr)"

	continueFlag = (errCode == noErr);

			// If everything read okay to here, then compare the format
			// string and the version string.

	if (continueFlag)
		{
				// Move the characters which describe the version date.

		BlockMoveData ((UCharPtr)&gTextString[23], (UCharPtr)&gTextString2[0], 8);
		if (gTextString2[6] == ' ')
			BlockMoveData ((UCharPtr)&gTextString[23], (UCharPtr)&gTextString2[0], 6);

				// Check if file is a MultiSpec project.  If the file is
				// not, set up return code indicating such.

		if (MGetString (gTextString3, kProjectStrID, IDS_Project8))
			stringCompare = strncmp (
				 (char*)gTextString, (char*)&gTextString3[1], gTextString3[0]);
		  
		if (stringCompare != 0)
			returnCode = -1;

		else	// stringCompare == 0
			{
					// File is MultiSpec Project file. Check the format
					// version.	 If version is not correct, set up return
					// code indicating such.

			stringCompare = -1;
			if (MGetString (gTextString3, kProjectVersionsStrID, IDS_ProjectVersion1))
				{
				StringToNum ((UCharPtr)gTextString3, (SInt32*)&lastVersionIndex);
				lastVersionIndex += IDS_ProjectVersion1;

				for (i = IDS_ProjectVersion2; i <= lastVersionIndex; i++)
					{
					if (MGetString (gTextString3, kProjectVersionsStrID, (UInt16)i))
						stringCompare = strncmp ((char*)gTextString2,
															(char*)&gTextString3[1],
															gTextString3[0]);

					if (stringCompare == 0)
						{
						StringToNum ((UCharPtr)gTextString3, &returnCode);
						break;

						}	// end "if (stringCompare == 0)"

					}	// end "for (i=2; i<=lastVersionIndex; i++)"

				}	// if (MGetString (...)

			if (stringCompare != 0)
				returnCode = -2;

			}	// end "else stringCompare == 0"

		}	// end "if (continueFlag)"

	return (returnCode);

}	// end "CheckIfProjectFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckImageHeader
//
//	Software purpose:	This routine checks the header of the image file to
//							try to determine the format of the file.  False is 
//							returned if the specifications can not be determined.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadImageInformation	in SOpenImage.cpp	
//
//	Coded By:			Larry L. Biehl			Date: 08/23/1988
//	Revised By:			Larry L. Biehl			Date: 03/30/2020

SInt16 CheckImageHeader (
				Handle								windowInfoHandle,
				Handle								fileInfoHandle,
				UInt32*								formatVersionCodePtr,
				SInt16								formatOnlyCode,
				SInt16								linkOffsetIndex)

{
	DoubleRect							boundingShapeRectangle;

	CMFileStream*						fileStreamPtr;

	FileInfoPtr							fileInfoPtr;

	char									headerRecordPtr[5000];

	SInt32								posOff;

	time_t								startTime;

	UInt32								count;

	SInt16								errCode,
											fileInfoLoaded,
											returnCode = 0,
											savedGDALReturnCode,
											savedProcessorCode,
											shapeFileIndex = 0,
											stringNumber = 0,
											version;

	Boolean								continueFlag = TRUE,
											convertFromLatLongToMapUnitsFlag;

	SignedByte							handleStatus;


			// returnCode values
			//		< 0: file format type found but some time of file error
			//		= 0: file format type found
			//		= 1: file format type not found
			//		= 2: file format type found (overlay file) but file is already loaded;
			//				just need to draw it into the active image window.
			//		= 3: file format type found (overlay file) but file is already loaded.


			// Get the handle status and then lock the memory, in case it
			// isn't already locked, until the information has been loaded
			// in. Then get the pointer to the file information block

	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																	fileInfoHandle, &handleStatus);

	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

			// Initialize variables.

	fileInfoLoaded = 0;
	fileInfoPtr->format = 0;
	fileInfoPtr->ancillaryInfoformat = 0;
	fileInfoPtr->classesComputedFlag = FALSE;

	if (formatVersionCodePtr != NULL)
		*formatVersionCodePtr = 0;

			// Make certain that we are at the beginning of the file.

	posOff = 0;
	errCode = MSetMarker (fileStreamPtr, fsFromStart, posOff, kErrorMessages);

			// Read first 4995 bytes of data.

	count = 4995;
	if (errCode == noErr)
		errCode = MReadData (fileStreamPtr, &count, headerRecordPtr, kNoErrorMessages);

	if (errCode != eofErr || count == 0)
		{
		if (count == 0 && formatOnlyCode != kLoadHeader)
			{
			MHSetState (fileInfoHandle, handleStatus);

			return (0);

			}	// end "if (count == 0 && formatOnlyCode != kLoadHeader)"

        IOCheck (errCode, fileStreamPtr);

		}	// end "if (errCode != eofErr || count == 0)"

	else	// errCode == eofErr && count > 0
		errCode = noErr;

	if (errCode == noErr)
		{
				// Make sure that there is a c string terminator in the record.

		headerRecordPtr[count] = 0;

				// Determine the image file format.
				//  Check if image file is in ERDAS format.

		if (fileInfoPtr->format == 0 && count >= 128)
			returnCode = ReadERDASHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);

				// Check if image file is in SUN screendump format.

		if (fileInfoPtr->format == 0 && count >= 32)
			returnCode = ReadSunScreenDumpHeader (
								fileInfoPtr, (SInt32*)headerRecordPtr, formatOnlyCode);

				// Check if image file is a MultiSpec Classification file.
				// Get the MacLARSYS identifier string.
				// This string is an early identifier for MultiSpec
				// classifications.

		if (fileInfoPtr->format == 0 && count >= 24)
			returnCode = ReadMultiSpecClassificationHeader (
									fileInfoPtr, headerRecordPtr, formatOnlyCode);

				// Check if image file is a Vicar formatted image file.
				// Get the VICAR identifier string.

		if (fileInfoPtr->format == 0 && count >= 8)
			returnCode = ReadVICARHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);

		if (fileInfoPtr->format == 0 && count >= 128)
			returnCode = ReadGAIAHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);

		if (fileInfoPtr->format == 0 && count >= 128)
			returnCode = ReadMacSADIEHeader (
                fileInfoPtr, (SInt16*)headerRecordPtr, formatOnlyCode);

		if (fileInfoPtr->format == 0 && count >= 128)
			returnCode = ReadTGAHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);

				//  Check if image file is a TIFF file.

		#if use_multispec_tiffcode
			if (fileInfoPtr->format == 0 && count >= 8)
				returnCode = ReadTIFFHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);
		#endif	// use_multispec_tiffcode

				//  Check if image file is in HDF format.
		
		#if include_hdf_capability
			if (fileInfoPtr->format == 0 && count >= 32)
				returnCode = ReadHDFHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);
		#endif		// include_hdf_capability
		
				// Check if file is an ERDAS Imagine HFA file

		#if !include_gdal_capability
			if (fileInfoPtr->format == 0 && count >= 128)
				returnCode = ReadImagineHeader (
													fileInfoPtr, headerRecordPtr, formatOnlyCode);
		#endif	// !include_gdal_capability

				//  Check if image contains a PDS formatted header						

		if (fileInfoPtr->format == 0 && count >= 128)
			returnCode = ReadPDSHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);

		if (fileInfoPtr->format == 0 && count >= 128)
			returnCode = ReadLARSYSMISTHeader (
										fileInfoPtr, (SInt32*)headerRecordPtr, formatOnlyCode);

		if (fileInfoPtr->format == 0 && count >= 300)
			returnCode = ReadLGSOWGHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);

				// Check if this is an ENVI header file.
		
		#if !include_gdal_capability
			if (fileInfoPtr->format == 0 && count >= 128)
				returnCode = ReadENVIHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);
		#endif
		
				// Check if this is a Landsat 7 fast format file.

		if (fileInfoPtr->format == 0 && count >= 128)
			{
			returnCode = ReadFastL7AHeader (fileInfoPtr, &version, 1, formatOnlyCode);

			if (fileInfoPtr->format == kFastL7AType &&
                    returnCode == noErr &&
                    formatVersionCodePtr != NULL)
				*formatVersionCodePtr = version;

			}	// end "if (fileInfoPtr->format == 0 && count >= 128)"
		/*			
		if (fileInfoPtr->format == 0 && count >= 128)
			{
					// Check if this is an ENVI header file.
						
			returnCode = ReadENVIHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);
			
					// Now check if a .hdr file exists in the same folder that contains
					// ENVI header information.

			//fileType = CheckForENVIHeader (fileInfoPtr, headerRecordPtr);
						
			}	// end "if (fileInfoPtr->format == 0 && count >= 128)"
		*/
				// Check if image file is a LAS file.	

		if (fileInfoPtr->format == 0 && count >= 128)
			{
			returnCode = ReadNDFHeader (fileInfoPtr,
												  headerRecordPtr,
												  &version,
												  1,
												  formatOnlyCode);

			if (fileInfoPtr->format == kNdfType &&
							returnCode == noErr &&
									formatVersionCodePtr != NULL)
				*formatVersionCodePtr = version;

			}	// end "if (fileInfoPtr->format == 0 && count >= 128)"

				// Check if this is an ArcView image file.	
				// Use gdal to read these files if available.

		#if !include_gdal_capability
			if (fileInfoPtr->format == 0 && count >= 128)
            returnCode = ReadArcViewHeader (
            									fileInfoPtr, headerRecordPtr, formatOnlyCode);
		#endif	// !include_gdal_capability

				//  Check if this is an ArcView shape file.
		
		if (fileInfoPtr->format == 0 && count >= 100)
			/*
			#if include_gdal_capability && !defined multispec_wxlin
				returnCode = ReadVectorWithGDALLibrary (windowInfoHandle,
																		fileInfoPtr,
																		headerRecordPtr,
																		formatOnlyCode);
			#endif
			*/
			returnCode = ReadArcViewShapeHeader (fileInfoPtr,
																 (UCharPtr)headerRecordPtr,
																 &shapeFileIndex,
																 &boundingShapeRectangle,
																 formatOnlyCode);
			 
		if (fileInfoPtr->format == 0 && count >= 54)
			returnCode = ReadWindowsBitMapHeader (fileInfoPtr,
																 headerRecordPtr,
																 formatOnlyCode);

		#if include_gdal_capability
			if (fileInfoPtr->format == 0 && count >= 20)
				{						
            returnCode = ReadHeaderWithGDALLibrary (fileInfoPtr,
																	  headerRecordPtr,
																	  formatOnlyCode);
																	  
            if (formatOnlyCode == kLoadHeader &&
                    (fileInfoPtr->format == kTIFFType ||
													fileInfoPtr->format == kGeoTIFFType) &&
                    					fileInfoPtr->dataCompressionCode == kNoCompression)
					{
					SInt64 fileSize;
					GetSizeOfFile (fileStreamPtr, &fileSize);
					/*
					sprintf ((char*)gTextString, " gdal returnCode = %5d%s", 
											returnCode,
											gEndOfLine);
					OutputString (NULL, 
										(char*)gTextString, 
										0, 
										gOutputForce1Code,
										true);
					*/
					if (fileInfoPtr->blockHeight * fileInfoPtr->blockWidth * fileInfoPtr->numberBytes > 16000000 || // 16000000
								fileSize > UInt32_MAX)
								//fileInfoPtr->numberLines == fileInfoPtr->blockHeight &&
								//fileInfoPtr->numberColumns == fileInfoPtr->blockWidth &&
								//fileInfoPtr->numberLines > 1)
						{
								// This will set things up so that MultiSpec will handle the
								// reading of the file. GDAL is not efficient with these type
								// of files. For most cases the entire image is read for each
								// line read. And memory is allocated twice for the entire
								// image for one channel.
								//
								// Also MultiSpec will handle tiff files larger than
								// UInt32_MAX bytes as long as none of the tags make reference
								// to an offset beyond UInt32_MAX. gdal will not ... at least
								// it will not for band sequential files that have been tried.
								//
								// We need to indicate that gdal is not being used for
								// processing here. If there is an error in using
								// ReadTIFFHeader then gdal will be used.

						GDALDatasetH gdalDataSetH = fileInfoPtr->gdalDataSetH;
						fileInfoPtr->gdalDataSetH = 0;
						savedGDALReturnCode = returnCode;

						returnCode = ReadTIFFHeader (
													fileInfoPtr, headerRecordPtr, formatOnlyCode);

						if (returnCode == noErr)
							{
							if (gdalDataSetH != 0)
								{
								GDALClose (gdalDataSetH);
								gNumberOfOpenFiles--;

                        }	// end "if (gdalDataSetH != 0)"

									// These variables are only used if gdal is being used to
									// read the image files. Set to zero so that they will not
									// be used when listing image description information.

							fileInfoPtr->blockHeight = 0;
							fileInfoPtr->blockWidth = 0;

							}	// end "if (returnCode == noErr)"

						else	// returnCode != noErr
							{
									// Error occurred trying to use MultiSpec TIFF read
									// function. Indicate that gdal will be used.
							
							fileInfoPtr->gdalDataSetH = gdalDataSetH;
							returnCode = savedGDALReturnCode;
							
							}	// end "else returnCode != noErr"

						}	// end "if (fileInfoPtr->blockHeight * fileInfoPtr->...

					}	// end "if (formatOnlyCode == kLoadHeader && ..."

            /*				
				if (fileInfoPtr->format == kArcViewType && formatOnlyCode == kLoadHeader)
					{
							// This is done to pick up some parameters that gdal may miss.

					savedSignedDataFlag = fileInfoPtr->signedDataFlag;
					ReadArcViewHeader (fileInfoPtr, headerRecordPtr, formatOnlyCode);
					fileInfoPtr->ancillaryInfoformat = kArcViewType;

							// Update gdalDataTypeCode if needed.

					if (savedSignedDataFlag == FALSE &&
																fileInfoPtr->signedDataFlag == TRUE)
						{
						if (fileInfoPtr->gdalDataTypeCode == GDT_UInt16)
							fileInfoPtr->gdalDataTypeCode = GDT_Int16;

						else if (fileInfoPtr->gdalDataTypeCode == GDT_UInt32)
							fileInfoPtr->gdalDataTypeCode = GDT_Int32;

						}	// end "if (savedSignedDataFlag == FALSE && ..."

					}	// end "if (fileInfoPtr->format == kArcViewType && ..."
				*/
				}	// end "if (fileInfoPtr->format == 0 && count >= 20)"
		#endif	// include_gdal_capability

		if (fileInfoPtr->format == 0 && count >= 128)
			returnCode = ReadLASHeader (
										fileInfoPtr, (UCharPtr)headerRecordPtr, formatOnlyCode);

        // At this point if gdal is available, gdal was not able to read the file.
        // Check if image file is a TIFF file that MultiSpec code can read.

		#if use_multispec_tiffcode
			if (fileInfoPtr->format == 0 && count >= 8)
            returnCode = ReadTIFFHeader (
            								fileInfoPtr, headerRecordPtr, formatOnlyCode);
		#endif	// use_multispec_tiffcode
		
		#if include_gdal_capability
					// At this point gdal is available. If gdal was not able to read the 
					//	file see MultiSpec can read.
					
					// If it is an ArcView type file that MultiSpec can read.
					
			if (fileInfoPtr->format == 0 && count >= 128)
            returnCode = ReadArcViewHeader (
													fileInfoPtr, headerRecordPtr, formatOnlyCode);
													
					// If it is an ENVI Header file that MultiSpec can read the 
					// associated image file
					
			if (fileInfoPtr->format == 0 && count >= 128)
				{
            returnCode = ReadENVIHeader (
													fileInfoPtr, headerRecordPtr, formatOnlyCode);
													
				if (fileInfoPtr->format == kENVIType)
					{
							// Try reading with GDAL Library again. The main image file
							// was found and set in the file info structure
							
					fileInfoPtr->format = 0;
					returnCode = ReadHeaderWithGDALLibrary (fileInfoPtr,
																			headerRecordPtr,
																			formatOnlyCode);
																			
					}	// end "if (fileInfoPtr->format == kENVIType)"
													
				}	// end "if (fileInfoPtr->format == 0 && count >= 128)"
		#endif	// !include_gdal_capability

		if (formatOnlyCode != kLoadHeader)
			{
			version = fileInfoPtr->format;

			if (formatOnlyCode == kAllReadableImageFiles &&
                    version == kArcViewShapeType)
				version = 0;

			else if (formatOnlyCode == kMultiFiles &&
                    (fileInfoPtr->thematicType || version == kArcViewShapeType))
				version = 0;

			else if (formatOnlyCode == kThematicFiles &&
                    (!fileInfoPtr->thematicType || version == kArcViewShapeType))
				version = 0;

			else if (formatOnlyCode == kShapeFiles && version != kArcViewShapeType)
				version = 0;

            MHSetState (fileInfoHandle, handleStatus);

			return (version);

			}	// end "if (formatOnlyCode != kLoadHeader)"

				// Make certain that the thematic flag is set.

		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			{
			fileInfoPtr->thematicType = TRUE;

            // If this is 8-bit data and the number of classes has not
            // be set then set the number of classes to the maximum 
            // possible of 256

			if (fileInfoPtr->numberBytes == 1 && fileInfoPtr->numberClasses <= 0)
				fileInfoPtr->numberClasses = 256;

			if (fileInfoPtr->numberChannels > 1)
				DisplayAlert (kErrorAlertID,
									kNoteAlert,
									kAlertStrID,
									IDS_Alert110,
									0,
									NULL);

			}	// end "if (gGetFileImageType == kThematicImageType)"
		/*
		#if defined multispec_win
			gImageFileFilterIndex = 2;
			if (fileInfoPtr->thematicType)
				gImageFileFilterIndex = 3;
			if (fileInfoPtr->format == kArcViewShapeType)
				gImageFileFilterIndex = 4;
		#endif	// defined multispec_win
		*/
		if (fileInfoPtr->format != kArcViewShapeType)
			{
			if (returnCode == 0 && fileInfoPtr->format != 0)
				{
						// Check if parameters make sense
				
				fileInfoLoaded = CheckFileInfoParameters (fileInfoPtr);
				
						// Continue section only if parameters make sense.

				if (fileInfoLoaded == 1)
					IntermediateFileUpdate (fileInfoPtr);

				FinishMapInformationSetUp (fileInfoPtr, headerRecordPtr);

            }	// end "if (returnCode == 0 && ..."

			}	// end "if (fileInfoPtr->format != kArcViewShapeType)"

		else	// fileInfoPtr->format == kArcViewShapeType
			{
			if (returnCode != 7)
				{
						// Selection is an ArcView Shape file.
						// Note that shape files in a multiple selection list will be
						// ignored; this is indicated by a returnCode of 7
						// Force text selection to start from end of present 		
						// text.																	

				ForceTextToEnd ();

				gOutputForce1Code = 0x0001;
				gOutputTextOKFlag = TRUE;

				savedProcessorCode = gProcessorCode;
				gProcessorCode = kDisplayOverlayProcessor;

				continueFlag = ListHeaderInfo (NULL,
															0,
															&gOutputForce1Code,
															kNoStatisticsUsed,
															continueFlag);

				convertFromLatLongToMapUnitsFlag = FALSE;
				if (returnCode == 10)
					{
					convertFromLatLongToMapUnitsFlag = TRUE;
					returnCode = 0;

					}	// end "if (returnCode == 10)"

                // Get string for alert if needed.

				if (returnCode == 5)
					stringNumber = IDS_Alert80;

							// Get start time for the processor.

				startTime = time (NULL);

				if (returnCode == 0)
					{
                    // Load the vector information that intesects with the image
                    // windows.											

					returnCode = ReadArcViewShapeFile (shapeFileIndex,
																	 convertFromLatLongToMapUnitsFlag,
																	 kCloseOverlayFileIfNoRecordsRead);

                    // Get string for alert if needed.

					if (returnCode == 5)
						stringNumber = IDS_Alert88;

					else if (returnCode == 6)
						stringNumber = IDS_Alert88;

					}	// end "if (returnCode == 0)"

				if (returnCode == 0 || returnCode == 2)
					{
                    // File is already loaded, just update the image.

					fileInfoLoaded = 1;

					#if defined multispec_mac
						DrawArcViewShapes (gActiveImageWindow,
													 gActiveImageWindowInfoH,
													 NULL,
													 kToImageWindow,
													 NULL);
					#endif	// defined multispec_mac

					#if defined multispec_win
						InvalidateWindow (gActiveImageWindow, kImageFrameArea, FALSE);
					#endif	// defined multispec_win

					#if defined multispec_wx
						InvalidateWindow (gActiveImageWindow, kImageFrameArea, FALSE);
					#endif	// defined multispec_win

					UpdateOverlayControl (gActiveImageWindow);

					}	// end "else if (returnCode == 2)"

				else	// returnCode == 1 || 3 || 4 || 5 || 6
                    // File is already loaded and drawn or no image available to
                    // draw it on.
					fileInfoLoaded = 1;

				if (returnCode == 5)
					{
                    // List message indicating that the image window and shape
                    // bounding rectangle do not intersect. List the rectangles.

					ListNonIntersectionMessage (gActiveImageWindowInfoH,
														 &boundingShapeRectangle,
														 stringNumber);

					}	// end "if (returnCode == 5)"

				else if (convertFromLatLongToMapUnitsFlag)
					{
                    // List message indicating that the shape file info was converted
                    // from lat-long units to the respective map projection units
                    // for the image.

					ListSpecifiedStringNumber (kAlertStrID,
														 IDS_Alert129,
														 (unsigned char*)gTextString,
														 NULL,
														 gOutputForce1Code,
														 TRUE);

					}	// end "else if (convertFromLatLongToMapUnitsFlag)"

						// Get the end time and print the time elapsed in the
						// output window.

				continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);

						// Scroll output window to the selection and adjust the
						// scroll bar.

				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);

				gProcessorCode = savedProcessorCode;

            }	// end "if (returnCode != 7)"

			else	// returnCode == 7
				fileInfoLoaded = returnCode;

			}	// end "else fileInfoPtr->format == kArcViewShapeType"

		if (fileInfoLoaded == 1)
			fileInfoLoaded = DetermineIfFileSpecsDialogNeedsCalled (windowInfoHandle,
																						 fileInfoPtr,
																						 linkOffsetIndex);

		}	// end "if (errCode == noErr)"

	MHSetState (fileInfoHandle, handleStatus);

	return (fileInfoLoaded);

}	// end "CheckImageHeader" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double CompareFileInfoAndFileSize
//
//	Software purpose:	The purpose of this routine is to compare the size of
//							the image file as specified by the file parameters
//							and the actual size of the file.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			FileSpecificationDialog in SOpenDialog.cpp
//							CheckFileInfoParameters in SOpenImage.cpp
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1992
//	Revised By:			Larry L. Biehl			Date: 12/19/2003	

double CompareFileInfoAndFileSize (
				FileInfoPtr							fileInfoPtr,
				UInt32								numberLines,
				UInt32								numberColumns,
				UInt32								numberChannels,
				SInt16								numberBytes,
				UInt32								preLineCalBytes,
				UInt32								postLineCalBytes,
				UInt32								preChannelBytes,
				UInt32								postChannelBytes,
				UInt32								headerBytes,
				UInt32								trailerBytes,
				SInt16								bandInterLeaveCode,
				Boolean								fourBitFlag)

{
	double								totalNumberBytes;
	SInt64								fileSize;
	SInt16								errCode;


	totalNumberBytes = numberLines * numberColumns;

	totalNumberBytes *= numberBytes;

	totalNumberBytes *= numberChannels;

			// Be sure to handle the 4-bit case correctly.

	if (numberBytes == 1 && fourBitFlag) // 4-bit case
		totalNumberBytes = floor ((totalNumberBytes + 1) / 2);

	switch (bandInterLeaveCode)
		{
		case kBIS:
			totalNumberBytes += numberLines * (preLineCalBytes + postLineCalBytes);
			break;

		case kBSQ:
			totalNumberBytes +=
						numberChannels * numberLines * (preLineCalBytes + postLineCalBytes);
			totalNumberBytes += numberChannels * (preChannelBytes + postChannelBytes);
			break;

		case kBIL:
			totalNumberBytes +=
						numberChannels * numberLines * (preChannelBytes + postChannelBytes);
			totalNumberBytes += numberLines * (preLineCalBytes + postLineCalBytes);
			break;

		}	// end "switch (bandInterLeaveCode)"

	totalNumberBytes += headerBytes + trailerBytes;

			// Allow for Old Style LAS format file with no pad bytes on last line.

	if (fileInfoPtr->format == kLASType2 && headerBytes == 512)
		totalNumberBytes -= postLineCalBytes;

	errCode = GetSizeOfFile (fileInfoPtr, &fileSize);

	if (errCode != noErr)
		fileSize = 0;

	totalNumberBytes -= fileSize;

	return (totalNumberBytes);

}	// end "CompareFileInfoAndFileSize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertERDASProjectionCodesToMultiSpecCodes
//
//	Software purpose:	This routine converts the codes used by ERDAS lan and imagine
//							formats to the codes used by MultiSpec
//							These codes come from document iau_docu1.pdf. Title of document
//							is Machine Independent Format.
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/17/2012
//	Revised By:			Larry L. Biehl			Date: 03/23/2012

SInt16 ConvertMultiSpecProjectionCodeToERDASCode (
				SInt16								referenceSystemCode,
				SInt16								projectionCode)

{
	SInt16 erdasCode = 0;


			// Translate ERDAS code number to code number used by MultiSpec

	if (referenceSystemCode == kGeographicRSCode)
		erdasCode = 0;

	else if (referenceSystemCode >= kUTM_NAD27RSCode &&
																referenceSystemCode <= kUTMRSCode)
		erdasCode = 1;

	else if (referenceSystemCode == kStatePlaneNAD27RSCode ||
													referenceSystemCode == kStatePlaneNAD83RSCode)
		erdasCode = 2;

	else	// referenceSystemCode != kGeographicReferenceSystemCode ...
		{
		switch (projectionCode)
			{
			case kAlbersConicalEqualAreaCode:
				 erdasCode = 3;
				 break;

			case kLambertConformalConicCode:
				 erdasCode = 4;
				 break;

			case kMercatorCode:
				 erdasCode = 5;
				 break;

			case kPolarStereographicCode:
				 erdasCode = 6;
				 break;

			case kPolyconicCode:
				 erdasCode = 7;
				 break;

			case kEquidistantConicCode:
				 erdasCode = 8;
				 break;

			case kTransverseMercatorCode:
				 erdasCode = 9;
				 break;

			case kStereographicCode:
				 erdasCode = 10;
				 break;

			case kLambertAzimuthalEqualAreaCode:
				 erdasCode = 11;
				 break;

			case kAzimuthalEquidistantCode:
				 erdasCode = 12;
				 break;

			case kGnomonicCode:
				 erdasCode = 13;
				 break;

			case kOrthographicCode:
				 erdasCode = 14;
				 break;

			case kGeneralVerticalNearSideCode:
				 erdasCode = 15;
				 break;

			case kSinusoidalCode:
				 erdasCode = 16;
				 break;

			case kEquirectangularCode:
				 erdasCode = 17;
				 break;

			case kMillerCylindricalCode:
				 erdasCode = 18;
				 break;

			case kVanderGrintenICode:
				 erdasCode = 19;
				 break;

			case kObliqueMercatorCode:
				 erdasCode = 20;
				 break;

			case kSpaceObliqueMercatorCode:
				 erdasCode = 21;
				 break;

			case kModifiedTransverseMercatorCode:
				 erdasCode = 22;
				 break;

			}	// end "switch (projectionCode)"

		}	// end "else referenceSystemCode != kGeographicReferenceSystemCode"

	return (erdasCode);

}	// end "ConvertERDASProjectionCodesToMultiSpecCodes"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreateDefaultClassName
//
//	Software purpose:	The purpose of this routine is to create a default class
//							name using the input class number.
//
//	Parameters in:		class name pointer
//							class number
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:			CreateDefaultClassNames in SOpenImage.cpp
//							GetClassesFromHistogram in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/14/1998
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void CreateDefaultClassName (
				UCharPtr								classNamePtr,
				UInt32								classNumber)

{
	UInt32								numberLength;


	CopyPToP (classNamePtr, (UCharPtr)"\0Class \0");

	NumToString (classNumber, (UCharPtr)gTextString2);
	numberLength = (UInt16)MIN (gTextString2[0], 25);
	BlockMoveData ((char*)&gTextString2[1], &classNamePtr[7], numberLength);
	classNamePtr[0] = (char)(6 + numberLength);

	if (classNamePtr[0] < 31)
		classNamePtr[classNamePtr[0] + 1] = 0;

}	// end "CreateDefaultClassName"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreateDefaultClassNames
//
//	Software purpose:	The purpose of this routine is to create a default set
//							of class names.
//
//	Parameters in:		class name pointer
//							number of classes
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:			GetClassesFromHistogram in SOpenImage.cpp
//							LoadClassNameDescriptions in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/07/1995
//	Revised By:			Larry L. Biehl			Date: 01/14/1998	

void CreateDefaultClassNames (
				UCharPtr								classNamePtr,
				HUInt16Ptr							classSymbolPtr,
				UInt32								numberClasses)

{
	UInt32								classNumber,
											index;


	if (classNamePtr != NULL)
		{
		for (index = 0; index < numberClasses; index++) 
			{
			if (classSymbolPtr != NULL)
				classNumber = classSymbolPtr[index];

			else	// classSymbolPtr == NULL
				classNumber = index + 1;

			CreateDefaultClassName (classNamePtr, classNumber);

			classNamePtr += 32;

			}	// end "for (index=0; index<numberClasses; ..."

		}	// end "if (classNamePtr != NULL)"

}	// end "CreateDefaultClassNames"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DetermineIfFileSpecsDialogNeedsCalled
//
//	Software purpose:	The purpose of this routine is to determine if the file
//							specifications dialog box should be called to allow the user
//							to make some settings such as for hdf formatted files.
//							It will not need to be called for cases where hdf type formatted
//							files are being linked together via a multiple selection in the
//							open image file dialog and the data set names are the same as for
//							the first one that was selected.
//
//	Parameters in:		File information structure
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2012
//	Revised By:			Larry L. Biehl			Date: 07/30/2018

SInt16 DetermineIfFileSpecsDialogNeedsCalled (
				Handle								windowInfoHandle,
				FileInfoPtr							fileInfoPtr,
				SInt16								linkOffsetIndex)

{
	FileInfoPtr							firstHdfFileInfoPtr,
											windowFileInfoPtr;

	HdfDataSets							*firstHdfDataSetsPtr,
											*hdfDataSetsPtr;

	Handle								windowFileInfoHandle;

	SInt16								numberImageFiles;

	Boolean								returnCode = 1;


	if ((fileInfoPtr->format == kHDF4Type || fileInfoPtr->format == kNETCDFType ||
			fileInfoPtr->format == kHDF5Type || fileInfoPtr->format == kNETCDF2Type ||
				fileInfoPtr->format == kHDF4Type2 || fileInfoPtr->format == kNITFType) &&
					fileInfoPtr->numberHdfDataSets > 1 &&
							gProcessorCode != kOpenProjectImageProcessor &&
												gProcessorCode != kOpenProjectFileProcessor)
		returnCode = 2;
	
	if (returnCode == 2 && /* gMultipleImageFileCode == 3 && */ linkOffsetIndex > 0)
		{
				// This is for the case when multiple hdf/netcdf type images are selected
				// for linking.
				// If this is not the first selected image, check if the selected data
				// set in the first image matches the name in this selected image.

		if (fileInfoPtr->numberHdfDataSets > gLastHDFDataSetSelection)
			{
			hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (
														  fileInfoPtr->hdfHandle,
														  kLock);

			if (hdfDataSetsPtr[gLastHDFDataSetSelection].name[1] == '*')
				{
                // The data set is a valid data set. Now verify that it has
                // the same name as the initial one in the multiple links.

				windowFileInfoHandle = GetFileInfoHandle (windowInfoHandle);
				numberImageFiles = (SInt16)GetNumberImageFiles (windowInfoHandle);

				windowFileInfoPtr = (FileInfoPtr)GetHandlePointer (windowFileInfoHandle);

				firstHdfFileInfoPtr =
										&windowFileInfoPtr[numberImageFiles - linkOffsetIndex];

				firstHdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (
																		firstHdfFileInfoPtr->hdfHandle);


				if (CompareStringsNoCase (hdfDataSetsPtr[gLastHDFDataSetSelection].name,
									firstHdfDataSetsPtr[gLastHDFDataSetSelection].name) == 0)
					{
                    // Datasets are the same. One does not need to call the
                    // FileSpecs dialog

					fileInfoPtr->hdfDataSetSelection = gLastHDFDataSetSelection;
					returnCode = 3;

					}	// end "if (CompareStringsNoCase (hdfDataSetsPtr..."

            }	// end "if (hdfDataSetsPtr[firstHdfDataSetSelection].name[1] == '*')"

			}	// end "if (fileInfoPtr->numberHdfDataSets > gLastHDFDataSetSelection)"

		}	// end "if (returnFlag && gMultipleImageFileCode == 3)"

	return (returnCode);

}	// end "DetermineIfFileSpecsDialogNeedsCalled"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DeterminePeruSatBandOrder
//
//	Software purpose:	The purpose of this routine is to determine the order of the
//							bands in the PeruSat image file. It is assumed that the
//							information is in the TIFF image description tag.
//
//	Parameters in:		File information structure
//
//	Parameters out:	None
//
//	Value Returned:	Code for band order
//								=0: band wavelength order
//								=1: original band order; red, green, blue, near IR
// 
// Called By:			
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/2018
//	Revised By:			Larry L. Biehl			Date: 06/28/2018

SInt16 DeterminePeruSatBandOrder (
				FileInfoPtr							fileInfoPtr)

{
	char									textString[1024];
	
	Boolean								returnCode = 0;

	
	GetTIFFImageDescription (fileInfoPtr,
										textString,
										1023);
	
	if (textString[0] != 0)
		{
		if (StrStrNoCase (textString, "B2 B1 B0 B3"))
			returnCode = 1;
			
		}	// end "if (textString[0] != 0)"

	return (returnCode);

}	// end "DeterminePeruSatBandOrder"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FinishMapInformationSetUp
//
//	Software purpose:	This routine finishes setting up the map information for the
//							newly read image file.
//
//	Parameters in:		file information pointer
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2012
//	Revised By:			Larry L. Biehl			Date: 04/29/2012

void FinishMapInformationSetUp (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr)

{
	MapProjectionInfoPtr mapProjectionInfoPtr = NULL;


			// Check for map information for image file if it has not
			// been read already.

	ReadArcViewWorldFile (fileInfoPtr);

			// If the file type is ArcView, then check if a GTOPO30 prj file is available.
			// It might be for GEOP030 DEM data such as SRTM data. If so read the
			// information in the file. May need to refine this further to just do for
			// GTOPO30 data if there is a good way to do this.

	if (fileInfoPtr->format == kArcViewType)
		ReadGTOPO30PrjFile (fileInfoPtr, headerRecordPtr);

			// Now complete setting up reference system information if needed.

	mapProjectionInfoPtr = (MapProjectionInfoPtr)
											GetHandlePointer (fileInfoPtr->mapProjectionHandle);

	if (mapProjectionInfoPtr != NULL)
		{
		if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == kUTMRSCode)
			mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
							GetSpecificUTMRSFromDatumOrEllipsoid (
									 mapProjectionInfoPtr->geodetic.datumCode,
									 mapProjectionInfoPtr->geodetic.spheroidCode,
									 mapProjectionInfoPtr->gridCoordinate.referenceSystemCode);

		else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																						kGeographicRSCode)
			mapProjectionInfoPtr->gridCoordinate.projectionCode = kGeographicCode;

		else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																						kNoRSDefinedCode)
			{
			if (mapProjectionInfoPtr->gridCoordinate.projectionCode > kNotDefinedCode ||
					  mapProjectionInfoPtr->geodetic.datumCode > kNoDatumDefinedCode ||
					  		mapProjectionInfoPtr->geodetic.spheroidCode > kNotDefinedCode)
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
																						kUserDefinedRSCode;

			}	// end "if (...->gridCoordinate.referenceSystemCode == kNoRSDefinedCode)"

			  // Get the factor to use to convert from meters (which is output of all
			  // non-geographic convertions to the native units for the image file. This
			  // is not many but some state planes are in survey feet.

		mapProjectionInfoPtr->planarCoordinate.metersToNativeFactor =
				 GetConversionFromMetersToNativeMapUnits (
										mapProjectionInfoPtr->planarCoordinate.mapUnitsCode);

		}	// end "if (mapProjectionInfoPtr != NULL)"

			// Load additional projection information if needed.

	LoadSpheroidInformation (fileInfoPtr->mapProjectionHandle);

}	// end "FinishMapInformationSetUp"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetClassesFromHistogram
//
//	Software purpose:	The purpose of this routine is to get the classes
//							that exist in the Thematic image file from a histogram of
//							the data in the file.  This routine
//							will read the entire image and make a list of the
//							classes.  This list will be used to make a symbol to
//							palette entry table and to adjust the number of classes.
//
//	Parameters in:		pointer to file information structure for active image file. 
//							collapse class selection code
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:			ModalFileSpecification in SOpenFileDialog.cpp
//							LoadClassNameDescriptions in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1990
//	Revised By:			Larry L. Biehl			Date: 04/13/2017

Boolean GetClassesFromHistogram (
				FileInfoPtr							fileInfoPtr,
				SInt16								collapseClassSelection)

{
	FileIOInstructionsPtr			fileIOInstructionsPtr;

	HUCharPtr							classNamePtr,
											ioBufferPtr,
											ioBuffer1Ptr,
											oldClassNamePtr;

	HUInt16Ptr							classSymbolPtr,
											ioBuffer2Ptr,
											newClassSymbolPtr,
											oldClassSymbolPtr;

	Handle								newDescriptionHandle;

	SInt32								lastPercentComplete,
											line,
											lineEnd,
											percentComplete;

	UInt32								index,
											maxClassNumberValue,
											maxNumberClasses,
											numberOutputClasses,
											numberSamples,
											oldIndex,
											oldNumberClasses;

	SInt16 errCode;

	Boolean								classNamesCopiedFlag,
											computationStoppedFlag,
											returnFlag;

	#if defined multispec_win
		USES_CONVERSION;
	#endif

   if (fileInfoPtr == NULL)
		return (FALSE);

			// Initialize local variables.

   returnFlag = FALSE;
   computationStoppedFlag = FALSE;
   maxNumberClasses = 256;
   if (fileInfoPtr->numberBytes == 2)
		maxNumberClasses = (UInt32)UInt16_MAX + 1;

			// Get vector to store the histogram information in for the
			// thematic layer.

	classSymbolPtr = GetSymbolToBinaryVector (fileInfoPtr);
	if (classSymbolPtr == NULL)
		return (FALSE);

			// Get block of memory to use as file IO buffer for the
			// requested layer of data.

	if (!GetIOBufferPointers (&gFileIOInstructions[0],
										gImageWindowInfoPtr,
										gImageLayerInfoPtr,
										fileInfoPtr,
										&ioBufferPtr,
										&ioBufferPtr,
										1,
										fileInfoPtr->numberColumns,
										1,
										1,
										NULL,
										kDoNotPackData,
										kDoNotForceBISFormat,
										kDoNotForceBytes,
										gHasThreadManager,
										&fileIOInstructionsPtr))
		{
		CheckAndDisposePtr ((Ptr)classSymbolPtr);
		return (FALSE);

		}	// end "else end "ioBufferPtr == NULL"

	MSetCursor (kWait);

	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													NULL,
													1,
													fileInfoPtr->numberLines,
													1,
													1,
													fileInfoPtr->numberColumns,
													1,
													1,
													NULL,
													kSetSpecialBILFlagFalse);

			// Get a list of the actual classes in the image.

	lineEnd = fileInfoPtr->numberLines;
	ioBuffer1Ptr = (HUCharPtr)ioBufferPtr;
	ioBuffer2Ptr = (HUInt16Ptr)ioBufferPtr;

	gNextTime = TickCount ();
	lastPercentComplete = -1;
	//numberNextTimes = 0;
	gStatusDialogPtr = NULL;

	for (line = 1; line<=lineEnd; line++)
		{
		errCode = GetLine (fileIOInstructionsPtr,
								 fileInfoPtr,
								 line,
								 0,
								 1,
								 fileInfoPtr->numberColumns,
								 &numberSamples,
								 (HUCharPtr)ioBufferPtr);

		if (errCode != noErr)
			break;

		if (fileInfoPtr->numberBytes == 1)
			{
			for (index = 0; index<fileInfoPtr->numberColumns; index++)
				classSymbolPtr[ioBuffer1Ptr[index]] = 1;
			}

		else if (fileInfoPtr->numberBytes == 2)
			{
			for (index = 0; index<fileInfoPtr->numberColumns; index++)
				classSymbolPtr[ioBuffer2Ptr[index]] = 1;
			}

				// Allow some other things to occur. Display message if it will take a
				// long time.

		percentComplete = 100 * line / lineEnd;
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (
									osMask + keyDownMask + updateMask + mDownMask + mUpMask))
				{
				computationStoppedFlag = TRUE;
				break;

            }	// end "if (TickCount () >= gNextTime)"
			
			if (percentComplete < 10 && gStatusDialogPtr == NULL)
				{
						// Display status dialog box to let user know what the process is

				gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);

				MGetString ((UCharPtr)gTextString, kDialogStrID, IDS_Dialog33);

				#if defined multispec_mac
					SetWTitle (GetDialogWindow (gStatusDialogPtr), (UCharPtr)gTextString);
				#endif		// defined multispec_mac

				#if defined multispec_win
					gStatusDialogPtr->SetWindowText (A2T((LPCSTR)gTextString));
				#endif		// defined multispec_win

				MGetString ((UCharPtr)gTextString, kReformatStrID, IDS_PercentComplete);
				LoadDItemString (
								gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
				LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)0);
				ShowStatusDialogWindow (kShortStatusInfoID);

            }	// end "if (percentComplete < 10 && gStatusDialogPtr == NULL)"
			
			}	// end "if (TickCount () >= nextTime)"

		if (gStatusDialogPtr != NULL)
			{
					// Update status dialog box.

			if (percentComplete != lastPercentComplete && gStatusDialogPtr != NULL)
				{
				LoadDItemValue (
						gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)percentComplete);
				lastPercentComplete = percentComplete;

            }	// end "if (percentComplete != lastPercentComplete && ..."

        }	// end "if (gStatusDialogPtr != NULL)"
		
		}	// end "for (line=1; ..."

	CloseStatusDialog (TRUE);

	CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

	if (errCode == noErr)
		{
				// Find highest class value that exists.
				// Note that if the max index is 0, maxClassNumberValue is alreay set
				// for it.

		maxClassNumberValue = 0;
		for (index = maxNumberClasses - 1; index > 0; index--)
			{
			if (classSymbolPtr[index])
				{
				maxClassNumberValue = index;
				break;

            }	// end "if (classSymbolPtr[index])"

			}	// end "for (index=maxNumberClasses-1; index>=0; index--)"

				// Determine if this image may be a classification probability file
				// in TIFF or GeoTIFF format. The chances are high that it is if
				// the highest class value is 121 and there are no values for 
				// the background (0) class. This is not foolproof but it should not
				// hurt things too much.

		if ((fileInfoPtr->format == kTIFFType || fileInfoPtr->format == kGeoTIFFType) &&
                maxClassNumberValue == 121 && classSymbolPtr[0] == 0 &&
                collapseClassSelection == kCollapseClass)
			fileInfoPtr->collapseClassSelection = collapseClassSelection =
																						kCollapseToHighest;

				// Now compact the classSymbolPtr array so that it only contains
				// those classes that the user requests at the top of the array.

		numberOutputClasses = 0;
		for (index = 0; index <= maxClassNumberValue; index++)
			{
			if (classSymbolPtr[index] || collapseClassSelection == kCollapseToHighest)
				{
				classSymbolPtr[numberOutputClasses] = (UInt16)index;
				numberOutputClasses++;

            }	// end "if (classSymbolPtr[index] || ..."

			}	// end "for (index=0; index<=maximumClassValue; index++)"

				// Make any changes that need to be made in the class symbol table
				// class descriptions and number of classes if there are some 
				// classes.															

		if (numberOutputClasses > 0)
			{
					// If this file is already represented by a class symbol vector
					// and the number of classes between the old and new are the
					// same then just verify that the same values are represted by the
					// old and new class symbol vectors. If they are the same then do
					// not redo which could force a reordering of the class table.

			classNamesCopiedFlag = FALSE;
			oldNumberClasses = fileInfoPtr->numberClasses;

			if (numberOutputClasses == oldNumberClasses)
				{
				if (fileInfoPtr->classDescriptionH != NULL &&
																		fileInfoPtr->descriptionsFlag)
					{
					oldClassNamePtr =
								(HUCharPtr)GetHandlePointer (fileInfoPtr->classDescriptionH);
					oldClassSymbolPtr =
							(HUInt16Ptr)&oldClassNamePtr[oldNumberClasses * sizeof (Str31)];

							// Assume that the class symbol lists are the same.

					classNamesCopiedFlag = TRUE;

					for (index = 0; index < numberOutputClasses; index++)
						{
						oldIndex = 0;
						while (oldIndex < oldNumberClasses)
							{
							if (oldClassSymbolPtr[oldIndex] == classSymbolPtr[index])
								break;

							else	// oldClassSymbolPtr[oldIndex] !=
								oldIndex++;

							}	// end "while (oldIndex < numberOutputClasses)"

						if (oldIndex >= oldNumberClasses)
							{
							classNamesCopiedFlag = FALSE;
							break;

							}	// end "if (oldIndex >= oldNumberClasses)"

						}	// end "for (index=0; index<numberOutputClasses; index++)"

					}	// end "if (...->classDescriptionH != NULL && ..."

				}	// end "if (numberOutputClasses == oldNumberClasses)"

			if (!classNamesCopiedFlag)
				{
						// Get the new list of class names.

				newDescriptionHandle = MNewHandle (numberOutputClasses *
                        (sizeof (Str31) + sizeof (UInt16)));

				if (newDescriptionHandle != NULL)
					{
					classNamePtr = (HUCharPtr)GetHandlePointer (
																		newDescriptionHandle, kLock);
					newClassSymbolPtr = (HUInt16Ptr)&classNamePtr[
                            numberOutputClasses * sizeof (Str31)];

							// Copy the classSymbolPtr array to its saved location.

					for (index = 0; index < numberOutputClasses; index++)
						newClassSymbolPtr[index] = classSymbolPtr[index];

							// Copy any old class names to the new location.

					if (fileInfoPtr->classDescriptionH != NULL &&
																		fileInfoPtr->descriptionsFlag)
						{
						oldClassNamePtr = (HUCharPtr)GetHandlePointer (
															fileInfoPtr->classDescriptionH, kLock);
						oldClassSymbolPtr =
							(HUInt16Ptr)&oldClassNamePtr[oldNumberClasses * sizeof (Str31)];

						for (index = 0; index < numberOutputClasses; index++)
							{
							oldIndex = 0;
							oldClassNamePtr = (HUCharPtr)GetHandlePointer (
																		fileInfoPtr->classDescriptionH);

							while (oldIndex < oldNumberClasses)
								{
								if (oldClassSymbolPtr[oldIndex] == newClassSymbolPtr[index])
									{
									BlockMoveData (oldClassNamePtr, classNamePtr, 32);
									classNamePtr += 32;
									break;

									}	// end "if (oldClassSymbolPtr[oldIndex] == ..."

								else	// oldClassSymbolPtr[index] != ...
									{
									oldIndex++;
									oldClassNamePtr += 32;

									}	// end "else oldClassSymbolPtr[index] != ..."

								}	// end "while (oldIndex < oldNumberClasses)"

							if (oldIndex >= oldNumberClasses)
								{
								CreateDefaultClassName (classNamePtr, newClassSymbolPtr[index]);
								classNamePtr += 32;

								}	// end "if (oldIndex >= oldNumberClasses)"

							}	// end "for (index=0; index<..."

						classNamePtr = (HUCharPtr)GetHandlePointer (newDescriptionHandle);

						classNamesCopiedFlag = TRUE;

						}	// end "if (fileInfoPtr->classDescriptionH != NULL && ...)"

							// Release the memory held by the input description handle.

					UnlockAndDispose (fileInfoPtr->classDescriptionH);

							// Update fileInfo parameters.

					fileInfoPtr->classDescriptionH = newDescriptionHandle;
					fileInfoPtr->numberClasses = numberOutputClasses;
					fileInfoPtr->asciiSymbols = TRUE;
						
					if (maxClassNumberValue >= 16384)
						fileInfoPtr->numberBits = 16;
						
					else if (maxClassNumberValue >= 4096)
						fileInfoPtr->numberBits = 14;
						
					else if (maxClassNumberValue >= 1024)
						fileInfoPtr->numberBits = 12;

					else if (maxClassNumberValue >= 256)
						fileInfoPtr->numberBits = 10;
						
					else if (maxClassNumberValue > 16 || fileInfoPtr->numberBits >= 8)
						fileInfoPtr->numberBits = 8;

					fileInfoPtr->numberBins =
                            (UInt32)ldexp ((double)1, fileInfoPtr->numberBits);

					fileInfoPtr->maxClassNumberValue = maxClassNumberValue;

							// Create a default set of class names.

					if (!classNamesCopiedFlag)
						CreateDefaultClassNames (
										classNamePtr, newClassSymbolPtr, numberOutputClasses);

					classNamesCopiedFlag = TRUE;

					CheckAndUnlockHandle (newDescriptionHandle);

					returnFlag = TRUE;

					}	// end "if (newDescriptionHandle != NULL)"

            }	// end "if (!classNamesCopiedFlag)"

			if (classNamesCopiedFlag)
				fileInfoPtr->classesComputedFlag = TRUE;

			}	// end "if (numberOutputClasses > 0"

		}	// end "if (errCode == noErr)"

	DisposeIOBufferPointers (fileIOInstructionsPtr, &ioBufferPtr, &ioBufferPtr);
	CheckAndDisposePtr ((Ptr)classSymbolPtr);
	MInitCursor ();

	return (returnFlag);

}	// end "GetClassesFromHistogram"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetDatumInfo
//
//	Software purpose:	This routine reads the datum information in the header portion
//							of the image file if available. It is currently used for:
//								FastL7A
//								ArcView other information.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if datum information is found and read
//							1 - if no datum information is found
//							n - the error number if not.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/17/2002
//	Revised By:			Larry L. Biehl			Date: 01/14/2013

SInt16 GetDatumInfo (
				char**								stringPtrPtr,
				SInt16*								datumCodePtr,
				SInt16								stringNumber,
				Boolean								skipEqualFlag)

{
	SInt16 tReturnCode = 1;


	if (*stringPtrPtr != NULL)
		{
				// Find "DATUM" in the buffer.

		tReturnCode = GetFileHeaderString (kFileIOStrID,
														 stringNumber, // DATUM. was DATUM =
														 stringPtrPtr,
														 5, // 7
														 skipEqualFlag,
														 (char*)gTextString,
														 0,
														 50,
														 kNoSubstringAllowed);

		if (tReturnCode == 0)
			{
					// Get the datum name

			if (strncmp ((char*)&gTextString[1], "NAD27", 5) == 0)
				 *datumCodePtr = kNAD27Code;

			else if (strncmp ((char*)&gTextString[1], "NAD83", 5) == 0)
				 *datumCodePtr = kNAD83Code;

			else if (strncmp ((char*)&gTextString[1], "GRS 19", 5) == 0)
				 *datumCodePtr = kNAD83Code;

			else if (strncmp ((char*)&gTextString[1], "WGS84", 5) == 0)
				 *datumCodePtr = kWGS84Code;

			else if (strncmp ((char*)&gTextString[1], "NONE", 4) == 0)
				 *datumCodePtr = kSphereDatumCode;

			}	// end "if (tReturnCode == 0)"

		}	// end "if (*stringPtrPtr != NULL)"

	return (tReturnCode);

}	// end "GetDatumInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetDefaultSupportFile
//
//	Software purpose:	This routine determines if the default support
//							statistics file (supportFileType=kISTAFileType) or gis trailer
//							file (kITRLFileType or kICLRFileType) exists for the given image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	TRUE if a file exists that is meets some specified criteria.
//							FALSE if file does not exist or does not meet some minimum length
//							or header value criteria.
//
// Called By:			LoadMultispectralDisplaySpecs in SDisplayMultispectral.cpp
//							LoadClassNameDescriptions in SOpenImage.cpp
//							GetDefaultThematicFilePalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1988
//	Revised By:			Larry L. Biehl			Date: 04/11/2020

Boolean GetDefaultSupportFile (
				Handle								windowInfoHandle,
				UInt32								supportFileType)

{
	SInt64								fileSize;

	Str15									tempString;

	char*									erdasStatHeader = (char*)"TRAIL";

	UInt32								count,
											localSupportFileType,
											minimumNumberBytes,
											numberImageFiles;

	CMFileStream						*imageFileStreamPtr,
											*supportFileStreamPtr;

	SInt16								errCode,
											statFileFormat;

	SignedByte							supportHandleStatus,
											imageHandleStatus;

	Boolean								fileAlreadyOpenFlag,
											supportFileFlag;


	if (windowInfoHandle == NULL)
																						return (FALSE);

	if (supportFileType != kISTAFileType &&
				supportFileType != kITRLFileType &&
						supportFileType != kICLRFileType)
																						return (FALSE);
																											
	supportFileFlag = FALSE;

			// Get the current default statistics file format.

	statFileFormat = GetHistogramSupportFileFormat (windowInfoHandle);

	numberImageFiles = GetNumberImageFiles (windowInfoHandle);

	if (numberImageFiles == 1 && statFileFormat == kImagineType)
		{
		supportFileFlag = CheckForImagineEstaStatistics (windowInfoHandle);

		if (!supportFileFlag)
            // The statistics information is not available in the Imagine file.
            // Indicate the that support file statistics will now come from a
            // ERDAS74 type stat file.
		SetHistogramSupportFileFormat (windowInfoHandle, kErdas74Type);

		}	// end "if (numberImageFiles == 1 && statFileFormat == kImagineType)"

   if (!supportFileFlag)
		{
		localSupportFileType = supportFileType;
		if (supportFileType == kITRLFileType)
			localSupportFileType = kISTAFileType;

				// Check if requested support file is available.

		supportFileStreamPtr = GetSupportFileStreamPointer	(windowInfoHandle,
																				&supportHandleStatus);

		imageFileStreamPtr = GetFileStreamPointer	(windowInfoHandle,
																	&imageHandleStatus);

		if (supportFileStreamPtr != NULL && imageFileStreamPtr != NULL)
			{
				// Get the default statistics file name if one does not already exist.																			

			if (!FileExists (supportFileStreamPtr))
				{
						// Copy file stream parameters for image file to the
						// support file stream.  We will assume that the support
						// file is in the same folder/directory as the image file is.

				InitializeFileStream (supportFileStreamPtr, imageFileStreamPtr);

				GetDefaultSupportFileName (imageFileStreamPtr,
													supportFileStreamPtr,
													supportFileType);

				SetType (supportFileStreamPtr, localSupportFileType);

            }	// end "if (!FileExists (supportFileStreamPtr))"

			fileAlreadyOpenFlag = FileOpen (supportFileStreamPtr);

			errCode = OpenFileReadOnly (supportFileStreamPtr,
												  kResolveAliasChains,
												  kLockFile,
												  !fileAlreadyOpenFlag);

			if (errCode != fnfErr && errCode != bdNamErr)
				IOCheck (errCode, supportFileStreamPtr);			
				
			#if defined multispec_wx
						// If the support file was not found by MultiSpec on mygeohub, we
						// need to also check the working directory if the check above was
						// for the read only data directory for the session. Use
						// wxSetWorkingDirectory (wxString) or wxFileDialog::SetDirectory
						// to do this. Use wxGetCwd or wxFileDialog::GetDirectory to
						// determine if the default one is the standard directory.
				#ifndef NetBeansProject
					if (errCode == fnfErr)
						{
						FileStringPtr			fileNamePtr;
												
						fileNamePtr =
										(FileStringPtr)supportFileStreamPtr->GetFileNameCPtr ();

						wxString workingDirectory = wxGetCwd ();
						workingDirectory.Append ("/");
						workingDirectory.Append (fileNamePtr);
						wxCharBuffer charWorkingDirectory = workingDirectory.ToAscii ();
						/*
						SInt16 numberChars3 = sprintf ((char*)gTextString3,
																	" workingDirectory: %s%s", 
																	charWorkingDirectory.data (),
																	gEndOfLine);
						ListString ((char*)gTextString3, numberChars3, gOutputTextH);
						
						SInt16 numberChars4 = sprintf ((char*)gTextString3,
																	" workingDirectory length: %d%s", 
																	numberChars3,
																	gEndOfLine);
						ListString ((char*)gTextString3, numberChars4, gOutputTextH);
						*/
								// Close the current file and reset the path name for
								// new .sta file
									
						supportFileStreamPtr->SetFilePathFromCharString (
														(StringPtr)charWorkingDirectory.data (),
														TRUE);  // force current file to be closed
																	
						fileAlreadyOpenFlag = FileOpen (supportFileStreamPtr);

						errCode = OpenFileReadOnly (supportFileStreamPtr,
															  kResolveAliasChains,
															  kLockFile,
															  kVerifyFileStream);
						
						#if defined multispec_wxlin
							if (errCode == fnfErr)
								{
										// File still not found. Check one last place. Check in
										// the users Data directory. The current working directory
										// is within a directory inside of directory titled
										// "Sessions" within this directory. The user may have
										// copied it up a couple of levels.

								fileNamePtr =
											(FileStringPtr)supportFileStreamPtr->GetFileNamePPtr ();
								
								wxFileName fileName;
								fileName.Assign (workingDirectory);
								
										// Remove the last 2 directories from the full file path
								
								if (fileName.GetDirCount () > 0)
									fileName.RemoveLastDir ();
								
								if (fileName.GetDirCount () > 0)
									fileName.RemoveLastDir ();
								
								workingDirectory = fileName.GetFullPath ();
								charWorkingDirectory = workingDirectory.ToAscii ();
								/*
								numberChars = sprintf ((char*)gTextString3,
																			" workingDirectory: %s%s",
																			charWorkingDirectory.data (),
																			gEndOfLine);
								ListString ((char*)gTextString3, numberChars, gOutputTextH);
								
								numberChars = sprintf ((char*)gTextString3,
																		" workingDirectory length: %ld%s",
																		numberChars,
																		gEndOfLine);
								ListString ((char*)gTextString3, numberChars, gOutputTextH);
								*/
										// Close the current file and reset the path name for new .sta file
											
								supportFileStreamPtr->SetFilePathFromCharString (
															(StringPtr)charWorkingDirectory.data (),
															TRUE);  // force current file to be closed
																			
								fileAlreadyOpenFlag = FileOpen (supportFileStreamPtr);

								errCode = OpenFileReadOnly (supportFileStreamPtr,
																	  kResolveAliasChains,
																	  kLockFile,
																	  kVerifyFileStream);
							
								}	// end "if (errCode == fnfErr)"
						#endif	// defined multispec_wxlin
							
						}	// end "if (errCode == fnfErr)"
				#endif	// end "#ifndef NetBeansProject"
			#endif	// end "#if defined multispec_wx"

			if (errCode == noErr)
				{
				count = 5;
				errCode = MReadData (supportFileStreamPtr,
											&count,
											&tempString,
											kNoErrorMessages);

            }	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
				if (localSupportFileType == kISTAFileType)
					{
					if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)
						{
						if (supportFileType == kISTAFileType)
							{
									// Make certain that the default .sta file is of the
									// correct size.

							errCode = GetSizeOfFile (supportFileStreamPtr, &fileSize);

							WindowInfoPtr windowInfoPtr =
											(WindowInfoPtr)GetHandlePointer (windowInfoHandle);
							UInt16 totalNumberChannels = windowInfoPtr->totalNumberChannels;

							minimumNumberBytes = (UInt32)totalNumberChannels * 9 * 128;
							if (errCode == noErr && fileSize >= minimumNumberBytes &&
                                    fileSize <= minimumNumberBytes + 1000)
                                // Decided on 3/13/2013 to force the .sta file to be
                                // the correct size. Use stats from a file for many
                                // more channels causes the image to not be displayed
                                // well. Far larger .sta file imply that they are for
                                // a different image. Another item to check is
                                // whether one is for 32-bit data and the other is
                                // for 16-bit or less data; may need this yet.
								supportFileFlag = TRUE;

							}	// end "if (supportFileType == kISTAFileType)" 

						else	// if (supportFileType != kISTAFileType) 
							supportFileFlag = TRUE;

						}	// end "if (strncmp ((char*)gTextString, ..."

					}	// end "if (localSupportFileType == kISTAFileType)"

				else	// must be .clr file. No items to check. Have to load all data from
                    // file to determine if all is okay. This will be done later. 
					supportFileFlag = TRUE;

            }	// end "if (errCode == noErr)"

					// Close the default support file if it was closed upon entry to this
					// routine.

			if (!fileAlreadyOpenFlag)
				CloseFile (supportFileStreamPtr);

			if (supportFileFlag)
				{
				if (localSupportFileType == kISTAFileType)
					SetHistogramSupportFileFormat (windowInfoHandle, kErdas74Type);

				else	// localSupportFileType == kICLRFileType)
					SetHistogramSupportFileFormat (windowInfoHandle, kArcViewType);

            }	// end "if (supportFileFlag)"

			else	// !supportFileFlag)			
						// If no file is found, set the default file name to 'nothing'.
				SetFileDoesNotExist (supportFileStreamPtr, kDoNotKeepUTF8CharName);

			}	// end "if (supportFileStreamPtr != NULL && imageFileStreamPtr != NULL)"

		UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);

		UnlockFileStream (windowInfoHandle, imageHandleStatus);

		}	// end "if (!supportFileFlag)"

	return (supportFileFlag);

}	// end "GetDefaultSupportFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void GetDefaultSupportFileName
//
//	Software purpose:	This routine determines the default ERDAS support
//							file name for either the statistics file 
//							(supportFileType=1) or gis trailer file 
//							(supportFileType=2) exists for the given image.  It
//							assumes that the file being used is the one for
//							the active image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/26/1991
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void GetDefaultSupportFileName (
				CMFileStream*						imageFileStreamPtr,
				CMFileStream*						supportFileStreamPtr,
				UInt32								supportFileType)

{
	UInt8									*imageFileNameCPtr,
											*imageFilePathPtr,
											*supportFilePathPtr;
	
	
	imageFilePathPtr = (UInt8*)GetFilePathPPointerFromFileStream (imageFileStreamPtr);
	supportFilePathPtr = (UInt8*)GetFilePathPPointerFromFileStream (supportFileStreamPtr);
	imageFileNameCPtr = (UInt8*)GetFileNameCPointerFromFileStream (imageFileStreamPtr);

	GetDefaultSupportFileName (imageFileStreamPtr,
										imageFilePathPtr,
										supportFilePathPtr,
										imageFileNameCPtr,
										supportFileType);

	#if defined multispec_mac
			// Force the uniFileName to be recreated to match the support file name.
		supportFileStreamPtr->uniFileName.length = 0;
	#endif

	#if defined multispec_win || defined multispec_wx
		supportFileStreamPtr->mWideFilePathName[0] = 0;
	#endif

}	// end "GetDefaultSupportFileName" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetDefaultSupportFileName
//
//	Software purpose:	This routine determines the default support
//							file name for either the statistics file 
//							(supportFileType=1) or gis trailer file 
//							(supportFileType=2) exists for the given image.  It
//							assumes that the file being used is the one for
//							the active image window.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			CreateSTASupportFile in SHistogram.cpp
//							GetDefaultSupportFile in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/26/1991
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

void GetDefaultSupportFileName (
				CMFileStream*						imageFileStreamPtr,
				UInt8*								imageFilePathPtr,
				UInt8*								supportFilePathPtr,
				UInt8*								imageFileNameCPtr,
				UInt32								supportFileType)

{
	UInt16								hdfDataSet,
											imageFilePathLength,
											numChars,
											supportFilePathLength;


	if (imageFilePathPtr != NULL && supportFilePathPtr != NULL) 
		{
				  // Copy the path name to the support file path string.
				  // The Windows version will have characters to move.
				  // The Macintosh version will not.

		#if defined multispec_mac
			supportFilePathLength = 0;
		#endif
		#if defined multispec_wx || defined multispec_win
			supportFilePathLength = imageFileStreamPtr->mUTF8PathLength;
		#endif
		memcpy ((char*)&supportFilePathPtr[2],
					(char*)&imageFilePathPtr[2],
					supportFilePathLength);
		//supportFilePathPtr[0] = (UInt8)numberPathCharacters;
		//supportFilePathPtr[numberPathCharacters+1] = 0;
		SetFileStringLength (supportFilePathPtr, supportFilePathLength);
		
				// If this is a multiple image file window, then get the default prefix.																	

		numChars = 0;
		if (gImageWindowInfoPtr != NULL && gImageWindowInfoPtr->numberImageFiles > 1) 
			{
			numChars = sprintf ((char*)&supportFilePathPtr[supportFilePathLength+2],
											"L%hd-",
											gImageWindowInfoPtr->numberImageFiles);
			/*
			numChars = sprintf ((char*)&supportFilePathPtr[numberPathCharacters+2],
											"L%hd-",
											gImageWindowInfoPtr->numberImageFiles);
			*/
			}	// end "if (gImageWindowInfoPtr != NULL && ...)"
		
		//supportFilePathPtr[0] += (UInt8)numChars;
		supportFilePathLength += numChars;

		gTextString[0] = 0;
		if (gImageFileInfoPtr != NULL && supportFileType == kISTAFileType &&
																gImageFileInfoPtr->hdfHandle != NULL)
			{
					// Get file name.

			GetHdfHeaderFileName (gImageFileInfoPtr, gTextString);
			//RemoveSuffix ((FileStringPtr)gTextString, kASCIICharString);
			RemoveSuffix ((FileStringPtr)gTextString);
			int headerFileStringLength = GetFileStringLength (gTextString);

			numChars = MIN (headerFileStringLength,
									gFilePathNameLengthLimit - supportFilePathLength);
			if (numChars > 0) 
				{
				memcpy (&supportFilePathPtr[supportFilePathLength+2],
							&gTextString[2],
							numChars);
				supportFilePathLength += numChars;

				}	// end "if (numChars > 0)"

			GetHdfDataSetName (gImageFileInfoPtr,
									  gImageFileInfoPtr->hdfDataSetSelection,
									  (StringPtr)gTextString,
									  &hdfDataSet);
					  
					// Note that dataSets are 0-based in hdf4 library and 1-based in gdal
					// library
			if (gImageFileInfoPtr->format == kHDF4Type ||
														gImageFileInfoPtr->format == kNETCDFType)
				hdfDataSet++;

			numChars = MIN (4, gFilePathNameLengthLimit - supportFilePathLength);
			if (numChars > 0) 
				{
				numChars = sprintf ((char*)&supportFilePathPtr[supportFilePathLength+2],
											"_%hd_",
											hdfDataSet);

				supportFilePathLength += (UInt8)numChars;

				}	// end "if (numChars > 0)"

			numChars = MIN (gTextString[0],
									gFilePathNameLengthLimit - supportFilePathLength);
			if (numChars > 0)
				{
				memcpy (&supportFilePathPtr[supportFilePathLength+2],
							&gTextString[1],
							numChars);
				supportFilePathLength += (UInt8)numChars;

				}	// end "if (numChars > 0)"

			numChars = 0;

			}	// end "if (gImageFileInfoPtr != NULL && ...)"

		if (gTextString[0] == 0) 
			{
					// Now append the image file name to the support path name.
			
			imageFilePathLength = GetFileStringLength (imageFilePathPtr);
			//numChars = imageFilePathLength - supportFilePathLength;
			numChars = strlen ((char*)imageFileNameCPtr);
			memcpy (&supportFilePathPtr[supportFilePathLength+2],
						imageFileNameCPtr,
						numChars);

			}	// end "if (gTextString[0] == 0)"

		supportFilePathLength += numChars;
			
		SetFileStringLength (supportFilePathPtr, supportFilePathLength);

				// Remove the .LAN, .GIS, .IMG or .DAT extension if it exits at the end
				// of the name.

		RemoveSuffix ((FileStringPtr)supportFilePathPtr);

		UInt8* uSupportFilePathPtr = supportFilePathPtr;

		if (supportFileType == kISTAFileType)
			ConcatFilenameSuffix (
								(FileStringPtr)uSupportFilePathPtr, (UInt8*)"\0.sta\0");

		else if (supportFileType == kITRLFileType)
			ConcatFilenameSuffix (
								(FileStringPtr)uSupportFilePathPtr, (UInt8*)"\0.trl\0");

		else if (supportFileType == kICLRFileType)
			ConcatFilenameSuffix (
								(FileStringPtr)uSupportFilePathPtr, (UInt8*)"\0.clr\0");
					
		}	// end "if (imageFilePathPtr != NULL && ..."

}	// end "GetDefaultSupportFileName"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFileHeaderString
//
//	Software purpose:	This routine reads the string for a specified item
//							from the header record.
//
//	Parameters in:		...
//							numberCharacters
//								if > 0: read that number of characters
//								if = 0: read until a blank is found
//								if = -1: Just return the string pointer such that it is just 
//												after the requested string
//								if = -2: read all characters to the comma
//								if = -3: read all characters to the ';'
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ReadArcViewHeader in SOpenImage.cpp
//							ReadENVIHeader in SOpenImage.cpp
//							ReadFastL7AHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1999
//	Revised By:			Larry L. Biehl			Date: 04/15/2020

SInt16 GetFileHeaderString (
				SInt16								stringID,
				SInt16								stringNumber,
				CharPtr*								bufferPtrPtr,
				SInt16								numberCharacters,
				Boolean								skipEqualFlag,
				CharPtr								stringPtr,
				UInt16								pathLength,
				SInt16								stringLimit,
				Boolean								substringAllowedFlag)

{
	char									*endOfStringPtr,
											*strLessOnePtr,
											*strPtr;

	SInt16								returnCode,
											stringLength;

	Boolean								skipCommaFlag = FALSE;


			// Get the string identifier.

	if (!MGetString (gTextString2, stringID, stringNumber))
		returnCode = -3;

	else	// MGetString (...
		{
		pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
		strPtr = (char*)*bufferPtrPtr;

				// Verify that the found string represents the start of a string in
				// the input buffer.

		do {
			strPtr = (char*)StrStrNoCase (strPtr, (char*)gTextString2);

			if (strPtr == NULL || strPtr == (char*)*bufferPtrPtr)
				break;

			strLessOnePtr = strPtr - 1;
			if (substringAllowedFlag || *strLessOnePtr <= ' ')
				break;

			else	// *strLessOnePtr > ' ' && no substring is allowed
                // This implies that we found a substring which does not represent
                // the beginning of the word that we are looking for. Continue
                // searching. 
				strPtr++;

			}	while (strPtr != NULL);

		if (strPtr != NULL)
			{
			strPtr += stringLength;

			if (skipEqualFlag)
				{
				strPtr = (char*)strstr (strPtr, "=");

				if (strPtr != NULL)
					{
					strPtr++;

							// Now skip any blanks after the equal character.

					//while (*strPtr == ' ')
					//	strPtr++;

					}	// end "if (strPtr != NULL)"

				}	// end "if (skipEqualFlag)"

			}	// end "if (strPtr != NULL)"

		if (strPtr != NULL)
			{
					// Now skip any blanks.

			while (*strPtr == ' ')
				strPtr++;

			returnCode = 0;

					// Now find the ',' if requested.  For this option one will want to
					// read all characters to the ','

			if (numberCharacters <= -2)
				{
				if (numberCharacters == -2)
					endOfStringPtr = (char*)strstr (strPtr, ",");

				else	// numberCharacters == -3  (or anything less than -3)
					endOfStringPtr = (char*)strstr (strPtr, ";");

				if (endOfStringPtr != NULL)
					numberCharacters = (SInt16)(endOfStringPtr - strPtr);

				if (numberCharacters > 0)
					skipCommaFlag = TRUE;

				else	// numberCharacters <= 0
					numberCharacters = -2;

            }	// end "if (numberCharacters <= -2)"

					// Set string limit to allow for pascal and c string identifiers.

			stringLimit -= 2;

			if (numberCharacters > 0)
				{
				numberCharacters = MIN (numberCharacters, stringLimit);

						// Get the string that follows the identifer that is
						// numberCharacters long.
						// Note that if pathLength is > 0, stringPtr is actually a FileStringPtr
						// for which the first two elements in the array represent the string
						// length. One needs to copy the name after the directory
						// path given by pathLength.
				
				if (pathLength > 0)
					{
					BlockMoveData (strPtr, &stringPtr[2+pathLength], numberCharacters);

					SetFileStringLength ((FileStringPtr)stringPtr, pathLength+numberCharacters);
					
					}	// end "if (pathLength > 0)"
				
				else	// pathLength is assumed to be 0
					{
					BlockMoveData (strPtr, &stringPtr[1], numberCharacters);

							// Make pascal and c string.

					stringPtr[0] = (char)(numberCharacters);
					stringPtr[numberCharacters+1] = 0;
					
					}	// end "else pathLength == 0"

						// Set numberCharacters so that the comma will be skipped at the
						// end if needed when set up the return address for the string
						// buffer.

				if (skipCommaFlag)
					numberCharacters++;

            }	// end "if (numberCharacters > 0)"

			else if (numberCharacters == 0)
				{
						// Return string until a blank is found.

				numberCharacters = 0;
				while (*strPtr != ' ' && numberCharacters < stringLimit)
					{
					numberCharacters++;

					stringPtr[numberCharacters] = *strPtr;

					strPtr++;

					}	// end "while (*strPtr != ' ' && ..."

                // Make pascal and c string.

				stringPtr[0] = (char)numberCharacters;
				stringPtr[numberCharacters + 1] = 0;

				}	// end "else if (numberCharacters == 0)"

			else if (numberCharacters == -1)
                // Just return the string pointer such that it is just after
                // the string that was found.
				numberCharacters = 0;

			else	// numberCharacters < -1
				{
                // This indicates an error reading the requested string in
                // the requested procedure.

				numberCharacters = 0;
				stringPtr[0] = 0;

				}	// end "else numberCharacters < -1"

			*bufferPtrPtr = strPtr + numberCharacters;

			}	// end "if (strPtr != NULL)"

		else	// strPtr == NULL
			{
			returnCode = MAX (stringLength, 1);
			stringPtr[0] = 0;

			}	// end "else strPtr == NULL"

		}	// end "else MGetString (..."

	return (returnCode);

}	// end "GetFileHeaderString"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetFileHeaderRealValue
//
//	Software purpose:	This routine reads the value for a specified item
//							from the header record.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2001
//	Revised By:			Larry L. Biehl			Date: 06/14/2002

double GetFileHeaderRealValue (
				SInt16								stringID,
				SInt16								stringNumber,
				CharPtr								bufferPtr,
				SInt16								numberValues,
				Boolean								skipEqualFlag,
				SInt16*								returnCodePtr)

{
	double								returnRealValue;

	char*									strPtr;

	SInt16								returnCode,
											stringLength;


			// Initialize variables.

	returnRealValue = 0.;
	returnCode = 0;

			// Get the string identifier.

	if (!MGetString (gTextString2, stringID, stringNumber))
		*returnCodePtr = -3;

	else	// MGetString (... 
		{
		pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
		strPtr = (char*)StrStrNoCase ((char*)bufferPtr, (char*)gTextString2);
		if (strPtr != NULL)
			{
			if (numberValues != 0)
				{
						// Go to the character just after the string identifier.

				strPtr += stringLength;

				if (skipEqualFlag)
					{
					strPtr = (char*)strstr (strPtr, "=");

					if (strPtr != NULL)
						strPtr++;

					}	// end "if (skipEqualFlag)"

				if (strPtr != NULL)
						// Get the value for the string identifier.
					returnCode = sscanf (strPtr, "%lf", &returnRealValue);

				if (returnCode == 1)
					*returnCodePtr = 0;

				else	// returnCode != 1
					*returnCodePtr = stringLength + 10;

				}	// end "if (numberValues > 0)"

			else	// numberValues == 0
					// No value is to be returned; just an indication that the label
					// was found.
				*returnCodePtr = 0;

			}	// end "if (strPtr != NULL)"

		else	// strPtr == NULL
			*returnCodePtr = stringLength;

		}	// end "else MGetString (..."

	return (returnRealValue);

}	// end "GetFileHeaderRealValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetFileHeaderValue
//
//	Software purpose:	This routine reads the value for a specified item
//							from the header record.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ReadPDSHeader in SOpenImage.cpp
//							ReadVICARHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/05/1991
//	Revised By:			Larry L. Biehl			Date: 03/01/2019

SInt32 GetFileHeaderValue (
				SInt16								stringID,
				SInt16								stringNumber,
				CharPtr								bufferPtr,
				SInt16								numberValues,
				Boolean								skipEqualFlag,
				SInt16*								returnCodePtr)

{
	char*									strPtr;

	SInt32								returnValue;

	SInt16								returnCode,
											stringLength;


			// Initialize variables.

	returnValue = 0;
	returnCode = 0;

			// Get the string identifier.

	if (!MGetString (gTextString2, stringID, stringNumber))
		*returnCodePtr = -3;

	else	// MGetString (... 
		{
		pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
		strPtr = (char*)StrStrNoCase ((char*)bufferPtr, (char*)gTextString2);
		if (strPtr != NULL)
			{
			if (numberValues != 0)
				{
						// Go to the character just after the string identifier.

				strPtr += stringLength;

				if (skipEqualFlag)
					{
					strPtr = (char*)strstr (strPtr, "=");

					if (strPtr != NULL)
						strPtr++;

					}	// end "if (skipEqualFlag)"

				if (strPtr != NULL)
						// Get the value for the string identifier.
					returnCode = sscanf (strPtr, "%d", &returnValue);

				if (returnCode == 1)
					*returnCodePtr = 0;

				else	// returnCode != 1
					*returnCodePtr = stringLength + 10;

				}	// end "if (numberValues > 0)"

			else	// numberValues == 0 
					// No value is to be returned; just an indication that the label
					// was found.
				*returnCodePtr = 0;

			}	// end "if (strPtr != NULL)"

		else	// strPtr == NULL
			*returnCodePtr = stringLength;

		}	// end "else MGetString (..."

	return (returnValue);

}	// end "GetFileHeaderValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetFileHeaderValues
//
//	Software purpose:	This routine reads the value for a specified item
//							from the header record.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ReadPDSHeader in SOpenImage.cpp
//							ReadVICARHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/28/2005
//	Revised By:			Larry L. Biehl			Date: 03/01/2019

SInt32 GetFileHeaderValues (
				SInt16								stringID,
				SInt16								stringNumber,
				CharPtr								bufferPtr,
				SInt16								numberValues,
				SInt32*								valuesPtr,
				Boolean								skipEqualFlag,
				SInt16*								returnCodePtr)

{
	char*									strPtr;

	SInt32								returnValue;

	SInt16								returnCode,
											stringLength;


			// Initialize variables.

	returnValue = 0;
	returnCode = 0;

			// Get the string identifier.

	if (!MGetString (gTextString2, stringID, stringNumber))
		*returnCodePtr = -3;

	else	// MGetString (... 
		{
		pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
		strPtr = (char*)StrStrNoCase ((char*)bufferPtr, (char*)gTextString2);
		if (strPtr != NULL)
			{
			if (numberValues > 0)
				{
						// Go to the character just after the string identifier.

				strPtr += stringLength;

				if (skipEqualFlag) 
					{
					strPtr = (char*)strstr (strPtr, "=");

					if (strPtr != NULL)
						strPtr++;

					}	// end "if (skipEqualFlag)"

						// Now skip the "("

				strPtr = (char*)strstr (strPtr, "(");

				if (strPtr != NULL)
					strPtr++;

				if (strPtr != NULL)
						// Get the values for the string identifier.
						// Note that this is set for 3 values for now. Only used for
						//  reading PDS type formats.
					returnCode = sscanf (strPtr,
												"%d,%d,%d",
												&valuesPtr[0],
												&valuesPtr[1],
												&valuesPtr[2]);

				if (returnCode == numberValues)
					{
					returnValue = numberValues;
					*returnCodePtr = 0;

					}	// end "if (returnCode == numberValues)"

				else	// returnCode != 1
					*returnCodePtr = stringLength + 10;

				}	// end "if (numberValues > 0)"

			else	// numberValues == 0
					// No value is to be returned; just an indication that the label
					// was found.
				*returnCodePtr = 0;

			}	// end "if (strPtr != NULL)"

		else	// strPtr == NULL
			*returnCodePtr = stringLength;

		}	// end "else MGetString (..."

	return (returnValue);

}	// end "GetFileHeaderValues"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetInstrumentChannelDescriptionsAndValues
//
//	Software purpose:	The purpose of this routine is to load any known channel 
//							descriptions from the resource file for the instrument code
//							given in the file information structure.  A flag is set for the
//							file information structure accordingly.
//
//	Parameters in:		Pointer to file information structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			ReadChannelDescriptionsAndValues
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1999
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

void GetInstrumentChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr)
				
{
	ChannelDescriptionPtr			channelDescriptionPtr;

	CharPtr								charPtr;

	float									*channelValuesPtr,
											*channelWidthsPtr;

	UInt8									*fileNamePtr,
											*filePathPPtr,
											*startBandIdentiferPtr;
	
	//UInt16								*channelWavelengthOrderPtr;
	
	int									channelNumber;

	UInt32								blankNumber,
											channel,
											count,
											index;

	SInt16								resourceStringID;

	UInt16								instrumentChannelNumber;

	Boolean								changedFlag,
											continueFlag;


	if (fileInfoPtr == NULL)
																									return;

			// Get handle to memory to store the channel information	in.

	count = fileInfoPtr->numberChannels * sizeof (ChannelDescription);
	channelDescriptionPtr = (ChannelDescriptionPtr)CheckHandleSize (
																&fileInfoPtr->channelDescriptionH,
																&continueFlag,
																&changedFlag,
																count);

	if (continueFlag)
		{
				// Get handle to memory to store the center channel (band) values,
				// channel (band) widths, and channel wavelength order.
		
		channelValuesPtr = CheckChannelValuesHandleSize (fileInfoPtr,
																			&continueFlag);
		
		if (continueFlag)
			{
			channelWidthsPtr = &channelValuesPtr[fileInfoPtr->numberChannels];
			
					// This will be flag indicating that widths are not available.
					
			*channelWidthsPtr = 0;
			
			//channelWavelengthOrderPtr =
			//						(UInt16*)&channelWidthsPtr[fileInfoPtr->numberChannels];
			
			}	// end "if (continueFlag)"

		}	// end "if (continueFlag)"

	if (continueFlag)
		{
		for (channel=1; channel<=fileInfoPtr->numberChannels; channel++)
			{
			resourceStringID = 0;
			channelNumber = 100;

			if (fileInfoPtr->instrumentCode == kLandsatMSS)
				{
						// Handle channel descriptions for Landsat MSS

				if (fileInfoPtr->numberChannels == 1)
					{
							// This implies USGS linked files. Get the band number from the
							// file name.

					fileNamePtr = (UInt8*)GetFileNameCPointerFromFileInfo (fileInfoPtr);
					startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, "_B");

					if (startBandIdentiferPtr != NULL)
						{
						if (sscanf (
								(char*)startBandIdentiferPtr, "_B%d", &channelNumber) != 1)
							channelNumber = 8;
						/*
								// For debugging
						sprintf ((char*)gTextString,
										"    channelNumber1 = %hd%s",
										channelNumber,
										gEndOfLine);
						ListString (
							(char*)gTextString, strlen ((char*)gTextString), gOutputTextH);
						*/
								// The channel numbers can be 40, 50, 60, 70 or
								//   4, 5, 6 & 7

						if (channelNumber >= 40)
							channelNumber /= 10;

						channelNumber -= 3;

						}	// end "if (startBandIdentifer != NULL)"

					}	// end "else if (GetNumberImageFiles (windowInfoHandle) > 1)"

				else	// combined file set
					{
					if (fileInfoPtr->numberChannels == 4)
						channelNumber = channel;

					}	// end "else combined file set"
				/*
				sprintf ((char*)gTextString,
								"    channelNumber2 = %hd%s",
								channelNumber,
								gEndOfLine);
				ListString (
							(char*)gTextString, strlen ((char*)gTextString), gOutputTextH);
				*/
				if (channelNumber >= 1 && channelNumber <= 4)
					resourceStringID = (SInt16)(IDS_ChanDescription48 + channelNumber - 1);

				else	//
					resourceStringID = IDS_ChanDescription36;

				}	// end "if (fileInfoPtr->instrumentCode == kLandsatMSS)"

			else if (fileInfoPtr->instrumentCode == kLandsatTM)
				{
						// Handle channel descriptions for Landsat TM

				if (fileInfoPtr->numberChannels == 1)
					{
							// This implies USGS linked files. Get the band number from the
							// file name.

					fileNamePtr = (UInt8*)GetFileNameCPointerFromFileInfo (fileInfoPtr);
					startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, "_B");

					if (startBandIdentiferPtr != NULL)
						channelNumber = (UInt32)startBandIdentiferPtr[2] - 48;
					
							// if not "_B" try "_sr_band" for surface reflectance data set
							
					else	// startBandIdentiferPtr == NULL
						{
						startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, 
																				"_sr_band");

						if (startBandIdentiferPtr != NULL)
							channelNumber = (UInt32)startBandIdentiferPtr[8] - 48;
							
						}	// end "else startBandIdentiferPtr == NULL"

					}	// end "else if (GetNumberImageFiles (windowInfoHandle) > 1)"

				else	// not linked file set
					{
							// Check if a combined file set.

					if (fileInfoPtr->numberChannels >= 6 &&
																	fileInfoPtr->numberChannels <= 7)
						channelNumber = channel;

							// The channels in the combined data set are assumed to be in
							// wavelength order. The info in the resource file is in band
							// number order. Adjust channel number to take this into account.

					if (channelNumber == 6)
						channelNumber = 7;

					else if (channelNumber == 7)
						channelNumber = 6;

					}	// end "else not kNdfType or kFastL7AType or other linked file set"

				if (channelNumber >= 1 && channelNumber <= 7)
					resourceStringID = (SInt16)(IDS_ChanDescription1 + channelNumber - 1);

				else	//
					resourceStringID = IDS_ChanDescription36;

				}	// end "if (fileInfoPtr->instrumentCode == kLandsatTM)"

			else if (fileInfoPtr->instrumentCode == kLandsatTM7)
				{
						// Handle channel descriptions for Landsat TM 7

				filePathPPtr = (UInt8*)GetFilePathPPointerFromFileInfo (fileInfoPtr);
				channelNumber = 9;
				if (fileInfoPtr->format == kFastL7AType)
					{
							// For FAST-L7A formatted data. The channel identifier is in 6th
							// character from the end of the file name.

					channelNumber = (UInt32)filePathPPtr[filePathPPtr[0] - 5];

							// Convert from ascii to binary.

					channelNumber -= 48;

					}	// end "if (fileInfoPtr->format == kFastL7AType)"

				else if (fileInfoPtr->format == kNdfType)
					{
							// For NLAPS formatted data, the channel identifier is the last
							// number in the file name.

					channelNumber = (UInt32)filePathPPtr[filePathPPtr[0]];

							// Convert from ascii to binary.

					channelNumber -= 48;

					if (channelNumber == 9)
						channelNumber = 6;

					}	// end "else if (fileInfoPtr->format == kNdfType)"

				else if (fileInfoPtr->numberChannels == 1)
					{
							// This implies USGS linked files. Get the band number from the
							// file name.

					fileNamePtr = (UInt8*)GetFileNameCPointerFromFileInfo (fileInfoPtr);
					startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, "_B");

					if (startBandIdentiferPtr != NULL)
						channelNumber = (UInt32)startBandIdentiferPtr[2] - 48;
						
					else	// startBandIdentifierPtr == NULL
						{
								// if not "_B" try "_sr_band" for surface reflectance data 
								// set
							
						startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, 
																				"_sr_band");

						if (startBandIdentiferPtr != NULL)
							channelNumber = (UInt32)startBandIdentiferPtr[8] - 48;
					
						}	// end "else startBandIdentifierPtr == NULL"

					}	// end "else if (GetNumberImageFiles (windowInfoHandle) > 1)"

				else	// not kNdfType or kFastL7AType or other linked file set
					{
							// Check if a combined file set.

					if (fileInfoPtr->numberChannels >= 6 &&
																	fileInfoPtr->numberChannels <= 8)
						channelNumber = channel;

							// The channels in the combined data set are assumed to be in
							// wavelength order. The info in the resource file is in band
							// number order. Adjust channel number to take this into account.

					if (channelNumber == 6)
						channelNumber = 7;

					else if (channelNumber == 7 || channelNumber == 8)
						channelNumber = 6;

					}	// end "else not kNdfType or kFastL7AType or other linked file set"

				if (channelNumber >= 1 && channelNumber <= 8)
					resourceStringID = (SInt16)(IDS_ChanDescription1 + channelNumber - 1);

				else	//
					resourceStringID = IDS_ChanDescription36;

				}	// end "if (fileInfoPtr->instrumentCode == kLandsatTM7)"

			else if (fileInfoPtr->instrumentCode == kLandsatLC8_OLI_TIRS ||
					  fileInfoPtr->instrumentCode == kLandsatLC8_OLI ||
					  fileInfoPtr->instrumentCode == kLandsatLC8_TIRS)
				{
						// Handle channel descriptions for Landsat 8

				if (fileInfoPtr->numberChannels == 1)
					{
							// This implies USGS linked files. Get the band number from the
							// file name.
					
					fileNamePtr = (UInt8*)GetFileNameCPointerFromFileInfo (fileInfoPtr);
					startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, "_B");

					if (startBandIdentiferPtr != NULL)
						{
						if (sscanf ((char*)startBandIdentiferPtr, 
										"_B%d", &channelNumber) != 1)
							channelNumber = 12;

						}	// end "if (startBandIdentifer != NULL)"
						
					if (startBandIdentiferPtr == NULL)
						{
								// if not "_B" try "_sr_band" for surface reflectance data 
								// set
							
						startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, 
																				"_sr_band");

						if (startBandIdentiferPtr != NULL)
							channelNumber = (UInt32)startBandIdentiferPtr[8] - 48;
					
						}	// end "if (startBandIdentiferPtr == NULL)"
						
					if (startBandIdentiferPtr == NULL)
						{
								// if not "_B" or "_sr_band", try _TAB for analysis ready
								// top of atomsphere reflectance set
							
						startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, 
																				"_TAB");

						if (startBandIdentiferPtr != NULL)
							channelNumber = (UInt32)startBandIdentiferPtr[4] - 48;
					
						}	// end "if (startBandIdentiferPtr == NULL)"
						
					if (startBandIdentiferPtr == NULL)
						{
								// if not "_B" or "_sr_band" or _TAB, try _SRB for analysis 
								// ready surface reflectance set
							
						startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, 
																				"_SRB");

						if (startBandIdentiferPtr != NULL)
							channelNumber = (UInt32)startBandIdentiferPtr[4] - 48;
					
						}	// end "if (startBandIdentiferPtr == NULL)"

					}	// end "if (fileInfoPtr->numberChannels == 1)"

				else	// not linked file set
					{
							// Check if a combined file set.
							// If the number of channels is 7, then the assumption is made
							// that this is a surface reflectance leve 2 data set.

					if (fileInfoPtr->instrumentCode == kLandsatLC8_TIRS &&
										fileInfoPtr->numberChannels >= 1 &&
													fileInfoPtr->numberChannels <= 2)
						channelNumber = channel + 9;

					else if (fileInfoPtr->numberChannels >= 7 &&
																	fileInfoPtr->numberChannels <= 10)
						channelNumber = channel;

					if (fileInfoPtr->numberChannels >= 8)
						{
								// The channels in the combined data set are assumed to be in
								// wavelength order. The info in the resource file is in band
								// number order. Adjust channel number to take this into
								// account.

						if (channelNumber == 6)
							channelNumber = 9;

						else if (channelNumber == 7 || channelNumber == 8)
							channelNumber--;

						else if (channelNumber == 9 || channelNumber == 10)
							channelNumber++;
						
						}	// end "if (fileInfoPtr->numberChannels >= 8)"

					}	// end "else not linked file set"

				if (channelNumber >= 1 && channelNumber <= 11)
					resourceStringID = (SInt16)(IDS_ChanDescription37 + channelNumber - 1);

				else	// channelNumber > 11
					resourceStringID = IDS_ChanDescription36;

				}	// end "if (fileInfoPtr->instrumentCode == kLandsatLC8)"

			else if (fileInfoPtr->instrumentCode == kSentinel2A_MSI ||
							fileInfoPtr->instrumentCode == kSentinel2B_MSI)
				{
						// Handle channel descriptions for Sentinel 2 MSI

				if (fileInfoPtr->numberChannels == 1)
					{
							// This implies linked files. Get the band number from the
							// file name.

					fileNamePtr = (UInt8*)GetFileNameCPointerFromFileInfo (fileInfoPtr);
					startBandIdentiferPtr = (UInt8*)strstr ((char*)fileNamePtr, "_B");

					if (startBandIdentiferPtr != NULL)
						{
						if (sscanf (
								(char*)startBandIdentiferPtr, "_B%2d", &channelNumber) != 1)
							channelNumber = 9;
							
						else if (channelNumber == 8)
							{
									// Check if this is channel 8A
									
							if (CompareStringsNoCase (
										startBandIdentiferPtr, (UCharPtr)"_B8A", 4) == 0 ||
																					channelNumber >= 9)
								channelNumber++;
								
							}	// end "else if (channelNumber == 8)"
							
						else if (channelNumber >= 9)
							channelNumber++;

						}	// end "if (startBandIdentifer != NULL)"

					}	// end "if (fileInfoPtr->numberChannels == 1)"

				else	// not linked file set
					{
							// Check if a combined file set.

					if (fileInfoPtr->numberChannels >= 1 &&
																	fileInfoPtr->numberChannels <= 3)
						{
						if (channel == 1)
							channelNumber = channel;
						else	// channel > 1
							channelNumber = 8 + channel;
						
						}	// end "if (fileInfoPtr->instrumentCode == kSentinel2_MSI && ..."

					else if (fileInfoPtr->numberChannels == 4)
						{
						channelNumber = 1 + channel;
						if (channel == 4)
							channelNumber = 8;
						
						}	// end "else if (fileInfoPtr->numberChannels == 4)"

					else if (fileInfoPtr->numberChannels >= 5 &&
																	fileInfoPtr->numberChannels <= 6)
						{
						channelNumber = 4 + channel;
						if (channel == 4)
							channelNumber = 9;
						else if (channel == 5)
							channelNumber = 12;
						else if (channel == 6)
							channelNumber = 13;							
						
						}	// end "else if (fileInfoPtr->numberChannels >= 5 && ..."

					}	// end "else not linked file set"

				if (channelNumber >= 1 && channelNumber <= 13)
					{
					resourceStringID = (SInt16)IDS_ChanDescription53;
					if (fileInfoPtr->instrumentCode == kSentinel2B_MSI)
						resourceStringID = (SInt16)IDS_ChanDescription66;
					
					resourceStringID += (channelNumber - 1);
					
					}	// end "if (channelNumber >= 1 && channelNumber <= 13)"

				else	// channelNumber > 13
					resourceStringID = IDS_ChanDescription36;

				}	// end "if (fileInfoPtr->instrumentCode == kSentinel2A_MSI ..."

			else if (fileInfoPtr->instrumentCode == kSPOT ||
					  fileInfoPtr->instrumentCode == kSPOT4)
				{
				resourceStringID = (SInt16)(IDS_ChanDescription9 + channel - 1);

				if (fileInfoPtr->instrumentCode == kSPOT && channel > 3)
					resourceStringID = IDS_ChanDescription36;

				else if (fileInfoPtr->instrumentCode == kSPOT4 && channel > 4)
					resourceStringID = IDS_ChanDescription36;

				}	// end "else if (fileInfoPtr->instrumentCode == kSPOT || ..."

			else if (fileInfoPtr->instrumentCode == kASTER)
				{
				instrumentChannelNumber = 0;
				if (fileInfoPtr->format == kHDF4Type ||
																fileInfoPtr->format == kNETCDFType)
					{
							// Try to get the channel number
					
					instrumentChannelNumber = GetHdfDataSetInstrumentChannelNumber (
																						 fileInfoPtr,
																						 (UInt16)channel);

					}	// end "if (fileInfoPtr->format == kHDF4Type || ..."

				else	// Some other data format which allows for instrument to be known
					{
					if (fileInfoPtr->numberChannels == 3)
						instrumentChannelNumber = 1;

					else if (fileInfoPtr->numberChannels == 6)
						instrumentChannelNumber = 4;

					else if (fileInfoPtr->numberChannels == 5)
						instrumentChannelNumber = 10;

					}	// end "else Some other data format which allows ..."

				if (instrumentChannelNumber > 0)
					resourceStringID = (SInt16)(IDS_ChanDescription14 +
																	instrumentChannelNumber - 1);

				}	// end "else if (fileInfoPtr->instrumentCode == kASTER)"

			else if (fileInfoPtr->instrumentCode == kTANSO_FTS) 
				{
				if (fileInfoPtr->numberChannels == 4)
					resourceStringID = (SInt16)(IDS_ChanDescription28 + channel - 1);

				}	// end "else if (fileInfoPtr->instrumentCode == kTANSO_FTS)"

			else if (fileInfoPtr->instrumentCode == kTANSO_CAI) 
				{
				if (fileInfoPtr->numberChannels == 4 ||
							fileInfoPtr->numberChannels == 8)
					{
					resourceStringID = (SInt16)(IDS_ChanDescription32 + channel - 1);

					if (channel > 4 && fileInfoPtr->numberChannels == 8)
							// This is probably for case when there are to sets of channels
							// in the same linked group
						resourceStringID = (SInt16)(IDS_ChanDescription32 + channel - 5);

					}	// end "if (fileInfoPtr->numberChannels == 4 || ..."

				}	// end "else if (fileInfoPtr->instrumentCode == kTANSO_CAI)"

			else if (fileInfoPtr->instrumentCode == kPeruSat)
				{
				if (fileInfoPtr->numberChannels == 4)
					{
							// A check needs to be made to determine the channel order.
							// PeruSat data directly from the source appears to be ordered
							// as red, green, blue, near IR. It is described in the TIFF
							// Image Description Tag.
					
					SInt16	peruSatChannel = channel;
					SInt16 bandOrderCode = DeterminePeruSatBandOrder (fileInfoPtr);
					if (bandOrderCode == 1)
						{
						if (channel == 1)
							peruSatChannel = 3;
						
						else if (channel == 3)
							peruSatChannel = 1;
						
						}	// end "if (bandOrderCode == 1)"
					
					resourceStringID = (SInt16)(IDS_ChanDescription79 + peruSatChannel - 1);
					
					}	// end "if (fileInfoPtr->numberChannels == 4)"
				
				else if (fileInfoPtr->numberChannels == 1)
					resourceStringID = (SInt16)(IDS_ChanDescription83 + channel - 1);

				}	// end "else if (fileInfoPtr->instrumentCode == kPeruSat)"

			if (continueFlag && resourceStringID != 0) 
				{
				if (MGetString ((UCharPtr)gTextString3,
										kChanDescriptionStrID,
										resourceStringID)) 
					{
					sscanf ((char*)&gTextString3[1],
								 "%f %f",
								 channelValuesPtr,
								 channelWidthsPtr);

							// Find the second blank character in the string.

					charPtr = (char*)&gTextString3[1];
					index = gTextString3[0];
					blankNumber = 0;
					
					while (blankNumber < 2)
						{
						while (*charPtr != ' ' && index > 0) 
							{
							charPtr++;
							index--;

							}	// end "while (*charPtr != ' ' && index > 0)"
							
						if (*charPtr == ' ')
							{
							blankNumber++;
							charPtr++;
							index--;
							
							}	// end "if (*charPtr == ' ')"
							
						}	// end "while (blankNumber < 2)"

							// Now skip the second blank character. This is the
							// start of the channel description.

					//charPtr++;

							// Now get the length of the channel description string.
							// Force it to be no more than kChannelDescriptionLength characters.

					count = (UInt32)strlen (charPtr);
					count = MIN (kChannelDescriptionLength, count);

							// Move the channel description to the channel description
							// variable and set rest of string to blank.

					BlockMoveData (charPtr, channelDescriptionPtr, count);

					charPtr = (char*)channelDescriptionPtr;
					index = count;
					while (index < kChannelDescriptionLength)
						{
						charPtr[index] = ' ';
						index++;

						}	// end "while (index < kChannelDescriptionLength)"

					fileInfoPtr->descriptionsFlag = TRUE;

					fileInfoPtr->maxNumberDescriptionCharacters = MAX (
													 fileInfoPtr->maxNumberDescriptionCharacters,
													 (SInt16)count);

					}	// end "if (MGetString (gTextString3, ..."

				}	// end "if (continueFlag && resourceStringID != 0)"
				
					// Set description code information.
					
			if (*channelWidthsPtr > 0)
				fileInfoPtr->descriptionCode |= kBandWidthInfoExists;
				
			if (*channelValuesPtr > 0)
				{
				if (*channelValuesPtr < 3.0)
					fileInfoPtr->descriptionCode |= kReflectiveData;
			
				else	// *channelValuesPtr >= 3.0
					fileInfoPtr->descriptionCode |= kThermalData;
					
				}	// end "if (*channelValuesPtr > 0)"
			
					// Initially assume that the channels are in wavelength order
			
			//*channelWavelengthOrderPtr = channel;

			channelDescriptionPtr++;
			channelValuesPtr++;
			channelWidthsPtr++;
			//channelWavelengthOrderPtr++;

			}	// end "for (channel=1; channel<=fileInfoPtr->numberChannels; ..."

		}	// end "if (continueFlag)"

	if (fileInfoPtr->descriptionsFlag) 
		{
		fileInfoPtr->descriptionCode |= kDescriptionExists;
		
		CheckAndUnlockHandle (fileInfoPtr->channelDescriptionH);
		CheckAndUnlockHandle (fileInfoPtr->channelValuesHandle);

		}	// end "if (fileInfoPtr->descriptionsFlag)"

	else	// !fileInfoPtr->descriptionsFlag
		{
		fileInfoPtr->channelDescriptionH =
											UnlockAndDispose (fileInfoPtr->channelDescriptionH);
		fileInfoPtr->channelValuesHandle =
											UnlockAndDispose (fileInfoPtr->channelValuesHandle);

		}	// end "else !fileInfoPtr->descriptionsFlag"

}	// end "GetInstrumentChannelDescriptionsAndValues"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetInstrumentCode
//
//	Software purpose:	The purpose of this routine is to get the instrument code for
//							the image. Currently an attempt will be made to get it from
//							the Tiff/Geotiff image description tag (270) where MultiSpec is
//							putting the instrument description if known.
//
//	Parameters in:		Pointer to file information structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/12/2013
//	Revised By:			Larry L. Biehl			Date: 07/19/2018

void GetInstrumentCode (
				FileInfoPtr							fileInfoPtr)

{
	char									textString[1024];
	TIFFImageFileDirectory			tiffFileDirectory;

	UCharPtr								fileNamePtr,
											startInstrumentNamePtr;
											
	CMFileStream*						fileStreamPtr;

	UInt32								index;

	SInt16								textStringLength;

	UInt16								instrumentResourceCode;


	if (fileInfoPtr->format == kTIFFType || fileInfoPtr->format == kGeoTIFFType)
		{
		if (GetSpecifiedTIFFKeyDirectory (fileInfoPtr, 270, &tiffFileDirectory)) 
			{
			textStringLength = (SInt16)MIN (tiffFileDirectory.count, 1023);

			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

			if (GetTIFFASCIIParameters (fileStreamPtr,
												 &tiffFileDirectory,
												 textString,
												 textStringLength) == noErr) 
				{
				startInstrumentNamePtr =
								(UCharPtr)strstr ((char*)textString, "| Instrument Name: ");
				if (startInstrumentNamePtr != NULL) 
					{
					startInstrumentNamePtr += 19;

							// Now loop through the instrument names to find if there is
							// a match

					instrumentResourceCode = IDS_InstrumentName01;
					for (index = 1; index <= kNumberInstrumentCodes; index++) 
						{
						if (MGetString ((UCharPtr)gTextString3,
												kInstrumentNameStrID,
												instrumentResourceCode)) 
							{
									// Now see if the name instrument name in the file matches
									// the resource name.

							if (!strncmp ((char*)&gTextString3[1], 
												(char*)startInstrumentNamePtr, 
												gTextString3[0])) 
								{
								fileInfoPtr->instrumentCode = (UInt16)index;
								break;

								}	// end "if (!strncmp ((char*)&gTextString3[1], ..."

							}	// end "if (MGetString ((UCharPtr)gTextString3, ..."

						instrumentResourceCode++;

						}	// end "for (instrumentResourceCode=IDS_InstrumentName01; "

					}	// end "if (startInstrumentNamePtr != NULL)"

				}	// end "if (GetTIFFASCIIParameters (fileStreamPtr, ..."

			}	// end "if (GetSpecifiedTIFFKeyDirectory (..."
			
		}	// end "if (fileInfoPtr->format == kTIFFType || ..."
		
	if (fileInfoPtr->instrumentCode == 0)
		{
				// See if the instrument can be determined from the file name. This may
				// be risky but can be helpful.
				
		fileNamePtr = (UCharPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);
				
		if (!CompareStringsNoCase ((UCharPtr)"L4_", fileNamePtr, 3) ||
									!CompareStringsNoCase ((UCharPtr)"L5_", fileNamePtr, 3))
			{
					// This may be a Landsat 5 Thematic Mapper data set.
					
			if (fileInfoPtr->numberChannels == 7 && fileInfoPtr->numberBytes == 1)
				fileInfoPtr->instrumentCode = (UInt16)kLandsatTM;
			
			}	// end "if (CompareStringsNoCase ("L4_", fileNamePtr, 3) ..."
				
		else if (StrStrNoCase ((char*)fileNamePtr, "L4") || 
												StrStrNoCase ((char*)fileNamePtr, "L5"))
			{									
					// This may be a Landsat 5 Thematic Mapper data set.
					
			if (fileInfoPtr->numberChannels == 7 && fileInfoPtr->numberBytes == 1)
				fileInfoPtr->instrumentCode = (UInt16)kLandsatTM;
			
			}	// end "else if (StrStrNoCase ((char*)fileNamePtr, "L4") || ..."
				
		else if (!CompareStringsNoCase ((UCharPtr)"L7_", fileNamePtr, 3))
			{
					// This may be a Landsat 7 Thematic Mapper data set.
					
			if (fileInfoPtr->numberChannels == 8 && fileInfoPtr->numberBytes == 1)
				fileInfoPtr->instrumentCode = (UInt16)kLandsatTM7;
			
			}	// end "else if (!CompareStringsNoCase ((UCharPtr)"L7_", ..."
				
		else if (!CompareStringsNoCase ((UCharPtr)"LC8_", fileNamePtr, 4))
			{
					// This may be a Landsat 5 Thematic Mapper data set.
					
			if (fileInfoPtr->numberChannels == 10 && fileInfoPtr->numberBytes == 2)
				fileInfoPtr->instrumentCode = (UInt16)kLandsatLC8_OLI_TIRS;
			
			}	// end "else if (!CompareStringsNoCase ((UCharPtr)"LC8_", ..."
				
		else if (!CompareStringsNoCase ((UCharPtr)"LO8_", fileNamePtr, 4))
			{
					// This may be a Landsat 5 Thematic Mapper data set.
					
			if (fileInfoPtr->numberChannels == 8 && fileInfoPtr->numberBytes == 2)
				fileInfoPtr->instrumentCode = (UInt16)kLandsatLC8_OLI;
			
			}	// end "else if (!CompareStringsNoCase ((UCharPtr)"LO8_", ..."
				
		else if (!CompareStringsNoCase ((UCharPtr)"LT8_", fileNamePtr, 4))
			{
					// This may be a Landsat 5 Thematic Mapper data set.
					
			if (fileInfoPtr->numberChannels == 8 && fileInfoPtr->numberBytes == 2)
				fileInfoPtr->instrumentCode = (UInt16)kLandsatLC8_TIRS;
			
			}	// end "else if (!CompareStringsNoCase ((UCharPtr)"LT8_", ..."
				
		else if (StrStrNoCase ((char*)fileNamePtr, "LC8"))
			{
					// This may be a Landsat 5 Thematic Mapper data set.
					
			if (fileInfoPtr->numberChannels == 10 && fileInfoPtr->numberBytes == 2)
				fileInfoPtr->instrumentCode = (UInt16)kLandsatLC8_OLI_TIRS;
			
			}	// end "else if (StrStrNoCase ((char*)fileNamePtr, "LC8"))"
				
		else if (StrStrNoCase ((char*)fileNamePtr, "S2A_"))
			{
					// This may be a Sentinel 2A, single channel data set.
					
			if (fileInfoPtr->numberChannels == 1 && fileInfoPtr->numberBytes == 2)
				fileInfoPtr->instrumentCode = (UInt16)kSentinel2A_MSI;
			
			}	// end "else if (StrStrNoCase ((char*)fileNamePtr, "S2A"))"
				
		else if (StrStrNoCase ((char*)fileNamePtr, "S2B_"))
			{
					// This may be a Sentinel 2B, single channel data set.
					
			if (fileInfoPtr->numberChannels == 1 && fileInfoPtr->numberBytes == 2)
				fileInfoPtr->instrumentCode = (UInt16)kSentinel2B_MSI;
			
			}	// end "else if (StrStrNoCase ((char*)fileNamePtr, "S2B"))"
				
		else if (StrStrNoCase ((char*)fileNamePtr, "PER1"))
			{
					// This may be a PeruSat data set.
					
			if (fileInfoPtr->numberChannels == 4 && fileInfoPtr->numberBytes == 2)
				fileInfoPtr->instrumentCode = (UInt16)kPeruSat;
					
			else if (fileInfoPtr->numberChannels == 1 && fileInfoPtr->numberBytes == 2)
						// Assume 0.7 meter panchromatic data
				fileInfoPtr->instrumentCode = (UInt16)kPeruSat;
			
			}	// end "else if (StrStrNoCase ((char*)fileNamePtr, "PER1"))"
		
		}	// end "if (fileInfoPtr->instrumentCode == 0)"

}	// end "GetInstrumentCode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetMapProjectionCodeFromGCTPNumber
//
//	Software purpose:	This routine maps the input GCTP number for the map projection
//							to MultiSpec's map projection code.
//							The ERDAS codes come from document iau_docu1.pdf. Title of
//							document is Machine Independent Format.
//							The GCTP and ERDAS codes are almost identical except for 21 and
// 						22. The ERDAS codes are not defined beyond 22
//							(with the information that I currently have).
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	Code for grid coordinate projection
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/12/2007
//	Revised By:			Larry L. Biehl			Date: 03/23/2012

SInt16 GetMapProjectionCodeFromGCTPNumber (
				SInt16								inputCodeNumber,
				Boolean								erdasCodeFlag,
				SInt16*								projectionCodePtr)

{
	SInt16								projectionCode,
											referenceSystemCode;


	referenceSystemCode = kNotDefinedCode;
	projectionCode = kNotDefinedCode;

	if (erdasCodeFlag && inputCodeNumber > 22)
		inputCodeNumber = -1;

	switch (inputCodeNumber)
		{
		case 0:
			referenceSystemCode = kGeographicRSCode;
			break;

		case 1:
			referenceSystemCode = kUTMRSCode;
			projectionCode = kTransverseMercatorCode;
			break;

		case 2:
			referenceSystemCode = kStatePlaneNAD27RSCode;
			projectionCode = kTransverseMercatorCode;
			break;

		case 3:
			projectionCode = kAlbersConicalEqualAreaCode;
			break;

		case 4:
			projectionCode = kLambertConformalConicCode;
			break;

		case 5:
			projectionCode = kMercatorCode;
			break;

		case 6:
			projectionCode = kPolarStereographicCode;
			break;

		case 7:
			projectionCode = kPolyconicCode;
			break;

		case 8:
			projectionCode = kEquidistantConicCode;
			break;

		case 9:
			projectionCode = kTransverseMercatorCode;
			break;

		case 10:
			projectionCode = kStereographicCode;
			break;

		case 11:
			projectionCode = kLambertAzimuthalEqualAreaCode;
			break;

		case 12:
			projectionCode = kAzimuthalEquidistantCode;
			break;

		case 13:
			projectionCode = kGnomonicCode;
			break;

		case 14:
			projectionCode = kOrthographicCode;
			break;

		case 15:
			projectionCode = kGeneralVerticalNearSideCode;
			break;

		case 16:
			projectionCode = kSinusoidalCode;
			break;

		case 17:
			projectionCode = kEquirectangularCode;
			break;

		case 18:
			projectionCode = kMillerCylindricalCode;
			break;

		case 19:
			projectionCode = kVanderGrintenICode;
			break;

		case 20:
			projectionCode = kObliqueMercatorCode;
			break;

		case 21:
			projectionCode = kRobinsonCode;
			break;

		case 22:
			projectionCode = kSpaceObliqueMercatorCode;
			break;

		case 23:
			projectionCode = kAlaskaConformalCode;
			break;

		case 24:
			projectionCode = kInterruptedGoodeHomolosineCode;
			break;

		case 25:
			projectionCode = kMollweideCode;
			break;

		case 26:
			projectionCode = kInterruptedMollweideCode;
			break;

		case 27:
			projectionCode = kHammerCode;
			break;

		case 28:
			projectionCode = kWagnerIVCode;
			break;

		case 29:
			projectionCode = kWagnerVIICode;
			break;

		case 30:
			projectionCode = kOblatedEqualAreaCode;
			break;

		}	// end "switch (inputCodeNumber)"

	if (erdasCodeFlag)
		{
		if (inputCodeNumber == 21)
			projectionCode = kSpaceObliqueMercatorCode;

		else if (inputCodeNumber == 22)
			projectionCode = kModifiedTransverseMercatorCode;

		}	// end "if (erdasCodeFlag)"

	if (projectionCode != kNotDefinedCode && referenceSystemCode == kNotDefinedCode)
		referenceSystemCode = kUserDefinedRSCode;

	*projectionCodePtr = projectionCode;

	return (referenceSystemCode);

}	// end "GetMapProjectionCodeFromGCTPNumber"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetMultiSpecChannelWidths
//
//	Software purpose:	The purpose of this routine is to get the channel widths from
//							the MultiSpec channel descriptions that have been read from 
//							the end of the file if possible.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ReadMultiSpecChannelDescriptionsAndValues in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/29/2018
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

void GetMultiSpecChannelWidths (
				FileInfoPtr							fileInfoPtr)

{
	char									tempString[32];
	
	ChannelDescriptionPtr			channelDescriptionPtr;
	
	char*									tempStringPtr;

	float									*channelValuesPtr,
											*channelWidthsPtr;
											
	float									bandEnd,
											bandStart;
											
	int									returnCode;

	UInt16								channel;


	if (fileInfoPtr != NULL &&
			fileInfoPtr->channelDescriptionH != NULL &&
					fileInfoPtr->channelValuesHandle != NULL)
		{
				// Get pointer to the channel descriptions.
				
		channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
																	  fileInfoPtr->channelDescriptionH,
																	  kLock);

				// Get handle to memory to store the channel values in.				

		channelValuesPtr = (float*)GetHandlePointer (fileInfoPtr->channelValuesHandle,
																	kLock);
																	 
		channelWidthsPtr = &channelValuesPtr[fileInfoPtr->numberChannels];
		
		bandStart = 0;
		bandEnd = 0;
			
		for (channel=0; channel<fileInfoPtr->numberChannels; channel++)
			{
					// Copy the channel description for the string to the temporary
					// string and forcd a string terminator the end of the channel
					// description.
					// The channel descriptions may not have a c terminator at the end
					// of the string and the string can be up to kChannelDescriptionLength
					// characters long.
					
			BlockMoveData (channelDescriptionPtr, tempString, kChannelDescriptionLength);
			tempString[kChannelDescriptionLength] = 0;
			
					// Skip the band identifer information if it exists. Current
					// standard is for this to end with ')'.

			tempStringPtr = strchr ((char*)tempString, ')');
			if (tempStringPtr == NULL)
				tempStringPtr = tempString;
				
			else	// the ')' character exists, skip it
				tempStringPtr++;
			
			returnCode = sscanf ((char*)tempStringPtr,
										 (char*)"%f-%f",
										 &bandStart,
										 &bandEnd);
										 
			if (returnCode == 1 && bandStart > bandEnd)
				{
						// Assume that bandEnd was not found because the '-' was next
						// to the band end value.
				
				tempStringPtr = strchr ((char*)tempStringPtr, '-');
				if (tempStringPtr != NULL)
					{
					tempStringPtr++;
					returnCode = sscanf (tempStringPtr,
												 (char*)"%f",
												 &bandEnd);
					
					if (returnCode == 1)
						returnCode++;
					
					}	// end "if (tempStringPtr != NULL)"
				
				}	// end "if (returnCode == 1 && bandStart > bandEnd)"
			
			if (returnCode == 2 && bandStart < bandEnd)
				{
				*channelWidthsPtr	= fabs (bandEnd - bandStart);
				fileInfoPtr->descriptionCode |= kBandWidthInfoExists;
				
				}	// end "if (returnCode == 2 && bandStart < bandEnd)"
			
			else	// something wrong; cannot get bandwidth
				*channelWidthsPtr = 0;
				
			channelDescriptionPtr++;
													 
			channelWidthsPtr++;

			}	// end "for (channel=0; channel<..."

		CheckAndUnlockHandle (fileInfoPtr->channelDescriptionH);
		CheckAndUnlockHandle (fileInfoPtr->channelValuesHandle);

		}	// end "if (fileInfoPtr != NULL)" 

}	// end "GetMultiSpecChannelWidths" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProjectionNameInfo
//
//	Software purpose:	This routine reads the projection name in the input string. 
//							It is currently used for:
//								FastL7A
//								ArcView other information.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	Code for grid coordinate projection
//
// Called By:			GetArcViewMapInformation in SArcView.cpp
//							CheckImageHeader in SOpenImage.cpp
//							ReadENVIHeaderMapInfo in SOpenImage.cpp
//							ReadFastL7AHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/17/2002
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 GetProjectionNameInfo (
				UCharPtr								stringPtr,
				SInt16*								referenceSystemCodePtr)

{
	SInt16								projectionCode = kNotDefinedCode,
											referenceSystemCode = kUserDefinedRSCode;


			// Get the code for the grid coordinate projection.
	
	if (CompareStringsNoCase (stringPtr, (UCharPtr)"UTM", 3) == 0)
		{
		referenceSystemCode = kUTMRSCode;
		projectionCode = kTransverseMercatorCode;

		}	// end "if (CompareStringsNoCase (stringPtr, "UTM", 3) == 0)"

	else if (CompareStringsNoCase (stringPtr, (UCharPtr)"SPCS", 4) == 0)
		referenceSystemCode = kStatePlaneNAD27RSCode;

	else if (CompareStringsNoCase (stringPtr, (UCharPtr)"ACEA", 4) == 0)
		projectionCode = kAlbersConicalEqualAreaCode;

	else if (CompareStringsNoCase (stringPtr, (UCharPtr)"ALBERS_EQUAL_AREA", 17) == 0)
		projectionCode = kAlbersConicalEqualAreaCode;

	else if (CompareStringsNoCase (stringPtr, (UCharPtr)"Geographic Lat/Lon", 12) == 0)
		referenceSystemCode = kGeographicRSCode;

	else if (CompareStringsNoCase (stringPtr, (UCharPtr)"GK Zone", 7) == 0)
		{
		referenceSystemCode = kGaussKrugerRSCode;
		projectionCode = kTransverseMercatorCode;

		}	// end "else if (CompareStringsNoCase (stringPtr, "GK Zone", 7) == 0)"

	else if (StrStrNoCase ((char*)stringPtr, "Lambert Azimuthal Equal Area", 27) !=
																											NULL)
		projectionCode = kLambertAzimuthalEqualAreaCode;

	else if (StrStrNoCase ((char*)stringPtr, "Sin1km", 6) != NULL)
		projectionCode = kSinusoidalCode;

	else if (CompareStringsNoCase (stringPtr, (UCharPtr)"Polar Stereo", 12) == 0)
		projectionCode = kPolarStereographicCode;

	*referenceSystemCodePtr = referenceSystemCode;

	return (projectionCode);

}	// end "GetProjectionNameInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		HUInt16Ptr GetSymbolToBinaryVector
//
//	Software purpose:	The purpose of this routine is to obtain memory
//							for a vector which can be used with a histogram
//							of thematic data values or as a table
//							which will convert thematic symbol values to
//							binary data.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:			GetClassesFromHistogram in SOpenImage.cpp
//							CreateSymbolToBinaryTable in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/07/1994
//	Revised By:			Larry L. Biehl			Date: 10/26/2006	

HUInt16Ptr GetSymbolToBinaryVector (
				FileInfoPtr							fileInfoPtr)

{
	UInt32								maxNumberClasses;
	HUInt16Ptr							symbolToOutputBinPtr;


	symbolToOutputBinPtr = NULL;
	if (fileInfoPtr != NULL)
		{
				// Initialize local variables.

		//maxNumberClasses = 256;
		//if (fileInfoPtr->numberBytes == 2)
		//		maxNumberClasses = (UInt32)UInt16_MAX + 1;

		maxNumberClasses = fileInfoPtr->numberBins;

				// Get a pointer to storage for the class symbols.

		symbolToOutputBinPtr = (HUInt16Ptr)MNewPointerClear (
																maxNumberClasses * sizeof (UInt16));

		}	// end "if (fileInfoPtr != NULL)"

	return (symbolToOutputBinPtr);

}	// end "GetSymbolToBinaryVector"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetChannelWavelengthOrder
//
//	Software purpose:	The purpose of this routine is to set the vector defining the
//							wavelength order of the channels. This will only be done if
//							information exists for the center wavelengths for each of the
//							channels.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	Channel wavelength order code:
//								=0; not applicable; no center wavelengths
//								=1; channels are in wavelength order
//								=2; channels are not in wavelength order
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/2018
//	Revised By:			Larry L. Biehl			Date: 06/29/2018

SInt16 GetChannelWavelengthOrder (
				FileInfoPtr							fileInfoPtr)

{
	float									*channelValuesPtr;
	
	UInt16								*channelWavelengthOrderPtr;
	
	UInt32								channelNumber,
											endIndex,
											lastIndex,
											index;
	
	SInt16								returnCode = 0;


	if (fileInfoPtr->channelValuesHandle != NULL)
		{
		returnCode = 1;
		channelValuesPtr = (float*)GetHandlePointer (
										fileInfoPtr->channelValuesHandle, kLock);
		channelWavelengthOrderPtr =
									(UInt16*)&channelValuesPtr[2*fileInfoPtr->numberChannels];
		
				// Initialize order
		
		for (index=0; index<fileInfoPtr->numberChannels; index++)
			channelWavelengthOrderPtr[index] = index;
		
				// This is a simple bubble sort

		endIndex = fileInfoPtr->numberChannels - 1;
		do
			{
			lastIndex = 0;
			for (index=0; index<endIndex; index++)
				{
            if (channelValuesPtr[channelWavelengthOrderPtr[index]] >
											channelValuesPtr[channelWavelengthOrderPtr[index+1]])
					{
							// swap channel number in the sorted list
					
					channelNumber = channelWavelengthOrderPtr[index];
					channelWavelengthOrderPtr[index] = channelWavelengthOrderPtr[index+1];
					channelWavelengthOrderPtr[index+1] = channelNumber;

					lastIndex = index;
					returnCode = 2;
					
					}	// end "if (channelValuesPtr[index] > channelValuesPtr[index+1])"
				
				}	// end "for (index=0; index< fileInfoPtr->numberChannels; index++)"
			
			endIndex = lastIndex;
			
			}	while (endIndex > 0);

		}	// end "if (fileInfoPtr->channelValuesHandle != NULL)"

	return (returnCode);

}	// end "GetChannelWavelengthOrder"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetChannelWavelengthOrder
//
//	Software purpose:	The purpose of this routine is to set the vector defining the
//							wavelength order of the channels. This will only be done if
//							information exists for the center wavelengths for each of the
//							channels.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	Channel wavelength order code:
//								=0; not applicable; no center wavelengths
//								=1; channels are in wavelength order
//								=2; channels are not in wavelength order
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/10/2018
//	Revised By:			Larry L. Biehl			Date: 07/11/2018

SInt16 GetChannelWavelengthOrder (
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr)

{
	FileInfoPtr							fileInfoPtr,
											localFileInfoPtr;
	
	float									*localChannelValuesPtr,
											*tempChannelValuesPtr;
	
	UInt16								*channelWavelengthOrderPtr;
	
	Handle								fileInfoHandle;
	
	UInt32								channelNumber,
											endIndex,
											fileIndex,
											lastIndex,
											index,
											tempIndex;
	
	SInt16								returnCode = 0;


			// Verify there are wavelength information for all the channels associated
			// with the window.
	
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
	if (fileInfoPtr == NULL)
																					return (returnCode);
	
	for (fileIndex=0; fileIndex<windowInfoPtr->numberImageFiles; fileIndex++)
		{
		localFileInfoPtr = &fileInfoPtr[fileIndex];
		if (localFileInfoPtr->channelValuesHandle == NULL)
																					return (returnCode);
		
		}	// end "for (fileIndex=0; fileIndex<windowInfoPtr->numberImageFiles; ..."
	
			// Load the channel values into a temporary vector.
	
	tempChannelValuesPtr = (float*)MNewPointer (
									2 * windowInfoPtr->totalNumberChannels * sizeof (float));
	
	if (tempChannelValuesPtr != NULL)
		{
		returnCode = 1;
		tempIndex = 0;
		for (fileIndex=0; fileIndex<windowInfoPtr->numberImageFiles; fileIndex++)
			{
			localFileInfoPtr = &fileInfoPtr[fileIndex];
			localChannelValuesPtr = (float*)GetHandlePointer (
															localFileInfoPtr->channelValuesHandle);
			
			for (index=0; index<localFileInfoPtr->numberChannels; index++)
				{
				tempChannelValuesPtr[tempIndex] = localChannelValuesPtr[index];
				tempIndex++;
				
				}	// end "for (index=0; index<localFileInfoPtr->numberChannels; index++)"
			
			}	// end "for (index=0; index<windowInfoPtr->numberImageFiles; index++)"
		
		CheckAndUnlockHandle (fileInfoHandle);

		channelWavelengthOrderPtr =
						(UInt16*)&tempChannelValuesPtr[windowInfoPtr->totalNumberChannels];
			
				// Initialize order
			
		for (index=0; index<windowInfoPtr->totalNumberChannels; index++)
			channelWavelengthOrderPtr[index] = index;
			
				// This is a simple bubble sort
	
		endIndex = windowInfoPtr->totalNumberChannels - 1;
		do
			{
			lastIndex = 0;
			for (index=0; index<endIndex; index++)
				{
				if (tempChannelValuesPtr[channelWavelengthOrderPtr[index]] >
									tempChannelValuesPtr[channelWavelengthOrderPtr[index+1]])
					{
							// swap channel number in the sorted list
					
					channelNumber = channelWavelengthOrderPtr[index];
					channelWavelengthOrderPtr[index] = channelWavelengthOrderPtr[index+1];
					channelWavelengthOrderPtr[index+1] = channelNumber;

					lastIndex = index;
					returnCode = 2;
					
					}	// end "if (tempChannelValuesPtr[index] > ..."
				
				}	// end "for (index=0; index<endIndex; index++)"
			
			endIndex = lastIndex;
			
			}	while (endIndex > 0);
		
				// Sorting has been completed. Move the wavelength order to the
				// layer information structure for later use.
			
		for (index=0; index<windowInfoPtr->totalNumberChannels; index++)
			{
			layerInfoPtr[index+1].wavelengthOrder = channelWavelengthOrderPtr[index];
			layerInfoPtr[channelWavelengthOrderPtr[index]+1].wavelengthIndex = index;
			
			}	// end "for (index=0; index<windowInfoPtr->totalNumberChannels; index++)"
		
		CheckAndDisposePtr (tempChannelValuesPtr);
		
		}	// end "if (tempChannelValuesPtr != NULL)"

	return (returnCode);

}	// end "GetChannelWavelengthOrder"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void IntermediateFileUpdate
//
//	Software purpose:	The purpose of this routine is to update the 
//							intermediate variable information for use by getLine
//							when reading the image data in.
//
//	Parameters in:		Pointer to the file information structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			LinkNDFFiles in SNDFFormat.cpp
//							FileSpecificationDialogOK in SOpenFileDialog.cpp
//							CheckImageHeader in SOpenImage.cpp
//							LinkFastL7AFiles in SOpenImage.cpp
//							ReadMultiSpecClassificationHeader in SOpenImage.cpp
//							CreateResultsDiskFile in SUtility.cpp
//							CopyFileInfoFromProject in SProject.cpp
//							MosaicImagesSideBySide in Mosaic.c
//							GetReformatOutputFile in SReformatUtilities.cpp
//							UpdateOutputFileStructure in SReformatUtilities.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 04/12/1988
//	Revised By:			Larry L. Biehl			Date: 07/11/2006

void IntermediateFileUpdate (
				FileInfoPtr							fileInfoPtr)

{
	SInt32 dataBytes;


	dataBytes = (SInt32)fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;

	switch (fileInfoPtr->bandInterleave)
		{
		case kBIL:
			fileInfoPtr->bytesPer1line1chan =
										  fileInfoPtr->numberPreChannelBytes + dataBytes +
										  fileInfoPtr->numberPostChannelBytes;

			fileInfoPtr->bytesPer1lineAllChans =
										  fileInfoPtr->numberChannels *
										  			fileInfoPtr->bytesPer1line1chan +
										  fileInfoPtr->numberPreLineBytes +
										  							fileInfoPtr->numberPostLineBytes;

			fileInfoPtr->bytesPer1chanAllLines =
										  (SInt32)fileInfoPtr->numberLines *
										  							fileInfoPtr->bytesPer1line1chan;
			break;

		case kBSQ:
		case kBNonSQ:
			fileInfoPtr->bytesPer1line1chan =
					  						fileInfoPtr->numberPreLineBytes + dataBytes +
					  									(SInt32)fileInfoPtr->numberPostLineBytes;

			fileInfoPtr->bytesPer1chanAllLines =
					  fileInfoPtr->numberLines * fileInfoPtr->bytesPer1line1chan +
					  			fileInfoPtr->numberPreChannelBytes +
					  					fileInfoPtr->numberPostChannelBytes;

			fileInfoPtr->bytesPer1lineAllChans =
					(SInt32)fileInfoPtr->numberChannels * fileInfoPtr->bytesPer1line1chan;
			break;

		case kBIS:
			fileInfoPtr->bytesPer1lineAllChans =
					  (SInt32)fileInfoPtr->numberPreLineBytes +
					  (SInt32)fileInfoPtr->numberChannels * dataBytes +
					  (SInt32)fileInfoPtr->numberPostLineBytes;

			fileInfoPtr->bytesPer1line1chan = NIL; // Not used for BIS

			fileInfoPtr->bytesPer1chanAllLines = NIL; // Not used for BIS
			break;

		case kBNonSQBlocked:
		case kBIBlock:
			fileInfoPtr->bytesPer1lineAllChans =
					  (SInt32)fileInfoPtr->numberChannels * dataBytes;

			fileInfoPtr->bytesPer1line1chan = NIL;

			fileInfoPtr->bytesPer1chanAllLines = NIL;
			break;

		}	// end "switch (fileInfoPtr->bandInterleave)"

			// Get the number of bins.

	fileInfoPtr->numberBins = 0;

	if (fileInfoPtr->numberBits > 0 && fileInfoPtr->numberBits <= 16)
		fileInfoPtr->numberBins =
							(UInt32)ldexp ((double)1, fileInfoPtr->numberBits);
	else if (fileInfoPtr->numberBits > 16)
		fileInfoPtr->numberBins = 2048;

			// Set min and max value useable data values and initialize
			// min and max data values in the image. Note that the min and max
			// data values will be updated when the image statistics are read.

	if (fileInfoPtr->dataTypeCode == kIntegerType)
		{
		double numberBinsD = ldexp ((double)1, fileInfoPtr->numberBits);
		UInt32 numberBinsLess1 = (UInt32)(numberBinsD - 1);

		fileInfoPtr->maxUsableDataValue = numberBinsLess1;
		if (fileInfoPtr->signedDataFlag)
			fileInfoPtr->maxUsableDataValue -= (UInt32)(numberBinsD / 2);

		fileInfoPtr->minUsableDataValue =
									fileInfoPtr->maxUsableDataValue - numberBinsLess1;

			// Set default values to be used until the histogram information
			// has been read.

		fileInfoPtr->maxNumberEDecimalDigits = 0;
		fileInfoPtr->maxNumberFDecimalDigits = 0;

		}	// end "if (fileInfoPtr->dataTypeCode == kIntegerType)"

	else	// fileInfoPtr->dataTypeCode == kRealType
		{
		if (fileInfoPtr->numberBytes == 4)
			{
			fileInfoPtr->maxUsableDataValue = FLT_MAX;
			fileInfoPtr->minUsableDataValue = -FLT_MAX;

			}	// end "if (fileInfoPtr->numberBytes == 4)"

		else	// fileInfoPtr->numberBytes > 4
			{
			fileInfoPtr->maxUsableDataValue = DBL_MAX;
			fileInfoPtr->minUsableDataValue = -DBL_MAX;

			}	// end "else fileInfoPtr->numberBytes > 4"

				// Set default values to be used until the histogram information
				// has been read.

		fileInfoPtr->maxNumberEDecimalDigits = 2;
		fileInfoPtr->maxNumberFDecimalDigits = 2;

		}	// end "else fileInfoPtr->dataTypeCode == kRealType"

			// Set up parameters for signed data if needed.

	fileInfoPtr->signedBitMask = 0x0000;
	fileInfoPtr->signedValueMask = 0x0000;
	fileInfoPtr->signedValueOffset = 0;
	if (fileInfoPtr->signedDataFlag && fileInfoPtr->numberBytes <= 2)
		{
		fileInfoPtr->signedBitMask = (UInt16)(fileInfoPtr->numberBins >> 1);
		fileInfoPtr->signedValueMask = (UInt16)(fileInfoPtr->numberBins - 1);
		fileInfoPtr->signedValueOffset = (fileInfoPtr->numberBins >> 1);

		}	// end "if (fileInfoPtr->signedDataFlag && ..."

	fileInfoPtr->maxDataValue = fileInfoPtr->maxUsableDataValue;
	fileInfoPtr->minDataValue = fileInfoPtr->minUsableDataValue;

}	// end "IntermediateFileUpdate"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 LinkFastL7AFiles
//
//	Software purpose:	The purpose of this routine is to link the image files in
//							the input image set.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	
// 
// Called By:			LinkFiles in SOpenFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/12/1999
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

SInt16 LinkFastL7AFiles (
				Handle								windowInfoHandle)

{
	CMFileStream						*fileStreamPtr,
											*inputFileStreamPtr;

	FileInfoPtr							fileInfoPtr,
											inputFileInfoPtr = NULL;

	Handle								fileInfoHandle,
											inputFileInfoHandle;

	UInt32								fileNumber,
											numberImageFiles;

	SInt16								returnCode,
											version;

	Boolean								continueFlag = FALSE,
											doneFlag;

	SignedByte							handleStatus;


	returnCode = 1;
	fileInfoHandle = MNewHandle (sizeof (MFileInfo));

	if (fileInfoHandle != NULL)
		{
		numberImageFiles = GetNumberImageFiles (windowInfoHandle);
		fileNumber = 1;
		doneFlag = FALSE;

		while (!doneFlag)
			{
			continueFlag = FALSE;
			fileNumber++;

					// Initialize the variables and handles in the structure.

			InitializeFileInfoStructure (fileInfoHandle, kNotPointer);

			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle,
																		  kLock);

					// Load the file name for the first channel
					// First get the file stream pointer for the structure to be used
					// to get the next file in the list.

			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

					// Now get the pointer to the input file information block that
					// contains the first image file name for this set. Remember that
					// the fileInfoHandle may get changed by the call to 
					// "AddToImageWindowFile" so we need to get it each time through
					// this loop.

			inputFileInfoHandle = GetFileInfoHandle (windowInfoHandle);
			inputFileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
												inputFileInfoHandle, &handleStatus);

			inputFileInfoPtr = &inputFileInfoPtr [numberImageFiles - 1];
			inputFileStreamPtr = GetFileStreamPointer (inputFileInfoPtr);

					// Now copy the file name and other information.

			InitializeFileStream (fileStreamPtr, inputFileStreamPtr);

			MHSetState (inputFileInfoHandle, handleStatus);

					// Load the header information for the specified file

			version = 2;
			if (ReadFastL7AHeader (fileInfoPtr, &version, fileNumber, kLoadHeader) ==
																										noErr)
				{
						// Check if parameters make sense

				if (CheckFileInfoParameters (fileInfoPtr) == 1)
					{
							// Set some additional parameters.

					IntermediateFileUpdate (fileInfoPtr);

					if (!fileInfoPtr->thematicType)
							// Set flag in file information structure indicating				
							// whether wavelength description information is available.	
						ReadChannelDescriptionsAndValues (fileInfoHandle);

							// Add this file to the current window.

					continueFlag = AddToImageWindowFile (windowInfoHandle, fileInfoHandle);

					}	// end "if (CheckFileInfoParameters (fileInfoPtr) == 1)"

				else	// CheckFileInfoParameters (fileInfoPtr) != 1
					continueFlag = FALSE;

				}	// end "if (ReadFastL7AHeader (fileInfoPtr, 2, fileNumber) == noErr)"

			else	// ReadFastL7AHeader (fileInfoPtr, ... != noErr)
				{
				continueFlag = TRUE;
				doneFlag = TRUE;

				}	// end "else ReadFastL7AHeader (fileInfoPtr, ... != noErr)"

			if (!continueFlag)
				doneFlag = TRUE;

			}	// end "while (!doneFlag)"

		}	// end "if (fileInfoHandle != NULL)"

	DisposeFileInfoHandle (fileInfoHandle);

	if (continueFlag)
		returnCode = noErr;

	return (returnCode);

}	// end "LinkFastL7AFiles"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassNameDescriptions
//
//	Software purpose:	The purpose of this routine is to load a set of
//							class name descriptions for Thematic type image files.
//							The class names for ERDAS GIS files will be loaded from
//							the trailer file.  The class names for other Thematic
//							type files will default to Class 1, Class 2, etc.  The
//							class symbols are loaded in order at the end of the
//							class names.  For binary data the symbols will be 0x00,
//							0x02, 0x03 etc.  For ASCII data the symbols will be,
//							for example, characters 1, 2, 3 etc.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			ModalFileSpecification in SFileIO.cpp
//							SetUpThematicImageWindow in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1989
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void LoadClassNameDescriptions (
				Handle								windowInfoHandle)

{
	UCharPtr								classNamePtr;
	FileInfoPtr							fileInfoPtr;
	UInt16*								classSymbolPtr;

	Handle								fileInfoHandle;

	UInt32								index,
											numberClasses;

	Boolean								classNamesExistFlag,
											loadClassNamesFlag,
											loadClassSymbolVectorFlag,
											supportFileExistsFlag;

	SignedByte							handleStatus;


			// Verify that this is a thematic type of window file.

	if (GetWindowType (windowInfoHandle) != kThematicWindowType)
																									return;

	loadClassNamesFlag = FALSE;
	loadClassSymbolVectorFlag = FALSE;

			// Get handle to the file information structure.

	fileInfoHandle = GetFileInfoHandle (windowInfoHandle);

			// Get handles/pointers to file information.

	fileInfoPtr =
				(FileInfoPtr)GetHandleStatusAndPointer (fileInfoHandle, &handleStatus);

	if (fileInfoPtr->classDescriptionH == NULL)
		{
				// Get handle to memory to store the class name information	in.
				// Do not load class names if the number of classes is larger than
				// the limit allowed for a list of class names.

		if (fileInfoPtr->numberClasses <= gClassListLimit)
			fileInfoPtr->classDescriptionH = MNewHandle (
											fileInfoPtr->numberClasses * (sizeof (Str31) + 2));

		if (fileInfoPtr->classDescriptionH != NULL)
			{
			loadClassNamesFlag = TRUE;
			loadClassSymbolVectorFlag = TRUE;

			}	// end "if (fileInfoPtr->classDescriptionH != NULL)"

		}	// end "if (fileInfoPtr->classDescriptionH == NULL)"

	else	// fileInfoPtr->classDescriptionH != NULL
		{
		if (gProcessorCode == kOpenImageFileProcessor)
				// This implies that the current set of class names were a
				// result of computing the number of classes.  We now want to check
				// if there is a set of class names already available.
			loadClassNamesFlag = TRUE;

		}	// end "else fileInfoPtr->classDescriptionH != NULL"

	if (loadClassNamesFlag)
		{
		classNamePtr = (UCharPtr)GetHandlePointer (
														fileInfoPtr->classDescriptionH, kLock);
		classSymbolPtr = (UInt16*)&classNamePtr[
									fileInfoPtr->numberClasses * sizeof (Str31)];

		numberClasses = fileInfoPtr->numberClasses;

		if (loadClassSymbolVectorFlag)
			{
					// Load the default class symbol vector.  We assume that the
					// classes represent the first 'numberClasses' values.

			for (index = 0; index < numberClasses; index++)
				classSymbolPtr[index] = (UInt16)index;

			}	// end "if (loadClassSymbolVectorFlag)"

				// First a check will be made for a default .clr file. This file will
				// override any other format specific files.

		supportFileExistsFlag = GetDefaultSupportFile (windowInfoHandle, kICLRFileType);

		if (supportFileExistsFlag)
			{
			if (!fileInfoPtr->asciiSymbols &&
					  fileInfoPtr->collapseClassSelection != kDoNotCollapseClass)
				{
						// This will find the classes that actually exist.

				classNamesExistFlag = GetClassesFromHistogram (
														fileInfoPtr,
														fileInfoPtr->collapseClassSelection);

						// Note that the class name and class symbol pointer may have
						// changed.

				classNamePtr = (UCharPtr)GetHandlePointer (
											fileInfoPtr->classDescriptionH, kLock);

				classSymbolPtr = (UInt16*)&classNamePtr[
							fileInfoPtr->numberClasses * sizeof (Str31)];

				}	// end "if (!fileInfoPtr->asciiSymbols && ..."

			else if (fileInfoPtr->format == kMultiSpecClassificationType)
						// Must first read the class names and set up the class symbol
						// vector from the classification file.
				ReadMultiSpecClassNames (
								fileInfoPtr, windowInfoHandle, classNamePtr);

			classNamesExistFlag = ReadArcViewClassNames (
												fileInfoPtr, windowInfoHandle, classNamePtr);

			}	// end "if (supportFileExistsFlag)"

		else	// !supportFileExistsFlag)
			{
					// Branch to appropriate section to determine if a support file
					// exists which contains the class names.

			classNamesExistFlag = FALSE;
			switch (fileInfoPtr->format)
				{
				case 0: // No identifying header.
				case kErdas73Type:
				case kErdas74Type:
					supportFileExistsFlag = GetDefaultSupportFile (
																	windowInfoHandle, kITRLFileType);

					if (supportFileExistsFlag)
						{
								// Read the class names into memory.

						classNamesExistFlag = ReadERDASClassNames (
									  fileInfoPtr, windowInfoHandle, classNamePtr);

						}	// end "if (supportFileExistsFlag)"
					break;

				case kSunScreenDumpType: // SUN screen dump format
					classNamesExistFlag = GetClassesFromHistogram (fileInfoPtr,
																fileInfoPtr->collapseClassSelection);
					classNamePtr = (UCharPtr)GetHandlePointer (
												fileInfoPtr->classDescriptionH, kLock);
					classSymbolPtr = (UInt16*)&classNamePtr[
													fileInfoPtr->numberClasses * sizeof (Str31)];
					break;

				case kMultiSpecClassificationType: // MultiSpec classification file
					classNamesExistFlag = ReadMultiSpecClassNames (
													fileInfoPtr, windowInfoHandle, classNamePtr);
					break;

				case kGAIAType:
				case kGAIA2Type:
					classNamesExistFlag = ReadGAIAClassNames (fileInfoPtr, classNamePtr);
					break;

				case kImagineType:
					classNamesExistFlag = ReadImagineClassNames (fileInfoPtr, classNamePtr);
					break;

				case kTIFFType:
				case kGeoTIFFType:
					if (!fileInfoPtr->asciiSymbols &&
								 fileInfoPtr->collapseClassSelection != kDoNotCollapseClass)
						{
								// This will find the classes that actually exist.
						classNamesExistFlag = GetClassesFromHistogram (
															  fileInfoPtr,
															  fileInfoPtr->collapseClassSelection);

								// Note that the class name and class symbol pointer may
								// have changed.

						classNamePtr = (UCharPtr)GetHandlePointer (
														fileInfoPtr->classDescriptionH, kLock);

						classSymbolPtr = (UInt16*)&classNamePtr[
													fileInfoPtr->numberClasses * sizeof (Str31)];

						}	// end "if (!fileInfoPtr->asciiSymbols && ..."

							// This will check for and load a default trailer file for class
							// name if it exists.

					supportFileExistsFlag = GetDefaultSupportFile (
																	windowInfoHandle, kITRLFileType);

					if (supportFileExistsFlag)
						{
								// Read the class names into memory.

						classNamesExistFlag = ReadERDASClassNames (
													fileInfoPtr, windowInfoHandle, classNamePtr);

						}	// end "if (supportFileExistsFlag)"
					break;

				}	// end "switch (fileInfoPtr->format)"

			}	// end "if (!supportFileExistsFlag)"

				// Create a default set of class names if needed.

		if (!classNamesExistFlag)
			CreateDefaultClassNames (classNamePtr, classSymbolPtr, numberClasses);

		fileInfoPtr->descriptionsFlag = TRUE;

		CheckAndUnlockHandle (fileInfoPtr->classDescriptionH);

		}	// end "if (loadClassNamesFlag)"

	MHSetState (fileInfoHandle, handleStatus);

}	// end "LoadClassNameDescriptions"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadImageInformation
//
//	Software purpose:	The purpose of this routine is to load the 
//							image file information such as number 
//							of lines, columns, channels, etc., into the image 
//							file structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			AppendFile in SReformatUtilities.cpp	
//							OpenImageFile in SOpenFileDialog.cpp	
//							GetSpecifiedImageFile in SProject.cpp
//							UserLocateProjectBaseImage in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/02/1991
//	Revised By:			Larry L. Biehl			Date: 01/25/2013

Boolean LoadImageInformation (
				Handle								windowInfoHandle,
				Handle								fileInfoHandle,
				Boolean								getFromProjectFlag,
				UInt32*								formatVersionCodePtr,
				SInt16								linkOffsetIndex)

{
	SInt16								returnCode;

	Boolean								fileInfoLoadedFlag;


			// Determine the file format and load the header information.
			// If returnCode == -1, file format was determined but it can't be	
			// used such as MultiSpec Classification with only training or test	
			// fields.  
			// If returnCode == 0, at least some of the parameters could not be
			//			determined, allow the user to set.
			// If returnCode == 1, the parameters were found.	
			// If returnCode == 2, the parameters were found but the File Specifications
			//			dialog box needs to be called to allow the user to select the
			//			desired data set
			//	If returnCode == 3, the parameters were found but the data set information
			//			needs to be loaded for the default selected data set.
			// If returnCode == 7, this is an ArcView Shape file that was in a multiple
			//			selection list. It was ignored.

	returnCode = CheckImageHeader (windowInfoHandle,
												fileInfoHandle,
												formatVersionCodePtr,
												kLoadHeader,
												linkOffsetIndex);

	fileInfoLoadedFlag = (returnCode >= 1 && returnCode <= 3);
		
	if (gGetFileImageType != kVectorImageType)
		{
		if (returnCode == 0 && getFromProjectFlag)
			fileInfoLoadedFlag = CopyFileInfoFromProject (fileInfoHandle);

		if (!fileInfoLoadedFlag && returnCode == 0)
			fileInfoLoadedFlag = ModalFileFormat (fileInfoHandle);

		if (fileInfoLoadedFlag && returnCode == 2)
			{
					// Allow user to select the hdf data set if needed.

			fileInfoLoadedFlag = FileSpecificationDialog (fileInfoHandle,
																		  NULL,
																		  NULL);

			}	// end "if (fileInfoLoadedFlag && returnCode == 2)"

		if (fileInfoLoadedFlag && returnCode == 3)
			{
					// Force the information for the default data set to
					// be loaded.

			fileInfoLoadedFlag = LoadSelectedDataSetInformation (fileInfoHandle);

			}	// end "if (fileInfoLoadedFlag && returnCode == 3)"

		if (fileInfoLoadedFlag)
			{
			if (gGetFileImageType == kMultispectralImageType)
				{
						// Set flag in file information structure indicating
						// whether wavelength description information is stored
						// at the end of the image data.

				ReadChannelDescriptionsAndValues (fileInfoHandle);

				}	// end "if (gGetFileImageType == kMultispectralImageType)"

			}	// end "if (fileInfoLoadedFlag)"

		}	// end "if (gGetFileImageType != kVectorImageType)"

	return (fileInfoLoadedFlag);

}	// end "LoadImageInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadSelectedDataSetInformation
//
//	Software purpose:	The purpose of this routine is to load the selected data set
//							information.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:				
// 
// Called By:			LoadImageInformation in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2012
//	Revised By:			Larry L. Biehl			Date: 03/16/2014

Boolean LoadSelectedDataSetInformation	(
				Handle								fileInfoHandle)

{
	FileInfoPtr							fileInfoPtr;

	SInt16								returnCode = 1;

	Boolean								returnFlag = FALSE;

	SignedByte							handleStatus;


	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
													fileInfoHandle, &handleStatus);

	if (fileInfoPtr != NULL)
		returnCode = LoadSelectedDataSetInformation (fileInfoPtr,
																	fileInfoPtr,
																	fileInfoPtr->hdfDataSetSelection,
																	TRUE);

			// Update some additional parameters.

	IntermediateFileUpdate (fileInfoPtr);

	MHSetState (fileInfoHandle, handleStatus);

	if (returnCode == noErr || returnCode == 20 || returnCode == 2)
		returnFlag = TRUE;

	return (returnFlag);

}	// end "LoadSelectedDataSetInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ModalFileFormat
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for determining the format of the image file.
//  						The modal dialog template is in the resource file.
//							The default format is the "other" tape format.
//
//							The dialog box in this routine is being bypassed as of 
//							12/18/1989.  I feel that it just adds an unneeded step.  
//							An ERDAS type file will be assumed and then the
//							'Set File Specification' dialog will be called.
//		
//	Parameters in:		Window (address of window grafport that image is in).
//
//	Parameters out:	None
//
//	Value Returned:	None				
//
// Called By:			LoadImageInformation in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/07/1988
//	Revised By:			Larry L. Biehl			Date: 04/01/2006

Boolean ModalFileFormat (
				Handle								fileInfoHandle)

{
	FileInfoPtr							activeFileInfoPtr,
											fileInfoPtr;

	Handle								activeImageFileInfoH;

	Boolean								returnFlag;

	SignedByte							handleStatus1,
											handleStatus2;


			// Get the handle status and then lock the memory, in case it
			// isn't already locked, until the information has been loaded			
			// in. Then get the pointer to the file information block

	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																fileInfoHandle, &handleStatus1);

	if (fileInfoPtr->format == 0)
		{
				// No parameters have been set yet; get some default parameters.

		activeImageFileInfoH = GetActiveImageFileInfoHandle ();

		if (activeImageFileInfoH != NULL)
			{
					// An active image window exists already, then use those
					// parameters as the default.

			activeFileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
					  activeImageFileInfoH, &handleStatus2);

			fileInfoPtr->numberLines = activeFileInfoPtr->numberLines;
			fileInfoPtr->numberColumns = activeFileInfoPtr->numberColumns;
			fileInfoPtr->numberChannels = activeFileInfoPtr->numberChannels;
			fileInfoPtr->dataTypeCode = activeFileInfoPtr->dataTypeCode;
			fileInfoPtr->numberBytes = activeFileInfoPtr->numberBytes;
			fileInfoPtr->numberBits = activeFileInfoPtr->numberBits;
			fileInfoPtr->swapBytesFlag = activeFileInfoPtr->swapBytesFlag;
			fileInfoPtr->startLine = activeFileInfoPtr->startLine;
			fileInfoPtr->startColumn = activeFileInfoPtr->startColumn;
			fileInfoPtr->bandInterleave = activeFileInfoPtr->bandInterleave;
			fileInfoPtr->numberHeaderBytes = activeFileInfoPtr->numberHeaderBytes;
			fileInfoPtr->numberTrailerBytes = activeFileInfoPtr->numberTrailerBytes;
			fileInfoPtr->numberPreLineBytes = activeFileInfoPtr->numberPreLineBytes;
			fileInfoPtr->numberPostLineBytes = activeFileInfoPtr->numberPostLineBytes;
			fileInfoPtr->numberPreChannelBytes =
														activeFileInfoPtr->numberPreChannelBytes;
			fileInfoPtr->numberPostChannelBytes =
														activeFileInfoPtr->numberPostChannelBytes;
			fileInfoPtr->signedDataFlag = activeFileInfoPtr->signedDataFlag;

			if (fileInfoPtr->thematicType == FALSE && fileInfoPtr->numberBits == 4)
				 fileInfoPtr->numberBits = 8;

			MHSetState (gActiveImageFileInfoH, handleStatus2);

			}	// end "if (gActiveImageFileInfoH)"

		else	// activeImageFileInfoH == NULL
			{
					// Initialize variables according to the default format.

			fileInfoPtr->numberLines = 256;
			fileInfoPtr->numberColumns = 256;
			fileInfoPtr->numberChannels = 1;
			fileInfoPtr->dataTypeCode = kIntegerType;
			fileInfoPtr->numberBytes = 1;
			fileInfoPtr->numberBits = 8;
			fileInfoPtr->numberBins = 256;
			fileInfoPtr->numberClasses = 0;
			fileInfoPtr->maxClassNumberValue = 0;
			fileInfoPtr->swapBytesFlag = FALSE;
			fileInfoPtr->asciiSymbols = FALSE;
			fileInfoPtr->thematicType = FALSE;
			fileInfoPtr->startLine = 1;
			fileInfoPtr->startColumn = 1;
			fileInfoPtr->bandInterleave = kBIL;
			fileInfoPtr->numberHeaderBytes = 0;
			fileInfoPtr->numberTrailerBytes = 0;
			fileInfoPtr->numberPreLineBytes = 0;
			fileInfoPtr->numberPostLineBytes = 0;
			fileInfoPtr->numberPreChannelBytes = 0;
			fileInfoPtr->numberPostChannelBytes = 0;

			}	// end "else gActiveImageFileInfoH == NULL"

		}	// end "if (fileInfoPtr->format == 0)"

	if (gGetFileImageType == 0)
		gGetFileImageType = kMultispectralImageType;

	if (gGetFileImageType == kThematicImageType)
		{
		if (fileInfoPtr->numberClasses == 0)
			fileInfoPtr->numberClasses = fileInfoPtr->numberBins;

		if (fileInfoPtr->numberClasses == 0)
			fileInfoPtr->numberClasses = 1;

		fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;

		fileInfoPtr->thematicType = TRUE;

		}	// end "if (gGetFileImageType == kThematicImageType)"

			// Get the modal dialog for the image file specification if the
			//	user requested okay.  Otherwise the user requested cancel.		
			// Assume that he may want to request a different file.					

	returnFlag = FileSpecificationDialog (fileInfoHandle, NULL, NULL);

			// Unlock the file information handle if needed.

	MHSetState (fileInfoHandle, handleStatus1);

	return (returnFlag);

}	// end "ModalFileFormat"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReadChannelDescriptionsAndValues
//
//	Software purpose:	The purpose of this routine is load the channel
//							descriptions and values if available.  A flag is set for 
//							the file information structure accordingly.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			LoadImageInformation in SFileIO.cpp
//							ModalFileSpecification in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/22/1993
//	Revised By:			Larry L. Biehl			Date: 07/10/2018

void ReadChannelDescriptionsAndValues (
				Handle								fileInfoHandle)

{
	SignedByte							handleStatus;

	FileInfoPtr							fileInfoPtr;


			// Get descriptions from the appropriate file depending upon the
			// image file type.

			// Get handles/pointers to file information

	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																	fileInfoHandle, &handleStatus);

	if (fileInfoPtr != NULL)
		{
		if (fileInfoPtr->format == kLARSYSMISTType)
			ReadLARSYSChannelDescriptionsAndValues (fileInfoPtr);
		
		else if (fileInfoPtr->format == kENVIType)
			ReadENVIChannelDescriptionsAndValues (fileInfoPtr);

		if (!fileInfoPtr->descriptionsFlag)
			ReadMultiSpecChannelDescriptionsAndValues (fileInfoPtr);

		if (fileInfoPtr->instrumentCode == 0)
			GetInstrumentCode (fileInfoPtr);
			
		if (!fileInfoPtr->descriptionsFlag && fileInfoPtr->instrumentCode > 0)
			GetInstrumentChannelDescriptionsAndValues (fileInfoPtr);
		
				// Get a vector with the channels in wavelength order
		
		//fileInfoPtr->channelsInWavelengthOrderCode =
		//												GetChannelWavelengthOrder (fileInfoPtr);

		}	// end "if (fileInfoPtr != NULL)"

	MHSetState (fileInfoHandle, handleStatus);

}	// end "ReadChannelDescriptionsAndValues"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadENVIHeader
//
//	Software purpose:	This routine reads the file specifications from an ENVI header 
//							record.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if ENVI header record parameters make sense
//							1 - if ENVI header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/10/1999
//	Revised By:			Larry L. Biehl			Date: 01/17/2019

SInt16 ReadENVIHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	char*									stringPtr;
	UInt8*								imageFileNamePtr;

	CMFileStream*						fileStreamPtr;

	SInt32								value;

	SInt16								errCode,
											fileType = 0,
											formatErrorCode = noErr,
											returnCode = 1,
											tReturnCode;

	//SignedByte						handleStatus;


	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
				// Check if this is an ENVI header file.

		if (strncmp (headerRecordPtr, "ENVI", 4) == 0)
			fileType = kENVIType;

		if (fileType == 0)
			{
					// Check if this is a header file that starts with "description".
					// If so, then it uses the same header format as the "ENVI" 
					// header does.

			if (strncmp (headerRecordPtr, "description", 11) == 0)
				fileType = kENVIType;

			}	// end "if (fileType == 0)"


		if (fileType == 0 && formatOnlyCode == kLoadHeader)
			{
					// Now check if a .hdr file exists in the same folder that contains
					// ENVI header information.

			fileType = CheckForENVIHeader (fileInfoPtr, headerRecordPtr);

			}	// end "if (fileType == 0 && formatOnlyCode == kLoadHeader)"

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																							return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

		errCode = noErr;

		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		imageFileNamePtr = (UInt8*)GetFilePathPPointerFromFileStream (fileStreamPtr);

				// The header file has been read.  
				// Check if the input file is the header file. If it is close the file
				// and find the associated image file.
				
		if (CompareSuffixNoCase ((char*)"\0.hdr\0", imageFileNamePtr, NULL)) 
			{
			CloseFile (fileStreamPtr);
			RemoveCharsNoCase ((char*)"\0.???\0", imageFileNamePtr);
			
					// This will be an attempt to read a file with no suffix.

			errCode = OpenFileReadOnly (fileStreamPtr,
												 kResolveAliasChains,
												 kLockFile,
												 kVerifyFileStream);

			if (errCode != noErr)
				{
						// That wasn't it. Now see if it is a ".bil" file.

				ConcatFilenameSuffix (imageFileNamePtr, (StringPtr)"\0.bil\0");

				errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

				}	// end "if (errCode != noErr)"

			if (errCode != noErr)
				{
						// Try getting file with ".bsq" suffix at the end.

				RemoveCharsNoCase ((char*)"\0.bil\0", imageFileNamePtr);
				ConcatFilenameSuffix (imageFileNamePtr, (StringPtr)"\0.bsq\0");

				errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

				}	// end "if (errCode != noErr)"

			if (errCode != noErr)
				{
						// Try getting file with ".dat" suffix at the end.

				RemoveCharsNoCase ((char*)"\0.bsq\0", imageFileNamePtr);
				ConcatFilenameSuffix (imageFileNamePtr, (StringPtr)"\0.dat\0");

				errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

				}	// end "if (errCode != noErr)"

			if (errCode != noErr)
				{
						// Try getting file with ".img" suffix at the end.

            RemoveCharsNoCase ((char*)"\0.dat\0", imageFileNamePtr);
            ConcatFilenameSuffix (imageFileNamePtr, (StringPtr)"\0.img\0");

            errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

				}	// end "if (errCode != noErr)"

			if (errCode != noErr)
				{
						// Try getting file with ".cal" suffix at the end.

            RemoveCharsNoCase ((char*)"\0.img\0", imageFileNamePtr);
            ConcatFilenameSuffix (imageFileNamePtr, (StringPtr)"\0.cal\0");

            errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

				}	// end "if (errCode != noErr)"

			if (errCode != noErr)
				{
						// Try getting file with ".drk" suffix at the end.

            RemoveCharsNoCase ((char*)"\0.cal\0", imageFileNamePtr);
            ConcatFilenameSuffix (imageFileNamePtr, (StringPtr)"\0.drk\0");

            errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

				}	// end "if (errCode != noErr)"

			if (errCode != noErr)
				{
						// Try getting file with ".lan" suffix at the end.

            RemoveCharsNoCase ((char*)"\0.drk\0", imageFileNamePtr);
            ConcatFilenameSuffix (imageFileNamePtr, (StringPtr)"\0.lan\0");

            errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

				}	// end "if (errCode != noErr)"

			if (errCode != noErr)
				{
						// Try getting file with no suffix at the end again.

				RemoveCharsNoCase ((char*)"\0.lan\0", imageFileNamePtr);

            errCode = OpenFileReadOnly (fileStreamPtr,
													  kResolveAliasChains,
													  kLockFile,
													  kVerifyFileStream);

            if (errCode == noErr)
							// Make sure that there is a c terminator at the correct place.
					imageFileNamePtr[imageFileNamePtr[0] + 1] = 0;

				}	// end "if (errCode != noErr)"
				
			}	// end "if (CompareSuffixNoCase ((char*)"\0.hdr\0", imageFileNamePtr, ..."
		
		if (errCode == noErr)
			{
					// Find "samples = " in the buffer.								

			value = GetFileHeaderValue (kFileIOStrID,
												  IDS_FileIO114, // samples
												  headerRecordPtr,
												  1,
												  kSkipEqual,
												  &tReturnCode);

			if (tReturnCode == 0 && value > 0)
				fileInfoPtr->numberColumns = value;

			else	// tReturnCode != 0
				errCode = tReturnCode;

			if (errCode == noErr)
				{
						// Find "lines = " in the buffer.								

				value = GetFileHeaderValue (kFileIOStrID,
														IDS_FileIO115, // lines
														headerRecordPtr,
														1,
														kSkipEqual,
														&tReturnCode);

            if (tReturnCode == 0 && value > 0)
					fileInfoPtr->numberLines = value;

            else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "bands = " in the buffer.								

            value = GetFileHeaderValue (kFileIOStrID,
													  IDS_FileIO116, // bands
													  headerRecordPtr,
													  1,
													  kSkipEqual,
													  &tReturnCode);

            if (tReturnCode == 0 && value > 0)
					fileInfoPtr->numberChannels = (UInt16)value;

            else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "header offset = " in the buffer.								

            value = GetFileHeaderValue (kFileIOStrID,
													  IDS_FileIO117, // header offset
													  headerRecordPtr,
													  1,
													  kSkipEqual,
													  &tReturnCode);

            if (tReturnCode == 0 && value >= 0)
					fileInfoPtr->numberHeaderBytes = value;

            else	// tReturnCode != 0
					fileInfoPtr->numberHeaderBytes = 0;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "data type = " in the buffer.								

				value = GetFileHeaderValue (kFileIOStrID,
													  IDS_FileIO118, // data type
													  headerRecordPtr,
													  1,
													  kSkipEqual,
													  &tReturnCode);

            if (tReturnCode == 0)
					{
					switch (value)
						{
						case 1:
							fileInfoPtr->numberBytes = (UInt16)1;
							fileInfoPtr->numberBits = 8;
							break;

						case 2:
							fileInfoPtr->numberBytes = (UInt16)2;
							fileInfoPtr->numberBits = 16;
							fileInfoPtr->signedDataFlag = TRUE;
							break;

						case 3:
							fileInfoPtr->numberBytes = (UInt16)4;
							fileInfoPtr->numberBits = 32;
							fileInfoPtr->signedDataFlag = TRUE;
							break;

						case 4:
							fileInfoPtr->numberBytes = (UInt16)4;
							fileInfoPtr->numberBits = 32;
							fileInfoPtr->signedDataFlag = TRUE;
							fileInfoPtr->dataTypeCode = kRealType;
							break;

						case 5:
							fileInfoPtr->numberBytes = (UInt16)8;
							fileInfoPtr->numberBits = 64;
							fileInfoPtr->signedDataFlag = TRUE;
							fileInfoPtr->dataTypeCode = kRealType;
							break;

						case 12:
							fileInfoPtr->numberBytes = (UInt16)2;
							fileInfoPtr->numberBits = 16;
							fileInfoPtr->signedDataFlag = FALSE;
							break;

						case 13:
							fileInfoPtr->numberBytes = (UInt16)4;
							fileInfoPtr->numberBits = 32;
							fileInfoPtr->signedDataFlag = FALSE;
							break;

						default:
							fileInfoPtr->numberBytes = 1;

							if (value < 1)
								formatErrorCode = 4;

							else	// value > 2
								{
								formatErrorCode = (SInt16)(value - 2);
								formatErrorCode = MIN (formatErrorCode, 4);

								}	// end "else value > 2"

						}	// end "switch (value)"

					}	// end "if (tReturnCode == 0)"

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "interleave = " in the buffer.								

            stringPtr = headerRecordPtr;
            tReturnCode = GetFileHeaderString (kFileIOStrID,
															  IDS_FileIO119, // interleave = 
															  &stringPtr,
															  3,
															  kSkipEqual,
															  (char*)gTextString,
																0,
															  50,
															  kNoSubstringAllowed);

            if (tReturnCode == 0)
					{
							// Get the band interleave format

					if (strncmp ((char*)&gTextString[1], "bil", 3) == 0)
						fileInfoPtr->bandInterleave = kBIL;

					else if (strncmp ((char*)&gTextString[1], "bsq", 3) == 0)
						fileInfoPtr->bandInterleave = kBSQ;

					else if (strncmp ((char*)&gTextString[1], "bip", 3) == 0)
						fileInfoPtr->bandInterleave = kBIS;

					else	// not known
						errCode = -1;

					}	// end "if (tReturnCode == 0)"

            else	// tReturnCode != 0
                errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr && fileInfoPtr->numberBytes >= 2)
				{
						// Find "byte order = " in the buffer.								

            value = GetFileHeaderValue (kFileIOStrID,
													  IDS_FileIO120, // byte order =
													  headerRecordPtr,
													  1,
													  kSkipEqual,
													  &tReturnCode);

            if (tReturnCode == 0 && value >= 0)
					{
					fileInfoPtr->swapBytesFlag = !gBigEndianFlag;
					if (value == 0)
						fileInfoPtr->swapBytesFlag = gBigEndianFlag;

					}	// end "if (tReturnCode == 0)"

            else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr && fileInfoPtr->numberBytes >= 2)"

			if (errCode == noErr)
				{
						// Find "xstart = " in the buffer.								

				value = GetFileHeaderValue (kFileIOStrID,
													  IDS_FileIO121, // xstart = 
													  headerRecordPtr,
													  1,
													  kSkipEqual,
													  &tReturnCode);

            if (tReturnCode == 0 && value > 0)
					fileInfoPtr->startColumn = value;

            else	// tReturnCode != 0
					fileInfoPtr->startColumn = 1;

						// Find "ystart = " in the buffer.								

            value = GetFileHeaderValue (kFileIOStrID,
													  IDS_FileIO122, // ystart = 
													  headerRecordPtr,
													  1,
													  kSkipEqual,
													  &tReturnCode);

            if (tReturnCode == 0 && value > 0)
					fileInfoPtr->startLine = value;

            else	// tReturnCode != 0
					fileInfoPtr->startLine = 1;

						// Check if map projection information exists. 
						// Find "mapinfo = " in the buffer.	

            ReadENVIHeaderMapInfo (fileInfoPtr, headerRecordPtr);

				}	// end "if (errCode == noErr)" 

			if (errCode == noErr)
				{
            if (formatErrorCode == noErr)
					{
					returnCode = 0;

							// Determine the image type if it has not been determined yet.

					if (gGetFileImageType == 0)
						{
						gGetFileImageType = kMultispectralImageType;
						fileInfoPtr->numberClasses = 0;

						}	// end "if (gGetFileImageType == 0)"

					fileInfoPtr->format = kENVIType;
					fileInfoPtr->ancillaryInfoformat = kENVIType;

					fileInfoPtr->thematicType = FALSE;
					if (gGetFileImageType == kThematicImageType)
						fileInfoPtr->thematicType = TRUE;

					}	// end "if (formatErrorCode == noErr)"

            else	// formatErrorCode != noErr
					{
							// Display an alert if MultiSpec cannot read this type of ENVI
							//	file.						

                DisplayAlert (kErrorAlertID,
                        kStopAlert,
                        kAlertStrID,
                        IDS_Alert69 + formatErrorCode,
                        0,
                        NULL);

					}	// end "if (formatErrorCode != noErr)"

				}	// end "if (errCode == noErr)"

			}	// end "if (errCode == noErr)"

		}	// end "if (fileType != 0)"

	return (returnCode);

}	// end "ReadENVIHeader"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadENVIHeaderBandNamesInfo
//
//	Software purpose:	This routine reads the band name information in an ENVI header 
//							record.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/19/2006
//	Revised By:			Larry L. Biehl			Date: 07/19/2006

SInt16 ReadENVIHeaderBandNamesInfo (
				FileInfoPtr 						fileInfoPtr,
				char*									stringPtr)

{
	Str63									bandNamesString;

	double								xReferencePixel,
											xPixelSize,
											x11Coordinate,
											yReferencePixel,
											yPixelSize,
											y11Coordinate;

	char*									endMapInfoStringPtr = NULL;
	MapProjectionInfoPtr				mapProjectionInfoPtr = NULL;

	SInt32								gridZone,
											string;

	SInt16								returnCode,
											tReturnCode;

	SignedByte							handleStatus;


	if (stringPtr != NULL)
		{
				// Check if map projection information exists.
				// Find "mapinfo = " in the buffer.		

		tReturnCode = GetFileHeaderString (kFileIOStrID,
														IDS_FileIO159, 	// band names = {
														&stringPtr, 
														-1, 
														kDoNotSkipEqual,
														(char*)gTextString,
														0,
														64,
														kNoSubstringAllowed);

		if (tReturnCode == 0)
			{
					// Find the ending bracket "}".

			endMapInfoStringPtr = (char*)strstr (stringPtr, "}");

			}	// end "if (tReturnCode == 0)"

		if (endMapInfoStringPtr != NULL)
			{
					// Now get the first band description to verify that it exists.
					// If so then set up to load the channel descriptions.

			if (GetStringToComma (stringPtr,
										 endMapInfoStringPtr,
										 (char*)bandNamesString,
										 64) != NULL)
				{
						// Get handle to memory to store the channel information	in.
				
				fileInfoPtr->descriptionH = MNewHandle (fileInfoPtr->numberChannels *
																			sizeof (ChannelDescription));
			
				if (fileInfoPtr->descriptionH != NULL)
					channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
																				fileInfoPtr->descriptionH,
																				kLock);

				}	// end "if (GetStringToComma (stringPtr, ..."

			}	// end "if (endMapInfoStringPtr != NULL)"

		if (channelDescriptionPtr != NULL)
			{
			fileInfoPtr->descriptionsFlag = TRUE;
			maxNumberCharactersUsed = 0;

					// The map info line was found.  Now read the number of channel
					// strings that should be in this line that are separated by commas.

			for (channel=0; channel<fileInfoPtr->numberChannels; channel++)
				{
				stringPtr = GetStringToComma (stringPtr,
														 endMapInfoStringPtr,
														 (char*)bandNamesString,
														 64);

				if (stringPtr != NULL)
					{
					charPtr = (char*)&channelDescriptionPtr[channel];
					BlockMoveData ((char*)&gTextString3[1], charPtr, 24);

					}	// end "if (stringPtr != NULL)"

				else	// stringPtr == NULL
					break;

				}	// end "for (channel=0; channel<fileInfoPtr->..."

				CheckAndUnlockHandle (fileInfoPtr->descriptionH);

				}	// end "if (channelDescriptionPtr != NULL)"

		}	// end "if (stringPtr != NULL)"

	return (0);
	
}	// end "ReadENVIHeaderBandNamesInfo"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadENVIHeaderMapInfo
//
//	Software purpose:	This routine reads the file specifications from an ENVI header 
//							record.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if ENVI header record parameters make sense
//							1 - if ENVI header record paramters do not make sense.
//
// Called By:			ReadENVIHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/14/2006
//	Revised By:			Larry L. Biehl			Date: 03/01/2019

SInt16 ReadENVIHeaderMapInfo (
				FileInfoPtr							fileInfoPtr,
				char*									inputStringPtr)

{
	Str63									mapInfoStrings[10];

	double								falseEasting,
											falseNorthing,
											latitudeCenter,
											longitudeCenter,
											radiusSpheroid,
											semiMajorAxis,
											semiMinorAxis,
											xReferencePixel,
											xPixelSize,
											x11Coordinate,
											yReferencePixel,
											yPixelSize,
											y11Coordinate;

	char									*endMapInfoStringPtr = NULL,
											*stringPtr,
											*strPtr;

	MapProjectionInfoPtr				mapProjectionInfoPtr = NULL;
	
	int									gridZone;

	SInt32								string;

	SInt16								returnCode,
											tReturnCode;

	SignedByte							handleStatus;


	if (inputStringPtr != NULL)
		{
		stringPtr = inputStringPtr;

			  // Check if map projection information exists.
			  // Find "mapinfo = " in the buffer.		

		tReturnCode = GetFileHeaderString (kFileIOStrID,
														 IDS_FileIO158, // map info = {
														 &stringPtr,
														 -1,
														 kDoNotSkipEqual,
														 (char*)gTextString,
														0,
														 64,
														 kNoSubstringAllowed);

		if (tReturnCode == 0)
			{
					// Find the ending bracket "}".

			endMapInfoStringPtr = (char*)strstr (stringPtr, "}");

			}	// end "if (tReturnCode == 0)"

		if (endMapInfoStringPtr != NULL)
			{
					// The map info line was found.  Now read the 9 or 10 strings that
					// should be in this line that are separated by commas.

			for (string = 0; string < 10; string++)
				{
				stringPtr = GetStringToComma (stringPtr,
														endMapInfoStringPtr,
														(char*)mapInfoStrings[string],
														64);

				if (stringPtr == NULL)
					{
					string--;
					break;

					}	// end "if (stringPtr == NULL)"

				}	// end "for (string=0; string<10; string++)"

			if (string >= 0)
				{
				fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();

				if (fileInfoPtr->mapProjectionHandle != NULL)
					{
					mapProjectionInfoPtr = (MapProjectionInfoPtr)
													GetHandleStatusAndPointer (
																	 fileInfoPtr->mapProjectionHandle,
																	 &handleStatus);

					}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"

				}	// end "if (string >= 0)"

			}	// end "if (endMapInfoStringPtr != NULL)"

		if (mapProjectionInfoPtr != NULL)
			{
			xReferencePixel = 0;
			yReferencePixel = 0;
			x11Coordinate = 0;
			y11Coordinate = 0;
			xPixelSize = 0;
			yPixelSize = 0;

					// Get the projection code

			mapProjectionInfoPtr->gridCoordinate.projectionCode = GetProjectionNameInfo (
									&mapInfoStrings[0][1],
									&mapProjectionInfoPtr->gridCoordinate.referenceSystemCode);

					// Check if this is Pulkova datum. This was done for Martin Sima. It
					// will have to be made more general when other projections/datums are
					// supported.

			if (mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																				kTransverseMercatorCode)
				{
						// Check if "Pulkova 1942" is in the string.

				strPtr = StrStrNoCase ((char*)&mapInfoStrings[0][1], "Pulkova 1942");
				if (strPtr != NULL)
					{
					mapProjectionInfoPtr->geodetic.datumCode = kPulkovo1942DatumCode;
					mapProjectionInfoPtr->geodetic.spheroidCode = kKrassovskyEllipsoidCode;

					returnCode = sscanf ((char*)&mapInfoStrings[0][1],
												 "GK Zone %d",
												 &gridZone);

					if (returnCode == 1)
						mapProjectionInfoPtr->gridCoordinate.gridZone =
																				(SInt16)abs (gridZone);

					}	// end "if (strPtr != NULL)"

				}	// end "if (...->gridCoordinate.code == kTransverseMercatorCode)"

					// Get the reference pixel information and the pixel size.

			returnCode = 0;
			if (string >= 6)
				{
				returnCode = sscanf ((char*)&mapInfoStrings[1][1],
												"%lf",
												&xReferencePixel);

				if (returnCode == 1)
					returnCode = sscanf ((char*)&mapInfoStrings[2][1],
												"%lf",
												&yReferencePixel);

				if (returnCode == 1)
					returnCode = sscanf ((char*)&mapInfoStrings[3][1],
												"%lf",
												&x11Coordinate);

				if (returnCode == 1)
					returnCode = sscanf ((char*)&mapInfoStrings[4][1],
												"%lf",
												&y11Coordinate);

				if (returnCode == 1)
					returnCode = sscanf ((char*)&mapInfoStrings[5][1],
												"%lf",
												&xPixelSize);

				if (returnCode == 1)
					returnCode = sscanf ((char*)&mapInfoStrings[6][1],
												"%lf",
												&yPixelSize);

				}	// end "if (string >= 6)"

			if (returnCode == 1)
				{
						// Note that if the projection is Sinusoidal, then the upper left
						// coordinates are given in km. This needs to be converted to meters.
						// This logic may need to be change when more variations are found.
						// This is based on projection labeled as "Sin1km".

				if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kSinusoidalCode)
					{
					x11Coordinate *= 1000;
					y11Coordinate *= 1000;

					}	// end "...->gridCoordinate.code == kSinusoidalCode"

				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = x11Coordinate;
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = y11Coordinate;

						// Force upper left reference value to represent the center of the
						// pixel if need.  For ENVI headers, reference pixel values of 1 
						// indicate that the map values are for the upper left part of the
						// upper left pixel.  Reference values of 1.5 indicate the center 
						// of the pixel.
				/*
				if (xReferencePixel == 1)
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +=
																								xPixelSize/2;

				if (yReferencePixel == 1)
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -=
																								yPixelSize/2;
				*/
				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +=
																(1.5 - xReferencePixel) * xPixelSize;
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -=
																(1.5 - yReferencePixel) * yPixelSize;

				mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
				mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;

				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = xPixelSize;
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = yPixelSize;

				}	// end "if (returnCode == 1)"

			if (string >= 8)
				{
				if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode >=
																						kUTM_NAD27RSCode &&
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode <=
																						kUTMRSCode)
					{
					returnCode = sscanf ((char*)&mapInfoStrings[7][1],
												 "%d",
												 &gridZone);

					if (CompareStringsNoCase (
												&mapInfoStrings[8][1], (UCharPtr)"south", 5) == 0)
						gridZone = -abs (gridZone);

					if (returnCode == 1)
						mapProjectionInfoPtr->gridCoordinate.gridZone = (SInt16)gridZone;

					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;

					if (CompareStringsNoCase (
											&mapInfoStrings[9][1], (UCharPtr)"WGS-84", 6) == 0)
						mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;

					}	// end "if (...->gridCoordinate.referenceSystemCode == ..."

				else if (mapProjectionInfoPtr->gridCoordinate.projectionCode !=
																							kNotDefinedCode)
					{
					if (CompareStringsNoCase (
											&mapInfoStrings[7][1], (UCharPtr)"WGS-84", 6) == 0)
						mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;

					else if (CompareStringsNoCase (
														&mapInfoStrings[7][1],
														(UCharPtr)"North America 1927", 6) == 0)
						{
						mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
						mapProjectionInfoPtr->geodetic.spheroidCode =
																				kClarke1866EllipsoidCode;

						}	// end "else if (CompareStringsNoCase (..."

					else if (mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																							kSinusoidalCode)
						{
						mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
						mapProjectionInfoPtr->geodetic.spheroidCode = kSphereEllipsoidCode;

						}	// end "...->gridCoordinate.projectionCode == kSinusoidalCode"

					}	// end "else if (...->gridCoordinate.code != kNotDefinedCode)"

				if (xPixelSize != 0 || yPixelSize != 0)
					{
					if (CompareStringsNoCase (
									&mapInfoStrings[8][1], (UCharPtr)"units=Degrees", 13) == 0)
						mapProjectionInfoPtr->planarCoordinate.mapUnitsCode =
																					kDecimalDegreesCode;

					else if (CompareStringsNoCase (
									&mapInfoStrings[8][1], (UCharPtr)"units=Meters", 12) == 0)
						mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;

					else if (CompareStringsNoCase (
									&mapInfoStrings[8][1], (UCharPtr)"units=Feet", 10) == 0)
						mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kFeetCode;

					}	// end "if (xPixelSize != 0 || yPixelSize != 0)"

						// Now define the spheroid based on the datum

				if (mapProjectionInfoPtr->geodetic.datumCode == kWGS84Code)
					mapProjectionInfoPtr->geodetic.spheroidCode = kWGS84EllipsoidCode;

				}	// end "if (string >= 8)"

			if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																						kUserDefinedRSCode)
				{
				stringPtr = inputStringPtr;

						// Check if projection information exists.
						// Find "projection info = " in the buffer.		

				tReturnCode = GetFileHeaderString (kFileIOStrID,
																IDS_FileIO160, // projection info = {
																&stringPtr,
																-1,
																kDoNotSkipEqual,
																(char*)gTextString,
																0,
																64,
																kNoSubstringAllowed);

				if (tReturnCode == 0)
					{
							// Find the ending bracket "}".

					endMapInfoStringPtr = (char*)strstr (stringPtr, "}");

					}	// end "if (tReturnCode == 0)"

				if (endMapInfoStringPtr != NULL)
					{
							// The projection info line was found.  Now read the 10 strings
							// that should be in this line that are separated by commas.

					for (string = 0; string < 10; string++)
						{
						stringPtr = GetStringToComma (stringPtr,
																  endMapInfoStringPtr,
																  (char*)mapInfoStrings[string],
																  64);

						if (stringPtr == NULL)
							{
							string--;
							break;

							}	// end "if (stringPtr == NULL)"

						}	// end "for (string=0; string<10; string++)"

					if (string >= 7 &&
							mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																		kLambertAzimuthalEqualAreaCode)
						{
						returnCode = sscanf ((char*)&mapInfoStrings[3][1],
													  "%lf",
													  &latitudeCenter);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[4][1],
														  "%lf",
														  &longitudeCenter);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[5][1],
														  "%lf",
														  &falseNorthing);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[6][1],
														  "%lf",
														  &falseEasting);

						if (returnCode == 1)
							SetLambertAzimuthalEqualAreaParameters (mapProjectionInfoPtr,
																				  longitudeCenter,
																				  latitudeCenter,
																				  falseEasting,
																				  falseNorthing);

						}	// end "if (string >= 7 && ... == kLambertAzimuthalEqualAreaCode))"

					else if (string >= 6 &&
								mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																							kSinusoidalCode)
						{
						returnCode = sscanf ((char*)&mapInfoStrings[1][1],
													  "%lf",
													  &radiusSpheroid);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[2][1],
														  "%lf",
														  &longitudeCenter);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[3][1],
														  "%lf",
														  &falseEasting);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[4][1],
														  "%lf",
														  &falseNorthing);

						if (returnCode == 1)
							SetSinusoidalParameters (mapProjectionInfoPtr,
															  radiusSpheroid,
															  longitudeCenter,
															  0,
															  falseEasting,
															  falseNorthing);

						}	// end "else if (string >= 7 && ... == kSinusoidalCode))"

					else if (string >= 6 &&
								mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																				kPolarStereographicCode)
						{
						returnCode = sscanf ((char*)&mapInfoStrings[1][1],
														"%lf",
														&semiMajorAxis);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[2][1],
															"%lf",
															&semiMinorAxis);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[3][1],
															"%lf",
															&latitudeCenter);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[4][1],
															"%lf",
															&longitudeCenter);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[5][1],
															"%lf",
															&falseEasting);

						if (returnCode == 1)
							returnCode = sscanf ((char*)&mapInfoStrings[6][1],
															"%lf",
															&falseNorthing);

						if (returnCode == 1)
							SetPolarStereographicParameters (mapProjectionInfoPtr,
																		semiMajorAxis,
																		semiMinorAxis,
																		longitudeCenter,
																		latitudeCenter,
																		falseEasting,
																		falseNorthing);

						}	// end "else if (string >= 7 && ... == kPolarStereographicCode))"

					}	// end "if (endMapInfoStringPtr != NULL)"

				}	// end "if (...->gridCoordinate.referenceSystemCode == ..."

			MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);

			}	// end "if (mapProjectionInfoPtr != NULL)"

		}	// end "if (inputStringPtr != NULL)"

	return (0);

}	// end "ReadENVIHeaderMapInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReadENVIChannelDescriptionsAndValues
//
//	Software purpose:	The purpose of this routine is to read the ENVI channel
//							descriptions and values from the ENVI header file if they exist.
//
//	Parameters in:		Pointer to file information structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			ReadChannelDescriptionsAndValues in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2017
//	Revised By:			Larry L. Biehl			Date: 01/29/2018

void ReadENVIChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr)

{
	ChannelDescriptionPtr			channelDescriptionPtr;

	char									*charPtr,
											*metaDataStringPtr;

	float									*channelValuesPtr,
											*channelWidthsPtr;
											
	float									reflectiveThermalCrossover;

	UInt32								channel,
											count,
											unitStringLength;

	Boolean								changedFlag,
											continueFlag = TRUE;


	#if include_gdal_capability
	if (fileInfoPtr == NULL)
																								return;

	
			// Get handle to memory to store the channel information	in.

	count = fileInfoPtr->numberChannels * sizeof (ChannelDescription);
	channelDescriptionPtr = (ChannelDescriptionPtr)CheckHandleSize (
																&fileInfoPtr->channelDescriptionH,
																&continueFlag,
																&changedFlag,
																count);

	if (continueFlag)
		{
				// Get handle to memory to store the center channel (band) values,
				// channel (band) widths, and channel wavelength order.
		
		channelValuesPtr = CheckChannelValuesHandleSize (fileInfoPtr,
																			&continueFlag);
		
		if (continueFlag)
			{
			channelWidthsPtr = &channelValuesPtr[fileInfoPtr->numberChannels];
			
					// This will be flag indicating that widths are not available.
					
			*channelWidthsPtr = 0;
			
			}	// end "if (continueFlag)"

		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Get a blank string to use as the default.

		if (!MGetString (gTextString3, kFileIOStrID, IDS_BlankString16))
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)"

	if (continueFlag)
		{			
		reflectiveThermalCrossover = 3.0;
				
				// Get the units for the wavelengths if available.
					
		metaDataStringPtr = (char*)GDALGetMetadataItem (
									GDALGetRasterBand (fileInfoPtr->gdalDataSetH, 1),
									"wavelength_units",
									NULL);
		
		unitStringLength = 0;
		if (metaDataStringPtr != NULL)
			{
					// Save the wavelength unit string with the file information
					// structure in case it will be needed for graph windows.
					
			count = (UInt32)MIN (strlen (metaDataStringPtr), 15);
			memcpy ((char*)fileInfoPtr->channelDescriptionUnitString, 
						metaDataStringPtr, 
						count);
			
					// If the unit string is 'Nanometers, shorten to nm'
					
			if (CompareStringsNoCase (
								(UCharPtr)metaDataStringPtr, (UCharPtr)"nanometers", 10) == 0)
				sprintf ((char*)fileInfoPtr->channelDescriptionUnitString, "nm");
				
			unitStringLength = 
						(UInt32)strlen ((char*)fileInfoPtr->channelDescriptionUnitString);
							
			if (CompareStringsNoCase (fileInfoPtr->channelDescriptionUnitString, 
												(UCharPtr)"nm",
												2))
				reflectiveThermalCrossover = 3000.;
			
			}	// end "if (metaDataStringPtr != NULL)"

		fileInfoPtr->maxNumberDescriptionCharacters = 0;
		for (channel=1; channel<=fileInfoPtr->numberChannels; channel++)
			{
					// Fill channel channel description with blanks
			
			memcpy ((char*)channelDescriptionPtr, (char*)&gTextString3[1], 16);
			
					// Get the wavelength string if it exists for the band
			
			metaDataStringPtr = (char*)GDALGetMetadataItem (
										GDALGetRasterBand (fileInfoPtr->gdalDataSetH, channel),
										"wavelength",
										NULL);
			
			if (metaDataStringPtr != NULL)
				{
						// For now do now allow more than 10 characters. Experience is that
						// when more than 10 characters, they are just zeros.
						
				count = (UInt32)MIN (strlen (metaDataStringPtr), 10);
				memcpy ((char*)channelDescriptionPtr, metaDataStringPtr, count);
				
						// Now add any units to the end of the description if there is
						// space.
						
				if (count+1+unitStringLength < 24)
					{
					charPtr = (char*)channelDescriptionPtr;
					memcpy (&charPtr[count+1],
								fileInfoPtr->channelDescriptionUnitString,
								unitStringLength);
					
					count += 1 + unitStringLength;
								
					}	// end "if (count+1+unitStringLength < 24)"
				
				*channelValuesPtr = (float)atof (metaDataStringPtr);
				
				fileInfoPtr->descriptionsFlag = TRUE;

				fileInfoPtr->maxNumberDescriptionCharacters = MAX (
													fileInfoPtr->maxNumberDescriptionCharacters,
													(SInt16)count);
				
						// Set description code information.
						
				if (*channelValuesPtr > 0)
					{
					if (*channelValuesPtr < reflectiveThermalCrossover)
						fileInfoPtr->descriptionCode |= kReflectiveData;
				
					else	// *channelValuesPtr >= reflectiveThermalCrossover
						fileInfoPtr->descriptionCode |= kThermalData;
						
					}	// end "if (*channelValuesPtr > 0)"
				
				}	// end "if (metaDataStringPtr != NULL)"

			channelDescriptionPtr++;
			channelValuesPtr++;

			}	// end "for (channel=0; channel<fileInfoPtr->numberChannels; ..."
			
		}	// end "if (continueFlag)"

	if (fileInfoPtr->descriptionsFlag) 
		{
		CheckAndUnlockHandle (fileInfoPtr->channelDescriptionH);
		CheckAndUnlockHandle (fileInfoPtr->channelValuesHandle);

		}	// end "if (fileInfoPtr->descriptionsFlag)"

	else	// !fileInfoPtr->descriptionsFlag
		{
		fileInfoPtr->channelDescriptionH =
											UnlockAndDispose (fileInfoPtr->channelDescriptionH);
		fileInfoPtr->channelValuesHandle =
											UnlockAndDispose (fileInfoPtr->channelValuesHandle);

		}	// end "else !fileInfoPtr->descriptionsFlag"
	#endif	// include_gdal_capability

}	// end "ReadENVIChannelDescriptionsAndValues"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadERDASClassNames
//
//	Software purpose:	The purpose of this routine is to read the class
//							names from the ERDAS .trl file.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			LoadClassNameDescriptions in SOpenImage.cpp
//							LoadThematicClasses in SSaveWrite.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1989
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ReadERDASClassNames (
				FileInfoPtr							fileInfoPtr,
				Handle								windowInfoHandle,
				UCharPtr								classNamePtr)

{
	//SInt64								fileSize;

	Boolean								checkIOErrorFlag,
											returnFlag;

	CMFileStream*						supportFileStreamPtr;

	SignedByte							supportHandleStatus;

	char									*endClassNamePtr;
	//										*erdasTRLHeader = "TRAIL";

	UInt16*								classSymbolPtr;

	UInt32								classNumber,
											count,
											//filePosOffset,
											numberClasses,
											fileReadStart;

	SInt16								classNameStart,
											firstNonBlankCharacter,
											histogramStart;

	SInt32								numberCharacters;
	SInt16								errCode,
											tilde = 0x007E; // ~


			// Initialize local variables.

	checkIOErrorFlag = TRUE;
	returnFlag = FALSE;
	histogramStart = 7 * 128;
	classNameStart = 16 * 128;

	if (fileInfoPtr == NULL || windowInfoHandle == NULL || classNamePtr == NULL)
																					return (returnFlag);

			// Get file stream pointer to the support file

	supportFileStreamPtr = GetSupportFileStreamPointer (windowInfoHandle,
																			&supportHandleStatus);

			// If the support file is not open, then open a path to the file
			// that contains the class names.

	errCode = OpenFileReadOnly (supportFileStreamPtr,
											kResolveAliasChains,
											kLockFile,
											kDoNotVerifyFileStream);

	if (errCode == fnfErr || errCode == bdNamErr)
		checkIOErrorFlag = FALSE;

			// Continue if okay.

	if (errCode == noErr)
		{
				// Read the class names.  If the class name is blank or we reach
				// the end of the file before all the names have been read, fill	
				// in a default name.															

		if (errCode == noErr)
			{
			numberClasses = fileInfoPtr->numberClasses;

					// Get a pointer to storage for the class symbols.

			classSymbolPtr = (UInt16*)&classNamePtr[numberClasses * sizeof (Str31)];

			for (classNumber = 1; classNumber <= numberClasses; classNumber++)
				{
						// If everything is okay, position file to start reading in
						// the class names.

				if (errCode == noErr)
					{
					fileReadStart = classNameStart + classSymbolPtr[classNumber - 1]*32;
					errCode = MSetMarker (supportFileStreamPtr,
													fsFromStart,
													fileReadStart,
													kNoErrorMessages);
					if (errCode == eofErr)
						checkIOErrorFlag = FALSE;

					}	// end "if (errCode == noErr)"

				count = 32;
				if (errCode == noErr)
					errCode = MReadData (supportFileStreamPtr,
												&count,
												gTextString,
												kNoErrorMessages);
				if (errCode == eofErr)
					checkIOErrorFlag = FALSE;

						// Determine the number of characters in the class name.

				numberCharacters = 0;
				firstNonBlankCharacter = 0;

				if (errCode == noErr)
					{
							// Find the end of the class name.

					gTextString[32] = kNullTerminator;
					endClassNamePtr = (char*)strchr ((char*)gTextString, (int)tilde);

					if (endClassNamePtr != NULL)
						{
						numberCharacters = (SInt32)(endClassNamePtr - (char*)gTextString);
						if (numberCharacters < 0 || numberCharacters > 31)
							numberCharacters = 31;

						}	// end "if (endClassNamePtr != NULL)"


					else	// endClassNamePtr == NULL 
						numberCharacters = 31;

					if (gTextString[0] == kNullTerminator)
						numberCharacters = 0;

							// Remove any leading blanks in the class name.

					while (numberCharacters != 0 &&
													gTextString[firstNonBlankCharacter] == ' ')
						{
						firstNonBlankCharacter++;
						numberCharacters--;

						}	// end "while (numberCharacters != 0 && ..."

					}	// end "if (errCode == noErr)"

				if (numberCharacters == 0)
					{
					if (classNumber == 1)
						{
								// Class 0 is assumed to be the background class
								// Tried leaving this blank in early 2003, but it 
								// caused confusion.		

						CopyPToP (classNamePtr, (UCharPtr)"\0background\0");
						//classNamePtr[0] = 0;

						}	// end "if (classNumber == 0)"

					else	// classNumber != 1
						CreateDefaultClassName (classNamePtr, classNumber);

					}	// end "if (numberCharacters == 1)"

				else	// numberCharacters != 0
					{
					BlockMoveData (&gTextString[firstNonBlankCharacter],
											&classNamePtr[1], numberCharacters);
					classNamePtr[0] = (UInt8)numberCharacters;
					if (numberCharacters < 31)
						classNamePtr[numberCharacters + 1] = kNullTerminator;

					}	// end "else numberCharacters != 0"

				classNamePtr += 32;

						// Load the assumed class symbol if this is being read
						// from a default trl file when the image is first being
						// loaded									

				//if (fileInfoPtr->format != kMultiSpecClassificationType)
				if (!fileInfoPtr->asciiSymbols)
					classSymbolPtr[classNumber - 1] = (UInt16)(classNumber - 1);

				}	// end "for (classNumber=1; classNumber<=..."

					// Indicate that the class names have been created.

			returnFlag = TRUE;

			fileInfoPtr->ancillaryInfoformat = kErdas74Type;

			}	// end "if (errCode == noErr)"

		}	// end "if (errCode == noErr)"

	if (checkIOErrorFlag)
		IOCheck (errCode, supportFileStreamPtr);

	CloseFile (supportFileStreamPtr);

	UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);

	return (returnFlag);

}	// end "ReadERDASClassNames"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadERDASHeader
//
//	Software purpose:	This routine reads the file specifications from
//							an ERDAS header record. Note that the ERDAS header
//							record is defined for a system architecture which
//							uses little-endian byte order.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if ERDAS header record parameters make sense
//							1 - if ERDAS header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//							GetProbabilityFile in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/04/1989
//	Revised By:			Larry L. Biehl			Date: 04/08/2013

SInt16 ReadERDASHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;

	double								horizontalPixelSize,
											verticalPixelSize;

	SignedByte							handleStatus;

	SInt16								iPack,
											version;

	UInt16								tempInt;

	Boolean								returnCode = 1;


	version = 0;
	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
				//  Check if image file is in ERDAS format.

		if (MGetString (gTextString3, kFileIOStrID, IDS_Erdas73Type))
			{
			if (strncmp ((char*)headerRecordPtr, (char*)&gTextString3[1], 6) == 0)
				version = kErdas73Type;

			}	// end "if (MGetString (gTextString3, ..."

		if (version == 0)
			{
					// Check if image file is in ERDAS 74 format.

			if (MGetString (gTextString3, kFileIOStrID, IDS_Erdas74Type))
				{
				if (strncmp ((char*)headerRecordPtr, (char*)&gTextString3[1], 6) == 0)
					version = kErdas74Type;

				}	// end "else if (MGetString (gTextString3, ..."

			}	// end "if (version == 0)"

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (formatOnlyCode != kLoadHeader)
		{
		fileInfoPtr->format = version;

		if (version != 0)
			{
					// Set up swap bytes flag depending upon the system architecture

			gSwapBytesFlag = gBigEndianFlag;

			tempInt = (UInt16)GetShortIntValue ((char*)&headerRecordPtr[90]);
			fileInfoPtr->numberClasses = (UInt32)tempInt;

			fileInfoPtr->numberChannels = GetShortIntValue ((char*)&headerRecordPtr[8]);

			if (fileInfoPtr->numberClasses > 0 && fileInfoPtr->numberChannels <= 1)
				fileInfoPtr->thematicType = TRUE;

			else	// ...->numberClasses <= 0 || ...->numberChannels > 1
				fileInfoPtr->thematicType = FALSE;

			gSwapBytesFlag = FALSE;

			}	// end "if (version != 0)"

																					return (returnCode);

		}	// end "if (formatOnlyCode != kLoadHeader)"

	if (version != 0)
		{
				// Set up swap bytes flag depending upon the system rchitecture

		gSwapBytesFlag = gBigEndianFlag;

		returnCode = 0;

				// Get pack type of the data -> 4, 8, or 16 bit data

		iPack = GetShortIntValue ((char*)&headerRecordPtr[6]);
		fileInfoPtr->numberBytes = 0;
		if (iPack == 0)
			{
			fileInfoPtr->numberBytes = 1;
			fileInfoPtr->numberBits = 8;

			}	// end "if (iPack == 0)"

		else if (iPack == 1)
			{
			fileInfoPtr->numberBytes = 1;
			fileInfoPtr->numberBits = 4;

			}	// end "else if (iPack == 1)"

		else if (iPack == 2)
			{
			fileInfoPtr->numberBytes = 2;
			fileInfoPtr->numberBits = 16;

			}	// end "else if (iPack == 2)"

				// Get number of channels in the image data

		fileInfoPtr->numberChannels = GetShortIntValue ((char*)&headerRecordPtr[8]);

				// Determine if signed data

		iPack = GetShortIntValue ((char*)&headerRecordPtr[10]);
		fileInfoPtr->signedDataFlag = (iPack == 1);

		//if (fileInfoPtr->numberBytes == 4)
		//		fileInfoPtr->signedDataFlag = TRUE;

				// Get number of columns in the image data

		if (version == kErdas73Type)
			fileInfoPtr->numberColumns =
								(UInt32)ConvertATRealtoInt ((char*)&headerRecordPtr[16]);
		else if (version == kErdas74Type)
			fileInfoPtr->numberColumns =
								(UInt32)GetLongIntValue ((char*)&headerRecordPtr[16]);

				// Get number of lines in the image data

		if (version == kErdas73Type)
			fileInfoPtr->numberLines =
								(UInt32)ConvertATRealtoInt ((char*)&headerRecordPtr[20]);
		else if (version == kErdas74Type)
			fileInfoPtr->numberLines =
								(UInt32)GetLongIntValue ((char*)&headerRecordPtr[20]);

				// Get start column of the image data

		if (version == kErdas73Type)
			fileInfoPtr->startColumn =
								(UInt32)ConvertATRealtoInt ((char*)&headerRecordPtr[24]);
		else if (version == kErdas74Type)
			fileInfoPtr->startColumn =
								(UInt32)GetLongIntValue ((char*)&headerRecordPtr[24]);

				// Get start line of the image data

		if (version == kErdas73Type)
			fileInfoPtr->startLine =
								(UInt32)ConvertATRealtoInt ((char*)&headerRecordPtr[28]);
		else if (version == kErdas74Type)
			fileInfoPtr->startLine =
								(UInt32)GetLongIntValue ((char*)&headerRecordPtr[28]);

				// Get number of classes in the image data.

		tempInt = (UInt16)GetShortIntValue ((char*)&headerRecordPtr[90]);
		fileInfoPtr->numberClasses = (UInt32)tempInt;

		if (fileInfoPtr->numberClasses > 0)
			fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;

				// Check if map projection information exists.

		tempInt = GetShortIntValue ((char*)&headerRecordPtr[88]);

		horizontalPixelSize = ConvertATRealtoReal ((char*)&headerRecordPtr[120]);

		verticalPixelSize = ConvertATRealtoReal ((char*)&headerRecordPtr[124]);

		if (tempInt == 0 && (horizontalPixelSize == 0 || verticalPixelSize == 0))
			tempInt = -1;

		if (tempInt <= 20)
			{
			fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();

			if (fileInfoPtr->mapProjectionHandle != NULL)
				{
				mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandleStatusAndPointer (
																	fileInfoPtr->mapProjectionHandle,
																	&handleStatus);

				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
							GetMapProjectionCodeFromGCTPNumber (
										tempInt,
										TRUE,
										&mapProjectionInfoPtr->gridCoordinate.projectionCode);

				mapProjectionInfoPtr->planarCoordinate.areaUnitsCode =
											GetShortIntValue ((char*)&headerRecordPtr[106]);

				mapProjectionInfoPtr->planarCoordinate.pixelArea =
											ConvertATRealtoReal ((char*)&headerRecordPtr[108]);

				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 =
											ConvertATRealtoReal ((char*)&headerRecordPtr[112]);

				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 =
											ConvertATRealtoReal ((char*)&headerRecordPtr[116]);

				mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
				mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;

				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize =
																						horizontalPixelSize;

						// For now we will force this to be a positive number since
						// I believe that is what the original expectation was by
						// ERDAS. The user can always change it.                                                                   
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize =
																				fabs (verticalPixelSize);

				if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kNotDefinedCode)
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kUnknownCode;

				else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																					kStatePlaneNAD27RSCode)
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kUSSurveyFeetCode;

				else	// ...gridCoordinate.referenceSystemCode != kNotDefinedCode && ...
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;

				MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);

				}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"

			}	// end "if (tempInt <= 20)"

				// Determine the image type if it has not been determined yet.

		if (gGetFileImageType == 0)
			{
			if (fileInfoPtr->numberClasses > 0 && fileInfoPtr->numberChannels <= 1)
				{
				gGetFileImageType = kThematicImageType;
				fileInfoPtr->numberChannels = 1;
				fileInfoPtr->thematicType = TRUE;

				}	// end "if (fileInfoPtr->numberClasses > 0 && ..."

			else	// ...->numberClasses <= 0 || ...->numberChannels > 1
				{
				gGetFileImageType = kMultispectralImageType;
				fileInfoPtr->thematicType = FALSE;
				fileInfoPtr->numberClasses = 0;

				}	// end "else ...->numberClasses <= 0 || ..."

			}	// end "if (gGetFileImageType == 0)"

		else	// gGetFileImageType != 0
			{
					// If this image is being treated as a multispectral image then make
					// sure the number of classes is 0.

			if (gGetFileImageType == kMultispectralImageType)
				 fileInfoPtr->numberClasses = 0;

			}	// end "else gGetFileImageType != 0"

				// Load the rest of the information for ERDAS formatted files

		fileInfoPtr->format = version;
		fileInfoPtr->ancillaryInfoformat = version;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->bandInterleave = kBIL;
		fileInfoPtr->numberHeaderBytes = 128;
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 0;
		fileInfoPtr->numberPostLineBytes = 0;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;

		if (returnCode == noErr)
			{
					// If 2-Byte type data, check if bytes need to be swapped.
					// This is only a general check.

			if (fileInfoPtr->numberBytes == 2)
				{
				FileIOInstructions			fileIOInstructions;
				FileIOInstructionsPtr		fileIOInstructionsPtr;

				HSInt16Ptr						ioSBufferPtr,
													ioSBufferPtr2;

				HUInt16Ptr						ioBufferPtr,
													ioBufferPtr2;

				SInt32							maxValue1,
													maxValue2,
													minValue1,
													minValue2;

				UInt32							count,
													j,
													line,
													numberColumns,
													numberPotentialSwaps,
													numberSwaps;

				Boolean							swapBytes;


						// Get block of memory to use as file IO buffer for the
						// requested layer of data.		

				if (GetIOBufferPointers (&fileIOInstructions,
													NULL,
													NULL,
													fileInfoPtr,
													(HUCharPtr*)&ioBufferPtr,
													(HUCharPtr*)&ioBufferPtr,
													1,
													2 * fileInfoPtr->numberColumns,
													1,
													1,
													NULL,
													kDoNotPackData,
													kDoNotForceBISFormat,
													kDoNotForceBytes,
													gHasThreadManager,
													&fileIOInstructionsPtr))
					{
					SetUpFileIOInstructions (fileIOInstructionsPtr,
													 NULL,
													 1,
													 1,
													 1,
													 1,
													 fileInfoPtr->numberColumns,
													 1,
													 1,
													 NULL,
													 kSetSpecialBILFlagFalse);

					line = fileInfoPtr->numberLines / 2 + 1;
					numberColumns = fileInfoPtr->numberColumns;

							// Initialize some parameters that will be used when reading the
							// data.

					fileInfoPtr->bytesPer1line1chan =
							 fileInfoPtr->numberPreChannelBytes +
									fileInfoPtr->numberColumns * fileInfoPtr->numberBytes +
											fileInfoPtr->numberPostChannelBytes;

					fileInfoPtr->bytesPer1lineAllChans =
							 fileInfoPtr->numberChannels *
									fileInfoPtr->bytesPer1line1chan +
											fileInfoPtr->numberPreLineBytes +
													fileInfoPtr->numberPostLineBytes;

					SInt16 errCode = GetLine (fileIOInstructionsPtr,
														 fileInfoPtr,
														 line,
														 0,
														 1,
														 numberColumns,
														 &count,
														 (HUCharPtr)ioBufferPtr);

					if (errCode == noErr)
						{
								// Now read the line of data again but this time with the
								// bytes swapped.

						fileInfoPtr->swapBytesFlag = TRUE;
						ioBufferPtr2 = &ioBufferPtr[numberColumns];
						errCode = GetLine (fileIOInstructionsPtr,
													fileInfoPtr,
													line,
													0,
													1,
													numberColumns,
													&count,
													(HUCharPtr)ioBufferPtr2);

						}	// end "if (errCode == noErr)"

					if (errCode == noErr)
						{
						swapBytes = FALSE;
						j = 0;

						minValue1 = LONG_MAX;
						minValue2 = LONG_MAX;
						maxValue1 = LONG_MIN;
						maxValue2 = LONG_MIN;

						numberSwaps = 0;
						numberPotentialSwaps = 0;

								// Check if all of the non-swapped data are less than the
								// swapped data. If so then do not swap the bytes.

						if (fileInfoPtr->signedDataFlag)
							{
							ioSBufferPtr = (HSInt16Ptr)ioBufferPtr;
							ioSBufferPtr2 = (HSInt16Ptr)ioBufferPtr2;
							do
								{
								if (ioSBufferPtr[j] != 0)
									{
									minValue1 = MIN (minValue1, ioSBufferPtr[j]);
									maxValue1 = MAX (maxValue1, ioSBufferPtr[j]);

									minValue2 = MIN (minValue2, ioSBufferPtr2[j]);
									maxValue2 = MAX (maxValue2, ioSBufferPtr2[j]);

									numberPotentialSwaps++;

									if (ioSBufferPtr[j] >= 0)
										{
										if (ioSBufferPtr[j] > ioSBufferPtr2[j])
											numberSwaps++;

										}	// end "if (ioSBufferPtr[j] >= 0)"

									else	// ioSBufferPtr[j] < 0
										{
										if (ioSBufferPtr[j] < ioSBufferPtr2[j])
											numberSwaps++;

										}	// end "else ioSBufferPtr[j] < 0"

									}	// end "if (ioSBufferPtr[j] != 0)"

								j++;

								}	while (j < numberColumns);

							}	// end "if (fileInfoPtr->signedDataFlag)"

						else	// !fileInfoPtr->signedDataFlag
							{
							do
								{
								if (ioBufferPtr[j] != 0)
									{
									minValue1 = MIN (minValue1, ioBufferPtr[j]);
									maxValue1 = MAX (maxValue1, ioBufferPtr[j]);

									minValue2 = MIN (minValue2, ioBufferPtr2[j]);
									maxValue2 = MAX (maxValue2, ioBufferPtr2[j]);

									numberPotentialSwaps++;
									if (ioBufferPtr[j] > ioBufferPtr2[j])
										numberSwaps++;

									}	// end "if (ioBufferPtr[j] != 0)"

								j++;

								}	while (j < numberColumns);

							}	// end "else !fileInfoPtr->signedDataFlag"

						if (numberSwaps <= numberPotentialSwaps / 2)
							fileInfoPtr->swapBytesFlag = FALSE;

						if (numberPotentialSwaps > 0)
							{
							if (maxValue1 - minValue1 < maxValue2 - minValue2)
								fileInfoPtr->swapBytesFlag = FALSE;

							}	// end "if (numberPotentialSwaps > 0)"

						}	// end "if (errCode == noErr)"

							// Close up any File IO Instructions structure that pertain to
							// the specific area used.

					CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

							// Dispose of the IO buffer.

					DisposeIOBufferPointers (fileIOInstructionsPtr,
													 (HUCharPtr*)&ioBufferPtr,
													 (HUCharPtr*)&ioBufferPtr);

					if (errCode != noErr)
						returnCode = 1;

					}	// end "if (ioBufferPtr != NULL)"/

				}	// end "if (fileInfoPtr->numberBytes == 2)"

			}	// end "if (returnCode == noErr)"

		gSwapBytesFlag = FALSE;

		}	// end "if (version != 0 && !formatOnlyFlag)"

	return (returnCode);

}	// end "ReadERDASHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadFastL7AHeader
//
//	Software purpose:	This routine reads the file specifications from a FAST-L7A
//							header record.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if FAST-L7A header record parameters make sense
//							1 - if FAST-L7A header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/10/1999
//	Revised By:			Larry L. Biehl			Date: 01/17/2019

SInt16 ReadFastL7AHeader (
				FileInfoPtr							fileInfoPtr,
				SInt16*								versionPtr,
				UInt32								fileNumber,
				SInt16								formatOnlyCode)

{
	CMFileStream						headerFileStream;

	double								lowerRightX,
											lowerRightY,
											realValue,
											upperLeftX,
											upperLeftY;

	CharPtr								headerSuffixName1 = (char*)"\0HPN.FST\0",
											headerSuffixName2 = (char*)"\0HRF.FST\0",
											headerSuffixName3 = (char*)"\0HTM.FST\0",
											ioBufferPtr,
											ioRecordBufferPtr,
											strPtr;

	CMFileStream						*fileStreamPtr,
											*headerFileStreamPtr;

	MapProjectionInfoPtr				mapProjectionInfoPtr = NULL;

	FileStringPtr						headerFileNameCPtr,
											headerFilePathPPtr,
											imageFileNameCPtr,
											imageFilePathPPtr;

	StringPtr							headerSuffixNamePtr[3];
	
	UInt16								pathLength;

	SInt32								value;

	UInt32								count,
											headerSet;

	SInt16								errCode,
											fileType,
											returnCode = 1,
											tReturnCode;

	Boolean								foundFlag;

	SignedByte							handleStatus;

	char									direction = ' ';


	errCode = noErr;
	fileType = 0;
	returnCode = 1;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

	if (fileStreamPtr != NULL)
		{
				// Determine if this is a FAST-L7A file.

		FileStringPtr		filePathPtr;
		UInt16				numSuffixChars;

		*versionPtr = 1;
		filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);

		if (CompareSuffixNoCase ((char*)"\0_HRF.FST",
											filePathPtr,
											&numSuffixChars))
			fileType = kFastL7AType;

		else if (CompareSuffixNoCase ((char*)"\0_HTM.FST",
												filePathPtr,
												&numSuffixChars))
			fileType = kFastL7AType;

		else if (CompareSuffixNoCase ((char*)"\0_HPN.FST",
												filePathPtr,
												&numSuffixChars))
			fileType = kFastL7AType;

		else if (CompareSuffixNoCase ((char*)"\0_B??.FST",
												filePathPtr,
												&numSuffixChars))
			{
			fileType = kFastL7AType;
			*versionPtr = 2;

			}	// end "else if (CompareSuffixNoCase ((char*)"\0_B??.FST", ..."

		}	// end "if (fileStreamPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
			return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

			  // Get a buffer to read in the administrative record, radiometric
			  // and geometric records. At least 3 * 1536 bytes

	  ioBufferPtr = (char*)MNewPointer (4700);
	  if (ioBufferPtr == NULL)
			return (1);

		if (*versionPtr == 1)
			{
					// The Fast-L7A header file was selected; read the administrative
					// record in the header.
					// Make certain that we are at the beginning of the file.

			errCode = MSetMarker (fileStreamPtr, fsFromStart, 0, kErrorMessages);

					// Read administrative record.

			count = 3 * 1536;
			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr, &count, ioBufferPtr, kErrorMessages);

			if (errCode == noErr)
				{
						// Make sure that there is a c string terminator at the end of
						// the administrative record to force searches to end there now.
						// Also put a c string terminator at the end of that which was read
						// in.

				ioBufferPtr[1535] = 0;
				ioBufferPtr[count] = 0;
				
						// Save the path length
				
				pathLength = 0;
				#if defined multispec_wx || defined multispec_win
					pathLength = fileStreamPtr->GetFileUTF8PathLength ();
				#endif

						// The header file has been read.  Close the header file.

				imageFilePathPPtr =
							(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
				CloseFile (fileStreamPtr);

						// Get the name of the first band file in the list.
						// Find "FILENAME =" in the buffer.

				strPtr = ioBufferPtr;
				tReturnCode = GetFileHeaderString (kFileIOStrID,
																IDS_FileIO112, // FILENAME =
																&strPtr,
																29,
																kDoNotSkipEqual,
																(char*)imageFilePathPPtr,
																pathLength,
																254,
																kNoSubstringAllowed);

				if (tReturnCode == 0)
					{
							// Locate the image file.

					errCode = OpenFileReadOnly (fileStreamPtr,
														 kResolveAliasChains,
														 kLockFile,
														 kVerifyFileStream);

					}	// end "if (tReturnCode == 0)"

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			}	// end "if (*versionPtr == 1)"

		else	// *versionPtr == 2
			{
					// The Fast-L7A image file was selected; get the header file for
					// this image.

			headerFileStreamPtr = &headerFileStream;
			InitializeFileStream (headerFileStreamPtr, fileStreamPtr);

					// Force the header file name to always have 1 as the 3rd
					// character. It could be a 2 in the image file name.

			headerFileNameCPtr =
					(FileStringPtr)GetFileNameCPointerFromFileStream (headerFileStreamPtr);
			headerFileNameCPtr[2] = '1';

			imageFileNameCPtr =
					(FileStringPtr)GetFileNameCPointerFromFileStream (fileStreamPtr);
			headerFilePathPPtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (headerFileStreamPtr);

			headerSuffixNamePtr[0] = (StringPtr)headerSuffixName1;
			headerSuffixNamePtr[1] = (StringPtr)headerSuffixName2;
			headerSuffixNamePtr[2] = (StringPtr)headerSuffixName3;

			foundFlag = FALSE;
			headerSet = 0;
			while (!foundFlag && headerSet < 3 && errCode == noErr)
				{
				RemoveCharsNoCase ((char*)"\0???.FST\0", headerFilePathPPtr);
				ConcatFilenameSuffix (headerFilePathPPtr, headerSuffixNamePtr[headerSet]);

				errCode = OpenFileReadOnly (headerFileStreamPtr,
														kResolveAliasChains,
														kLockFile,
														kVerifyFileStream);

				if (errCode == noErr)
					{
					count = 3 * 1536;
					errCode = MReadData (headerFileStreamPtr,
												 &count,
												 ioBufferPtr,
												 kErrorMessages);

							// Make sure that there is a c string terminator at the end of
							// the administrative record to force searches to end there now.
							// Also put a c string terminator at the end of that which was
							// read in.

					if (errCode == noErr)
						{
						ioBufferPtr[1535] = 0;
						ioBufferPtr[count] = 0;

						}	// end "if (errCode == noErr)"

					CloseFile (headerFileStreamPtr);

					}	// end "if (errCode == noErr)"

				if (errCode == noErr)
					{
							// Now find the image file name in this administrative record.
							// Skip the first 3 characters in the name because the 3rd
							// character may be 1 or 2. Also remember that
							// 'imageFileNameCPtr' is a c string.

					strPtr = strstr (ioBufferPtr, (char*)&imageFileNameCPtr[3]);
					if (strPtr != NULL)
						foundFlag = TRUE;

					}	// end "if (errCode == noErr)"

				if (errCode == fnfErr)
					errCode = noErr;

				headerSet++;

				if (!foundFlag && headerSet >= 3 &&
							headerFileNameCPtr[2] == '1' && errCode == noErr)
					{
							// Try with a 2 at character position 2.

					headerFileNameCPtr[2] = '2';
					headerSet = 0;

					}	// end "if (!foundFlag && headerSet >= 3 && ..."

				}	// end "while (!foundFlag && headerSet<3 && errCode == noErr)"

			if (!foundFlag)
				errCode = -1;

			if (foundFlag && fileNumber > 1)
				{
						// Save the path length
				
				pathLength = 0;
				#if defined multispec_wx || defined multispec_win
					pathLength = fileStreamPtr->GetFileUTF8PathLength ();
				#endif
				
						// Get the name of the specified file in the list.
						// Find "FILENAME =" in the buffer.

				imageFilePathPPtr =
							(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);

				for (count = 2; count <= fileNumber; count++)
					{
					tReturnCode = GetFileHeaderString (kFileIOStrID,
																	 IDS_FileIO112, // FILENAME =
																	 &strPtr,
																	 29,
																	 kDoNotSkipEqual,
																	 (char*)imageFilePathPPtr,
																	 pathLength,
																	 254,
																	 kSubstringAllowed);

					if (tReturnCode != noErr)
						errCode = tReturnCode;

					else if (count == fileNumber)
						{
								// Locate the image file.

						errCode = OpenFileReadOnly (fileStreamPtr,
															  kResolveAliasChains,
															  kLockFile,
															  kVerifyFileStream);

						}	// end "if (tReturnCode == noErr)"

					if (errCode != noErr)
						break;

					}	// end "for (count=2; count<=fileNumber; count++)"

				}	// end "if (foundFlag && fileNumber > 1)"

			}	// end "else *versionPtr == 2"

		if (errCode == noErr)
			{
					// Find "PIXELS PER LINE =" in the buffer.

			value = GetFileHeaderValue (kFileIOStrID,
												  IDS_FileIO108, // PIXELS PER LINE =
												  ioBufferPtr,
												  1,
												  kDoNotSkipEqual,
												  &tReturnCode);

			if (tReturnCode == 0)
				fileInfoPtr->numberColumns = value;

			else	// tReturnCode != 0
				errCode = tReturnCode;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Find "LINES PER BAND =" in the buffer.

			value = GetFileHeaderValue (kFileIOStrID,
												  IDS_FileIO109, // LINES PER BAND =
												  ioBufferPtr,
												  1,
												  kDoNotSkipEqual,
												  &tReturnCode);

			if (tReturnCode == 0)
				fileInfoPtr->numberLines = value;

			else	// tReturnCode != 0
				errCode = tReturnCode;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Find "OUTPUT BITS PER PIXEL =" in the buffer.

			value = GetFileHeaderValue (kFileIOStrID,
												  IDS_FileIO110, // OUTPUT BITS PER PIXEL =
												  ioBufferPtr,
												  1,
												  kDoNotSkipEqual,
												  &tReturnCode);

			if (tReturnCode == 0)
				{
				fileInfoPtr->numberBits = (UInt16)value;

				if (fileInfoPtr->numberBits <= 8)
					fileInfoPtr->numberBytes = 1;

				else	// fileInfoPtr->numberBits > 8
					fileInfoPtr->numberBytes = 2;

				}	// end "if (tReturnCode == 0)"

			else	// tReturnCode != 0
				errCode = tReturnCode;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Find "BANDS PRESENT =" in the buffer.

			strPtr = ioBufferPtr;
			tReturnCode = GetFileHeaderString (kFileIOStrID,
														  IDS_FileIO111, // BANDS PRESENT =
														  &strPtr,
														  0,
														  kDoNotSkipEqual,
														  (char*)gTextString,
															0,
														  50,
														  kNoSubstringAllowed);

			if (tReturnCode == 0)
				fileInfoPtr->numberChannels = 1;

			else	// tReturnCode != 0
				errCode = tReturnCode;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
			fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();

			if (fileInfoPtr->mapProjectionHandle != NULL)
				{
				mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandleStatusAndPointer (
																	fileInfoPtr->mapProjectionHandle,
																	&handleStatus);

				}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"

			}	// end "if (errCode == noErr)"

		if (mapProjectionInfoPtr != NULL)
			{
			ioRecordBufferPtr = &ioBufferPtr[2 * 1536];

			if (errCode == noErr)
				{
						// Find "PROJECTION =" in the buffer.

				strPtr = ioRecordBufferPtr;
				tReturnCode = GetFileHeaderString (kFileIOStrID,
																IDS_FileIO133, // PROJECTION =
																&strPtr,
																4,
																kDoNotSkipEqual,
																(char*)gTextString,
																0,
																50,
																kNoSubstringAllowed);

				if (tReturnCode == 0)
					{
							// Get the projection code

					mapProjectionInfoPtr->gridCoordinate.projectionCode =
							GetProjectionNameInfo (
									&gTextString[1],
									&mapProjectionInfoPtr->gridCoordinate.referenceSystemCode);

					if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																				kStatePlaneNAD27RSCode)
						mapProjectionInfoPtr->planarCoordinate.mapUnitsCode =
																				kUSSurveyFeetCode;

					else	// ...referenceSystemCode != kStatePlaneNAD27RSCode
						mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;

					}	// end "if (tReturnCode == 0)"

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "PIXEL SIZE =" in the buffer.

				realValue = GetFileHeaderRealValue (kFileIOStrID,
																IDS_FileIO134, // PIXEL SIZE =
																ioBufferPtr,
																1,
																kDoNotSkipEqual,
																&tReturnCode);

				if (tReturnCode == 0)
					{
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = realValue;
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = realValue;

					}	// end "if (tReturnCode == 0)"

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "ELLIPSOID =" in the buffer.

				strPtr = ioRecordBufferPtr;
				tReturnCode = GetFileHeaderString (kFileIOStrID,
																IDS_FileIO135, // ELLIPSOID =
																&strPtr,
																7,
																kDoNotSkipEqual,
																(char*)gTextString,
																0,
																50,
																kNoSubstringAllowed);

				if (tReturnCode == 0)
					{
							// Get the ellipsoid name

					if (strncmp ((char*)&gTextString[1], "GRS_80", 6) == 0)
						mapProjectionInfoPtr->geodetic.spheroidCode = kGRS80EllipsoidCode;

					}	// end "if (tReturnCode == 0)"

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "DATUM =" in the buffer.

				strPtr = ioRecordBufferPtr;
				errCode = GetDatumInfo (&strPtr,
												&mapProjectionInfoPtr->geodetic.datumCode,
												IDS_FileIO140,
												kSkipEqual);

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "ZONE =" in the buffer.

				value = GetFileHeaderValue (kFileIOStrID,
														IDS_FileIO136, // ZONE =
														ioRecordBufferPtr,
														1,
														kDoNotSkipEqual,
														&tReturnCode);

				if (tReturnCode == 0)
					mapProjectionInfoPtr->gridCoordinate.gridZone = (SInt16)value;

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Find "UL =" in the buffer.

				tReturnCode = GetFileHeaderString (kFileIOStrID,
																IDS_FileIO137, // UL =
																&strPtr,
																-1,
																kDoNotSkipEqual,
																(char*)gTextString,
																0,
																50,
																kNoSubstringAllowed);

				if (tReturnCode == 0)
					{
							// Determine whether this image is from the northern
							// or southern hemisphere. It will be used for UTM zone
							// defination if that is the projection.

					if (strPtr[25] == 'N')
						direction = 'N';

					else if (strPtr[25] == 'S')
						direction = 'S';

							// Now get the values 3rd and 4th values in the line.

					strPtr += 26;
					tReturnCode = sscanf (strPtr, "%lf", &realValue);
					if (tReturnCode == 1)
						upperLeftX = realValue;

					strPtr += 14;
					tReturnCode = sscanf (strPtr, "%lf", &realValue);
					if (tReturnCode == 1)
						upperLeftY = realValue;

					}	// end "if (tReturnCode == 0)"

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)" 

			if (errCode == noErr)
				{
						// Find "LL =" in the buffer.

				tReturnCode = GetFileHeaderString (kFileIOStrID,
																IDS_FileIO139, // LR =
																&strPtr,
																-1,
																kDoNotSkipEqual,
																(char*)gTextString,
																0,
																50,
																kNoSubstringAllowed);

				if (tReturnCode == 0)
					{
							// Now get the values 3rd and 4th values in the line.

					strPtr += 26;
					tReturnCode = sscanf (strPtr, "%lf", &realValue);
					if (tReturnCode == 1)
						lowerRightX = realValue;

					strPtr += 14;
					tReturnCode = sscanf (strPtr, "%lf", &realValue);
					if (tReturnCode == 1)
						lowerRightY = realValue;

					}	// end "if (tReturnCode == 0)"

				else	// tReturnCode != 0
					errCode = tReturnCode;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Now set the actual sampled pixel width and the upper left
						// pixel center coordinates.

				//mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize =
				//					(lowerRightX - upperLeftX)/(fileInfoPtr->numberColumns-1);

				//mapProjectionInfoPtr->planarCoordinate.verticalPixelSize =
				//					(lowerRightY - upperLeftY)/(fileInfoPtr->numberLines-1);

				if (lowerRightX < upperLeftX)
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize =
						-fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);

				if (lowerRightY > upperLeftY)
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize =
						-fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);

				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = upperLeftX;
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = upperLeftY;

				mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
				mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;

						//	The direction is taken care of by the sign of the
						// yMapCoordinate11. If positive then N, if negative then S. 				/*

				}	// end "if (errCode == noErr)"

			MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);

			}	// end "if (mapProjectionInfoPtr != NULL)"

		if (errCode == noErr)
			{
			returnCode = noErr;

					// Determine the image type if it has not been determined yet.

			if (gGetFileImageType == 0)
				{
				gGetFileImageType = kMultispectralImageType;
				fileInfoPtr->numberClasses = 0;

				}	// end "if (gGetFileImageType == 0)"

			fileInfoPtr->format = kFastL7AType;
			fileInfoPtr->ancillaryInfoformat = kFastL7AType;
			fileInfoPtr->instrumentCode = kLandsatTM7;

			fileInfoPtr->thematicType = FALSE;
			if (gGetFileImageType == kThematicImageType)
				fileInfoPtr->thematicType = TRUE;

			}	// end "if (returnCode == noErr)"

		CheckAndDisposePtr (ioBufferPtr);

		}	// end "if (fileInfoPtr != NULL)"

	return (returnCode);

}	// end "ReadFastL7AHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadLARSYSMISTHeader
//
//	Software purpose:	The purpose of this routine is to read the LARSYS MIST header. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned: 	None	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1992
//	Revised By:			Larry L. Biehl			Date: 12/05/2011

SInt16 ReadLARSYSMISTHeader (
				FileInfoPtr							fileInfoPtr,
				SInt32*								headerRecordPtr,
				SInt16								formatOnlyCode)

{
	SInt32								longHeaderRecord[20];

	SInt32								highOrder2BytesCheck;

	SInt16								fileType = 0;

	Boolean								returnCode = 1;


	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
		highOrder2BytesCheck = 0xffff0000;
		//if (!gBigEndianFlag)
		//		highOrder2BytesCheck = 0x0000ffff;

				// Check if image file is a LARSYS multispectral formatted
				// image file (MIST).
				// If bytes 12 thru 15 of the header are 0, then the file
				// may be a MIST file.
				// long words 4, 5, & 19 should only have values in the 
				// last two bytes.		

		BlockMoveData ((char*)headerRecordPtr, (char*)&longHeaderRecord[0], 80);

		if (!gBigEndianFlag)
			{
			Swap4Bytes ((HUInt32Ptr)&longHeaderRecord[0], 1);
			Swap4Bytes ((HUInt32Ptr)&longHeaderRecord[1], 1);
			Swap4Bytes ((HUInt32Ptr)&longHeaderRecord[3], 1);
			Swap4Bytes ((HUInt32Ptr)&longHeaderRecord[4], 1);
			Swap4Bytes ((HUInt32Ptr)&longHeaderRecord[5], 1);
			Swap4Bytes ((HUInt32Ptr)&longHeaderRecord[19], 1);

			}	// end "if (!gBigEndianFlag)"

		if (longHeaderRecord[3] == 0 &&
				 longHeaderRecord[4] > 0 &&
				 longHeaderRecord[5] > 0 &&
				 longHeaderRecord[19] > 0 &&
				 !(highOrder2BytesCheck & longHeaderRecord[4]) &&
				 !(highOrder2BytesCheck & longHeaderRecord[5]) &&
				 !(highOrder2BytesCheck & longHeaderRecord[19]))
			fileType = kLARSYSMISTType;

				// Now do some further checks.

		if (fileType != 0)
			{
					// The first two bytes of ID1 (tape number) should be 0.

			if (highOrder2BytesCheck & longHeaderRecord[0])
				fileType = 0;

				 // The first two bytes of ID2 (file number) should be 0.					

			else if (highOrder2BytesCheck & longHeaderRecord[1])
				fileType = 0;

				 // ID4 (continuation code) should be 0.										

			else if (0xffffffff & longHeaderRecord[3])
				fileType = 0;

				 // The first two bytes of ID5 (number of channels) should be 0.		

			else if (highOrder2BytesCheck & longHeaderRecord[4])
				fileType = 0;

				 // The first two bytes of ID6 (number of samples) should be 0.			

			else if (highOrder2BytesCheck & longHeaderRecord[5])
				fileType = 0;

				 // The first two bytes of ID20 (number of lines) should be 0.			

			else if (highOrder2BytesCheck & longHeaderRecord[19])
				fileType = 0;

			}	// end "if (fileType != 0)"

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																							return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

		if (gGetFileImageType == 0)
			gGetFileImageType = kMultispectralImageType;

				// Set up swap bytes flag depending upon the system architecture

		gSwapBytesFlag = !gBigEndianFlag;

		returnCode = noErr;

				// Get number of channels in the multispectral image.

		fileInfoPtr->numberChannels =
				 (UInt16)GetLongIntValue ((char*)&headerRecordPtr[4]);

				// Get number of columns in the multispectral image file.

		fileInfoPtr->numberColumns =
				 GetLongIntValue ((char*)&headerRecordPtr[5]) - 6;

				// Get number of lines in the multispectral image file.

		fileInfoPtr->numberLines =
				 GetLongIntValue ((char*)&headerRecordPtr[19]);

				// Load the rest of the information for LARSYS MIST formatted file.

		fileInfoPtr->numberHeaderBytes = 800;
		fileInfoPtr->numberBytes = 1;
		fileInfoPtr->numberBits = 8;
		fileInfoPtr->numberBins = 256;
		fileInfoPtr->format = kLARSYSMISTType;
		fileInfoPtr->ancillaryInfoformat = kLARSYSMISTType;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->bandInterleave = kBIL;
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 4;
		fileInfoPtr->numberPostLineBytes = 0;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 6;
		fileInfoPtr->startColumn = 1;
		fileInfoPtr->startLine = 1;

		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			fileInfoPtr->thematicType = TRUE;

		gSwapBytesFlag = FALSE;

		}	// end "if (fileType != 0)"

	return (returnCode);

}	// end "ReadLARSYSMISTHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadLASHeader
//
//	Software purpose:	The purpose of this routine is to read the MacSADIE header.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1995
//	Revised By:			Larry L. Biehl			Date: 04/15/2020

SInt16 ReadLASHeader (
				FileInfoPtr							fileInfoPtr,
				UCharPtr								headerRecordPtr,
				SInt16								formatOnlyCode)

{
	ldiv_t								lDivideStruct;

   CharPtr								IMAGCode = (char*)"IMAG";

   CMFileStream						ddrFileStream;

   MapProjectionInfoPtr				mapProjectionInfoPtr;

   FileStringPtr						fileNamePtr;

   CMFileStream						*fileStreamPtr,
											*fileStreamddrPtr;

	double								lineInc,
											projectionCoeffiecients[15],
											sampleInc,
											upperLeftX,
											upperLeftY,
											xWidth,
											yWidth;

	UInt32								count;

	SInt32								cornerCoordinatesValidCode,
											groundUnitsValidCode,
											groundDistanceValidCode,
											incrementValidCode,
											projCode,
											projCoefValidCode,
											projectionValidCode,
											spheroidCode,
											spheroidValidCode,
											zoneCode,
											zoneValidCode;

	SInt16								errCode,
											fileType = 0,
											returnCode = 1;

	UInt16								numSuffixChars;

	SignedByte							handleStatus;


   if (headerRecordPtr != NULL && fileInfoPtr != NULL) 
		{
      fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

				// Get the file name in pascal format.
				// Note that the file name will be used in CompareSuffixNoCase which is
				// expecting a FileStringPtr with the first two elements reserved for the
				// file length.

      GetCopyOfPFileNameFromFileStream (fileStreamPtr, &gTextString[1]);
      int stringLength = gTextString[1];
      fileNamePtr = (FileStringPtr)gTextString;
      SetFileStringLength (fileNamePtr, stringLength);

				// Check if file is the LAS header file.  File suffix of
				// .ddr

      if (CompareSuffixNoCase ((char*)"\0.ddr\0", fileNamePtr, &numSuffixChars))
			fileType = kLASType1;

				// File is not ddr file; check if file is a LAS file.  File 
				// suffix of .img

      if (fileType == 0 && CompareSuffixNoCase (
                (char*)"\0.img\0", fileNamePtr, &numSuffixChars))
			fileType = kLASType2;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (fileType != 0) 
		{
      if (formatOnlyCode != kLoadHeader) 
			{
         fileInfoPtr->format = fileType;
         fileInfoPtr->thematicType = FALSE;
																							return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

      errCode = noErr;
      fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

				// Set up swap bytes flag depending upon the system architecture

      gSwapBytesFlag = !gBigEndianFlag;

      if (fileType == kLASType1) 
			{
					// The header file has been read.  Close the header file.

			fileNamePtr =
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
			CloseFile (fileStreamPtr);

					// Locate the image file.

			RemoveCharsNoCase ((char*)"\0.ddr\0", fileNamePtr);
			ConcatFilenameSuffix (fileNamePtr, (StringPtr)"\0.img\0");

			errCode = OpenFileReadOnly (fileStreamPtr,
												  kResolveAliasChains,
												  kLockFile,
												  kVerifyFileStream);

			if (errCode != fnfErr && errCode != bdNamErr)
				IOCheck (errCode, fileStreamPtr);

			//if (errCode != noErr)
			//																		return (returnCode);

			}	// end "if (fileType == kLASType1)"

		if (errCode == noErr && fileType == kLASType2)
			{
					// The LAS image file was selected; get the header	file
					// for this LAS image file.

			fileStreamddrPtr = &ddrFileStream;
			InitializeFileStream (fileStreamddrPtr, fileStreamPtr);

			fileNamePtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamddrPtr);

			RemoveCharsNoCase ((char*)"\0.img\0", fileNamePtr);
			ConcatFilenameSuffix (fileNamePtr, (StringPtr)"\0.ddr\0");

			errCode = OpenFileReadOnly (fileStreamddrPtr,
												  kResolveAliasChains,
												  kLockFile,
												  kVerifyFileStream);

			if (errCode == noErr)
				{
				count = 415;
				errCode = MReadData (fileStreamddrPtr,
											&count,
											headerRecordPtr,
											kErrorMessages);

				CloseFile (fileStreamddrPtr);

				}	// end "if (errCode == noErr)"

			if (errCode != fnfErr && errCode != bdNamErr)
				IOCheck (errCode, fileStreamPtr);

			//if (errCode != noErr)
			//																		return (returnCode);

			}	// end "if (fileType == kLASType2)"

		if (errCode == noErr)
			{
			if (gGetFileImageType == 0)
				 gGetFileImageType = kMultispectralImageType;

					// Get the number of lines in the image.

			fileInfoPtr->numberLines = GetLongIntValue ((char*)&headerRecordPtr[79]);


					// Get the number of columns in the image.

			fileInfoPtr->numberColumns = GetLongIntValue ((char*)&headerRecordPtr[83]);

					// Get number of channels in the multispectral image.

			fileInfoPtr->numberChannels =
										(UInt16)GetLongIntValue ((char*)&headerRecordPtr[87]);

					// Get number of bytes per data value.

			fileInfoPtr->numberBytes =
										(UInt16)GetLongIntValue ((char*)&headerRecordPtr[91]);

					// Get the start line number

			fileInfoPtr->startLine = GetLongIntValue ((char*)&headerRecordPtr[95]);

					// Get the start column number

			fileInfoPtr->startColumn = GetLongIntValue ((char*)&headerRecordPtr[99]);

					// Get the valid flags

			projectionValidCode = GetLongIntValue ((char*)&headerRecordPtr[103]);
			zoneValidCode = GetLongIntValue ((char*)&headerRecordPtr[107]);
			spheroidValidCode = GetLongIntValue ((char*)&headerRecordPtr[111]);
			projCoefValidCode = GetLongIntValue ((char*)&headerRecordPtr[115]);
			groundUnitsValidCode = GetLongIntValue ((char*)&headerRecordPtr[119]);
			groundDistanceValidCode = GetLongIntValue ((char*)&headerRecordPtr[123]);
			cornerCoordinatesValidCode = GetLongIntValue ((char*)&headerRecordPtr[127]);
			incrementValidCode = GetLongIntValue ((char*)&headerRecordPtr[131]);

					// Get the GCTP projection codes

			projCode = GetLongIntValue ((char*)&headerRecordPtr[135]);
			zoneCode = GetLongIntValue ((char*)&headerRecordPtr[139]);
			spheroidCode = GetLongIntValue ((char*)&headerRecordPtr[143]);

					// Get the projection parameters

			projectionCoeffiecients[0] = GetDoubleValue (&headerRecordPtr[199]);
			projectionCoeffiecients[1] = GetDoubleValue (&headerRecordPtr[207]);
			projectionCoeffiecients[2] = GetDoubleValue (&headerRecordPtr[215]);
			projectionCoeffiecients[3] = GetDoubleValue (&headerRecordPtr[223]);
			projectionCoeffiecients[4] = GetDoubleValue (&headerRecordPtr[231]);
			projectionCoeffiecients[5] = GetDoubleValue (&headerRecordPtr[239]);
			projectionCoeffiecients[6] = GetDoubleValue (&headerRecordPtr[247]);
			projectionCoeffiecients[7] = GetDoubleValue (&headerRecordPtr[255]);
			projectionCoeffiecients[8] = GetDoubleValue (&headerRecordPtr[263]);
			projectionCoeffiecients[9] = GetDoubleValue (&headerRecordPtr[271]);
			projectionCoeffiecients[10] = GetDoubleValue (&headerRecordPtr[279]);
			projectionCoeffiecients[11] = GetDoubleValue (&headerRecordPtr[287]);
			projectionCoeffiecients[12] = GetDoubleValue (&headerRecordPtr[295]);
			projectionCoeffiecients[13] = GetDoubleValue (&headerRecordPtr[303]);
			projectionCoeffiecients[14] = GetDoubleValue (&headerRecordPtr[311]);

					// Get the upper left coordinate values

			upperLeftY = GetDoubleValue (&headerRecordPtr[303]);
			upperLeftX = GetDoubleValue (&headerRecordPtr[311]);
			/*		
			lineInc = GetDoubleValue ((char*)&headerRecordPtr[319]);
			sampleInc = GetDoubleValue ((char*)&headerRecordPtr[327]);					

			lineInc = GetDoubleValue ((char*)&headerRecordPtr[335]);
			sampleInc = GetDoubleValue ((char*)&headerRecordPtr[343]);					

			lineInc = GetDoubleValue ((char*)&headerRecordPtr[351]);
			sampleInc = GetDoubleValue ((char*)&headerRecordPtr[359]);
			*/
					// Get the pixel widths

			yWidth = GetDoubleValue (&headerRecordPtr[367]);
			xWidth = GetDoubleValue (&headerRecordPtr[375]);

					// Get the line/column increments used to create the image

			lineInc = GetDoubleValue (&headerRecordPtr[383]);
			sampleInc = GetDoubleValue (&headerRecordPtr[391]);

					// Make certain that the number of bytes per data value
					// are within the bounds of MultiSpec.

			if (fileInfoPtr->numberBytes != 1 &&
					  fileInfoPtr->numberBytes != 2 &&
					  fileInfoPtr->numberBytes != 4)
																					return (returnCode);

					// Verify that this is not an old style LAS image in which
					// there is a header at the beginning of the .IMG file.
					// Also for the old style format, there was a requirement that
					// each line of data be padded so that a line represented a 
					// multiple of 512 bytes.
					// If this is an old style header, 'IMAG' will be stored
					// at bytes 48-51.

			errCode = MSetMarker (fileStreamPtr, fsFromStart, 0, kNoErrorMessages);

			if (errCode == noErr)
				{
				count = 128;
				errCode = MReadData (fileStreamPtr,
											&count,
											headerRecordPtr,
											kNoErrorMessages);

				}	// end "if (errCode == noErr)"

			if (errCode != noErr)
																					return (returnCode);

			if (strncmp ((char*)&headerRecordPtr[48], IMAGCode, 4) == 0) 
				{
						// This is an old LAS formatted file.
						// Count extra columns used to pad to a multiple of 512
						// bytes as post line bytes.	

				lDivideStruct = ldiv (
								fileInfoPtr->numberColumns * fileInfoPtr->numberBytes, 512);
				if (lDivideStruct.rem > 0)
					fileInfoPtr->numberPostLineBytes = 512 - lDivideStruct.rem;

						// Load the rest of the information for LAS formatted file.

				fileInfoPtr->numberHeaderBytes = 512;

				}	// end "if (strncmp (&headerRecordPtr[48],..."

			else	// new style format
				fileInfoPtr->numberHeaderBytes = 0;

			if (fileInfoPtr->numberBytes == 1)
				fileInfoPtr->numberBits = 8;

			else if (fileInfoPtr->numberBytes == 2)
				fileInfoPtr->numberBits = 16;

			else if (fileInfoPtr->numberBytes == 4)
				fileInfoPtr->numberBits = 32;

			fileInfoPtr->format = kLASType2;
			fileInfoPtr->ancillaryInfoformat = kLASType2;
			fileInfoPtr->swapBytesFlag = FALSE;
			fileInfoPtr->asciiSymbols = FALSE;

			fileInfoPtr->bandInterleave = kBSQ;

			fileInfoPtr->thematicType = FALSE;
			if (gGetFileImageType == kThematicImageType)
				fileInfoPtr->thematicType = TRUE;

			if (cornerCoordinatesValidCode == 1 &&
					  upperLeftX != 0 && upperLeftX != 0 && xWidth != 0 && yWidth != 0)
				{
				fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();

				if (fileInfoPtr->mapProjectionHandle != NULL)
					{
					mapProjectionInfoPtr = (MapProjectionInfoPtr)
														GetHandleStatusAndPointer (
																 fileInfoPtr->mapProjectionHandle,
																 &handleStatus);

					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = upperLeftX;
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = upperLeftY;

					mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
					mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;

					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = xWidth;

					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = yWidth;

					if (projectionValidCode == 1)
						{
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
								GetMapProjectionCodeFromGCTPNumber (
										  (SInt16)projCode,
										  FALSE,
										  &mapProjectionInfoPtr->gridCoordinate.projectionCode);

						if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode >=
																				kStatePlaneNAD27RSCode &&
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode <=
																								kUTMRSCode)
							mapProjectionInfoPtr->gridCoordinate.gridZone = (SInt16)zoneCode;

						if (mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																							kNotDefinedCode)
							mapProjectionInfoPtr->planarCoordinate.mapUnitsCode =
																								kUnknownCode;

						else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																				kStatePlaneNAD27RSCode ||
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																					kStatePlaneNAD83RSCode)
							mapProjectionInfoPtr->planarCoordinate.mapUnitsCode =
																							kUSSurveyFeetCode;

						else	// ...projectionCode != kNotDefinedCode && ...
							mapProjectionInfoPtr->planarCoordinate.mapUnitsCode =
																								kMetersCode;

						if (mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																		kLambertAzimuthalEqualAreaCode)
							SetLambertAzimuthalEqualAreaParameters (
								mapProjectionInfoPtr,
								ConvertPackedDegreesToDecimalDegrees (
																			projectionCoeffiecients[2]),
								ConvertPackedDegreesToDecimalDegrees (
																			projectionCoeffiecients[3]),
								0,
								0);

						}	// end "if (projectionValidCode == 1)"

					if (spheroidValidCode != 1)
						spheroidCode = -1;

					switch (spheroidCode)
						{
						case 0:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																				kClarke1866EllipsoidCode;
							mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
							break;

						case 1:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																				kClarke1880EllipsoidCode;
							break;

						case 2:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																				kBesselEllipsoidCode;
							break;

						case 4:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																		kInternational1909EllipsoidCode;
							break;

						case 5:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																				kWGS72EllipsoidCode;
							break;

						case 6:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																					kEverestEllipsoidCode;
							break;

						case 8:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																					kGRS80EllipsoidCode;
							mapProjectionInfoPtr->geodetic.datumCode = kNAD83Code;
							break;

						case 9:
							mapProjectionInfoPtr->geodetic.spheroidCode = kAiryEllipsoidCode;
							break;

						case 12:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																					kWGS84EllipsoidCode;
							mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;
							break;

						case 14:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																				kAustralianEllipsoidCode;
							break;

						case 15:
							mapProjectionInfoPtr->geodetic.spheroidCode =
																				kKrassovskyEllipsoidCode;
							break;

						default:
							 mapProjectionInfoPtr->geodetic.spheroidCode =
																				kNoEllipsoidDefinedCode;

						}	// end "switch (spheroidCode)"

					MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);

					}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"

				}	// end "if (tempInt >= 0 && tempInt <= 100)"

			returnCode = noErr;

			}	// end "if (errCode == noErr)"

		gSwapBytesFlag = FALSE;

		}	// end "if (fileType != 0)"

	return (returnCode);

}	// end "ReadLASHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadLGSOWGHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a LGSOWG header record.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if LGSOWG header record parameters make sense
//							1 - if LGSOWG header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/17/1999
//	Revised By:			Larry L. Biehl			Date: 03/01/2019

SInt16 ReadLGSOWGHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	char									tempString[16];

	CMFileStream*						fileStreamPtr;

	unsigned char						charKeyCode[4];
	
	int									returnValue;

	SInt32								numberLeftBorderPixels,
											numberBytesPerRecord,
											numberPostImageRecordBytes,
											numberPrefixBytes,
											numberPreImageRecordBytes;

	SInt16								errCode,
											fileType = 0,
											returnCode = 1,
											sscanfReturnCode;

	Boolean								swapBytesFlag;


	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
				// Check if image file is a LGSOWG formatted image file.

		charKeyCode[0] = 0x3f;
		charKeyCode[1] = 0xc0;
		charKeyCode[2] = 0x12;
		charKeyCode[3] = 0x12;

		if (strncmp ((char*)&headerRecordPtr[4], (char*)charKeyCode, 4) == 0)
			fileType = kLGSOWGType;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																							return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

		if (gGetFileImageType == 0)
			gGetFileImageType = kMultispectralImageType;

		returnCode = 0;
		errCode = noErr;

		numberLeftBorderPixels = 0;
		numberBytesPerRecord = 0;
		numberPrefixBytes = 0;
		numberPreImageRecordBytes = 0;
		numberPostImageRecordBytes = 0;

		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

				// Get the byte swap flag.

		if (strncmp (&headerRecordPtr[230], "LR", 2) == 0)
			swapBytesFlag = gBigEndianFlag;

		else	// strncmp (&headerRecordPtr[230], "LR", 2) != 0
			swapBytesFlag = !gBigEndianFlag;

				// Get the header record length.
				// Documentation says that binary fields are right justified.

		gSwapBytesFlag = !gBigEndianFlag;
		fileInfoPtr->numberHeaderBytes = GetLongIntValue (&headerRecordPtr[8]);

				// Get the number of bits.

		BlockMoveData (&headerRecordPtr[216], tempString, 4);
		tempString[4] = ' ';
		sscanfReturnCode = sscanf (tempString,
											 "%d",
											 &returnValue);

		if (sscanfReturnCode == 1)
			fileInfoPtr->numberBits = (UInt16)returnValue;

		else	// sscanfReturnCode != 1
			errCode = 1;

				// Get the number of bytes per pixel.

		if (errCode == noErr)
			{
			BlockMoveData (&headerRecordPtr[224], tempString, 4);
			tempString[4] = ' ';
			sscanfReturnCode = sscanf (tempString,
												  "%d",
												  &returnValue);

			if (sscanfReturnCode == 1)
				{
				fileInfoPtr->numberBytes = (UInt16)returnValue;

				if (fileInfoPtr->numberBytes >= 2)
					fileInfoPtr->swapBytesFlag = swapBytesFlag;

				}	// end "if (sscanfReturnCode == 1)"

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

					// Get the number of channels.

		if (errCode == noErr)
			{
			BlockMoveData (&headerRecordPtr[232], tempString, 4);
			tempString[4] = ' ';
			sscanfReturnCode = sscanf (tempString,
												  "%d",
												  &returnValue);

			if (sscanfReturnCode == 1)
				fileInfoPtr->numberChannels = (UInt16)returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

				// Get the number of lines.

		if (errCode == noErr)
			{
			BlockMoveData (&headerRecordPtr[236], tempString, 8);
			tempString[8] = ' ';
			sscanfReturnCode = sscanf (tempString,
												  "%d",
												  &returnValue);

			if (sscanfReturnCode == 1)
				fileInfoPtr->numberLines = returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

					// Add in any top and bottom border pixels.
					// First get the number of top border pixels.

			if (errCode == noErr)
				{
				BlockMoveData (&headerRecordPtr[260], tempString, 4);
				tempString[4] = ' ';
				sscanfReturnCode = sscanf (tempString,
													"%d",
													&returnValue);

				if (sscanfReturnCode == 1)
					fileInfoPtr->numberLines += returnValue;

				else	// sscanfReturnCode != 1
					errCode = 1;

				}	// end "if (errCode == noErr)"

					// Now get the number of bottom border pixels.

			if (errCode == noErr)
				{
				BlockMoveData (&headerRecordPtr[264], tempString, 4);
				tempString[4] = ' ';
				sscanfReturnCode = sscanf (tempString,
													"%d",
													&returnValue);

				if (sscanfReturnCode == 1)
					fileInfoPtr->numberLines += returnValue;

				else	// sscanfReturnCode != 1
					errCode = 1;

				}	// end "if (errCode == noErr)"

			}	// end "if (errCode == noErr)"

				// Get the number of bytes in one channel and line.

		if (errCode == noErr)
			{
			BlockMoveData (&headerRecordPtr[186], tempString, 6);
			tempString[6] = ' ';
			sscanfReturnCode = sscanf (tempString,
												  "%d",
												  &returnValue);

			if (sscanfReturnCode == 1)
				numberBytesPerRecord = returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

				// Get the number of left border pixels

		if (errCode == noErr)
			{
			BlockMoveData (&headerRecordPtr[244], tempString, 4);
			tempString[4] = ' ';
			sscanfReturnCode = sscanf (tempString,
												  "%d",
												  &returnValue);

			if (sscanfReturnCode == 1)
				numberLeftBorderPixels = returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

				// Get the number of columns of image data.

		if (errCode == noErr)
			{
			BlockMoveData (&headerRecordPtr[248], tempString, 8);
			tempString[8] = ' ';
			sscanfReturnCode = sscanf (tempString,
												  "%d",
												  &returnValue);

			if (sscanfReturnCode == 1)
				fileInfoPtr->numberColumns = returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

				// Get the number of prefix bytes per line/channel.

		if (errCode == noErr)
			{
			BlockMoveData (&headerRecordPtr[276], tempString, 4);
			tempString[4] = ' ';
			sscanfReturnCode = sscanf (tempString,
												  "%d",
												  &returnValue);

			if (sscanfReturnCode == 1)
				numberPrefixBytes = returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

				// Get the band interleave format

		if (strncmp (&headerRecordPtr[268], "BIL", 3) == 0)
			fileInfoPtr->bandInterleave = kBIL;

		else if (strncmp (&headerRecordPtr[268], "BSQ", 3) == 0)
			fileInfoPtr->bandInterleave = kBSQ;

		else if (strncmp (&headerRecordPtr[268], "BIP", 3) == 0)
			fileInfoPtr->bandInterleave = kBIS;

				// Set the code for the instrument system.

		if (MGetString (gTextString2, kFileIOStrID, IDS_FileIO106))
			{
			CopyPascalStringToC ((ConstStr255Param)gTextString2, (char*)gTextString2);
			if (strncmp ((char*)&headerRecordPtr[16], (char*)gTextString2, 10) == 0)
				fileInfoPtr->instrumentCode = kSPOT;

			}	// end "if (MGetString (gTextString2, ..."

		if (fileInfoPtr->instrumentCode == 0)
			{
			if (MGetString (gTextString2, kFileIOStrID, IDS_FileIO107))
				{
				CopyPascalStringToC ((ConstStr255Param)gTextString2, (char*)gTextString2);
				if (strncmp ((char*)&headerRecordPtr[16], (char*)gTextString2, 10) == 0)
					fileInfoPtr->instrumentCode = kSPOT4;

				}	// end "if (MGetString (gTextString2, ..."

			}	// end "if (fileInfoPtr->instrumentCode == 0)"

		if (errCode == noErr)
			{
					// Load the rest of the information for LGSOWG formatted files

			fileInfoPtr->format = kLGSOWGType;
			fileInfoPtr->ancillaryInfoformat = kLGSOWGType;
			fileInfoPtr->asciiSymbols = FALSE;
			fileInfoPtr->numberTrailerBytes = 0;

					// Get the pre and post line and channel bytes.
					// 	There are always 12 ID record bytes per line/channel

			numberPreImageRecordBytes = 12 + numberPrefixBytes +
											numberLeftBorderPixels * fileInfoPtr->numberBytes;

			numberPostImageRecordBytes = numberBytesPerRecord -
					  					numberPreImageRecordBytes -
					  					fileInfoPtr->numberBytes * fileInfoPtr->numberColumns;

			if (numberPostImageRecordBytes < 0)
				fileInfoPtr->numberColumns =
												numberBytesPerRecord / fileInfoPtr->numberBytes;

			if (fileInfoPtr->bandInterleave == kBIL)
				{
				fileInfoPtr->numberPreLineBytes = 0;
				fileInfoPtr->numberPostLineBytes = 0;

				if (numberPostImageRecordBytes >= 0)
					{
					fileInfoPtr->numberPreChannelBytes = numberPreImageRecordBytes;
					fileInfoPtr->numberPostChannelBytes = numberPostImageRecordBytes;

					}	// end "if (numberPostImageRecordBytes >= 0)"

				else	// numberPostImageRecordBytes < 0
					{
					fileInfoPtr->numberPreChannelBytes = 0;
					fileInfoPtr->numberPostChannelBytes = numberBytesPerRecord -
								 fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;

					}	// end "else numberPostImageRecordBytes < 0"

				}	// end "if (fileInfoPtr->bandInterleave == kBIL)"

			else if (fileInfoPtr->bandInterleave == kBSQ)
				{
				fileInfoPtr->numberPreChannelBytes = 0;
				fileInfoPtr->numberPostChannelBytes = 0;

				if (numberPostImageRecordBytes >= 0)
					{
					fileInfoPtr->numberPreLineBytes = numberPreImageRecordBytes;
					fileInfoPtr->numberPostLineBytes = numberPostImageRecordBytes;

					}	// end "if (numberPostImageRecordBytes >= 0)"

				else	// numberPostImageRecordBytes < 0
					{
					fileInfoPtr->numberPreLineBytes = 0;
					fileInfoPtr->numberPostLineBytes = numberBytesPerRecord -
								 fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;

					}	// end "else numberPostImageRecordBytes < 0"

				}	// end "else if (fileInfoPtr->bandInterleave == kBSQ)"

			else	// fileInfoPtr->bandInterleave == kBIS
				{
				fileInfoPtr->numberColumns =
												numberBytesPerRecord / fileInfoPtr->numberBytes;

				fileInfoPtr->numberPreChannelBytes = 0;
				fileInfoPtr->numberPostChannelBytes = 0;

				fileInfoPtr->numberPreLineBytes = 0;
				fileInfoPtr->numberPostLineBytes = numberBytesPerRecord -
										fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;

				}	// end "else fileInfoPtr->bandInterleave == kBIS)"

			fileInfoPtr->thematicType = FALSE;
			if (gGetFileImageType == kThematicImageType)
				fileInfoPtr->thematicType = TRUE;

			}	// end "if (errCode == noErr)"

		else	// errCode != noErr
			returnCode = 1;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	gSwapBytesFlag = FALSE;

	return (returnCode);

}	// end "ReadLGSOWGHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadMacSADIEHeader
//
//	Software purpose:	The purpose of this routine is to read the MacSADIE header. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/22/1992
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

SInt16 ReadMacSADIEHeader (
				FileInfoPtr							fileInfoPtr,
				SInt16*								headerRecordPtr,
				SInt16								formatOnlyCode)

{
	CMFileStream*						fileStreamPtr;

	SInt32								systemFileCreator,
											systemFileType;

	SInt16								fileType = 0;

	Boolean								returnCode = 1;


	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		systemFileType = GetFileType (fileStreamPtr);
		systemFileCreator = GetFileCreator (fileStreamPtr);

				// Check if image file is a MacSADIE formatted image file.

		if (systemFileCreator == kMacSADIECreator &&
																systemFileType == kMacSADIEFileType)
			fileType = kSADIEType;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																							return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

		if (gGetFileImageType == 0)
			gGetFileImageType = kMultispectralImageType;

				// Get number of channels in the multispectral image.

		fileInfoPtr->numberChannels = headerRecordPtr[0];

				// Get number of lines in the multispectral image file.

		fileInfoPtr->numberLines = headerRecordPtr[1];

				// Get number of columns in the multispectral image file.

		fileInfoPtr->numberColumns = headerRecordPtr[2];

				// Get number of bits per pixel in the multispectral image file.

		fileInfoPtr->numberBits = headerRecordPtr[3];

		fileInfoPtr->numberHeaderBytes = 512;

				// Load the rest of the information for MacSADIE formatted file.

		fileInfoPtr->numberBytes = 1;
		if (fileInfoPtr->numberBits > 8)
			fileInfoPtr->numberBytes = 2;

		fileInfoPtr->format = kSADIEType;
		fileInfoPtr->ancillaryInfoformat = kSADIEType;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->bandInterleave = kBIL;
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 0;
		fileInfoPtr->numberPostLineBytes = 0;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;

		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			fileInfoPtr->thematicType = TRUE;

		returnCode = noErr;

		}	// end "if (fileType != 0)"

	return (returnCode);

}	// end "ReadMacSADIEHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadMultiSpecClassificationHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a MultiSpec Classification file.
//
//	Parameters in:		Pointer to file information structure		
//
//	Parameters out:	None		
//
//	Value Returned:	0 - if header record parameters make sense
//							1 - if header record paramters do not make sense
//							-1 - if not all of the parameters were found
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/17/1990
//	Revised By:			Larry L. Biehl			Date: 12/06/2019

SInt16 ReadMultiSpecClassificationHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	SInt64								fileSize,
											imageSize;

	char									*ioBufferPtr,
											//*stringPtr,
											*strPtr,
											*strPtr2;

	CMFileStream*						fileStreamPtr;
	
	SInt64								lContBlock;

	int									columnInterval,
											firstColumn,
											firstLine,
											lastColumn,
											lastLine,
											lineInterval;
											//numberTrailerBytes;

	UInt32								bufferSize,
											count,
											index,
											maxBufferSize = 3000,
											minBufferSize = 1000,
											posOff;

	SInt16								errCode,
											fileType,
											nextSet,
											returnCode,
											sscanCode,
											stringCompare,
											stringLength,
											stringNumber;

	Boolean								atEndOfFile,
											continueFlag,
											notFound;


			// Initialize local variables.

	fileType = 0;
	ioBufferPtr = NULL;
	returnCode = 1;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

	if (fileInfoPtr != NULL && fileStreamPtr != NULL)
		{
				// First verify that this is a MultiSpec Classification file.

		for (index = 0; index < 2; index++)
			{
			stringCompare = -1;

			if (index == 0)
				continueFlag = MGetString (gTextString2, kFileIOStrID, IDS_MacLARSYSType);

			else if (index == 1)
				continueFlag = MGetString (gTextString2, kFileIOStrID, IDS_MultiSpecType);

			if (continueFlag)
				{
						// Skip the carriage return/line feed if there is one.

				posOff = 0;
				if (headerRecordPtr[0] == kCarriageReturn)
					{
					posOff = 1;

					if (headerRecordPtr[1] == kLineFeed)
						posOff = 2;

					}	// end "if (headerRecordPtr[0] == kCarriageReturn)"

				CopyPascalStringToC ((ConstStr255Param)gTextString2, (char*)gTextString2);
				stringCompare = strncmp (
										&headerRecordPtr[posOff], (char*)gTextString2, 24);

				}	// end "if (if (MGetString (...)"

			if (stringCompare == 0)
				{
				fileType = kMultiSpecClassificationType;
				break;

				}	// end "if (stringCompare == 0)"

			}	// end "for (index=0, index<2; index++)"

		}	// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"

	if (formatOnlyCode != kLoadHeader)
		{
		fileInfoPtr->format = fileType;

		if (fileType != 0)
			fileInfoPtr->thematicType = TRUE;
		return (returnCode);

		}	// end "if (formatOnlyCode != kLoadHeader)"

	if (fileType != 0)
		{
		if (gGetFileImageType == 0)
			gGetFileImageType = kThematicImageType;

				// Load the default values.

		fileInfoPtr->numberLines = 100;
		fileInfoPtr->numberColumns = 100;
		fileInfoPtr->numberClasses = 2;
		fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;
		fileInfoPtr->startColumn = 1;
		fileInfoPtr->startLine = 1;
		fileInfoPtr->numberHeaderBytes = 0;

				// Load the rest of the information into the file informataion
				// structure.

		fileInfoPtr->numberChannels = 1;
		fileInfoPtr->numberBits = 8;
		fileInfoPtr->numberBytes = 1;
		fileInfoPtr->numberBins = 256;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->asciiSymbols = TRUE;
		fileInfoPtr->bandInterleave = kBIL;
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 0;
		fileInfoPtr->numberPostLineBytes = 1;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;

		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			fileInfoPtr->thematicType = TRUE;

		returnCode = noErr;

				// Signify that file is of "MultiSpec Classification" type.

		fileInfoPtr->format = kMultiSpecClassificationType;
		fileInfoPtr->ancillaryInfoformat = kMultiSpecClassificationType;

				// Load the rest of the information for MultiSpec classification.
				// Get a buffer to read in the header part of the file.  Try to 	
				// get as large a buffer as possible without being larger than		
				// file itself and not more than maxBufferSize.							

		errCode = GetSizeOfFile (fileStreamPtr, &fileSize);
		MGetFreeMemory (&lContBlock);
		bufferSize = (UInt32)MIN (fileSize, maxBufferSize);
		bufferSize = MIN (bufferSize, lContBlock - 20000);
		bufferSize = MAX (bufferSize, minBufferSize);

		ioBufferPtr = (char*)MNewPointer (bufferSize);
		if (ioBufferPtr == NULL)
			return (-1);

				// Get the needed parameters.

		posOff = 0;
		atEndOfFile = FALSE;
		notFound = TRUE;
		nextSet = 1;
		while (notFound && errCode == noErr)
			{
			errCode = MSetMarker (fileStreamPtr,
											fsFromStart,
											posOff,
											kNoErrorMessages);
			if (errCode != eofErr)
				IOCheck (errCode, fileStreamPtr);

					// Read in a buffer of data.

			if (errCode == noErr)
				{
				count = bufferSize - 1;
				errCode = MReadData (fileStreamPtr,
											&count,
											ioBufferPtr,
											kNoErrorMessages);
				if (errCode != eofErr)
					IOCheck (errCode, fileStreamPtr);

				else	// errCode == eofErr
					{
					atEndOfFile = TRUE;
					errCode = noErr;

					}	// end "else errCode == eofErr"

				posOff += count;
				strPtr = ioBufferPtr;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Add null terminator to the buffer.

				ioBufferPtr[count] = kNullTerminator;

						// Branch to set of information to get.

				if (nextSet == 1)
					{
							// Find "Image start line and column =" in the buffer.

					if (!MGetString (gTextString2, kFileIOStrID, IDS_ImageStartLine)) // 36
						errCode = -3;
					pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
					strPtr = (char*)strstr ((char*)ioBufferPtr, (char*)gTextString2);
					if (strPtr != NULL)
						{
								// Get the image start line and column.

						strPtr += stringLength;
						sscanCode = sscanf (strPtr,
													"%d, %d\r",
													&fileInfoPtr->startLine,
													&fileInfoPtr->startColumn);

						if (sscanCode == 2)
							nextSet = 2;

						else	// sscanCode != 1 
							posOff -= (stringLength + 10);

						}	// end "if (strPtr != NULL)"

					else	// strPtr == NULL
						posOff -= stringLength;

					}	// end "if (nextSet == 1)"

						// Branch to set of information to get.

				if (nextSet == 2)
					{
							// Find "Number classes =" in the buffer.

					MGetString (gTextString2, kFileIOStrID, IDS_NumberClasses); // 29
					if (gTextString2[0] == 0)
						errCode = -3;
					pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
					strPtr = (char*)strstr (ioBufferPtr, (char*)gTextString2);
					if (strPtr != NULL)
						{
								// Get the number of classes.

						strPtr += stringLength;
						sscanCode = sscanf (strPtr, "%d\r", &fileInfoPtr->numberClasses);

						if (sscanCode == 1)
							nextSet = 3;

						else	// sscanCode != 1 
							posOff -= (stringLength + 10);

						}	// end "if (strPtr != NULL)"

					else	// strPtr == NULL 
						posOff -= stringLength;

					}	// end "if (nextSet == 2)"

				 if (nextSet == 3)
					{
							// Find "Classification of Selected Area" in the buffer.

					MGetString (gTextString2, kFileIOStrID, IDS_ClassifySelectedArea); // 30
					if (gTextString2[0] == 0)
						errCode = -3;
					pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
					strPtr = (char*)strstr (ioBufferPtr, (char*)gTextString2);

					if (strPtr != NULL)
						{
						strPtr += stringLength + 1;

								// Verify that a carriage return exists in the string signifying that
								// the string we are looking for is a complete line

						strPtr2 = (char*)strchr (strPtr, kCarriageReturn);
						if (strPtr2 == NULL)
									// Check if a line feed exists in the string. This could be the
									// situation for linux (or files generated by wxWidgets).
							strPtr2 = (char*)strchr (strPtr, kLineFeed);

						if (strPtr2 != NULL)
							{
									// Check if this is a file created under Windows.

							if (strPtr2[1] == kLineFeed)
								fileInfoPtr->numberPostLineBytes = 2;

							sscanCode = sscanf (
										strPtr,
										"  Lines %d to %d by %d.  Columns %d to %d by %d",
										&firstLine, &lastLine, &lineInterval,
										&firstColumn, &lastColumn, &columnInterval);

							if (sscanCode == 6)
								{
										// Get the number lines and columns in the image.

								fileInfoPtr->numberLines =
											(lastLine - firstLine + lineInterval) / lineInterval;

								fileInfoPtr->numberColumns =
											(lastColumn - firstColumn + columnInterval) /
																							columnInterval;

								fileInfoPtr->startColumn += firstColumn - 1;
								fileInfoPtr->startLine += firstLine - 1;

										// Get the number of header bytes.

								fileInfoPtr->numberHeaderBytes =
										(UInt32)(strPtr2 - ioBufferPtr +
											 fileInfoPtr->numberPostLineBytes + posOff - count);
								notFound = FALSE;

								}	// end "if (sscanCode == 6)"

							else	// sscanCode != 6
								posOff -= (stringLength + 70);

							}	// end "if (strPtr2 != NULL)"

						else	// strPtr2 != NULL
							posOff -= (stringLength + 70);

						}	// end "if (strPtr != NULL)"

					else	// strPtr == NULL
						posOff -= stringLength;

					}	// end "if (nextSet == 3)"

				}	// end "if (errCode == noErr)"

			if (atEndOfFile && notFound && nextSet > 1)
				{
						// Display an alert if no classification results were found
						// in the file.

				stringNumber = IDS_Alert6;
				returnCode = -1;
				switch (nextSet)
					{
					case 2:
						stringNumber = IDS_Alert4;
						returnCode = 0;
						break;

					case 3:
					case 4:
						stringNumber = IDS_Alert6;
						break;

					}	// end "switch (nextSet)"

				sscanCode = DisplayAlert (
							kErrorAlertID, kStopAlert, kAlertStrID, stringNumber, 0, NULL);

				errCode = eofErr;

				}	// end "if (atEndOfFile && notFound && nextSet>1)"

			if (atEndOfFile && notFound && nextSet == 1)
				{
						// Skip trying to read the start line and column.  We will
						// assume that they are 1.

				posOff = 0;
				atEndOfFile = FALSE;
				nextSet = 2;

				}	// end "if (atEndOfFile && notFound && nextSet==1)"

			}	// end "while (notFound && errCode == noErr)"

		if (returnCode == noErr && errCode != noErr)
			returnCode = 1;

		if (returnCode == noErr)
			{
					// Check if parameters make sense

			if (CheckFileInfoParameters (fileInfoPtr) == 0)
				returnCode = 1;

					// Continue section only if parameters make sense

			if (returnCode == noErr)
				{
				IntermediateFileUpdate (fileInfoPtr);

						// Get the number of trailer bytes.	This is the number of
						// bytes after the image and before the Group Indentifier
						// string if it exists.

						// Get the size of the image without any trailer bytes
						// which is currently 0.

				imageSize = GetSizeOfImage (fileInfoPtr);

						// Allow for class summary table - the major portion of it.

				posOff = (UInt32)(imageSize + 33 * fileInfoPtr->numberClasses);

						// Set file marker to end of image data and after most of
						// summary table.

				errCode = MSetMarker (fileStreamPtr,
												fsFromStart,
												posOff,
												kNoErrorMessages);

						// Read in a buffer of data.

				strPtr = NULL;
				if (errCode == noErr)
					{
					count = bufferSize - 1;
					errCode = MReadData (fileStreamPtr,
												&count,
												ioBufferPtr,
												kNoErrorMessages);
					if (errCode == eofErr)
						errCode = noErr;

					ioBufferPtr[count] = 0;

					}	// end "if (errCode == noErr)"

						// Get the Group Identifier String.

				continueFlag = MGetString (gTextString2,
													kFileIOStrID,
													IDS_GroupDescriptions);

				if (continueFlag && errCode == noErr)
					{
							// Make a c-string.

					gTextString2 [gTextString2[0] + 1] = 0;

							// Now try to locate the Group Identifier String in the
							// buffer just read.

					strPtr = (char*)strstr (ioBufferPtr, (char*)&gTextString2[1]);

					if (strPtr != NULL)
						fileInfoPtr->numberTrailerBytes =
									(UInt32)((posOff - imageSize) + (strPtr - ioBufferPtr));

					}	// end "if (continueFlag && errCode == noErr)"

				if (strPtr == NULL)
					{
							// Now try to locate the line that is the end of the processor
							// listing "----------".

					strPtr = (char*)strstr (ioBufferPtr, "----------");

					if (strPtr != NULL)
						fileInfoPtr->numberTrailerBytes =
								 (UInt32)((posOff - imageSize) + (strPtr - ioBufferPtr) + 10);

					}	// end "if (strPtr == NULL)"

				}	// end "if (returnCode > 0)"

			}	// end "if (returnCode > 0)"

		}	// end "if (fileType != 0)"

	CheckAndDisposePtr (ioBufferPtr);

	return (returnCode);

}	// end "ReadMultiSpecClassificationHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReadLARSYSChannelDescriptionsAndValues
//
//	Software purpose:	The purpose of this routine is to read the channel
//							descriptions and values from a LARSYS header record.
//
//	Parameters in:		Pointer to file information structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			ReadChannelDescriptionsAndValues in SOpenFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/22/1993
//	Revised By:			Larry L. Biehl			Date: 06/28/2018

void ReadLARSYSChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr)

{
	double								realValue1,
											realValue2;

	ChannelDescriptionPtr			channelDescriptionPtr;
	char*									charPtr;

	CMFileStream*						fileStreamPtr;

	float									*channelValuesPtr,
											*channelWidthsPtr;

	UInt32								count;

	SInt16								channel,
											errCode;

	Boolean								changedFlag,
											continueFlag;


	if (fileInfoPtr == NULL)
																									return;

	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr == NULL)
																									return;

	errCode = noErr;

			// Get handle to memory to store the channel information	in.

	count = fileInfoPtr->numberChannels * sizeof (ChannelDescription);
	channelDescriptionPtr = (ChannelDescriptionPtr)CheckHandleSize (
																&fileInfoPtr->channelDescriptionH,
																&continueFlag,
																&changedFlag,
																count);

	if (!continueFlag)
		errCode = -1;

	if (errCode == noErr)
		{
				// Get handle to memory to store the center channel (band) values,
				// channel (band) widths, and channel wavelength order.
		
		channelValuesPtr = CheckChannelValuesHandleSize (fileInfoPtr,
																			&continueFlag);
		
		if (continueFlag)
			{
			channelWidthsPtr = &channelValuesPtr[fileInfoPtr->numberChannels];
			
					// This will be flag indicating that widths are not available.
					
			*channelWidthsPtr = 0;
			
			}	// end "if (continueFlag)"

		else	// !continueFlag)
			errCode = -1;

		}	// end "if (errCode == noErr)"

	if (errCode == noErr)
		{
				// Set position mark in file to the start of the channel	descriptions.

		count = 200;
		errCode = MSetMarker (fileStreamPtr,
									 fsFromStart,
									 count,
									 kNoErrorMessages);

		}	// end "if (errCode == noErr)"

	if (errCode == noErr)
		{
				// Get a blank string to use as the default.

		if (!MGetString (gTextString3, kFileIOStrID, IDS_BlankString16))
			errCode = 1;

		}	// end "if (errCode == noErr)"

			// Set up swap bytes flag depending upon the system architecture

	gSwapBytesFlag = !gBigEndianFlag;

	if (errCode == noErr)
		{
		channel = 0;
		count = 20;

		while (channel < fileInfoPtr->numberChannels)
			{
			charPtr = (char*)&channelDescriptionPtr[channel];

					// If there has been no file error, read channel description
					//	into memory.																

			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr,
											  &count,
											  gTextString,
											  kNoErrorMessages);

			if (errCode == noErr)
				{
				realValue1 = ConvertIBM370RealtoReal ((SInt32*)gTextString);

				sprintf (charPtr, "%5.2f-", realValue1);

				realValue2 = ConvertIBM370RealtoReal ((SInt32*)&gTextString[4]);

				sprintf (&charPtr[6], "%5.2f um ", realValue2);
				charPtr[15] = ' ';

				*channelValuesPtr = (float)((realValue1 + realValue2) / 2.);

				}	// end "if (errCode == noErr)"

			else	// errCode != noErr 
				{
				BlockMoveData ((char*)gTextString3, charPtr, 16);
				*channelValuesPtr = -1.;

				}	// end "else errCode != noErr"

			channel++;
			channelValuesPtr++;

			}	// end "while (channel<..."

		if (errCode == noErr)
			fileInfoPtr->descriptionsFlag = TRUE;

		}	// end "if (errCode == noErr)"

	gSwapBytesFlag = FALSE;

	CheckAndUnlockHandle (fileInfoPtr->channelDescriptionH);
	CheckAndUnlockHandle (fileInfoPtr->channelValuesHandle);

	fileInfoPtr->maxNumberDescriptionCharacters = 14;

}	// end "ReadLARSYSChannelDescriptionsAndValues"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReadMultiSpecChannelDescriptionsAndValues
//
//	Software purpose:	The purpose of this routine is to check if a set
//							of channel descriptions and or values are located 
//							at the end of the image data.  A flag is set for the
//							file information structure accordingly.
// 						There are 2 possible positions. On 8/28/2020, the max length
//							of the channel descriptions was changed from 16 to 24.
//							Now need to check for both.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			LoadImageInformation in SFileIO.cpp
//							ModalFileSpecification in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1989
//	Revised By:			Larry L. Biehl			Date: 08/28/2020

void ReadMultiSpecChannelDescriptionsAndValues (
				FileInfoPtr							fileInfoPtr)

{
	CMFileStream*						fileStreamPtr;

	UInt32								channelDescriptionLength1,
											channelDescriptionLength2,
											count;

	SInt64								channelDescription1Index,
											channelDescription2Index,
											sizeFile,
											sizeImageFile;

	SInt16								errCode,
											foundCode;
	

			// Only try to get channel descriptions and values if the data are not
			// compressed.

	if (fileInfoPtr->dataCompressionCode == kNoCompression)
		{
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

				// Get the size of the image without any channel descriptions.

		sizeImageFile = GetSizeOfImage (fileInfoPtr);
		
		if (fileInfoPtr->numberHeaderBytes == 0 &&
					(fileInfoPtr->format == kTIFFType ||
															fileInfoPtr->format == kGeoTIFFType))
			{
					// This implies that the tiff image has multiple strips and the number
					// of header bytes is not known. If the MultiSpec channel descriptions
					// exist they should be at a set location from the end of the file.
					// Try that location.
			
			errCode = GetSizeOfFile (fileInfoPtr, &sizeFile);
			foundCode = -1;
			
			if (errCode == noErr)
				{
				channelDescriptionLength1 = 20 + fileInfoPtr->numberChannels * 16 +
																20 + fileInfoPtr->numberChannels * 9;

				channelDescriptionLength2 = 20 +
							fileInfoPtr->numberChannels * kChannelDescriptionLength +
														20 + fileInfoPtr->numberChannels * 9;
																
						// Get size of the channel descriptions.
			
				channelDescription1Index = sizeFile - channelDescriptionLength1;
				channelDescription2Index = sizeFile - channelDescriptionLength2;
				
				//sizeFile -= (20 + fileInfoPtr->numberChannels * 16 +
				//											20 + fileInfoPtr->numberChannels * 9);
				//
				//if (sizeFile >= 0)
				//	sizeImageFile = sizeFile;
					
				MGetString (gTextString2, kFileIOStrID, IDS_ChannelDescriptions);
			
				if (gTextString2[0] > 0)
					{
					if (channelDescription1Index >= 0)
						{
								// Now need to check if "CHANNEL DESCRIPTION" is at the
								// location expected for Channel Description version 1
									
						sizeImageFile = channelDescription1Index;
						
						errCode = MSetMarker (fileStreamPtr,
													 fsFromStart,
													 sizeImageFile,
													 kNoErrorMessages);
						if (errCode != eofErr)
							IOCheck (errCode, fileStreamPtr);

						if (errCode == noErr)
							{
							count = 20;
							errCode = MReadData (fileStreamPtr,
															&count,
															gTextString,
															kNoErrorMessages);

							if (errCode != eofErr)
								IOCheck (errCode, fileStreamPtr);
							
							}	// end "if (errCode == noErr)"

						if (errCode == noErr)
							{
							if (strncmp ((char*)gTextString,
												(char*)&gTextString2[1],
												19) == 0)
								foundCode = 1;
								
							}	// end "if (errCode == noErr && ...)"
						
						}	// end "if (channelDescription1Index >= 0)"
						
					if (foundCode == -1 && channelDescription2Index >= 0)
						{
								// Assume that any channel descriptions that exist represent
								// version 2. The next section of code will confirm this.
											
						sizeImageFile = channelDescription2Index;
						
						}	// end "if (foundCode == -1 && channelDescription2Index >= 0)"
											
					}	// end "if (gTextString2[0] > 0)"
			
				}	// end "if (errCode == noErr)"
			
			}	// end "if (fileInfoPtr->numberHeaderBytes == 0 && ..."
		
				// Set file pointer to end of image data.  If no eof error, then
				// read in characters to check for channel description identifier.	
		
		errCode = MSetMarker (fileStreamPtr,
									 fsFromStart,
									 sizeImageFile,
									 kNoErrorMessages);
		if (errCode != eofErr)
			IOCheck (errCode, fileStreamPtr);

		if (errCode == noErr)
			{
			count = 20;
			errCode = MReadData (fileStreamPtr,
											&count,
											gTextString,
											kNoErrorMessages);

			if (errCode != eofErr)
				IOCheck (errCode, fileStreamPtr);
			
			}	// end "if (errCode == noErr)"

		foundCode = -1;
		if (errCode == noErr)
			{
					// If not at end of the file, then check if characters read in are
					// "CHANNEL DESCRIPTIONS" or "CHANNEL DESCRIPTION2"

			MGetString (gTextString2, kFileIOStrID, IDS_ChannelDescriptions);

			if (gTextString2[0] > 0)
				{
				if (strncmp ((char*)gTextString,
									(char*)&gTextString2[1],
									gTextString2[0]) == 0)
					foundCode = 1;
					
				}	// end "if (errCode == noErr && ...)"

			if (foundCode == -1)
				{
				MGetString (gTextString2, kFileIOStrID, IDS_ChannelDescription2);
				
				if (gTextString2[0] > 0)
					{
					if (strncmp ((char*)gTextString,
										(char*)&gTextString2[1],
										gTextString2[0]) == 0)
						foundCode = 2;
						
					}	// end "if (gTextString2[0] > 0)"
					
				}	// end "if (foundCode == -1)"
			
			}	// end "if (errCode == noErr)"

		if (foundCode >= 0)
			{
			errCode = ReadMultiSpecChannelDescriptions (fileInfoPtr, foundCode);

			if (errCode == noErr)
				{
						// Try to read in the identifier for the channel values.

				count = 20;
				errCode = MReadData (fileStreamPtr,
											&count,
											gTextString,
											kNoErrorMessages);

            }	// end "if (errCode == noErr)"

			}	// end "if (foundCode >= 0)"

		MGetString (gTextString2, kFileIOStrID, IDS_ChannelValues); // 56
		foundCode = -1;

		if (errCode == noErr &&
                gTextString2[0] > 0 && strncmp (
                (char*)gTextString, (char*)&gTextString2[1], gTextString2[0]) == 0)
			foundCode = 0;
		 
		if (foundCode >= 0)
			errCode = ReadMultiSpecChannelValues (fileInfoPtr);
			
				// Try to get the channel (band) widths from the descriptions.
			
		if (foundCode >= 0 && errCode == noErr)
			GetMultiSpecChannelWidths (fileInfoPtr);

		}	// end "if (fileInfoPtr->dataCompressionCode == kNoCompression)"

}	// end "ReadMultiSpecChannelDescriptionsAndValues"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadMultiSpecChannelDescriptions
//
//	Software purpose:	The purpose of this routine is to check if a set
//							of channel descriptions are located at the end
//							of the image data.  A flag is set for the
//							file information structure accordingly.
//
//	Parameters in:		Pointer to file information structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			ReadMultiSpecChannelDescriptionsAndValues in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1989
//	Revised By:			Larry L. Biehl			Date: 08/29/2020

SInt16 ReadMultiSpecChannelDescriptions (
				FileInfoPtr							fileInfoPtr,
				SInt16								versionNumber)
				
{
	ChannelDescriptionPtr			channelDescriptionPtr;
	char*									charPtr;

	UInt32								count;

	SInt16								errCode,
											index,
											maxNumberCharactersUsed;

	UInt16								channel;


	errCode = -1;
   maxNumberCharactersUsed = 0;

	if (fileInfoPtr != NULL)
		{
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

				// Get handle to memory to store the channel information	in.

		fileInfoPtr->channelDescriptionH =
				 MNewHandle (fileInfoPtr->numberChannels * sizeof (ChannelDescription));

		if (fileInfoPtr->channelDescriptionH != NULL)
			{
			channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
																	  fileInfoPtr->channelDescriptionH,
																	  kLock);

			channel = 0;
			
			count = 16;			// for versionNumber == 1
			if (versionNumber == 2)
				count = kChannelDescriptionLength;
				
			errCode = noErr;

					// Get a blank string.

			MGetString (gTextString3, kFileIOStrID, IDS_BlankString16);

			while (channel < fileInfoPtr->numberChannels)
				{
				charPtr = (char*)&channelDescriptionPtr[channel];

						// If there has been no file error, read channel description
						//	into memory.

				if (errCode == noErr)
					errCode = MReadData (fileStreamPtr,
												&count,
												charPtr,
												kNoErrorMessages);

						// If file error has occurred then load a blank description
						// in for the channel.

				if (errCode != noErr)
					BlockMoveData ((char*)&gTextString3[1], charPtr, 24);

				else	// errCode == noErr
					{
					charPtr += kChannelDescriptionLength - 1;

							// Determine maximum number of non-blank characters.

					for (index=kChannelDescriptionLength; index>0; index--)
						{
						if (*charPtr != ' ')
							break;

						charPtr--;

						}	// end "for (index=kChannelDescriptionLength; index>0; index--)"

					maxNumberCharactersUsed = MAX (index, maxNumberCharactersUsed);

					}	// end "else errCode == noErr"

				channel++;

				}	// end "while (channel<..."

			fileInfoPtr->descriptionsFlag = TRUE;
			CheckAndUnlockHandle (fileInfoPtr->channelDescriptionH);

			}	// end "if (fileInfoPtr->channelDescriptionH != ..."

		}	// end "if (fileInfoPtr != NULL)"

	fileInfoPtr->maxNumberDescriptionCharacters = maxNumberCharactersUsed;

	return (errCode);

}	// end "ReadMultiSpecChannelDescriptions"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadMultiSpecChannelValues
//
//	Software purpose:	The purpose of this routine is to check if a set
//							of channel values are located at the end
//							of the image data.  If so, the data are read into
//							memory identified in the file information structure
//							for that image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ReadMultiSpecChannelDescriptionsAndValues in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/15/1992
//	Revised By:			Larry L. Biehl			Date: 06/28/2018

SInt16 ReadMultiSpecChannelValues (
				FileInfoPtr							fileInfoPtr)

{
	SInt64								fileSize,
											positionOffset;

	float									*channelValuesPtr,
											*channelWidthsPtr;

	UInt32								count;

	SInt16								errCode;

	UInt16								channel;

	Boolean								changedFlag,
											continueFlag,
											disposeBufferHandleFlag;


    errCode = -1;

    if (fileInfoPtr != NULL)
		{
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
				// Get handle to memory to store the center channel (band) values,
				// channel (band) widths, and channel wavelength order.
		
		errCode = noErr;
		channelValuesPtr = CheckChannelValuesHandleSize (fileInfoPtr,
																			&continueFlag);
		
		if (continueFlag)
			{
			channelWidthsPtr = &channelValuesPtr[fileInfoPtr->numberChannels];
			
					// This will be flag indicating that widths are not available.
					
			*channelWidthsPtr = 0;
			
			}	// end "if (continueFlag)"

		else	// !continueFlag
			errCode = -1;

				// Get temporary memory to read the channel values in.				
				// Estimate the size of memory needed and then make sure that it	
				// is not larger than the rest of the file size.						

		if (errCode == noErr)
			errCode = GetSizeOfFile (fileInfoPtr, &fileSize);

		if (errCode == noErr)
			errCode = MGetMarker (fileStreamPtr,
										 &positionOffset,
										 kNoErrorMessages);
		
		if (errCode == noErr)
			{
			count = 0;
			if (fileSize >= positionOffset)
				count = (UInt32)(fileSize - positionOffset);
				
			count = MIN (count, (UInt32)fileInfoPtr->numberChannels * 10);

			disposeBufferHandleFlag = (gCharBufferHandle1 == NULL);

			gCharBufferPtr1 = (CharPtr)CheckHandleSize (&gCharBufferHandle1,
																	  &continueFlag,
																	  &changedFlag,
																	  count + 1);

			if (!continueFlag)
				errCode = -1;

			}	// end "if (errCode == noErr)"

					// If there has been no memory or file error, read all channel 	
					// values into memory.															

		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr,
										 &count,
										 gCharBufferPtr1,
										 kNoErrorMessages);
										 
		if (errCode == noErr)
			{
			HPtr inputStringPtr = gCharBufferPtr1;
			errno = 0;
			inputStringPtr[count] = 0;

			for (channel=0; channel<fileInfoPtr->numberChannels; channel++)
				{
				*channelValuesPtr = (float)strtod (
														inputStringPtr, (char**)&inputStringPtr);
				if (errno == ERANGE)
					{
					errCode = -2;
					break;

					}	// end "if (errno == ERANGE)" 
				
						// Set description code information.
						
				if (*channelValuesPtr > 0)
					{
					if (*channelValuesPtr < 3.0)
						fileInfoPtr->descriptionCode |= kReflectiveData;
				
					else	// *channelValuesPtr >= 3.0
						fileInfoPtr->descriptionCode |= kThermalData;
						
					}	// end "if (*channelValuesPtr > 0)"
				
				channelValuesPtr++;

            }	// end "for (channel=0; channel<..."

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			CheckAndUnlockHandle (fileInfoPtr->channelValuesHandle);

		else	// errCode != noErr
			{
			fileInfoPtr->channelValuesHandle =
                    UnlockAndDispose (fileInfoPtr->channelValuesHandle);

			}	// end "else errCode != noErr" 

		if (disposeBufferHandleFlag)
			{
			gCharBufferHandle1 = UnlockAndDispose (gCharBufferHandle1);
			gCharBufferPtr1 = NULL;

			}	// end "if (disposeBufferHandleFlag)"

		}	// end "if (fileInfoPtr != NULL)" 

	return (errCode);

}	// end "ReadMultiSpecChannelValues" 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadMultiSpecClassNames
//
//	Software purpose:	The purpose of this routine is to read the class
//							names from a MultiSpec classification file.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			LoadClassNameDescriptions in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/17/1990
//	Revised By:			Larry L. Biehl			Date: 03/22/2019

Boolean ReadMultiSpecClassNames (
				FileInfoPtr							fileInfoPtr,
				Handle								windowInfoHandle,
				UCharPtr								classNamePtr)

{
	SInt64								fileSize;

	CMFileStream*						fileStreamPtr;

	HUCharPtr							ioBufferPtr,
											strPtr;

	UInt16*								classSymbolPtr;
	
	SInt64								lContBlock;

	UInt32								bufferSize,
											classIndex,
											count,
											maxBufferSize = 2000,
											minBufferSize = 500,
											filePosOffset,
											nameLength;

	SInt16								errCode,
											nextSet,
											stringLength;


	if (fileInfoPtr == NULL || windowInfoHandle == NULL || classNamePtr == NULL)
																							return (FALSE);

	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr == NULL)
																							return (FALSE);

			// Get a pointer to storage for the class symbols.

	classSymbolPtr = (UInt16*)&classNamePtr[fileInfoPtr->numberClasses * sizeof (Str31)];

			// Get a buffer to read in the header part of the file.  Try to
			// get as large a buffer as possible without being larger than			
			// file itself and not more than maxBufferSize.								

	errCode = GetSizeOfFile (fileInfoPtr, &fileSize);
	MGetFreeMemory (&lContBlock);
	bufferSize = (UInt32)MIN (fileSize, maxBufferSize);
	bufferSize = MIN (bufferSize, lContBlock - 20000);
	bufferSize = MAX (bufferSize, minBufferSize);

	ioBufferPtr = (HUCharPtr)MNewPointer (bufferSize);
	if (ioBufferPtr == NULL)
																							return (FALSE);

			// Read the header data and get the class names and symbols.

	filePosOffset = 0;
	classIndex = 0;
	nextSet = 1;
	while (classIndex < fileInfoPtr->numberClasses && errCode == noErr)
		{
		errCode = MSetMarker (fileStreamPtr,
									 fsFromStart,
									 filePosOffset,
									 kNoErrorMessages);
		if (errCode != eofErr)
			IOCheck (errCode, fileStreamPtr);

		if (filePosOffset > fileSize)
			errCode = eofErr;

				// Read in a buffer of data.

		if (errCode == noErr)
			{
			count = bufferSize - 1;
			errCode = MReadData (fileStreamPtr,
										  &count,
										  ioBufferPtr,
										  kNoErrorMessages);
			if (errCode != eofErr)
				IOCheck (errCode, fileStreamPtr);

			else	// errCode == eofErr 
				errCode = noErr;

			filePosOffset += bufferSize - 1;
			strPtr = ioBufferPtr;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Add null terminator to the buffer.

			ioBufferPtr[count] = kNullTerminator;

					// Find the start of the class list if needed.
					// Just after "Classes used: ... Symbol".

			if (nextSet == 1)
				{
				MGetString (gTextString2, kFileIOStrID, IDS_ClassesUsed); // 31
				if (gTextString2[0] == 0)
					errCode = -3;
				pstr ((char*)gTextString2, (char*)gTextString2, &stringLength);
				strPtr = (HUCharPtr)strstr (
											(char*)ioBufferPtr, (char*)gTextString2);
				if (strPtr != NULL)
					{
					strPtr += stringLength;
					nextSet = 2;

					}	// end "if (strPtr != NULL)"

				else	// strPtr == NULL
					filePosOffset -= stringLength;

				}	// end "if (nextSet == 1)"

			if (nextSet == 2)
				{
						// Get the class name and symbol for class n.

				while (classIndex < fileInfoPtr->numberClasses && strPtr != NULL)
					{
					strPtr = (HUCharPtr)strchr ((char*)strPtr, ':');
					if (strPtr != NULL)
						{
								// Make certain that we can read the entire class
								// name and symbol in before we reach the end of the	
								// buffer.  If we can't get ready to read the next		
								// buffer in.														

						if ((UInt32)(strPtr - ioBufferPtr + 35) > count)
							{
							filePosOffset -= (count + (SInt32)(ioBufferPtr - strPtr));
							strPtr = NULL;

							}	// end "if ((strPtr-ioBufferPtr+35 > count)"

						else	// (strPtr-ioBufferPtr+35) <= count 
							{
									// Get the class name and symbol for class n.

							strPtr += 2;
							BlockMoveData (strPtr, &classNamePtr[1], 31);

									// Do not count the trailing blanks in the class name.

							nameLength = 31;
							while (classNamePtr[nameLength] == ' ' && nameLength > 0)
								nameLength--;
							classNamePtr[0] = (char)nameLength;

							strPtr += 33;
							classSymbolPtr[classIndex] = strPtr[0];
							strPtr++;

									// Check if this is the threshold class.

							if (classIndex == 0 &&
									classSymbolPtr[0] == ' ' &&
									classNamePtr[1] != 'T')
								CopyPToP (classNamePtr, (UCharPtr)"\0background\0");

							classNamePtr += 32;
							classIndex++;

							}	// end "else (strPtr-ioBufferPtr+35) <= count"

						}	// end "if (strPtr != NULL)"

					}	// end "while (classIndex < fileInfoPtr->..."

				}	// end "if (nextSet == 2)"

			}	// end "if (errCode == noErr)"

		else	// errCode != noErr
			DisplayAlert (kErrorAlertID, kCautionAlert, kAlertStrID, IDS_Alert5, 0, NULL);

		}	// end "while (classIndex < fileInfoPtr->numberClasses ..."

			// If there was an error before all the class names could be
			// loaded in, finish the names with a default set.

	while (classIndex < fileInfoPtr->numberClasses)
		{
		CreateDefaultClassName (classNamePtr, classIndex + 1);

		classIndex++;
		classNamePtr += 32;

		}	// end "while (classIndex < fileInfoPtr->numberClasses)"

	CheckAndDisposePtr ((Ptr)ioBufferPtr);

	return (TRUE);

}	// end "ReadMultiSpecClassNames"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadPDSHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a PDS formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/14/1996
//	Revised By:			Larry L. Biehl			Date: 01/09/2020

SInt16 ReadPDSHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	char									localHeaderRecord[5000];
	 
	char									*ioBufferPtr,
											*stringPtr;

	CMFileStream*						fileStreamPtr;
	#if include_gdal_capability
		FileStringPtr						fileNamePtr;
	#endif

	SInt32								values[3];

	UInt32								bufferSize,
											count,
											posOff,
											recordBytes;

	SInt16								errCode = noErr,
											fileType = 0,
											index,
											nextSet,
											numberAxes,
											returnCode,
											stringCompare,
											stringNumber[3],
											tReturnCode;

	Boolean								atEndOfFile,
											notFound;


			// Initialize local variables.													

	returnCode = 1;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

	if (fileInfoPtr != NULL && fileStreamPtr != NULL)
		{
				// First check for PDS identifier.
				
		stringCompare = -1;
		if (MGetString (gTextString2, kFileIOStrID, IDS_PDSIdentifier)) // 78
			{
			CopyPascalStringToC ((ConstStr255Param)gTextString2, (char*)gTextString2);
			stringCompare = strncmp ((char*)headerRecordPtr, (char*)gTextString2, 12);							
			
			if (stringCompare == 0)
				fileType = kPDSType;
				
			}	// end "if (MGetString (gTextString2, ...)"

		if (fileType == 0)
			{
			if (MGetString (gTextString2, kFileIOStrID, IDS_FileIO152))	// PDS_VERSION_ID
				{
				CopyPascalStringToC ((ConstStr255Param)gTextString2, (char*)gTextString2);
				stringCompare = strncmp ((char*)headerRecordPtr, (char*)gTextString2, 14);		
			
				if (stringCompare == 0)
					{
					fileType = -10;
					stringPtr = headerRecordPtr;
					
					}	// end "if (stringCompare == 0)"

            }	// end "if (MGetString (gTextString2, kFileIOStrID, IDS_FileIO152))"
				
			}	// end "if (fileType == 0)"
				
		if (fileType == 0)
			{
					// No pds identifer. Now check if this is a .img file. If so there
					// may be a .lbl file with the pds header information
			
			FileStringPtr		filePathPtr;		
			UInt16				numSuffixChars;
		
			filePathPtr =
							(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		
			if (CompareSuffixNoCase ((char*)"\0.img", 
												filePathPtr,
												&numSuffixChars))
				{
						// Now see if .lbl file exists
						
				if (CheckForPDS_LBL_File (fileInfoPtr, localHeaderRecord))
					{
							// Now check for identifier in the label file.
							
					if (MGetString (gTextString2, kFileIOStrID, IDS_FileIO152))	// PDS_VERSION_ID
						{
						CopyPascalStringToC ((ConstStr255Param)gTextString2,
														(char*)gTextString2);
						stringCompare = strncmp ((char*)&localHeaderRecord[0],
															(char*)gTextString2,
															14);
			
						if (stringCompare == 0)
							{
							fileType = -20;
							stringPtr = &localHeaderRecord[0];
							
							}	// end "if (stringCompare == 0)"

						}	// end "if (MGetString (gTextString2, kFileIOStrID, IDS_FileIO152))"
						
					}	// end "if (CheckForPDS_LBL_File (fileInfoPtr, headerRecordPtr))"
				
				}	// end "if (CompareSuffixNoCase ((char*)"\0.img", ..."
				
			}	// end "if (fileType == 0)"
											
		if (fileType < 0)
			{
			#if include_gdal_capability
				if (stringCompare == 0)
					{
							// This is a PDS type of file. Check if the version is PDS3. If so
							// then allow gdal to read the file if gdal library is available.
							
					tReturnCode = GetFileHeaderString (kFileIOStrID,
																  IDS_FileIO152,		// PDS_VERSION_ID
																  &stringPtr,
																  4,
																  kSkipEqual,
																  (char*)gTextString2,
																	0,
																  50,
																  kNoSubstringAllowed);
																  
					if (tReturnCode == 0)
						{
						stringCompare = CompareStringsNoCase (&gTextString2[1],
																			(UCharPtr)"pds3",
																			4);
						
						if (stringCompare == 0)
							{		
									// Let gdal handle this version of pds
							stringCompare = 1;
							
									// If LBL file found to go with selected image file, then
									// need to change the file being used to the LBL file so
									// that gdal will handle it correctly.
									
							if (fileType == -20)
								{
								CloseFile (fileStreamPtr);
								fileNamePtr = (FileStringPtr)
												GetFilePathPPointerFromFileStream (fileStreamPtr);
								RemoveCharsNoCase ((char*)"\0.???\0", fileNamePtr);
								ConcatFilenameSuffix (fileNamePtr, (StringPtr)"\0.LBL\0");
								errCode = OpenFileReadOnly (fileStreamPtr,
																	  kResolveAliasChains,
																	  kLockFile,
																	  kVerifyFileStream);
								
								}	// end "if (fileType == -20)"
							
							}	// end "if (stringCompare == 0)"
						
						}	// end "if (tReturnCode == 0)"
					
					}	// end "if (stringCompare == 0)"
			#endif	// include_gdal_capability
					
			fileType = 0;
			if (stringCompare == 0)											  										  
				fileType = kPDSType;

			}	// end "if (fileType < 0)"

		}	// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																							return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

		if (gGetFileImageType == 0)
			gGetFileImageType = kMultispectralImageType;

				// Load the default values which are different than the initialized
				// MFileInfo structure values.													

		fileInfoPtr->numberHeaderBytes = 512;
		fileInfoPtr->numberLines = 100;
		fileInfoPtr->numberColumns = 100;
		fileInfoPtr->swapBytesFlag = gBigEndianFlag;

		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			fileInfoPtr->thematicType = TRUE;

		returnCode = noErr;

				// Signify that file is of "PDS" type. 		

		fileInfoPtr->format = kPDSType;
		fileInfoPtr->ancillaryInfoformat = kPDSType;

				// Get a 2048 byte buffer plus a little extra to read in the header.
				// I have seen PDS files that have seen have a 512 to 25K bytes header.
				// I have not seen the specs for the PDS format.

		bufferSize = 2050;

		ioBufferPtr = (char*)MNewPointer (bufferSize);
		if (ioBufferPtr == NULL)
																								return (1);

				// Get the needed parameters.

		posOff = 0;
		atEndOfFile = FALSE;
		notFound = TRUE;
		nextSet = 1;
		while (notFound && errCode == noErr)
			{
			errCode = MSetMarker (fileStreamPtr,
										  fsFromStart,
										  posOff,
										  kNoErrorMessages);
			if (errCode != eofErr)
				IOCheck (errCode, fileStreamPtr);

					// Read in a buffer of data.

			if (errCode == noErr)
				{
				count = 2048;
				errCode = MReadData (fileStreamPtr,
											&count,
											ioBufferPtr,
											kNoErrorMessages);
				if (errCode != eofErr)
					IOCheck (errCode, fileStreamPtr);

				else	// errCode == eofErr
					{
					atEndOfFile = TRUE;
					errCode = noErr;

					}	// end "else errCode == eofErr"

				posOff += count;

            }	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Add null terminator to the buffer.

				ioBufferPtr[count] = kNullTerminator;

						// Branch to set of information to get.

				if (nextSet == 1)
					{
							// Find "RECORD_BYTES" in the buffer.

					recordBytes = GetFileHeaderValue (kFileIOStrID,
																	IDS_FileIO79, // RECORD_BYTES
																	ioBufferPtr,
																	1,
																	kSkipEqual,
																	&tReturnCode);

					if (tReturnCode == 0)
						nextSet = 2;

					else	// tReturnCode != 0
						{
						if (tReturnCode < 0)
							errCode = tReturnCode;

						if (tReturnCode > 0)
							posOff -= tReturnCode;

						}	// end "else tReturnCode != 0"

					}	// end "if (nextSet == 1)"

						// Get next set of information.

				if (nextSet == 2)
					{
							// Find "LABEL_RECORDS" in the buffer

					count = GetFileHeaderValue (kFileIOStrID,
															IDS_FileIO80,
															ioBufferPtr,
															1,
															kSkipEqual,
															&tReturnCode);

					if (tReturnCode == 0)
						{
						fileInfoPtr->numberHeaderBytes = count * recordBytes;
						nextSet = 3;

						}	// end "if (tReturnCode == 0)"

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 2)"

				if (nextSet == 3)
					{
							// Find "LINE_SAMPLES" in the buffer.

					count = GetFileHeaderValue (kFileIOStrID,
															IDS_FileIO81,
															ioBufferPtr,
															1,
															kSkipEqual,
															&tReturnCode);

					if (tReturnCode == 0)
						{
						fileInfoPtr->numberColumns = count;
						nextSet = 4;

						}	// end "if (tReturnCode == 0)"

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 3)"

				if (nextSet == 4)
					{
							// Find "LINES" in the buffer.

					count = GetFileHeaderValue (kFileIOStrID,
															IDS_FileIO82,
															ioBufferPtr,
															1,
															kSkipEqual,
															&tReturnCode);

					if (tReturnCode == 0)
						{
						fileInfoPtr->numberLines = count;
						nextSet = 5;

						}	// end "if (tReturnCode == 0)"

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 4)"

				if (nextSet == 5)
					{
							// Find "BANDS" in the buffer.

					count = GetFileHeaderValue (kFileIOStrID,
															IDS_FileIO83,
															ioBufferPtr,
															1,
															kSkipEqual,
															&tReturnCode);

					if (tReturnCode == 0)
						{
						fileInfoPtr->numberChannels = (UInt16)count;
						nextSet = 6;

						}	// end "if (tReturnCode == 0)"

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 5)"

				if (nextSet == 6)
					{
							// Find "SAMPLE_BITS=" in the buffer.

					count = GetFileHeaderValue (kFileIOStrID,
															IDS_FileIO84,
															ioBufferPtr,
															1,
															kSkipEqual,
															&tReturnCode);

					if (tReturnCode == 0)
						{
						fileInfoPtr->numberBits = (SInt16)count;

						if (count > 8)
							fileInfoPtr->numberBytes = 2;

						nextSet = 7;

						}	// end "if (tReturnCode == 0)"

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 6)"

				if (nextSet == 7)
					{
							// Find "SAMPLE_TYPE=BYTE" in the buffer or
							// Find "SAMPLE_TYPE=INTEGER" in the buffer.

					index = 0;
					stringNumber[0] = IDS_FileIO85;
					stringNumber[1] = IDS_FileIO86;
					do {
						count = GetFileHeaderValue (kFileIOStrID,
																stringNumber[index],
																ioBufferPtr,
																0,
																kDoNotSkipEqual,
																&tReturnCode);

						if (tReturnCode == 0)
							{
							fileInfoPtr->numberBytes = index + 1;

							if (index == 1)
								fileInfoPtr->signedDataFlag = TRUE;

							nextSet = 8;

							}	// end "if (tReturnCode == 0)"

						index++;

						}	while (tReturnCode > 0 && index < 3);

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 7)"

				if (nextSet == 8)
					{
							// Find "BAND_STORAGE=LINE INTERLEAVED" in the buffer or
							// Find "BAND_STORAGE=BAND INTERLEAVED" in the buffer or
							// Find "BAND_STORAGE=SAMPLE INTERLEAVED" in the buffer.

					index = 0;
					stringNumber[0] = IDS_FileIO87;
					stringNumber[1] = IDS_FileIO88;
					stringNumber[2] = IDS_FileIO89;
					do {
						count = GetFileHeaderValue (kFileIOStrID,
																stringNumber[index],
																ioBufferPtr,
																0,
																kDoNotSkipEqual,
																&tReturnCode);

						if (tReturnCode == 0)
							{
							fileInfoPtr->bandInterleave = index + 1;
							nextSet = 9;

							}	// end "if (tReturnCode == 0)"

						index++;

						}	while (tReturnCode > 0 && index < 3);

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 8)"

				if (nextSet == 9)
					{
							// Find "BYTEORDER=" in the buffer.

					count = GetFileHeaderValue (kFileIOStrID,
															IDS_FileIO90,
															ioBufferPtr,
															1,
															kDoNotSkipEqual,
															&tReturnCode);

					if (tReturnCode == 0)
						{
						if (count == 12)
							fileInfoPtr->swapBytesFlag = gBigEndianFlag;

						else	// count == 21    
							fileInfoPtr->swapBytesFlag = !gBigEndianFlag;

						}	// end "if (tReturnCode == 0)"

							// All information has been found. The byte order
							// is not required.

					notFound = FALSE;

					if (tReturnCode < 0)
						errCode = tReturnCode;

					}	// end "if (nextSet == 9)"

				if (notFound && nextSet == 3)
					{
							// Now try to see if this is a Qub type of file.
							// Find "AXES=" in the buffer.

					numberAxes = (SInt16)GetFileHeaderValue (kFileIOStrID,
																			IDS_FileIO153,
																			ioBufferPtr,
																			1,
																			kSkipEqual,
																			&tReturnCode);

					if (tReturnCode == 0 && numberAxes == 3)
						nextSet = 4;

					if (tReturnCode < 0)
						errCode = tReturnCode;

					else if (tReturnCode > 0)
						posOff -= tReturnCode;

					if (nextSet == 4)
						{
						fileInfoPtr->bandInterleave = kBIL;

								// Find "AXIS NAME=" in the buffer.

						count = GetFileHeaderValue (kFileIOStrID,
																IDS_FileIO154,
																ioBufferPtr,
																0,
																kDoNotSkipEqual,
																&tReturnCode);

						if (tReturnCode == 0)
							{
							fileInfoPtr->bandInterleave = kBSQ;
							nextSet = 5;

							}	// end "if (tReturnCode == 0)"

						if (tReturnCode < 0)
							errCode = tReturnCode;

						else if (tReturnCode > 0)
							posOff -= tReturnCode;

						}	// end "if (nextSet == 4)"

					if (nextSet == 5)
						{
								// Find "CORE_ITEMS=" in the buffer.

						count = GetFileHeaderValues (kFileIOStrID,
																IDS_FileIO155,
																ioBufferPtr,
																numberAxes,
																values,
																kSkipEqual,
																&tReturnCode);

						if (tReturnCode == 0)
							{
							switch (fileInfoPtr->bandInterleave)
								{
								case kBIL:
									fileInfoPtr->numberColumns = values[0];
									fileInfoPtr->numberChannels = (UInt16)values[1];
									fileInfoPtr->numberLines = values[2];
									break;

								case kBSQ:
									fileInfoPtr->numberColumns = values[0];
									fileInfoPtr->numberLines = values[1];
									fileInfoPtr->numberChannels = (UInt16)values[2];
									break;

								}	// end "switch (fileInfoPtr->bandInterleave)"

							nextSet = 6;

							}	// end "if (tReturnCode == 0)"

						if (tReturnCode < 0)
							errCode = tReturnCode;

						else if (tReturnCode > 0)
							posOff -= tReturnCode;

						}	// end "if (nextSet == 5)"

					if (nextSet == 6)
						{
								// Find "CORE_ITEM_BYTES=" in the buffer.

						count = GetFileHeaderValue (kFileIOStrID,
																IDS_FileIO156,
																ioBufferPtr,
																1,
																kSkipEqual,
																&tReturnCode);

						if (tReturnCode == 0)
							{
							fileInfoPtr->numberBytes = (UInt16)count;

							if (fileInfoPtr->numberBytes > 1)
								fileInfoPtr->signedDataFlag = TRUE;

							nextSet = 7;

							}	// end "if (tReturnCode == 0)"

						if (tReturnCode < 0)
							errCode = tReturnCode;

						else if (tReturnCode > 0)
							posOff -= tReturnCode;

						}	// end "if (nextSet == 6)"

					if (nextSet == 7)
						{
								// Find "CORE_ITEM_TYPE=" in the buffer.

						count = GetFileHeaderValue (kFileIOStrID,
																IDS_FileIO157,
																ioBufferPtr,
																0,
																kDoNotSkipEqual,
																&tReturnCode);

						if (tReturnCode == 0)
							fileInfoPtr->swapBytesFlag = gBigEndianFlag;

								// All information has been found. The byte order
								// is not required.

						notFound = FALSE;

						}	// end "if (nextSet == 7)"

					}	// end "if (notFound && nextSet == 3)"

				}	// end "if (errCode == noErr)"

			if (notFound && (posOff + 512 >= fileInfoPtr->numberHeaderBytes))
				atEndOfFile = TRUE;

			if (atEndOfFile && notFound)
				{
						// Display an alert if the needed parameters were not
						// found in the file.

				tReturnCode = DisplayAlert (
                        kErrorAlertID, kStopAlert, kAlertStrID, IDS_Alert52, 0, NULL);
				errCode = eofErr;

            }	// end "if (atEndOfFile && notFound)"

			}	// end "while (notFound && errCode == noErr)"

		if (returnCode == noErr && errCode != noErr)
			returnCode = 1;

		CheckAndDisposePtr (ioBufferPtr);

		}	// end "if (fileType != 0)"

	return (returnCode);

}	// end "ReadPDSHeader"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadSPOTHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a SPOT header record.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if SPOT header record parameters make sense
//							1 - if SPOT header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/09/1999
//	Revised By:			Larry L. Biehl			Date: 11/09/1999

SInt16 ReadSPOTHeader (
				FileInfoPtr 						fileInfoPtr,
				char*									headerRecordPtr)

{
	CMFileStream						*fileStreamPtr;

	SInt32								returnValue;

	UInt32								count;

	SInt16								errCode,
											returnCode = 1,
											sscanfReturnCode;
	
			
	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
		returnCode = 0;
				
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

						  // Get the band interleave format

		if (strncmp (&headerRecordPtr[60], "BIL", 3) == 0)
				fileInfoPtr->bandInterleave = kBIL;

		else if (strncmp (&headerRecordPtr[60], "BSQ", 3) == 0)
				fileInfoPtr->bandInterleave = kBSQ;

		else if (strncmp (&headerRecordPtr[60], "BIP", 3) == 0)
				fileInfoPtr->bandInterleave = kBIS;

				// Now read the next 128 bytes in to get the rest of the parameters.

		count = 128;
		errCode = MReadData (fileStreamPtr, &count, headerRecordPtr, kErrorMessages);

		if (errCode == noErr)
			{
					// Get the number of columns and the number of header bytes.
			
			sscanfReturnCode = sscanf (&headerRecordPtr[60],
												"%ld",
												&returnValue);
 
			if (sscanfReturnCode == 1)
				{
				fileInfoPtr->numberHeaderBytes = returnValue;
				fileInfoPtr->numberColumns = returnValue;

				}	// end "if (sscanfReturnCode == 1)"

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Get the number of bits.

			sscanfReturnCode = sscanf (&headerRecordPtr[91], 
												"%ld",
												&returnValue);
 
			if (sscanfReturnCode == 1)
				fileInfoPtr->numberBits = (UInt16)returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Get the number of bytes.

			sscanfReturnCode = sscanf (&headerRecordPtr[95], 
												"%ld",
												&returnValue);
 
			if (sscanfReturnCode == 1)
				fileInfoPtr->numberBytes = (UInt16)returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Get the number of channels.

			sscanfReturnCode = sscanf (&headerRecordPtr[107], 
												"%ld",
												&returnValue);
 
			if (sscanfReturnCode == 1)
				fileInfoPtr->numberChannels = (UInt16)returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Get the number of lines.

			sscanfReturnCode = sscanf (&headerRecordPtr[111], 
												"%ld",
												&returnValue);
 
			if (sscanfReturnCode == 1)
				fileInfoPtr->numberLines = returnValue;

			else	// sscanfReturnCode != 1
				errCode = 1;

			}	// end "if (errCode == noErr)"

		if (errCode == noErr)
			{
					// Load the rest of the information for SPOT formatted files

			fileInfoPtr->format = kLGSOWGType;
			fileInfoPtr->swapBytesFlag = FALSE;
			fileInfoPtr->asciiSymbols = FALSE;
			fileInfoPtr->numberTrailerBytes = 0;
			fileInfoPtr->numberPreLineBytes = 0;
			fileInfoPtr->numberPostLineBytes = 0;
			fileInfoPtr->numberPreChannelBytes = 0;
			fileInfoPtr->numberPostChannelBytes = 0;

			}	// end "if (errCode == noErr)"
	
		else	// errCode != noErr
			returnCode = 1;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

  return (returnCode);
	
}	// end "ReadSPOTHeader"
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadSunScreenDumpHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a Sun screen dump header record.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/04/1989
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

SInt16 ReadSunScreenDumpHeader (
				FileInfoPtr							fileInfoPtr,
				SInt32*								headerRecordPtr,
				SInt16								formatOnlyCode)

{
	unsigned char						charKeyCode[4];

	SInt32								numberBytes;

	SInt16								fileType,
											returnCode;


			// Initialize local variables.

	returnCode = 1;
	fileType = 0;

	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
				// Check SUN screendump identifier.

		charKeyCode[0] = 0x59;
		charKeyCode[1] = 0xa6;
		charKeyCode[2] = 0x6a;
		charKeyCode[3] = 0x95;

		if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4) == 0)
			fileType = kSunScreenDumpType;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (formatOnlyCode != kLoadHeader)
		{
		fileInfoPtr->format = fileType;
																					return (returnCode);

		}	// end "if (formatOnlyCode != kLoadHeader)"

	if (fileType != 0)
		{
				// Set up swap bytes flag depending upon the system architecture

		gSwapBytesFlag = !gBigEndianFlag;

				// Signify that file is of "Sun Screen Dump type.

		fileInfoPtr->format = kSunScreenDumpType;
		fileInfoPtr->ancillaryInfoformat = kSunScreenDumpType;

		fileInfoPtr->numberColumns = GetLongIntValue ((char*)&headerRecordPtr[1]);

		fileInfoPtr->numberLines = GetLongIntValue ((char*)&headerRecordPtr[2]);

		fileInfoPtr->numberBits = (UInt16)GetLongIntValue ((char*)&headerRecordPtr[3]);
		fileInfoPtr->numberClasses = GetLongIntValue ((char*)&headerRecordPtr[7]) / 3;
		fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;

		fileInfoPtr->numberBytes = 1;
		fileInfoPtr->numberChannels = 1;
		fileInfoPtr->numberBins = 0;
		fileInfoPtr->bandInterleave = kBIL;

		fileInfoPtr->thematicType = TRUE;
		if (fileInfoPtr->numberBits > 8)
			{
			fileInfoPtr->numberBits = 8;

			if (gGetFileImageType != kThematicImageType)
				{
				fileInfoPtr->numberClasses = 0;
				fileInfoPtr->numberChannels = 3;
				fileInfoPtr->thematicType = FALSE;
				fileInfoPtr->bandInterleave = kBIS;
				gGetFileImageType = kMultispectralImageType;

				}	// end "if (gGetFileImageType != kThematicImageType)"

			else	// gGetFileImageType == kThematicImageType 
				{
				fileInfoPtr->numberClasses = 256;
				fileInfoPtr->maxClassNumberValue = 255;

				}	// end "else gGetFileImageType == kThematicImageType"

			}	// end "if (fileInfoPtr->numberBits > 8)"

		else	// fileInfoPtr->numberBits <= 8
			{
			if (gGetFileImageType != kMultispectralImageType)
				gGetFileImageType = kThematicImageType;

			}	// end "else fileInfoPtr->numberBits <= 8"

		if (fileInfoPtr->numberBits > 1)
			fileInfoPtr->numberBins = (UInt32)ldexp ((double)1, fileInfoPtr->numberBits);

				// Load the rest of the information for SUN screen dump formatted files.

		fileInfoPtr->startColumn = 1;
		fileInfoPtr->startLine = 1;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->numberHeaderBytes =
											32 + GetLongIntValue ((char*)&headerRecordPtr[7]);
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 0;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;

				// If the number of bytes in a line of data is odd,
				// indicate that there is a post calibration byte, since the 		
				// number of bytes in the file is rounded out to a multiple of 16	
				// bits.																				

		fileInfoPtr->numberPostLineBytes = 0;
		numberBytes = fileInfoPtr->numberColumns * fileInfoPtr->numberChannels;
		if (numberBytes & 0x0001)
			fileInfoPtr->numberPostLineBytes = 1;

		returnCode = noErr;

		gSwapBytesFlag = FALSE;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	return (returnCode);

}	// end "ReadSunScreenDumpHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadTGAHeader
//
//	Software purpose:	The purpose of this routine is to read the TGA header. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 06/29/1993
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

SInt16 ReadTGAHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	SInt32*								longHeaderRecordPtr;

	SInt16								fileType = 0,
											returnCode = 1,
											tempInt,
											tempValue;


	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
		longHeaderRecordPtr = (SInt32*)headerRecordPtr;

				// Check if image file is a TGA formatted file.

		fileType = kTGAType;

		//if (systemFileType == kTGAFileType)
		//		fileType = kTGAType;

				// Color Map Type field should be 0 or 1.

		if (headerRecordPtr[1] != 0x00 && headerRecordPtr[1] != 0x01)
			fileType = 0;

				// Image Type field should be 2 or 3 for MultiSpec to recognize.

		if (headerRecordPtr[2] != 0x02 && headerRecordPtr[2] != 0x03)
			fileType = 0;

				// If the Color Map Type field is '0' then bytes 4-8 should be 0.

		if (headerRecordPtr[1] == 0x00 &&
				 (headerRecordPtr[3] != 0x00 || longHeaderRecordPtr[1] != 0x00))
			fileType = 0;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																							return (noErr);

			}	// end "if (formatOnlyCode != kLoadHeader)"

				// Get the number of columns in the image.

		BlockMoveData ((char*)&headerRecordPtr[12], (char*)&tempInt, 2);
		tempValue = 0;
		tempValue = ((tempInt & 0xff00) >> 8) | ((tempInt & 0x00ff) << 8);
		fileInfoPtr->numberColumns = tempValue;

				// Get the number of lines in the image.

		BlockMoveData ((char*)&headerRecordPtr[14], (char*)&tempInt, 2);
		tempValue = 0;
		tempValue = ((tempInt & 0xff00) >> 8) | ((tempInt & 0x00ff) << 8);
		fileInfoPtr->numberLines = tempValue;

				// Get number of channels in the multispectral image.

		fileInfoPtr->numberChannels = 1;
		if (headerRecordPtr[2] == 2)
			fileInfoPtr->numberChannels = 3;

				// Make certain that the number of bits per pixel makes sense
				// relative to the number of channels in the image.

		if (fileInfoPtr->numberChannels * 8 != (UInt16)headerRecordPtr[16])
																					return (returnCode);

				// Load the rest of the information for TGA formatted file.

		fileInfoPtr->numberHeaderBytes = 18 + headerRecordPtr[0];
		fileInfoPtr->numberBytes = 1;
		fileInfoPtr->numberBits = 8;
		fileInfoPtr->numberBins = 256;
		fileInfoPtr->format = kTGAType;
		fileInfoPtr->ancillaryInfoformat = kTGAType;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->asciiSymbols = FALSE;

		fileInfoPtr->bandInterleave = kBIL;
		if (fileInfoPtr->numberChannels == 3)
			fileInfoPtr->bandInterleave = kBIS;

		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 0;
		fileInfoPtr->numberPostLineBytes = 0;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;
		fileInfoPtr->startColumn = 1;
		fileInfoPtr->startLine = 1;

		if (gGetFileImageType == 0)
			gGetFileImageType = kMultispectralImageType;

		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			fileInfoPtr->thematicType = TRUE;

		returnCode = noErr;

		}	// end "if (fileType != 0)"

	return (returnCode);

}	// end "ReadTGAHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadThematicGroups
//
//	Software purpose:	The purpose of this routine is to read the thematic
//							group information either from a thematic color file or from
//							the end of the image file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			LoadThematicDisplaySpecs in SDisplayThematic.cpp
//							LoadThematicGroups in SSaveWrite.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/10/2011
//	Revised By:			Larry L. Biehl			Date: 06/05/2011

Boolean ReadThematicGroups (
				FileInfoPtr							inputFileInfoPtr,
				FileInfoPtr							outputFileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr)

{
	Handle activeImageWindowInfoHandle;
	Boolean continueFlag;


	if (inputFileInfoPtr->ancillaryInfoformat == kArcViewDefaultSupportType) 
		{
		if (gProcessorCode == kOpenThematicInfo) 
			{
					// For now assume that the number of classes in the input clr file
					// is the same as in the thematic image file. This will be sorted
					// out for sure later.

			inputFileInfoPtr->numberClasses = outputFileInfoPtr->numberClasses;
			inputFileInfoPtr->classDescriptionH = outputFileInfoPtr->classDescriptionH;

			}	// end "if (gProcessorCode == kOpenThematicInfo)"

		activeImageWindowInfoHandle = GetActiveImageWindowInfoHandle ();
		continueFlag = ReadArcViewGroups (inputFileInfoPtr,
														outputFileInfoPtr,
														displaySpecsPtr,
														activeImageWindowInfoHandle);

		if (gProcessorCode == kOpenThematicInfo) 
			{
					// Now make sure that we do not loose the class description handle
					// when the input file information structure goes away.

			inputFileInfoPtr->classDescriptionH = NULL;

			}	// end "if (gProcessorCode == kOpenThematicInfo)"

		}	// end "...->ancillaryInfoformat == kArcViewDefaultSupportType"

	else	// inputFileInfoPtr->ancillaryInfoformat != kArcViewDefaultSupportType
		continueFlag = ReadThematicGroupsFromImageFile (inputFileInfoPtr,
																			outputFileInfoPtr,
																			displaySpecsPtr);

	if (continueFlag)
				// Indicate now that the colors are user defined so that that
				// no attempt is made to read them in again.
		displaySpecsPtr->thematicGroupPaletteType = kUserDefinedColors;

	return (continueFlag);

}	// end "ReadThematicGroups"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadThematicGroupsFromImageFile
//
//	Software purpose:	The purpose of this routine is to read the thematic
//							groups at the end of the thematic image file. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ReadThematicGroups in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/10/1992
//	Revised By:			Larry L. Biehl			Date: 02/16/2016

Boolean ReadThematicGroupsFromImageFile (
				FileInfoPtr							inputFileInfoPtr,
				FileInfoPtr							outputFileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr)

{
	RGBColor								theColor;

	CMFileStream*						fileStreamPtr;
	ColorSpec*							colorSpecPtr;
	CTabHandle							cTableHandle;
	CTabPtr								cTablePtr;

	char									*groupNamePtr,
											*stringPtr;

	SInt16								*classToGroupPtr,
											*groupToPalettePtr;

	Handle								groupNameHandle,
											groupTablesHandle;

	SInt64								filePosition;

	SInt32								colorTableSize;

	UInt32								bytesNeeded,
											classIndex,
											count,
											nameLength;

	SInt16								returnCode;

	UInt16								group,
											groupCount,
											numberCharacters,
											numberGroups,
											paletteOffset;

	Boolean								changedFlag,
											continueFlag,
											groupToClassFlag;


	if (displaySpecsPtr == NULL ||
				inputFileInfoPtr == NULL ||
						outputFileInfoPtr == NULL ||
								gCharBufferHandle1 != NULL)
																							return (FALSE);

	fileStreamPtr = GetFileStreamPointer (inputFileInfoPtr);
	if (fileStreamPtr == NULL)
																							return (FALSE);

	cTableHandle = NULL;
	groupNameHandle = NULL;
	groupTablesHandle = NULL;

			// First determine if any group descriptions exist for the input
			// image window.
			// Get the size of the image without any group descriptions.

	filePosition = GetSizeOfImage (inputFileInfoPtr);

			// Set file pointer to end of image data.  If no eof error, then
			// read in characters to check for group description identifier.

	returnCode = MSetMarker (fileStreamPtr,
			fsFromStart,
			filePosition,
			kNoErrorMessages);
			
	if (returnCode != eofErr)
		IOCheck (returnCode, fileStreamPtr);

	continueFlag = (returnCode == noErr);

	if (continueFlag) 
		{
				// Not at end of the file, now check if characters read in			
				// match the group identifier.												 

		continueFlag =
				 MGetString (gTextString2, kFileIOStrID, IDS_GroupDescriptions); // 55
		count = gTextString2[0];

		}	// end "if (continueFlag)" 

	if (continueFlag) 
		{
		returnCode = MReadData (fileStreamPtr,
										&count,
										&gTextString[0],
										kNoErrorMessages);

		if (returnCode != eofErr)
			IOCheck (returnCode, fileStreamPtr);

		continueFlag = (returnCode == noErr);

		}	// end "if (continueFlag)" 

	if (continueFlag) 
		{
		if (strncmp ((char*)gTextString, (char*)&gTextString2[1], gTextString2[0]) != 0)
			continueFlag = FALSE;

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
				// Get temporary space for reading the group descriptions.
				// Allow for the null terminator. 			

		bytesNeeded = 5 * inputFileInfoPtr->numberClasses + 1;
		bytesNeeded = MAX (bytesNeeded, 256);
		gCharBufferPtr1 = (char*)MNewPointer (bytesNeeded);
		continueFlag = (gCharBufferPtr1 != NULL);

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
				// Read in the summary part of the information.							

		count = 23;
		returnCode = MReadData (fileStreamPtr,
										&count,
										gCharBufferPtr1,
										kNoErrorMessages);
		gCharBufferPtr1[count] = kNullTerminator;

		if (returnCode == noErr) 
			{
			returnCode = sscanf (gCharBufferPtr1,
										" 1 1 9 Grouping1 %hd %hd",
										&numberGroups,
										&paletteOffset);

			if (returnCode == 2) 
				{
				if (numberGroups <= 0)
					continueFlag = FALSE;

				}	// end "if (returnCode == 1)"

			else	// returnCode != 1 
				continueFlag = FALSE;

			}	// end "if (returnCode == noErr)"

		else	// returnCode != noErr 
			continueFlag = FALSE;

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
		Boolean		groupNameChangedFlag,
						groupTableChangedFlag;

				// First make certain that we have memory allocated for 			
				// a class to grouping table. Allow for the same number of groups
				// as classes.

		continueFlag = GetGroupStructureMemory (
											MAX (inputFileInfoPtr->numberClasses, numberGroups),
											&groupTablesHandle,
											&groupNameHandle,
											&groupToPalettePtr,
											&groupNamePtr,
											&groupTableChangedFlag,
											&groupNameChangedFlag);

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
				// Read the class to group table.											

		count = inputFileInfoPtr->numberClasses * 5;
		returnCode = MReadData (fileStreamPtr,
										&count,
										gCharBufferPtr1,
										kErrorMessages);
		gCharBufferPtr1[count] = kNullTerminator;
		continueFlag = (returnCode == noErr);

		if (continueFlag) 
			{
			classToGroupPtr = &groupToPalettePtr[256];
			stringPtr = gCharBufferPtr1;
			for (classIndex = 0;
					  classIndex < inputFileInfoPtr->numberClasses;
							classIndex++)
				{
				returnCode = sscanf (stringPtr,
											" %hd",
											&classToGroupPtr[classIndex]);

				if (returnCode != 1) 
					{
					continueFlag = FALSE;
					break;

					}	// end "if (returnCode != 1)"

				stringPtr += 5;
				classToGroupPtr[classIndex]--;

				}	// end "for (classIndex=0; ..."

			}	// end "if (continueFlag)"

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
				// Get memory for the color table.										

		colorTableSize = displaySpecsPtr->numberPaletteLevels;
		colorTableSize = MIN (256, colorTableSize);
		bytesNeeded = colorTableSize * sizeof (ColorSpec) + sizeof (ColorTable);
		cTablePtr = (CTabPtr) CheckHandleSize ((Handle*)&cTableHandle,
															 &continueFlag,
															 &changedFlag,
															 bytesNeeded);

		if (cTableHandle != NULL) 
			{
			colorSpecPtr = (ColorSpec*)&cTablePtr->ctTable;

			cTablePtr->ctSeed = GetCTSeed ();
			cTablePtr->ctFlags = 0;
			cTablePtr->ctSize = (SInt16)(colorTableSize - 1);

			}	// end "if (cTableHandle)"

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
				// Get the current file position.											

		returnCode = MGetMarker (fileStreamPtr,
											&filePosition,
											kErrorMessages);
		continueFlag = (returnCode == noErr);

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
				// Read the group names and colors.											

		groupCount = 0;
		for (group = 0; group < numberGroups; group++) 
			{
			groupToClassFlag = TRUE;

					// If this is not the first time through the loop, set the 		
					// file position for the start of the next group.					

			if (group > 0) 
				{
				returnCode = MSetMarker (fileStreamPtr,
													fsFromStart,
													filePosition,
													kErrorMessages);
				continueFlag = (returnCode == noErr);

				}	// end "if (group > 0)"

			if (continueFlag) 
				{
						// Read in the header part of the information.					

				count = 53;
				returnCode = MReadData (fileStreamPtr,
													&count,
													gCharBufferPtr1,
													kNoErrorMessages);
				gCharBufferPtr1[count] = kNullTerminator;

				if (returnCode != eofErr) 
					{
					IOCheck (returnCode, fileStreamPtr);
					continueFlag = (returnCode == noErr);

					}	// end "if (errCode != eofErr)"

				}	// end "if (continueFlag)"

			if (continueFlag) 
				{
				stringPtr = gCharBufferPtr1;

						// Get the number of characters in the group name.
						// Maximum number of characters for a group name is 31.				

				returnCode = sscanf (stringPtr,
											" %hd ",
											&numberCharacters);
				continueFlag = (returnCode == 1);
				
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
				stringPtr += 6;
				numberCharacters = MIN (numberCharacters, 31);

						// Get the group name.													

				BlockMoveData (stringPtr, &groupNamePtr[1], numberCharacters);
				groupNamePtr[0] = (UInt8)numberCharacters;
				stringPtr += numberCharacters;

						// Do not count the trailing blanks in the group name.

				nameLength = numberCharacters;
				while (groupNamePtr[nameLength] == ' ' && nameLength > 0)
					nameLength--;
				groupNamePtr[0] = (char)nameLength;

						// Get the group color.													

				returnCode = sscanf (stringPtr,
											" %hx %hx %hx",
											&theColor.red,
											&theColor.green,
											&theColor.blue);
				continueFlag = (returnCode == 3);	
				
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
				#ifdef multispec_wx
					theColor.red = theColor.red >> 8;
					theColor.green = theColor.green >> 8;
					theColor.blue = theColor.blue >> 8;     
				#endif
				
				for (classIndex = 0;
						classIndex < inputFileInfoPtr->numberClasses;
						classIndex++) 
					{
					if (classToGroupPtr[classIndex] == (SInt16)group) 
						{
						colorSpecPtr[classIndex].rgb = theColor;

						if (groupToClassFlag) 
							{
							groupToPalettePtr[groupCount] = (UInt16)classIndex;
							groupToClassFlag = FALSE;

							}	// end "if (groupToClassFlag)"

						}	// end "if (classToGroupPtr[classIndex] == group)"

					}	// end "for (classIndex=0; ..."

				filePosition += 6 + numberCharacters + 15;  

				if (!groupToClassFlag) 
					{
					groupCount++;
					groupNamePtr += 32;

					}	// end "if (!groupToClassFlag)"

				}	// end "if (continueFlag)"

			if (!continueFlag)
				break;

			}	// end "for (group=0; ..."

		}	// end "if (continueFlag)"

	if (continueFlag) 
		{
		outputFileInfoPtr->numberGroups = groupCount;

		UnlockAndDispose ((Handle)displaySpecsPtr->savedGroupCTableHandle);
		displaySpecsPtr->savedGroupCTableHandle = cTableHandle;

		UnlockAndDispose (outputFileInfoPtr->groupNameHandle);
		outputFileInfoPtr->groupNameHandle = groupNameHandle;

		UnlockAndDispose (outputFileInfoPtr->groupTablesHandle);
		outputFileInfoPtr->groupTablesHandle = groupTablesHandle;

				// Get the number of entries in the palette that are used for		
				// classes/groups.  3 colors are always allowed for black, white	
				// and the background color.													

		displaySpecsPtr->numPaletteEntriesUsed =
									(UInt16)MIN (256, displaySpecsPtr->numberPaletteLevels);
		displaySpecsPtr->paletteOffset = 0;

		gPaletteOffset = displaySpecsPtr->paletteOffset;
		gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;

		}	// end "if (continueFlag)"

	else	// !continueFlag
		{
		UnlockAndDispose ((Handle)cTableHandle);
		UnlockAndDispose (groupNameHandle);
		UnlockAndDispose (groupTablesHandle);

		}	// end "if (!continueFlag)"

	CheckAndUnlockHandle ((Handle)displaySpecsPtr->savedGroupCTableHandle);
	CheckAndUnlockHandle (outputFileInfoPtr->groupNameHandle);
	CheckAndUnlockHandle (outputFileInfoPtr->groupTablesHandle);
	gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);

	return (continueFlag);

}	// end "ReadThematicGroupsFromImageFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadVICARHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a VICAR formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/05/1991
//	Revised By:			Larry L. Biehl			Date: 03/22/2019

SInt16 ReadVICARHeader (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	SInt64								fileSize;

	char									*ioBufferPtr;
											//*stringPtr;

	CMFileStream*						fileStreamPtr;
	
	SInt64								lContBlock;

	UInt32								bufferSize,
											count,
											maxBufferSize = 3000,
											minBufferSize = 1000,
											posOff;

	SInt16								errCode,
											fileType,
											index,
											nextSet,
											returnCode,
											stringCompare,
											stringNumber[3],
											tReturnCode;

	Boolean								atEndOfFile,
											notFound;


			// Initialize local variables.

	fileType = 0;
	ioBufferPtr = NULL;
	returnCode = 1;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);

	if (fileInfoPtr != NULL && fileStreamPtr != NULL)
		{
		stringCompare = -1;
		if (MGetString (gTextString2, kFileIOStrID, IDS_VICARLabelSize)) // 44
			{
			//stringPtr = PtoCstr (gTextString2);
			CopyPascalStringToC ((ConstStr255Param)gTextString2, (char*)gTextString2);
			stringCompare = strncmp (headerRecordPtr, (char*)gTextString2, 8);

			}	// end "if (gTextString2[0] != 0)"

		if (stringCompare == 0)
			fileType = kVICARType;

		}	// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"

	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																													return (0);

			}	// end "if (formatOnlyCode != kLoadHeader)"

		if (gGetFileImageType == 0)
			gGetFileImageType = kMultispectralImageType;

				// Load the default values.

		fileInfoPtr->numberLines = 100;
		fileInfoPtr->numberColumns = 100;
		fileInfoPtr->numberClasses = 1;
		fileInfoPtr->startColumn = 1;
		fileInfoPtr->startLine = 1;
		fileInfoPtr->numberHeaderBytes = 0;

				// Load the rest of the information into the file information structure.

		fileInfoPtr->numberChannels = 1;
		fileInfoPtr->numberBits = 8;
		fileInfoPtr->numberBytes = 1;
		fileInfoPtr->numberBins = 256;
		fileInfoPtr->swapBytesFlag = gBigEndianFlag;
		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->bandInterleave = kBIL;

		fileInfoPtr->thematicType = FALSE;
		if (gGetFileImageType == kThematicImageType)
			fileInfoPtr->thematicType = TRUE;

		returnCode = noErr;

				// Signify that file is of "VICAR" type.

		fileInfoPtr->format = kVICARType;
		fileInfoPtr->ancillaryInfoformat = kVICARType;

				// Get a buffer to read in the header part of the file.  Try to
				// get as large a buffer as possible without being larger than		
				// file itself and not more than maxBufferSize.							

		errCode = GetSizeOfFile (fileInfoPtr, &fileSize);
		MGetFreeMemory (&lContBlock);
		bufferSize = (UInt32)MIN (fileSize, maxBufferSize);
		bufferSize = MIN (bufferSize, lContBlock - 20000);
		bufferSize = MAX (bufferSize, minBufferSize);

		ioBufferPtr = (char*)MNewPointer (bufferSize);
		if (ioBufferPtr == NULL)
																								return (1);

				// Get the needed parameters.

		posOff = 0;
		atEndOfFile = FALSE;
		notFound = TRUE;
		nextSet = 1;
		while (notFound && errCode == noErr)
			{
			errCode = MSetMarker (fileStreamPtr,
										  fsFromStart,
										  posOff,
										  kNoErrorMessages);
			if (errCode != eofErr)
				IOCheck (errCode, fileStreamPtr);

					// Read in a buffer of data.

			if (errCode == noErr)
				{
				count = bufferSize - 1;
				errCode = MReadData (fileStreamPtr,
											&count,
											ioBufferPtr,
											kNoErrorMessages);
				if (errCode != eofErr)
					IOCheck (errCode, fileStreamPtr);

				else	// errCode == eofErr 
					{
					atEndOfFile = TRUE;
					errCode = noErr;

					}	// end "else errCode == eofErr"

				posOff += count;

				}	// end "if (errCode == noErr)"

			if (errCode == noErr)
				{
						// Add null terminator to the buffer.

				ioBufferPtr[count] = kNullTerminator;

						// Branch to set of information to get.

				if (nextSet == 1)
					{
							// Find "LBLSIZE=" in the buffer.

					fileInfoPtr->numberHeaderBytes = GetFileHeaderValue (
																				kFileIOStrID,
																				IDS_VICARLabelSize,
																				ioBufferPtr,
																				1,
																				kDoNotSkipEqual,
																				&tReturnCode); // 44

					if (tReturnCode == 0)
						nextSet = 2;

					else	// tReturnCode != 0 
						{
						if (tReturnCode < 0)
							errCode = tReturnCode;

						if (tReturnCode > 0)
								posOff -= tReturnCode;

						}	// end "else tReturnCode != 0" 

					}	// end "if (nextSet == 1)" 

						// Get next set of information.										

				if (nextSet == 2)
					{
					index = 0;
					stringNumber[0] = IDS_FileIO60;
					stringNumber[1] = IDS_FileIO45;
					do {
								// Find "FORMAT='HALF'" in the buffer or						
								// Find "FORMAT='BYTE'" in the buffer.							

						count = GetFileHeaderValue (kFileIOStrID,
																stringNumber[index],
																ioBufferPtr,
																0,
																kDoNotSkipEqual,
																&tReturnCode);

						if (tReturnCode == 0)
							{
							fileInfoPtr->numberBytes = index + 1;
							fileInfoPtr->numberBits = 8 * fileInfoPtr->numberBytes;

							fileInfoPtr->signedDataFlag = TRUE;
							if (fileInfoPtr->numberBytes == 1)
								fileInfoPtr->signedDataFlag = FALSE;

							nextSet = 3;

							}	// end "if (tReturnCode == 0)" 

						index++;

						} while (tReturnCode > 0 && index < 2);

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 2)" 

				if (nextSet == 3)
					{
					index = 0;
					stringNumber[0] = IDS_FileIO46;
					stringNumber[1] = IDS_FileIO58;
					stringNumber[2] = IDS_FileIO59;
					do {
								// Find "ORG='BIL'" in the buffer or							
								// Find "ORG='BSQ'" in the buffer or							 
								// Find "ORG='BIS'" in the buffer.								

						count = GetFileHeaderValue (kFileIOStrID,
															  stringNumber[index],
															  ioBufferPtr,
															  0,
															  kDoNotSkipEqual,
															  &tReturnCode);

						if (tReturnCode == 0)
							{
							fileInfoPtr->bandInterleave = index + 1;
							nextSet = 4;

							}	// end "if (tReturnCode == 0)" 

						index++;

						}	while (tReturnCode > 0 && index < 3);

					if (tReturnCode < 0)
						errCode = tReturnCode;

					if (tReturnCode > 0)
						posOff -= tReturnCode;

					}	// end "if (nextSet == 3)" 

				if (nextSet == 4)
					{
							// Find "NL=" in the buffer.										

					fileInfoPtr->numberLines = GetFileHeaderValue (kFileIOStrID,
																					IDS_FileIO47,
																					ioBufferPtr,
																					1,
																					kDoNotSkipEqual,
																					&tReturnCode);

					if (tReturnCode == 0)
						nextSet = 5;

					else	// tReturnCode != 0 
						{
						if (tReturnCode < 0)
							errCode = tReturnCode;

						if (tReturnCode > 0)
							posOff -= tReturnCode;

						}	// end "else tReturnCode != 0" 

					}	// end "if (nextSet == 4)" 

				if (nextSet == 5)
					{
							// Find "NS=" in the buffer.										

					fileInfoPtr->numberColumns = GetFileHeaderValue (kFileIOStrID,
																						IDS_FileIO48,
																						ioBufferPtr,
																						1,
																						kDoNotSkipEqual,
																						&tReturnCode);

					if (tReturnCode == 0)
						nextSet = 6;

					else	// tReturnCode != 0 
						{
						if (tReturnCode < 0)
							errCode = tReturnCode;

						if (tReturnCode > 0)
							posOff -= tReturnCode;

						}	// end "else tReturnCode != 0" 

					}	// end "if (nextSet == 5)" 

				if (nextSet == 6)
					{
							// Find "NB=" in the buffer.										

					fileInfoPtr->numberChannels = (UInt16)GetFileHeaderValue (
																						kFileIOStrID,
																						IDS_FileIO49,
																						ioBufferPtr,
																						1,
																						kDoNotSkipEqual,
																						&tReturnCode);

					if (tReturnCode == 0)
						nextSet = 7;

					else	// tReturnCode != 0 
						{
						if (tReturnCode < 0)
							errCode = tReturnCode;

						if (tReturnCode > 0)
							posOff -= tReturnCode;

						}	// end "else tReturnCode != 0" 

					}	// end "if (nextSet == 6)" 

						// Get next set of information.										

				if (nextSet == 7)
					{
							// Find "INTFMT='HIGH'" in the buffer.							

					count = GetFileHeaderValue (kFileIOStrID,
														 IDS_FileIO50,
														 ioBufferPtr,
														 0,
														 kDoNotSkipEqual,
														 &tReturnCode);

					if (tReturnCode >= 0)
						{
						if (tReturnCode == 0)
							fileInfoPtr->swapBytesFlag = !gBigEndianFlag;

						notFound = FALSE;

						}	// end "if (tReturnCode >= 0)" 

					else	// tReturnCode < 0 
						errCode = tReturnCode;

					}	// end "if (nextSet == 7)"									
				/*						
				if (nextSet == 7)
					{
							// Find "USER='AVIRIS'" in the buffer.							

					count = GetFileHeaderValue (kFileIOStrID, 
														 IDS_FileIO50, 
														 ioBufferPtr, 
														 0,
														 kDoNotSkipEqual, 
														 &tReturnCode);

					if (tReturnCode >= 0)
						{
						if (tReturnCode == 0)
							{
							fileInfoPtr->numberBits = 14;
							fileInfoPtr->swapBytesFlag = gBigEndianFlag;

							}	// end "if (tReturnCode == 0)" 

						notFound = FALSE;

						}	// end "if (tReturnCode >= 0)" 

					else	// tReturnCode < 0 
						errCode = tReturnCode;

					}	// end "if (nextSet == 7)" 
				*/
				}	// end "if (errCode == noErr)" 

			if (atEndOfFile && notFound)
				{
						// Display an alert if the needed parameters were not 		
						// found in the file.													

				tReturnCode = DisplayAlert (
							kErrorAlertID, kStopAlert, kAlertStrID, IDS_Alert20, 0, NULL);
				errCode = eofErr;

				}	// end "if (atEndOfFile && notFound)" 

			}	// end "while (notFound && errCode == noErr)" 

		if (returnCode == noErr && errCode != noErr)
			returnCode = 1;

		CheckAndDisposePtr (ioBufferPtr);

		}	// end "if (fileType != 0)"

	return (returnCode);

}	// end "ReadVICARHeader"


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadWindowsBitMapHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a Windows BitMap header record.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/25/2002
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

SInt16 ReadWindowsBitMapHeader	(
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	unsigned char						charKeyCode[2];

	UInt32								numberBytesPerLine;

	SInt32								compressionCode,
											numberOfColors,
											numberOfColorsUsed;

	SInt16								errorString,
											fileType,
											returnCode;


			// Initialize local variables.													

	returnCode = 1;
	fileType = 0;

	if (headerRecordPtr != NULL && fileInfoPtr != NULL)
		{
				//  Check Windows BitMap identifier.

		charKeyCode[0] = 0x42;
		charKeyCode[1] = 0x4D;

		if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 2) == 0)
			fileType = kWindowsBitMapType;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"

	if (formatOnlyCode != kLoadHeader)
		{
		fileInfoPtr->format = fileType;
																						return (returnCode);

		}	// end "if (formatOnlyCode != kLoadHeader)"

	if (fileType != 0)
		{
				// Set up swap bytes flag depending upon the system architecture

		gSwapBytesFlag = gBigEndianFlag;

				// Signify that file is of "Sun Screen Dump type. 						

		fileInfoPtr->format = kWindowsBitMapType;
		fileInfoPtr->ancillaryInfoformat = kWindowsBitMapType;
		fileInfoPtr->numberHeaderBytes = GetLongIntValue (&headerRecordPtr[10]);

		fileInfoPtr->numberColumns = GetLongIntValue (&headerRecordPtr[18]);

		fileInfoPtr->numberLines = GetLongIntValue (&headerRecordPtr[22]);

		fileInfoPtr->numberBits = (UInt16)GetLongIntValue (&headerRecordPtr[28]);

				// Check for compression.

		compressionCode = GetLongIntValue (&headerRecordPtr[30]);

				// Read the number of colors.

		numberOfColors = GetLongIntValue (&headerRecordPtr[46]);
		if (numberOfColors > 256 && fileInfoPtr->numberBits <= 8)
			numberOfColors = 0;

				// Read the number of colors used.

		numberOfColorsUsed = GetLongIntValue (&headerRecordPtr[50]);

		fileInfoPtr->numberBytes = 1;
		fileInfoPtr->numberChannels = 1;
		fileInfoPtr->numberBins = 0;
		fileInfoPtr->bandInterleave = kBIL;

		fileInfoPtr->thematicType = FALSE;
		if (fileInfoPtr->numberBits >= 8)
			{
			if (gGetFileImageType == kMultispectralImageType ||
					  (gGetFileImageType != kThematicImageType && numberOfColors == 0) ||
							(gGetFileImageType != kThematicImageType &&
									numberOfColors == 256 && fileInfoPtr->numberBits == 8)) 
				{
				if (fileInfoPtr->numberBits > 8) 
					fileInfoPtr->numberChannels = 3;

				else	// fileInfoPtr->numberBits == 8
					fileInfoPtr->numberChannels = 1;

				fileInfoPtr->numberBits = 8;
				fileInfoPtr->numberClasses = 0;
				fileInfoPtr->thematicType = FALSE;
				fileInfoPtr->bandInterleave = kBIS;
				gGetFileImageType = kMultispectralImageType;

				}	// end "if (gGetFileImageType != kThematicImageType && ..." 

			else	// gGetFileImageType == kThematicImageType || numberOfColors > 0 ...
				{
				fileInfoPtr->numberClasses = 256;
				gGetFileImageType = kThematicImageType;

				}	// end "else gGetFileImageType == kThematicImageType || ..."

			}	// end "if (fileInfoPtr->numberBits > 8)" 

		else	// fileInfoPtr->numberBits < 8 
			{
			if (gGetFileImageType == kMultispectralImageType)
				gGetFileImageType = kThematicImageType;

			}	// end "else fileInfoPtr->numberBits <= 8" 

		if (fileInfoPtr->numberBits > 1)
			fileInfoPtr->numberBins = (UInt32)ldexp ((double)1, fileInfoPtr->numberBits);

				// Get the number of post line bytes to make the total number of bytes in
				// each line of data a multiple of 4.

		numberBytesPerLine = fileInfoPtr->numberChannels * fileInfoPtr->numberColumns;
		numberBytesPerLine = ((numberBytesPerLine + 3) / 4) * 4;
		fileInfoPtr->numberPostLineBytes = numberBytesPerLine -
									fileInfoPtr->numberChannels * fileInfoPtr->numberColumns;

				// Load the rest of the information for Windows BitMap formatted files.																

		fileInfoPtr->startColumn = 1;
		fileInfoPtr->startLine = 1;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->numberPreLineBytes = 0;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;

		errorString = 0;
		if (fileInfoPtr->numberBits < 4)
			errorString = IDS_Alert113;

		else if (compressionCode != 0)
			errorString = IDS_Alert112;

		if (errorString == 0)
			returnCode = noErr;

		else	// end "errorString != 0"
			{
			DisplayAlert (kErrorAlertID,
								kNoteAlert,
								kAlertStrID,
								errorString,
								0,
								NULL);

			}	// end "if (compressionCode != 0)"

		gSwapBytesFlag = FALSE;

		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)" 

	return (returnCode);

}	// end "ReadWindowsBitMapHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		WindowPtr SetUpEmptyWindow
//
//	Software purpose:	The purpose of this routine is to set up a window for the
//							shape file specified by the information associated with the
//							input file information handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			OpenImageFile in SOpenFileDialog.cpp
//							OpenProjectImageWindow in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/13/2020
//	Revised By:			Larry L. Biehl			Date: 03/12/2020

Boolean SetUpEmptyWindow (
				Handle								windowInfoHandle)

{
	Boolean continueFlag = FALSE;

	
	if (windowInfoHandle != NULL)
		{
		/*
		#if defined multispec_mac
					// Create image window for file.

			continueFlag = CreateImageWindow (windowInfoHandle, FALSE);

			if (continueFlag)
				{
						// Make size of window such that it just fits around the
						// default display specification for the image.

				AdjustImageWSize (windowInfoHandle);

						// Force update of menu items.

				gUpdateFileMenuItemsFlag = TRUE;
				gUpdateProjectMenuItemsFlag = TRUE;
				gUpdateProcessorMenuItemsFlag = TRUE;
				gUpdatePaletteMenuItemsFlag = TRUE;

				EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
				if (gNumberOfIWindows == 1)
					gRedrawMenuBar = TRUE;

						// Put display dialog up for the user to select portion to
						// go into the image window.

				gMemoryTypeNeeded = 1;
				DisplayImage ();
				gMemoryTypeNeeded = 0;

				}	// end "if (continueFlag)"
		#endif	// defined multispec_mac

		#if defined multispec_win
			WindowInfoPtr windowInfoPtr =
										(WindowInfoPtr)GetHandlePointer (windowInfoHandle);

			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;

			if (imageWindowCPtr != NULL)
				{
						// Create image window for file.

				continueFlag = imageWindowCPtr->CreateImageWindow (FALSE);

				if (continueFlag)
					{
							// Make size of window such that it just fits around the
							// default display specification for the image.

					AdjustImageWSize (windowInfoHandle);

					gMemoryTypeNeeded = 1;
					DisplayImage ();
					gMemoryTypeNeeded = 0;
	
					}	// end "if (continueFlag)"

				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win
		*/	/*
		#if defined multispec_wx
			WindowInfoPtr windowInfoPtr =
									(WindowInfoPtr)GetHandlePointer (windowInfoHandle);

			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;

			if (imageWindowCPtr != NULL)
				{
						// Create image window for file.

				continueFlag = imageWindowCPtr->CreateImageWindow (FALSE);
				
				if (continueFlag)
					{
							// Make size of window such that it just fits around the
							// default display specification for the image.

					AdjustImageWSize (windowInfoHandle);

							// If the display dialog box will be displayed, force the image
							// window to be displayed before the dialog box is displayed.
					
					if (gCallProcessorDialogFlag)
						CheckSomeEvents (0);

					gMemoryTypeNeeded = 1;
					DisplayImage ();
					gMemoryTypeNeeded = 0;

					}	// end "if (continueFlag)"

				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_wx
		*/
		}	// end "if (windowInfoHandle != NULL)"
	
	return (continueFlag);

}	// end "SetUpEmptyWindow"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		WindowPtr SetUpImageWindow
//
//	Software purpose:	The purpose of this routine is to set up a window
//							for the multispectral image file specified by the
//							information associated with the input file 
//							information handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			OpenImageFile in SOpenFileDialog.cpp
//							OpenProjectImageWindow in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/03/1989
//	Revised By:			Larry L. Biehl			Date: 11/27/2017	

Boolean SetUpImageWindow (
				Handle								windowInfoHandle)

{
	Boolean continueFlag = FALSE;


	if (windowInfoHandle != NULL) 
		{
		#if defined multispec_mac
					// Create image window for file.												

			continueFlag = CreateImageWindow (windowInfoHandle, FALSE);

			if (continueFlag) 
				{
						// Make size of window such that it just fits around the			
						// default display specification for the image.						

				AdjustImageWSize (windowInfoHandle);

						// Force update of menu items.		

				gUpdateFileMenuItemsFlag = TRUE;
				gUpdateProjectMenuItemsFlag = TRUE;
				gUpdateProcessorMenuItemsFlag = TRUE;
				gUpdatePaletteMenuItemsFlag = TRUE;

				EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
				if (gNumberOfIWindows == 1)
					gRedrawMenuBar = TRUE;

						// Put display dialog up for the user to select portion to		
						// go into the image window.												

				gMemoryTypeNeeded = 1;
				DisplayImage ();
				gMemoryTypeNeeded = 0;

				}	// end "if (continueFlag)" 
		#endif	// defined multispec_mac		

		#if defined multispec_win		
			WindowInfoPtr windowInfoPtr =
										(WindowInfoPtr)GetHandlePointer (windowInfoHandle);

			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;

			if (imageWindowCPtr != NULL) 
				{
						// Create image window for file.

				continueFlag = imageWindowCPtr->CreateImageWindow (FALSE);

				if (continueFlag) 
					{
							// Make size of window such that it just fits around the
							// default display specification for the image.

					AdjustImageWSize (windowInfoHandle);

					gMemoryTypeNeeded = 1;
					DisplayImage ();
					gMemoryTypeNeeded = 0;
	
					}	// end "if (continueFlag)"

				}	// end "if (imageWindowCPtr != NULL)"			
		#endif	// defined multispec_win

		#if defined multispec_wx
			WindowInfoPtr windowInfoPtr =
									(WindowInfoPtr)GetHandlePointer (windowInfoHandle);

			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;

			if (imageWindowCPtr != NULL) 
				{
						// Create image window for file.

				continueFlag = imageWindowCPtr->CreateImageWindow (FALSE);
				
				if (continueFlag) 
					{
							// Make size of window such that it just fits around the
							// default display specification for the image.

					AdjustImageWSize (windowInfoHandle);

							// If the display dialog box will be displayed, force the image
							// window to be displayed before the dialog box is displayed.
					
					if (gCallProcessorDialogFlag)
						CheckSomeEvents (0);

					gMemoryTypeNeeded = 1;
					DisplayImage ();
					gMemoryTypeNeeded = 0;

					}	// end "if (continueFlag)"

				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_wx
		}	// end "if (windowInfoHandle != NULL)"

	return (continueFlag);

}	// end "SetUpImageWindow" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		WindowPtr SetUpThematicImageWindow
//
//	Software purpose:	The purpose of this routine is to set up a window
//							for the  Thematic image file specified by the information
//							associated with the input file information handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			OpenImageFile in SOpenFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1989
//	Revised By:			Larry L. Biehl			Date: 11/27/2017	

Boolean SetUpThematicImageWindow (
				Handle								windowInfoHandle)

{
	Boolean								continueFlag = FALSE;


	if (windowInfoHandle != NULL)
		{
		#if defined multispec_mac

				// Create image window for file.												

			continueFlag = CreateImageWindow (windowInfoHandle, TRUE);

			if (continueFlag)
				{
						// Load in the thematic class names.									

				LoadClassNameDescriptions (windowInfoHandle);

						// Make size of window such that it just fits around the			
						// default display specification for the Thematic image.			

            AdjustImageWSize (windowInfoHandle);

						// Force update of menu items.

            gUpdateFileMenuItemsFlag = TRUE;
            gUpdateProcessorMenuItemsFlag = TRUE;
            gUpdatePaletteMenuItemsFlag = TRUE;

            EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
            if (gNumberOfIWindows == 1)
                gRedrawMenuBar = TRUE;

						// Put display dialog up for the user to select portion to
						// go into the Thematic image window.

            gMemoryTypeNeeded = 1;
            DisplayImage ();
            gMemoryTypeNeeded = 0;

				}	// end "if (continueFlag)" 
		#endif	// defined multispec_mac

		#if defined multispec_win
			WindowInfoPtr windowInfoPtr =
					(WindowInfoPtr)GetHandlePointer (windowInfoHandle);

			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;

			if (imageWindowCPtr != NULL) 
				{
						// Create image window for file.

            continueFlag = imageWindowCPtr->CreateImageWindow (TRUE);

				if (continueFlag) 
					{
							// Load in the thematic class names.									

					LoadClassNameDescriptions (windowInfoHandle);

							// Make size of window such that it just fits around the
							// default display specification for the image.

					AdjustImageWSize (windowInfoHandle);

					gMemoryTypeNeeded = 1;
					DisplayImage ();
					gMemoryTypeNeeded = 0;

					}	// end "if (continueFlag)"

				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// defined multispec_win
		
		#if defined multispec_wx
			WindowInfoPtr windowInfoPtr =
										(WindowInfoPtr)GetHandlePointer (windowInfoHandle);

			CMImageWindow* imageWindowCPtr = windowInfoPtr->cImageWindowPtr;

			if (imageWindowCPtr != NULL)
				{
						// Create image window for file.

				continueFlag = imageWindowCPtr->CreateImageWindow (TRUE);

            if (continueFlag)
					{
							// Load in the thematic class names.

					LoadClassNameDescriptions (windowInfoHandle);

							// Make size of window such that it just fits around the
							// default display specification for the image.

					AdjustImageWSize (windowInfoHandle);

							// Force the image window to be displayed before the dialog
							// box is displayed.
					
					CheckSomeEvents (0);

					gMemoryTypeNeeded = 1;
					DisplayImage ();
					gMemoryTypeNeeded = 0;

					}	// end "if (continueFlag)"

				}	// end "if (imageWindowCPtr != NULL)"
		#endif	// end "multispec_wx"

		}	// end "if (windowInfoHandle != NULL)"

	return (continueFlag);

}	// end "SetUpThematicImageWindow"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SizeOfImageFileCanBeCalculated
//
//	Software purpose:	The purpose of this routine is to determine whether the size of
//							the file can be calulated based on just the image parameters.
//							It cannot be calculated if the data are compressed or if in some
//							specific formats.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2012
//	Revised By:			Larry L. Biehl			Date: 08/14/2018

Boolean SizeOfImageFileCanBeCalculated (
				FileInfoPtr							fileInfoPtr)

{
	Boolean returnFlag = TRUE;


	if (fileInfoPtr->dataCompressionCode != kNoCompression ||
			fileInfoPtr->format == kGRIBType ||
			fileInfoPtr->format == kImagineType ||
			fileInfoPtr->format == kArcGISASCIIGridType ||
			fileInfoPtr->format == kGRASSASCIIGridType ||
			(fileInfoPtr->format == kPDSType && fileInfoPtr->gdalDataSetH != NULL) ||
			fileInfoPtr->format == kGDALVRTType ||
			fileInfoPtr->format == kPCIDSKType)
		returnFlag = FALSE;

	return (returnFlag);

}	// end "SizeOfImageFileCanBeCalculated" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateLayerInfoStructure
//
//	Software purpose:	The purpose of this routine is to update the layer
//							information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True if update was successful.
//							False if update was not possible because of lack of
//								available.
// 
// Called By:			CreateImageWindow in SImageWindow_class.cpp
//							AddToImageWindowFile in SOpenFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/15/1991
//	Revised By:			Larry L. Biehl			Date: 07/11/2018

Boolean UpdateLayerInfoStructure (
				WindowInfoPtr						windowInfoPtr,
				Handle								fileInfoHandle)

{
	FileInfoPtr							fileInfoPtr,
											newFileInfoPtr;
											//oldFileInfoPtr;

	LayerInfoPtr						layerInfoPtr;

	Handle								layerInfoHandle,
											newFileInfoHandle;
											//oldFileInfoHandle;

	SInt32								bytesNeeded;

	UInt16								channel,
											index,
											newTotalNumberChannels,
											oldTotalNumberChannels;

	Boolean								changedFlag,
											continueFlag;

	SignedByte							handleStatus;


			// Check input variables.															

	if (windowInfoPtr == NULL || fileInfoHandle == NULL)
																						return (FALSE);

			// Initialize local variables.											

	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);

	oldTotalNumberChannels = windowInfoPtr->totalNumberChannels;
	newTotalNumberChannels = windowInfoPtr->totalNumberChannels +
																fileInfoPtr->numberChannels;

			// Make certain that the total number of channels is not over the		
			// limit.																				

	if (newTotalNumberChannels > kMaxNumberChannels)
																						return (FALSE);

	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																		fileInfoHandle, &handleStatus);

	layerInfoHandle = GetLayerInfoHandle (windowInfoPtr);

			// Allocate LayerInfo structure to store relationship of					
			// "window" channels to file channels.  This allows one to link		
			// the channels in multiple files together as one pseudo file.			

	bytesNeeded = sizeof (LayerInfo) * (newTotalNumberChannels + 1);
	layerInfoPtr = (LayerInfoPtr)CheckHandleSize (&layerInfoHandle,
																	&continueFlag,
																	&changedFlag,
																	bytesNeeded);

	if (continueFlag) 
		{
				// Make sure that the gActiveImageLayerInfoH is up to date.
				
		if (gActiveImageLayerInfoH != NULL &&
				gActiveImageLayerInfoH == GetLayerInfoHandle (windowInfoPtr) &&
					layerInfoHandle != gActiveImageLayerInfoH)
			gActiveImageLayerInfoH = layerInfoHandle;
				
		SetLayerInfoHandle (windowInfoPtr, layerInfoHandle);

				// Update some "window" file parameters.

		windowInfoPtr->totalNumberChannels = newTotalNumberChannels;

		if (newTotalNumberChannels == fileInfoPtr->numberChannels) 
			{
					// This is first file for the window.		

			windowInfoPtr->maxUsableDataValue = fileInfoPtr->maxUsableDataValue;
			windowInfoPtr->minUsableDataValue = fileInfoPtr->minUsableDataValue;

			windowInfoPtr->maxNumberLines = fileInfoPtr->numberLines;
			windowInfoPtr->maxNumberColumns = fileInfoPtr->numberColumns;
			windowInfoPtr->numberBytes = fileInfoPtr->numberBytes;
			windowInfoPtr->numberBits = fileInfoPtr->numberBits;
			windowInfoPtr->numberBins = fileInfoPtr->numberBins;
			windowInfoPtr->bandInterleave = fileInfoPtr->bandInterleave;
			windowInfoPtr->localMaxNumberBytes = fileInfoPtr->numberBytes;
			windowInfoPtr->dataTypeCode = fileInfoPtr->dataTypeCode;
			windowInfoPtr->signedDataFlag = fileInfoPtr->signedDataFlag;
			windowInfoPtr->maxNumberDescriptionCharacters =
											fileInfoPtr->maxNumberDescriptionCharacters;
			windowInfoPtr->localBytesDifferFlag = FALSE;

			if (windowInfoPtr->windowType == 0)
				windowInfoPtr->windowType = gGetFileImageType;

					// The first index of layerInfo is not used.							

			layerInfoPtr->fileInfoIndex = 0;
			layerInfoPtr->fileChannelNumber = 0;
			layerInfoPtr->wavelengthOrder = 0;

			}	// end "if (newTotalNumberChannels == ..." 

		else	// end "newTotalNumberChannels != fileInfoPtr->numberChannels" 
			{
					// For now do not allow combining of a BIS image file to any	
					// other type of image file.  To do so will require some major	
					// changes in the way the file input-output is carried out.		

			if (windowInfoPtr->bandInterleave == kBIS ||
											fileInfoPtr->bandInterleave == kBIS)
				continueFlag = FALSE;

					// Force the handle for the file information to be represent	
					// enough memory for a structure for each of the image files	
					// assigned to the image window.											
					// The file information for the selected image file will be 	
					// copied to the end of	the file information for the other		
					// image files assigned to the image window.							

			bytesNeeded = sizeof (MFileInfo) * (windowInfoPtr->numberImageFiles + 1);
			newFileInfoHandle = GetFileInfoHandle (windowInfoPtr);
			if (continueFlag)
				newFileInfoPtr = (FileInfoPtr)CheckHandleSize (&newFileInfoHandle,
																				&continueFlag,
																				&changedFlag,
																				bytesNeeded);
			/*								
			if (continueFlag)
				{
				newFileInfoHandle = MNewHandle (bytesNeeded);
				newFileInfoPtr = (FileInfoPtr)GetHandlePointer (newFileInfoHandle, 
																				kLock);
				continueFlag =  (newFileInfoPtr != NULL);	
																										
				}	// end "if (continueFlag)"

			if (continueFlag)
				{
				oldFileInfoHandle = GetFileInfoHandle (windowInfoPtr);
				oldFileInfoPtr = (FileInfoPtr)GetHandlePointer (oldFileInfoHandle, 
																				kLock);
				continueFlag =  (oldFileInfoPtr != NULL);	
																										
				}	// end "if (continueFlag)"
			*/
			if (continueFlag) 
				{
				windowInfoPtr->maxUsableDataValue =
														MAX (windowInfoPtr->maxUsableDataValue,
																fileInfoPtr->maxUsableDataValue);
				windowInfoPtr->minUsableDataValue =
														MIN (windowInfoPtr->minUsableDataValue,
																fileInfoPtr->minUsableDataValue);

				windowInfoPtr->maxNumberLines =
						MIN (windowInfoPtr->maxNumberLines, fileInfoPtr->numberLines);
				windowInfoPtr->maxNumberColumns =
						MIN (windowInfoPtr->maxNumberColumns, fileInfoPtr->numberColumns);

				if (windowInfoPtr->bandInterleave != fileInfoPtr->bandInterleave)
					windowInfoPtr->bandInterleave = kMixed;

				if (windowInfoPtr->numberBytes != fileInfoPtr->numberBytes ||
									windowInfoPtr->dataTypeCode != fileInfoPtr->dataTypeCode)
					windowInfoPtr->localBytesDifferFlag = TRUE;

				windowInfoPtr->numberBytes =
						MAX (windowInfoPtr->numberBytes, fileInfoPtr->numberBytes);
				windowInfoPtr->numberBits =
						MAX (windowInfoPtr->numberBits, fileInfoPtr->numberBits);
				windowInfoPtr->numberBins =
						MAX (windowInfoPtr->numberBins, fileInfoPtr->numberBins);

				windowInfoPtr->localMaxNumberBytes = windowInfoPtr->numberBytes;

				if (fileInfoPtr->dataTypeCode == kRealType)
					windowInfoPtr->dataTypeCode = kRealType;

				if (fileInfoPtr->signedDataFlag)
					windowInfoPtr->signedDataFlag = TRUE;

				windowInfoPtr->maxNumberDescriptionCharacters =
										MAX (windowInfoPtr->maxNumberDescriptionCharacters,
												fileInfoPtr->maxNumberDescriptionCharacters);
												
				if (fileInfoPtr->descriptionsFlag)
					windowInfoPtr->descriptionCode |= fileInfoPtr->descriptionCode;
				
				else	// !fileInfoPtr->descriptionsFlag
							// If description information does not exist for one of
							// the linked files, need to indicate that it does not exist
							// for the window or there will be problems with selection
							// graphs.
					windowInfoPtr->descriptionCode = 0;
				
				newFileInfoPtr = &newFileInfoPtr[windowInfoPtr->numberImageFiles];
				bytesNeeded = sizeof (MFileInfo);
				BlockMoveData (fileInfoPtr, newFileInfoPtr, bytesNeeded);
				
						// For the Mac version make sure that the file stream does not get
						// closed.
						// For the Windows version make sure that the file stream pointer
						// does not get deleted or used for another file.

				#if defined multispec_mac	 
					SetFileDoesNotExist (&fileInfoPtr->fileStream, kDoNotKeepUTF8CharName);
				#endif	// defined multispec_mac

				#if defined multispec_win || defined multispec_wx 
					fileInfoPtr->fileStreamCPtr = NULL;
				#endif	// defined multispec_win || defined multispec_wx

				CheckAndUnlockHandle (newFileInfoHandle);
				
						// Make sure that the gActiveFileInformationH is up to date.
				
				if (gActiveImageFileInfoH != NULL &&
						gActiveImageFileInfoH == GetFileInfoHandle (windowInfoPtr) &&
							newFileInfoHandle != gActiveImageFileInfoH)
					gActiveImageFileInfoH = newFileInfoHandle;
				
				SetFileInfoHandle (windowInfoPtr, newFileInfoHandle);
				
            }	// end "if (continueFlag)" 

			}	// end "else newTotalNumberChannels != ..." 

		SetBoundingMapRectangle (windowInfoPtr->windowInfoHandle);
		
				// Update the image second order statistics structure. Release any
				// memory already in use for the sums and sums of squares.
		
		ImageSecondOrderStatsPtr imageSecondOrderStatsPtr =
															&windowInfoPtr->imageSecondOrderStats;

		imageSecondOrderStatsPtr->sumsHandle =
								UnlockAndDispose (imageSecondOrderStatsPtr->sumsHandle);
		imageSecondOrderStatsPtr->sumSquaresHandle =
								UnlockAndDispose (imageSecondOrderStatsPtr->sumSquaresHandle);

		imageSecondOrderStatsPtr->lineStart = 1;
		imageSecondOrderStatsPtr->lineEnd = windowInfoPtr->maxNumberLines;
		imageSecondOrderStatsPtr->lineInterval = 1;
		imageSecondOrderStatsPtr->columnStart = 1;
		imageSecondOrderStatsPtr->columnEnd = windowInfoPtr->maxNumberColumns;
		imageSecondOrderStatsPtr->columnInterval = 1;

		imageSecondOrderStatsPtr->totalPixels = 0;
		imageSecondOrderStatsPtr->numberChannels = 0;
		
		}	// end "if (continueFlag)" 

	MHSetState (fileInfoHandle, handleStatus);

	if (continueFlag) 
		{
				// Now update the information in the layer structure.					

		channel = 1;
		for (index=oldTotalNumberChannels+1; index<=newTotalNumberChannels; index++)
			{
			layerInfoPtr[index].fileInfoIndex = windowInfoPtr->numberImageFiles;
			layerInfoPtr[index].fileChannelNumber = channel;
			layerInfoPtr[index].wavelengthOrder = index - 1;
			channel++;

			}	// end "for (index=oldTotalNumberChannels; ..." 

		windowInfoPtr->numberImageFiles++;

				// Get the wavelength order information in case it will be needed later
		
		windowInfoPtr->channelsInWavelengthOrderCode =
									GetChannelWavelengthOrder (windowInfoPtr, layerInfoPtr);

		}	// end "if (continueFlag)"

	CheckAndUnlockHandle (layerInfoHandle);

	return (continueFlag);

}	// end "UpdateLayerInfoStructure"  

