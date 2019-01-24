//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SPalette.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/19/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	Display a pattern image file on the screen.
//
//	Functions in file:	void 				ActivateImageWindowPalette
//								Boolean			CreatePalette
//								Boolean 			CreateThematicColorPalette
//								void 				Create1CPalette
//								void 				Create2CPalette
//								void 				Create3CPalette
//								void 				CreatePPalette
//								void				DrawCPalette
//								void				DrawPalette
//								pascal void 	DrawPalettePopUp
//								void				DrawPPalette
//								Boolean 			GetDefaultThematicFilePalette
//								PaletteHandle 	GetPaletteHandle
//								short int 		GetPaletteID
//								short int 		GetPaletteOffset
//								Boolean 			ReadOneBytePalette
//								Boolean			ReadPaletteFromResource
//								Boolean			SetBackgroundPaletteEntries
//								void 				SetUpPalettePopUpMenu
//								void 				UpdateActiveImageLegend
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*
			// Template for debugging.
		int numberChars = sprintf ((char*)gTextString3,
									" SPalette:xxxx (): %s",
									gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"     

#if defined multispec_lin
	#include "CPalette.h"
   #include "LDialog.h"
	#include "LImageDoc.h"
	#include "LImageView.h"
   #include "LFalseColorDialog.h"
	#include "LMainFrame.h"
#endif 

#if defined multispec_mac    
	
#endif	// defined multispec_mac   
                             
#if defined multispec_win     
	#include "WImageView.h"
	#include "CPalette.h"  
	#include	"WFalseColorDialog.h"
	#include "WMainFrame.h"	 
#endif	// defined multispec_win

#define kArcViewDefaultSupportType		1024

//#include	"SExtGlob.h"	



extern void CreateDefaultGroupTable (
				FileInfoPtr							fileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr);
/*
extern Boolean	GetClassColorTable (
				FileInfoPtr							gisFileInfoPtr,
				UInt16								numberClasses, 
				SInt16*								classPtr,
				UInt16								numberListClasses,
				UInt16*								numberTRLClassesPtr,  
				ColorSpec**							inputColorSpecPtrPtr,
				CMPaletteInfo						paletteHandle,
				UInt16*								paletteIndexPtr,
				SInt16								paletteType,
				UInt16								numberPaletteEntriesToRead, 
				SInt16								paletteOffset, 
				SInt16								classNameCode,
				SInt16								thematicListType,
				SInt16								collapseClassCode);	
*/
extern void LoadOSXColorTable (
				CMPaletteInfo						paletteHandle,
				UCharPtr								osxColorTablePtr);	

extern void LoadTwoBytePalette (
				ColorSpec*							colorSpecPtr,
				FileInfoPtr							imageFileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt16*								classSymbolPtr,
				UInt16*								paletteCodePtr,
				UInt32								colorVectorLength,
				UInt16*								vectorBluePtr,
				UInt16*								vectorGreenPtr,
				UInt16*								vectorRedPtr);

extern void UpdateOSXColorSpace (
				CMPaletteInfo						paletteHandle,
				UCharPtr								osxColorTablePtr);

extern Boolean ReadArcViewColorPalette (
				ColorSpec*							colorSpecPtr,
				FileInfoPtr							imageFileInfoPtr, 
				CMFileStream*						paletteFileStreamPtr,
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt16*								paletteCodePtr);



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.

Boolean CopyColorsFromClassTableToGroupTable (
				FileInfoPtr							fileInfoPtr,
				ColorSpec*							colorSpecPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt16								numberPaletteEntriesUsed,
				UInt32								numberClasses,
				UInt16*								paletteCodePtr);

Boolean CreateFalseColorPalette (
				FileInfoPtr							fileInfoPtr,
				ColorSpec*							colorSpecPtr, 
				UInt16*								classSymbolPtr,
				UInt32								numberClasses, 
				SInt16								classGroupCode,
				UInt16*								paletteCodePtr);

Boolean CreateGrayLevelPalette (
				ColorSpec*							colorSpecPtr,
				UInt32								numberClasses);

void CreatePPalette (
				Handle								windowInfoHandle,
				DisplaySpecsPtr					displaySpecsPtr);											

Boolean CreateThematicColorPalette (
				Handle								windowInfoHandle,
				DisplaySpecsPtr					displaySpecsPtr);

void Create1CPalette (
				DisplaySpecsPtr					displaySpecsPtr);

void Create2CPalette (
				DisplaySpecsPtr					displaySpecsPtr);

void Create3CPalette (
				DisplaySpecsPtr					displaySpecsPtr);

void  DrawCPalette (
				Rect*									rectPtr,
				DisplaySpecsPtr					displaySpecsPtr);

void 	DrawPPalette (
				Rect*									rectPtr,
				DisplaySpecsPtr					displaySpecsPtr);

Boolean GetBackgroundIncludedFlag (
				FileInfoPtr							fileInfoPtr,
				UInt16*								classSymbolPtr);

SInt16 GetPaletteID (
				SInt16								thematicPaletteType);
										
Boolean MVerifyPaletteSize (
				CMPaletteInfo						paletteObject,
				UInt32								numberEntries);

Boolean PaletteExists (
				CMPaletteInfo						paletteObject);

Boolean ReadOneBytePalette (
				ColorSpec*							colorSpecPtr,
				FileInfoPtr							imageFileInfoPtr, 
				CMFileStream*						paletteFileStreamPtr, 
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt32								paletteStart, 
				SInt16								paletteOffset, 
				SInt16								colorOrder,
				UInt16*								paletteCodePtr);

Boolean ReadPaletteFromResource (
				FileInfoPtr							fileInfoPtr,
				ColorSpec*							colorSpecPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				Boolean								channelThematicDisplayTypeFlag,
				CMPaletteInfo						inputResourcePHandle,
				UInt16								inputPaletteOffset,
				UInt16								numberPaletteEntriesUsed, 
				UInt16*								classSymbolPtr, 
				UInt16								thematicPaletteType,
				UInt32								numberClasses,
				SInt16								classGroupCode,
				UInt16*								paletteCodePtr);

Boolean SetBackgroundPaletteEntries (
				CTabHandle							cTableHandle,
				DisplaySpecsPtr					displaySpecsPtr, 
				FileInfoPtr							fileInfoPtr); 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ActivateImageWindowPalette
//
//	Software purpose:	The purpose of this routine is to activate the palette
//							for the active image window.  This is done after the
//							palette has been modified and the active image window
//							needs to reflect the characteristics of the new palette.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			DisplayThematicImage in SDisThem.cpp
//							UpdateActiveImageLegend in SPalette.cpp
//							DoThematicWColorsUpdate in SThemWin.cpp
//							EditClassGroupPalette in SThemWin.cpp
//							UpdateUserDefinedGroupColorTable in SThemWin.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1991
//	Revised By:			Larry L. Biehl			Date: 10/25/2001	

void ActivateImageWindowPalette (
				CMPaletteInfo						paletteHandle)

{
	#if defined multispec_mac 
		CTabHandle							cTabHandle;
		Handle								offScreenMapH;
		SInt32								ctSeed;
		

				// The palette changed.  Make certain that the new palette is set 	
				// in the offscreen pix map.														
						
		offScreenMapH = ((WindowInfoPtr)*gActiveImageWindowInfoH)->offScreenMapHandle;
		cTabHandle = ((PixMapPtr)*offScreenMapH)->pmTable;
		Palette2CTab (paletteHandle, cTabHandle);
		ctSeed = GetCTSeed ();
		(*cTabHandle)->ctSeed = ctSeed;
		if (gQD32IsImplemented)
			(*cTabHandle)->ctFlags = (*cTabHandle)->ctFlags | 0x4000;
		ActivatePalette (gActiveImageWindow);
		
		if (gOSXCoreGraphicsFlag)
			{
			unsigned char						osxColorTablePtr[3*256];
			
			LoadOSXColorTable (paletteHandle, osxColorTablePtr);
			UpdateOSXColorSpace (paletteHandle, osxColorTablePtr);
			
			}	// end "if (gOSXCoreGraphicsFlag)"
	
		InvalidateWindow (gActiveImageWindow, kFrameArea, FALSE);
	#endif	// defined multispec_mac 

	#if defined multispec_win 
		CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp ()->m_pMainWnd;
		pAppFrame->SendMessage (WM_QUERYNEWPALETTE, NULL, NULL);
	
		//gActiveImageViewCPtr->SendMessage (WM_DOREALIZE, 0, 1);
		//gActiveImageViewCPtr->Invalidate (FALSE); 
	#endif	// defined multispec_win 
							
	#if defined multispec_lin
			// Right now, i dont think anything has to be done for wxWidgets
	#endif

}	// end "ActivateImageWindowPalette"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CopyColorsFromClassTableToGroupTable
//
//	Software purpose:	The purpose of this routine is to copy the colors in the class 
//							color table to the group color table. This is done when the 
//							group table is first generated.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/07/2011
//	Revised By:			Larry L. Biehl			Date: 06/07/2011

Boolean CopyColorsFromClassTableToGroupTable (
				FileInfoPtr							fileInfoPtr, 
				ColorSpec*							colorSpecPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt16								numberPaletteEntriesUsed,
				UInt32								numberClasses,
				UInt16*								paletteCodePtr)
 
{
	ColorSpec*							classColorSpecPtr;
	CTabPtr								classCTablePtr;
	
	CTabHandle							classCTableHandle;
	
	UInt16								index,
											numberColors;
											
			
			// Get the number of colors.  					
			
	numberColors = (UInt16)MIN (256, numberClasses);
	
			// Get the class color table that is to be copied to the group
			// color table.
			
	classCTableHandle = displaySpecsPtr->savedClassCTableHandle;
		
	classCTablePtr = (CTabPtr)GetHandlePointer ((Handle)classCTableHandle, kLock);
	classColorSpecPtr = classCTablePtr->ctTable;
				
	for (index=0; index<numberColors; index++)
		{
		colorSpecPtr->value     = classColorSpecPtr->value;
		colorSpecPtr->rgb.red   = classColorSpecPtr->rgb.red;
		colorSpecPtr->rgb.green = classColorSpecPtr->rgb.green;
		colorSpecPtr->rgb.blue  = classColorSpecPtr->rgb.blue;
		
		colorSpecPtr++;
		classColorSpecPtr++;
		
		}	// end "for (index=0; index<numberColors; index++)" 
	  						
	CheckAndUnlockHandle ((Handle)classCTableHandle);
			
	return (TRUE);
	
}	// end "CopyColorsFromClassTableToGroupTable" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateFalseColorPalette
//
//	Software purpose:	The purpose of this routine is to create a FALSE color
//							palette based on the class means for the designated
//							channel to color gun assignments. 
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/14/1996
//	Revised By:			Larry L. Biehl			Date: 05/03/2011

Boolean CreateFalseColorPalette (
				FileInfoPtr							fileInfoPtr, 
				ColorSpec*							colorSpecPtr, 
				UInt16*								classSymbolPtr,
				UInt32								numberClasses, 
				SInt16								classGroupCode,
				UInt16*								paletteCodePtr)
 
{
	double 								maxMean[3],
											minMean[3],
											maxPaletteValue,
											paletteValue;
										
	ChannelStatistics					classChannelStats;
	CovarianceStatistics				classCovariance;
	HDoublePtr							classChannelMeanPtr,
											savedClassChannelMeanPtr;
										
	SInt32								longPaletteValue;
	
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
	
	Boolean								backgroundFlag,
											classCodeFlag,
											continueFlag;
	
	UInt16								classIndex,
											featurePtr[3],
											index,
											numberColors;
	
   #ifdef multispec_lin
      maxPaletteValue = 255;
   #else
      maxPaletteValue = 65535;
   #endif
   
	if (gProjectInfoPtr == NULL)
																							return (FALSE);
	
			// Check that there is a valid class to group handle if needed.		
			
	if (classGroupCode != kClassDisplay && fileInfoPtr->groupTablesHandle == NULL)
																							return (FALSE);
	
			// Initialize local variables.
			
	classChannelMeanPtr = NULL;
	
	minMean[0] = DBL_MAX;
	minMean[1] = DBL_MAX;
	minMean[2] = DBL_MAX;
	maxMean[0] = -DBL_MAX;
	maxMean[1] = -DBL_MAX;
	maxMean[2] = -DBL_MAX;
			
	featurePtr[0] = gProjectInfoPtr->falseColorPaletteBlue;
	featurePtr[1] = gProjectInfoPtr->falseColorPaletteGreen;
	featurePtr[2] = gProjectInfoPtr->falseColorPaletteRed;
	
	continueFlag = TRUE;
	if (continueFlag)
		{
		UInt32 bytesNeeded = (UInt32)numberClasses * 3 * sizeof (double);
		savedClassChannelMeanPtr = (HDoublePtr)MNewPointer (bytesNeeded);
		continueFlag = (savedClassChannelMeanPtr != NULL);
		
		}	// end "if (continueFlag)"
		
	if (continueFlag)
		{
				// Get the number of colors.  If the number of classes is more			
				// than 254 then set the number of colors to be created to 254.  
				// This is so that there is space in			
				// the 256 entry palette for white and black.  								
				// Note that if there is less that 254 classes then space is 			
				// allowed for a background color.	If there is 254 classes or more	
				// then the background color is forced to be white.						
				
		numberColors = (UInt16)MIN (256, numberClasses);
		
		classCodeFlag = (classGroupCode == kClassDisplay);
		
				// Get the class to group pointer if needed.									
		
		if (!classCodeFlag)
			{
			classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
			groupToPalettePtr = GetGroupToPalettePointer (fileInfoPtr);
			
			}	// end "if (!classCodeFlag)"
			
				// Determine if first class is the background class. This check
				// is for MultiSpec ascii and ERDAS classifications.
		
		backgroundFlag = GetBackgroundIncludedFlag (fileInfoPtr, classSymbolPtr);
			
				// Get the requested channel means for all classes and determine
				// the minimum and maximum mean.
		
		classChannelMeanPtr = savedClassChannelMeanPtr;
		UInt16 projectClassIndex = 0;
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
			if (classIndex == 0 && backgroundFlag)
				{
						// Background class. Set up for white.
						
				for (index=0; index<3; index++)
					{
					*classChannelMeanPtr = maxPaletteValue;
					classChannelMeanPtr++;
					
					}	// end "for (index=0; index<3; index++)"
				
				}	// end "if (classIndex == 0 && backgroundFlag)"
				
			else	// !backgroundFlag || classIndex] != 0
				{
				for (index=0; index<3; index++)
					{
					GetClassCovarianceMatrix (1,
														&classChannelStats,
														&classCovariance, 
														&featurePtr[index], 
														projectClassIndex, 
														kTriangleOutputMatrix,
														kMeanStdDevOnly,
														kOriginalStats);
					
					*classChannelMeanPtr = classChannelStats.mean;
								
					minMean[index] = MIN (
										minMean[index],
										classChannelStats.mean - classChannelStats.standardDev);
				
					maxMean[index] = MAX (
										maxMean[index],
										classChannelStats.mean + classChannelStats.standardDev);

					classChannelMeanPtr++;
				
					}	// end "for (index=0; index<3; index++)"
					
				projectClassIndex++;
					
				}	// end "else !backgroundFlag || classIndex] != 0"
				
			}	// end "for (classIndex=0; classIndex<numberClasses;..."
			
				// Verify that the min and max are not the same value.
				
		for (index=0; index<3; index++)
			{
			if (minMean[index] >= maxMean[index])
				maxMean[index] = minMean[index] + 1.;
		
			}	// end "for (index=0; index<3; index++)"
																					
				// Compute the colors for the palette based class means. Make 
				// certain that white is the first color in the palette and black 
				// is the last color in the palette.
			
		for (index=1; index<=numberColors; index++)
			{	
					// Get the palette index for a class or group display.

			if (classCodeFlag)
				{
				classIndex = index - 1;
				//inputIndex = classSymbolPtr[index];

				}	// end "if (classCodeFlag)"	
						
			else	// !classCodeFlag
				{ 
				classIndex = classToGroupPtr[index-1];
				classIndex = groupToPalettePtr[classIndex];
				
				}	// end "else !classCodeFlag"
				
			classChannelMeanPtr = &savedClassChannelMeanPtr[3*classIndex];
				
			colorSpecPtr->value = index;
			
			paletteValue = (classChannelMeanPtr[0] - minMean[0]) /
																(maxMean[0] - minMean[0]) * maxPaletteValue;
			longPaletteValue = (SInt32)(paletteValue + .5);
			longPaletteValue = (SInt32)MIN (maxPaletteValue, longPaletteValue);
			longPaletteValue = MAX (0, longPaletteValue);
			colorSpecPtr->rgb.blue = (SInt16)longPaletteValue;
				
			paletteValue = (classChannelMeanPtr[1] - minMean[1]) /
																(maxMean[1] - minMean[1]) * maxPaletteValue;
			longPaletteValue = (SInt32)(paletteValue + .5);
			longPaletteValue = (SInt32)MIN (maxPaletteValue, longPaletteValue);
			longPaletteValue = MAX (0, longPaletteValue);
			colorSpecPtr->rgb.green = (SInt16)longPaletteValue;
				
			paletteValue = (classChannelMeanPtr[2] - minMean[2])/
																(maxMean[2] - minMean[2]) * maxPaletteValue;
			longPaletteValue = (SInt32)(paletteValue + .5);
			longPaletteValue = (SInt32)MIN (maxPaletteValue, longPaletteValue);
			longPaletteValue = MAX (0, longPaletteValue);
			colorSpecPtr->rgb.red = (SInt16)longPaletteValue;
			
			colorSpecPtr++;
			
			}	// end "for (index=1; index<=numberColors; index++)"
			
		}	// end "if (continueFlag)"
		
	CheckAndDisposePtr (savedClassChannelMeanPtr); 
	UnlockGroupTablesHandle (fileInfoPtr);

	return (continueFlag);
	
}	// end "CreateFalseColorPalette"
											


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateGrayLevelPalette
//
//	Software purpose:	The purpose of this routine is to set up the palette 
//							with different gray levels for each of the number
//							of input classes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1996
//	Revised By:			Larry L. Biehl			Date: 05/03/2011	

Boolean CreateGrayLevelPalette (
				ColorSpec*							colorSpecPtr,
				UInt32								numberClasses)
 
{
	UInt32								paletteIncrement,
											paletteValue;
	
	UInt16								index,
											numberColors;
	
	
	if (colorSpecPtr == NULL)
																						return (FALSE);
																				
			// Get the last palette class.
	
	numberColors = (UInt16)MIN (256, numberClasses);

			// Make a default gray level color scheme for the classes.		
			
	paletteIncrement = kMaxColorValue/(numberColors+1);
	paletteValue = paletteIncrement;
																					
			// Compute the colors for the palette based class means. Make 
			// certain that white is the first color in the palette and black 
			// is the last color in the palette.
		
	for (index=1; index<=numberColors; index++)
		{
		colorSpecPtr->value = index;
		colorSpecPtr->rgb.red = (UInt16)paletteValue;
		colorSpecPtr->rgb.green = (UInt16)paletteValue;
		colorSpecPtr->rgb.blue = (UInt16)paletteValue;
		
		colorSpecPtr++;
		
		paletteValue += paletteIncrement;
		if (paletteValue > kMaxColorValue)
			paletteValue = kMaxColorValue;
		
		}	// end "for (index=1; index<=numberColors; index++)"
		
	return (TRUE);
	
}	// end "CreateGrayLevelPalette" 
											


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreatePalette
//
//	Software purpose:	The purpose of this routine is to set up the palette 
//							based on the user requests.
//		
//							Note that the palette is being set up with the first 
//							pattern as black and the second pattern as white.  
//							The display palette that will be used to draw images 
//							will start at pattern 3.  This was done to be 
//							compatible with the color palette used for the Mac II 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:			DisplayMultispectralImage in SDisMulc.cpp
//							DisplayThematicImage in SDisThem.cpp
//							DrawPalette in SPalette.cpp
//							UpdateActiveImageLegend in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 12/19/2018

Boolean CreatePalette (
				Handle								windowInfoHandle, 
				DisplaySpecsPtr					displaySpecsPtr,
				SInt16								windowType)
 
{
			// Declare local variables.
														
	Handle								checkHandle;
	
	UInt32								minMemoryForPaletteCreation = 20000,
											numberEntries;
	
	
	if (windowInfoHandle == NULL || displaySpecsPtr == NULL)
																						return (FALSE);
																							
	if (!displaySpecsPtr->paletteUpToDateFlag)             	
		{										
				// For thematic type images only.											
				// Make certain that there is enough memory left for creating		
				// an inverse color table.  There has been problems when copying	
				// a color table to palette under low memory conditions.  I am		
				// assuming that that is the problem.  I will check for a minimum	
				// amount of memory and exit if not available.	20000 is a guess.	
				// See notes in IMV on MakeITable.											
		
		if (windowType == kThematicWindowType)
			{
			checkHandle = MNewHandle (minMemoryForPaletteCreation);
			if (checkHandle == NULL)
																						return (FALSE);
			UnlockAndDispose (checkHandle);
			
			}	// end "if (windowType == kThematicWindowType)" 
			
				// Save the number of entries in the current palette that are 		
				// used for image display in case it is needed later.					
				
		gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;
		
		//if (displaySpecsPtr->pixelSize == 1)
		//	CreatePPalette (windowInfoHandle, displaySpecsPtr);
										
		if (displaySpecsPtr->pixelSize > 1)
			{
					// Find the number of entries needed in the palette.				
						
			numberEntries = displaySpecsPtr->numberPaletteLevels;
			if (displaySpecsPtr->displayType == 4 || displaySpecsPtr->displayType == 5)
				numberEntries *= numberEntries;
				
			else if (displaySpecsPtr->displayType == 3)
				numberEntries *= numberEntries*numberEntries;
				
					// Make certain that the number of entries is not more than 256.
					
			numberEntries = MIN (256, numberEntries);
			
					// Check if new palette for the color window is needed.			
			
			if (PaletteExists (displaySpecsPtr->paletteObject))
				{
						// Make certain that the size of the palette is okay.			
				 
				if (!MVerifyPaletteSize (displaySpecsPtr->paletteObject, numberEntries))
																						return (FALSE);
				
				}	// end "if (PaletteExists (..." 
			
			if (!PaletteExists (displaySpecsPtr->paletteObject))
				{
						// A new palette needs to be generated.							
				//#ifdef multispec_lin
				//	displaySpecsPtr->paletteObject = NULL;
				//#endif
				
				if (!MNewPalette (&displaySpecsPtr->paletteObject,
										(SInt16)numberEntries))
																						return (FALSE);
				
				}	// end "if (!PaletteExists (..." 
				
         #if defined multispec_win || defined multispec_lin
				if (windowType == kThematicWindowType)
					{
							// Check if new background palette for the color window 
							// is needed. This is only needed in the Windows version
							// for thematic image windows.			
					
					if (PaletteExists (displaySpecsPtr->backgroundPaletteObject))
						{
								// Make certain that the size of the palette is okay.			
						 
						if (!MVerifyPaletteSize (
											displaySpecsPtr->backgroundPaletteObject,
											numberEntries))
																						return (FALSE);
						
						}	// end "if (PaletteExists (..." 
					
					if (!PaletteExists (displaySpecsPtr->backgroundPaletteObject))
						{
								// A new palette needs to be generated.							
						
						if (!MNewPalette (
										&displaySpecsPtr->backgroundPaletteObject,
										(SInt16)numberEntries)) 	
																						return (FALSE);
						
						}	// end "if (!PaletteExists (..."
						
					}	// end "if (windowType == kThematicWindowType)"
			#endif	// defined multispec_win | lin 
			
					// Save number of entries in palette to be used for the			
					// display.
			
			if (windowType == kImageWindowType)
				{
				displaySpecsPtr->numPaletteEntriesUsed = (UInt16)numberEntries;
			
				if (displaySpecsPtr->displayType == 2 || 
														displaySpecsPtr->displayType == 7)
					Create1CPalette (displaySpecsPtr);
										
				if (displaySpecsPtr->displayType == 3)
					Create3CPalette (displaySpecsPtr);
										
				if (displaySpecsPtr->displayType == 4 ||
														displaySpecsPtr->displayType == 5)
					Create2CPalette (displaySpecsPtr);
				
				}	// end "if (windowType == kImageWindowType)" 
			
			if (windowType == kThematicWindowType)
				{ 
						// Adjust number of palette entries to allow for the 			
						// background color.														
						
				displaySpecsPtr->numPaletteEntriesUsed = (UInt16)numberEntries;
				
				if (!CreateThematicColorPalette (windowInfoHandle, displaySpecsPtr))
																						return (FALSE);
				
				}	// end "if (windowType == kThematicWindowType)" 
				
			}	// end "if (displaySpecsPtr->pixelSize > 1)" 
		
				// Indicate that the palette is up-to-date.								
		
		displaySpecsPtr->paletteUpToDateFlag = TRUE;
												
		}	// end "if (!displaySpecsPtr->paletteUpToDateFlag)" 
		
	return (TRUE);
	
}	// end "CreatePalette"  


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CreatePPalette
//
//	Software purpose:	The purpose of this routine is to set up the pattern 
//							palette based on the user requests.
//		
//							Note that the palette is being set up with the first 
//							pattern as white and the last pattern as black.  
//							The display palette that will be used to draw images 
//							will start at pattern 2.  This was done to be 
//							compatible with the color palette used for the Mac II
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/2006	

void CreatePPalette (
				Handle								windowInfoHandle, 
				DisplaySpecsPtr					displaySpecsPtr)
 
{
#if defined multispec_mac  
	Handle								imageFileInfoH;
	
	SInt16								*displayClassGroupsPtr,
											switchValue,
											windowType;
	
	UInt32								maxIndex;
	
	UInt16								classNumber,
											groupNumber;
		
	SInt16								*classToGroupPtr;
	unsigned char						*symbolToPalettePtr;
	
	Boolean								groupCodeFlag;
	SInt16								setting;
	UInt32								i,
											index,
											numberLevels;
	
	
			// Intialize local variables.  Make certain that number of levels		
			// is not more than what the pattern array will allow taking into		
			// account the black and white patterns.										
	
	numberLevels = MIN (displaySpecsPtr->numberPaletteLevels, kNumPatterns - 2);
	windowType = ((WindowInfoPtr)*windowInfoHandle)->windowType;
	
			// Load the white pattern for the first element of the pattern array.
			
	GetIndPattern (&gPatterns[0], kPatternID, 1);
	
			// Set up the indices into the pattern resource file taking into		
			// account whether the palette is to be inverted.							
			//		non-inverted order is black to white.									
			//		inverted order is white to black.										
	
	switchValue = 10;
	if (windowType == kThematicWindowType)
		{
		switchValue = 9;
		numberLevels = MIN (numberLevels, kNumPatterns - 3);
		
		}	// end "if (windowType == kThematicWindowType)" 
			
	if (displaySpecsPtr->numberPaletteLevels <= switchValue)
		{
		index = 10;
		if (displaySpecsPtr->invertValues)
			index = 1;
		
		}	// end "if (displaySpecsPtr->numberPaletteLevels <= 10)" 
			
	if (displaySpecsPtr->numberPaletteLevels > switchValue)
		{
		index = 26;
		if (displaySpecsPtr->invertValues)
			index = 11;
		
		}	// end "if (displaySpecsPtr->numberPaletteLevels > 10)" 
		
			// Load the pattern set into memory from the resource file.				
	
	for (i=1; i<=numberLevels; i++)
		{
		GetIndPattern (&gPatterns[i], kPatternID, index);
		
		if (!displaySpecsPtr->invertValues)
			index--;
			
		else	// displaySpecsPtr->invertValues 
		   index++;
		   
		}	// end "for (i=1; i<=numberLevels; i++)" 
		
			// Load the black pattern for the last element of the pattern 			
			// array.
			
	GetIndPattern (&gPatterns[i], kPatternID, 10);
	
			// Initialize the variable numberEntries in displayPalette				
	
	displaySpecsPtr->numPaletteEntriesUsed = numberLevels;
	
			// Initialize the palette offset parameter.  It will always be 1 for	
			// pattern type displays.															
			
	displaySpecsPtr->paletteOffset = 1;
	
			// Now update the symbol to palette pointer for thematic type			
			// images.																				
			
	if (windowType == kThematicWindowType)
		{
				// Get pointer to symbol-to-palette vector.								
				
		HLock (displaySpecsPtr->symbolToPaletteEntryH);
		symbolToPalettePtr = (unsigned char*)*displaySpecsPtr->symbolToPaletteEntryH;
																						
				// Get pointer to the classes/groups to be displayed.					
		
		HLock (displaySpecsPtr->displayClassGroupsHandle);
		displayClassGroupsPtr = 
							(short int*)*displaySpecsPtr->displayClassGroupsHandle;
			
		if (displaySpecsPtr->classGroupCode == kClassDisplay)
			setting = 0x0001;
			
		else	// displaySpecsPtr->classGroupCode != kClassDisplay 
			setting = 0x0100;
						
				// Determine whether this is a class or group display.				
				
		groupCodeFlag = (displaySpecsPtr->classGroupCode != kClassDisplay);
		
		imageFileInfoH = ((WindowInfoPtr)*windowInfoHandle)->fileInfoHandle;
		maxIndex = displaySpecsPtr->numberPaletteLevels;
		if (((FileInfoPtr)*imageFileInfoH)->asciiSymbols)
			maxIndex = 256;
		
		if (groupCodeFlag)
			{
			classToGroupPtr = GetClassToGroupPointer (imageFileInfoH);
			
			for (index=0; index<maxIndex; index++)
				{
				if (symbolToPalettePtr[index] > 0)
					{
					classNumber = symbolToPalettePtr[index] - 1;
				
					if (!(displayClassGroupsPtr[classNumber] & setting))
						symbolToPalettePtr[index] = 0;
					
					else	// (displayClassGroupsPtr[index] & setting) 
						{
						groupNumber = classToGroupPtr[classNumber] + 1;
						
						if (groupNumber > numberLevels)
							groupNumber = ((groupNumber - 1) % numberLevels) + 1; 
						
						symbolToPalettePtr[index] = groupNumber;
						
						}	// end "else (displayClassGroupsPtr[index] & setting)" 
						
					}	// end "if (symbolToPalettePtr[index] > 0)"
				
				}	// end "for (index=0; index<..." 
			
			UnlockGroupTablesHandle (imageFileInfoH);
			
			}	// end "if (groupCodeFlag)" 
						
		else	// !groupCodeFlag 
			{
			for (index=0; index<maxIndex; index++)
				{
				classNumber = symbolToPalettePtr[index] - 1;
				
				if (!(displayClassGroupsPtr[classNumber] & setting))
					symbolToPalettePtr[index] = 0;
					
				else if (symbolToPalettePtr[index] > numberLevels)
					symbolToPalettePtr[index] = (classNumber % numberLevels) + 1; 
				
				}	// end "for (index=0; index<..." 
			
			}	// end "else !groupCodeFlag" 
			
		HUnlock (displaySpecsPtr->symbolToPaletteEntryH);
		HUnlock (displaySpecsPtr->displayClassGroupsHandle);
													
		}	// end "if (windowType == kThematicWindowType)" 
#endif	// defined multispec_mac
	
}	// end "CreatePPalette" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateThematicColorPalette
//
//	Software purpose:	The purpose of this routine is to set up the color 
//							palette for Thematic type windows based on the user 
//							requests.
//		
//							Parts of this routine originally came from Absoft's 
//							MacFortran to demonstrate the palette manager.  It 
//							was converted to C and then modified again to be 
//							used within MultiSpec.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			CreatePalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1989
//	Revised By:			Larry L. Biehl			Date: 07/28/2017	

Boolean CreateThematicColorPalette (
				Handle								windowInfoHandle, 
				DisplaySpecsPtr					displaySpecsPtr)
 
{
	ColorSpec*							colorSpecPtr;
	CTabPtr								savedCTablePtr;
	FileInfoPtr							fileInfoPtr;
	
	CTabHandle							savedCTableHandle;
	Handle								imageFileInfoH;
										
	SInt16								thematicPaletteType;
										
	UInt16								paletteCode,
											tableSize;
											
	Boolean								continueFlag,
											tryCreateGroupPaletteFlag;
											
	SignedByte							fileHandleStatus;
	
	
			// Check input variables.															
			
	if (windowInfoHandle == NULL)											
																						return (FALSE);
	
			// Initialize local variables.													
			
	continueFlag = TRUE;
	tryCreateGroupPaletteFlag = FALSE;
	
			// Get the appropriate palette handle to be updated.						
			
	if (displaySpecsPtr->classGroupCode == kClassDisplay)
		{
		savedCTableHandle = displaySpecsPtr->savedClassCTableHandle;
		thematicPaletteType = displaySpecsPtr->thematicClassPaletteType;
		if (savedCTableHandle == NULL)
			{
			displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
			
			if (displaySpecsPtr->savedGroupCTableHandle == NULL)
				tryCreateGroupPaletteFlag = TRUE;
			
			}	// end "if (savedCTableHandle == NULL)"
		
		}	// end "if (displaySpecsPtr->classGroupCode == kClassDisplay)" 
		
	else	// displaySpecsPtr->classGroupCode == kGroupDisplay 
		{
		savedCTableHandle = displaySpecsPtr->savedGroupCTableHandle;
		thematicPaletteType = displaySpecsPtr->thematicGroupPaletteType;
		if (displaySpecsPtr->classToGroupChangeFlag || savedCTableHandle == NULL)
			displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
			
		if (savedCTableHandle == NULL)
			thematicPaletteType = kCopyColorsFromClassTable;
			
		displaySpecsPtr->classToGroupChangeFlag = FALSE;
		
		}	// end "else displaySpecsPtr->classGroupCode == kGroupDisplay" 
		
			// Get the number of palette entries.											
			
	tableSize = MGetNumberPaletteEntries (displaySpecsPtr->paletteObject);
		
			// If the color table handle is empty, allocate memory for one.		
			
	if (savedCTableHandle == NULL)
		{
		savedCTableHandle = (CTabHandle)MNewHandle (
				(SInt32)tableSize*sizeof (ColorSpec) + sizeof (ColorTable));
		if (savedCTableHandle == NULL)					
																						return (FALSE);
		
		if (displaySpecsPtr->classGroupCode == kClassDisplay)
			displaySpecsPtr->savedClassCTableHandle = savedCTableHandle;
			
		else	// displaySpecsPtr->classGroupCode != kClassDisplay 
			displaySpecsPtr->savedGroupCTableHandle = savedCTableHandle;
		
		savedCTablePtr = (CTabPtr)GetHandlePointer ((Handle)savedCTableHandle, kLock);
		savedCTablePtr->ctSeed = GetCTSeed ();
		savedCTablePtr->ctFlags = 0;
		savedCTablePtr->ctSize = tableSize - 1;
		CheckAndUnlockHandle ((Handle)savedCTableHandle);
		
		}	// end "if (savedPaletteHandle == NULL)" 
		
				// Get pointers to the file information.									
	
	imageFileInfoH = GetFileInfoHandle (windowInfoHandle);	
	if (imageFileInfoH == NULL)											
																						return (FALSE);
	
			// Check status of file information structure.								
			// The Get pointer to file information.										
			
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																imageFileInfoH, &fileHandleStatus);
	
	if (displaySpecsPtr->readPaletteFromDiskFlag)
		{
					// Lock handle to color table.	
					
		savedCTablePtr = (CTabPtr)GetHandlePointer ((Handle)savedCTableHandle, kLock);
		colorSpecPtr = savedCTablePtr->ctTable;
		
		displaySpecsPtr->paletteOffset = 0;
		
		paletteCode = 0;
		if (thematicPaletteType != kUserDefinedColors)
			continueFlag = LoadColorSpecTable (windowInfoHandle,
															NULL,
															displaySpecsPtr,
															colorSpecPtr,
															NULL,
															2,
															tableSize,
															displaySpecsPtr->numPaletteEntriesUsed,
															thematicPaletteType,
															displaySpecsPtr->numberPaletteLevels,
															displaySpecsPtr->classGroupCode,
															&paletteCode);
		
				// Update palette offset and number of entries used based on 
				// the returned code.
													
		if (paletteCode & 0x0001)
			{
			displaySpecsPtr->paletteOffset = 0;
			displaySpecsPtr->numPaletteEntriesUsed++;
			
			}	// end "if (paletteCode & 0x0001)"
		
		if (paletteCode & 0x0002)
			displaySpecsPtr->numPaletteEntriesUsed++;
		
		if (paletteCode & 0x0004)
			displaySpecsPtr->numPaletteEntriesUsed++;
			
		if (continueFlag)
			displaySpecsPtr->readPaletteFromDiskFlag = FALSE;
			
		CheckAndUnlockHandle ((Handle)savedCTableHandle);
		/*
		if (displaySpecsPtr->classGroupCode == kClassDisplay)
			fileInfoPtr->classChangedFlag = TRUE;
			
		else	// displaySpecsPtr->classGroupCode != kClassDisplay
			fileInfoPtr->groupChangedFlag = TRUE;
		*/
				// Create a default group table if possible.
		
		if (tryCreateGroupPaletteFlag)
			CreateDefaultGroupTable (fileInfoPtr, displaySpecsPtr);
			
		}	// end "if (displaySpecsPtr->readPaletteFromDiskFlag)" 
		
	else	// !displaySpecsPtr->readPaletteFromDiskFlag 
		displaySpecsPtr->numPaletteEntriesUsed = gClassPaletteEntries;
	
			// Now set color for any class that is not to be shown to the			
			// background color.																	
			
	if (continueFlag)
		continueFlag = SetBackgroundPaletteEntries (
											savedCTableHandle, displaySpecsPtr, fileInfoPtr);
				
			// Unlock the file information handle if needed.							
	
	MHSetState (imageFileInfoH, fileHandleStatus);
	
	return (continueFlag);

}	// end "CreateThematicColorPalette"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Create1CPalette
//
//	Software purpose:	The purpose of this routine is to set up the color 
//							palette based on the user requests.
//		
//							Parts of this routine originally came from Absoft's 
//							MacFortran to demonstrate the palette manager.  It 
//							was converted to C and then modified again to be 
//							used within multiSpec.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			CreatePalette
//
//	Coded By:			Larry L. Biehl			Date: 02/21/1988
//	Revised By:			Larry L. Biehl			Date: 04/26/2011

void Create1CPalette (
				DisplaySpecsPtr					displaySpecsPtr)
 
{  
	CMPaletteInfo						paletteObject;                                   
	RGBColor								theColor;
	
	SInt32								binc,
											ginc,
											rinc,
											increment[2];
	
	UInt32								entry;
	
	SInt16								direction;
	
	
			// Get local copy of palette object.											
			
	paletteObject = displaySpecsPtr->paletteObject;
		
			// Set the color increment value based on the number of display		 
			// levels																				

	increment[0] = 0;
	
	increment[1] = (SInt32)kMaxColorValue;
	if (displaySpecsPtr->numberPaletteLevels > 1)
		increment[1] = (SInt32)(kMaxColorValue /
														(displaySpecsPtr->numberPaletteLevels-1));
	
	rinc = increment[1];
	ginc = increment[1];
	binc = increment[1];
	
			// Set up the initial value of the palette based on whether the		
			// palette is to be inverted (light to dark) or not-inverted (dark	
			// to light). Assume inverted to start with. This will properly 		
			// initialize the parameters for the non-inverted case					

	theColor.red   = (UInt16)kMaxColorValue;
	theColor.green = (UInt16)kMaxColorValue;
	theColor.blue  = (UInt16)kMaxColorValue;
	direction		= -1;
	
	if (!displaySpecsPtr->invertValues)
		{
		if (rinc != 0)	
			theColor.red   = 0;
			
		if (ginc != 0)	
			theColor.green = 0;
			
		if (binc != 0)	
			theColor.blue  = 0;
			
		direction = 1;
		
		}	// end "if (!displaySpecsPtr->invertValues)" 
	
			// Put the colors into the palette that will be used to display the	
			// image																					
			
	MSetEntryColor (paletteObject, 0, &theColor);
			
	for (entry=1; entry<displaySpecsPtr->numberPaletteLevels; entry++) 
		{
		theColor.red   += (UInt16)(direction*rinc);
		theColor.green += (UInt16)(direction*ginc);
		theColor.blue  += (UInt16)(direction*binc);
		MSetEntryColor (paletteObject, (UInt16)entry, &theColor);
		
		}	// end "for (entry=1; entry<displaySpecsPtr->numberPaletteLevels;...)" 
		
}	// end "Create1CPalette"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Create2CPalette
//
//	Software purpose:	The purpose of this routine is to set up the color 
//							palette for a 2-channel display.
//		
//							Parts of this routine originally came from Absoft's 
//							MacFortran to demonstrate the palette manager.  It 
//							was converted to C and then modified again to be 
//							used within multiSpec.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			CreatePalette
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 08/15/1988
//	Revised By:			Larry L. Biehl			Date: 04/26/2011

void Create2CPalette (
				DisplaySpecsPtr					displaySpecsPtr)

{
	CMPaletteInfo						paletteObject;
	RGBColor								theColor;
	
	SInt32								increment;
	
	UInt32								blue,
											green,
											numberLevels,
											red;
	
	SInt16								displayType;
	
	UInt16								blueInit,
											greenInit,
											paletteEntry,
											redInit;
	
	
			// Initialize local variables.												
			
	paletteObject = displaySpecsPtr->paletteObject;
	displayType = displaySpecsPtr->displayType;
	numberLevels = (UInt16)displaySpecsPtr->numberPaletteLevels;
		
			// Set the color increment value based on the number of display		
			// levels																				
	
	increment = (SInt32)kMaxColorValue;
	if (displaySpecsPtr->numberPaletteLevels > 1)
		increment = (SInt32)(kMaxColorValue / (displaySpecsPtr->numberPaletteLevels-1));
	
			// Set up the initial value of the palette based on whether				
			// the palette is to be inverted (dark to light) or						
			// not-inverted (light to dark)													

	if (!displaySpecsPtr->invertValues)
		{
		redInit   = 0;
		greenInit = 0;
		blueInit  = 0;
		
		}	// end "if (!displaySpecsPtr->invertValues)" 
	
	else	// displaySpecsPtr->invertValues 
		{
		redInit   = kMaxColorValue;
		greenInit = kMaxColorValue;
		blueInit  = kMaxColorValue;
		
		increment = -increment;
		
		}	// end "else displaySpecsPtr->invertValues" 
		
	paletteEntry = 0;
	
	if (displaySpecsPtr->rgbColors == kRGColor)
		{
				// Set color table for red-green display.  If displaytype is 5, then	
				// blue will be the same level as for green.									
				
		theColor.blue = 0;
		for (red=0; red<numberLevels; red++)
			{
			theColor.red = (UInt16)(redInit + red*increment);
	
			for (green=0; green<numberLevels; green++) 
				{
				theColor.green = (UInt16)(greenInit + green*increment);
				if (displayType == 5)
					theColor.blue = theColor.green;
				MSetEntryColor (paletteObject, paletteEntry, &theColor);
				paletteEntry++;
				
				}	// end 'for (green=0; ...' 
				
			}	// end 'for (red=0; ...' 
			
		}	// if (rgbColors == kRGColor) 
	
	if (displaySpecsPtr->rgbColors == kGBColor)
		{
				// Set color table for green-blue display. If displaytype is 5, then	
				// red will be the same level as for blue.									
				
		theColor.red = 0;
		for (green=0; green<numberLevels; green++)
			{
			theColor.green = (UInt16)(greenInit + green*increment);
	
			for (blue=0; blue<numberLevels; blue++) 
				{
				theColor.blue = (UInt16)(blueInit + blue*increment);
				if (displayType == 5)
					theColor.red = theColor.blue;
				MSetEntryColor (paletteObject, paletteEntry, &theColor);
				paletteEntry++;
				
				}	// end 'for (blue=0; ...' 
				
			}	// end 'for (green=0; ...' 
			
		}	// if (rgbColors == kGBColor) 
	
	if (displaySpecsPtr->rgbColors == kRBColor)
		{
		theColor.green = 0;
		for (red=0; red<numberLevels; red++)
			{
					// Set color table for red-blue display. If displaytype is 5, then
					// green will be the same level as for red.
				
			theColor.red = (UInt16)(redInit + red*increment);
			if (displayType == 5)
				theColor.green = theColor.red;
	
			for (blue=0; blue<numberLevels; blue++) 
				{
				theColor.blue = (UInt16)(blueInit + blue*increment);
				MSetEntryColor (paletteObject, paletteEntry, &theColor);
				paletteEntry++;
				
				}	// end 'for (blue=0; ...' 
				
			}	// end 'for (red=0; ...' 
			
		}	// if (rgbColors == kRBColor)
	/*
			// Now put the system colors in the palette.							
				
	theColor.red   = 22016;
	theColor.green = 11421;
	theColor.blue  = 1316;
	paletteEntry++;
	MSetEntryColor (paletteObject, paletteEntry, &theColor);							
				
	theColor.red   = 0;
	theColor.green = 25765;
	theColor.blue  = 4541;
	paletteEntry++;
	MSetEntryColor (paletteObject, paletteEntry, &theColor);							
				
	theColor.red   = 0;
	theColor.green = 0;
	theColor.blue  = 54272;
	paletteEntry++;
	MSetEntryColor (paletteObject, paletteEntry, &theColor);							
				
	theColor.red   = 577;
	theColor.green = 43860;
	theColor.blue  = 60159;
	paletteEntry++;
	MSetEntryColor (paletteObject, paletteEntry, &theColor);							
				
	theColor.red   = 62167;
	theColor.green = 2134;
	theColor.blue  = 34028;
	paletteEntry++;
	MSetEntryColor (paletteObject, paletteEntry, &theColor);							
				
	theColor.red   = 56680;
	theColor.green = 2242;
	theColor.blue  = 1698;
	paletteEntry++;
	MSetEntryColor (paletteObject, paletteEntry, &theColor);							
				
	theColor.red   = 65535;
	theColor.green = 25738;
	theColor.blue  = 652;
	paletteEntry++;
	MSetEntryColor (paletteObject, paletteEntry, &theColor);
	
	SInt16 index;
	increment = abs (increment);
	theColor.red   = 0;
	theColor.green = 0;
	theColor.blue  = 0;
	for (index=paletteEntry+1; index<displaySpecsPtr->numPaletteEntriesUsed; index++)
		{
		theColor.red   += increment;
		theColor.green += increment;
		theColor.blue  += increment;
		MSetEntryColor (paletteObject, index, &theColor);
		
		}	// end "for (index=paletteEntry+1; ..."
	*/
}	// end "Create2CPalette" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Create3CPalette
//
//	Software purpose:	The purpose of this routine is to set up the color 
//							palette for a 3-channel display.
//		
//							Parts of this routine originally came from Absoft's 
//							MacFortran to demonstrate the palette manager.  It 
//							was converted to C and then modified again to be 
//							used within multiSpec.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CreatePalette
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 07/13/1988
//	Revised By:			Larry L. Biehl			Date: 04/27/2011	

void Create3CPalette (
				DisplaySpecsPtr					displaySpecsPtr)
				 
{
	CMPaletteInfo						paletteObject;
	RGBColor								theColor;
	
	UInt32								blue,
											green,
											red;
											
	SInt32								increment;
	
	UInt16								blueInit,
											greenInit,
											redInit,
											tableSize;
	
	SInt16								paletteEntry;
	
	Boolean								limitReachedFlag;
									
	
			// Get local copy of palette object.
		
	paletteObject = displaySpecsPtr->paletteObject;
	tableSize = MGetNumberPaletteEntries (displaySpecsPtr->paletteObject);
	
			// Set the color increment value based on the number of display		
			// levels																				
	
	increment = (SInt32)kMaxColorValue;
	if (displaySpecsPtr->numberPaletteLevels > 1)
		increment = (SInt32)(kMaxColorValue / (displaySpecsPtr->numberPaletteLevels-1));
	
			// Put the colors into the palette.
			// Set up the initial value of the palette based on whether
			// the palette is to be inverted (dark to light) or						
			// not-inverted (light to dark)													

	if (!displaySpecsPtr->invertValues)
		{
		redInit   = 0;
		greenInit = 0;
		blueInit  = 0;
		
		}	// end "if (!displaySpecsPtr->invertValues)" 
	
	else	// displaySpecsPtr->invertValues	
		{
		redInit   = kMaxColorValue;
		greenInit = kMaxColorValue;
		blueInit  = kMaxColorValue;
		increment = -increment;
		
		}	// end "else displaySpecsPtr->invertValues" 
		
	paletteEntry = 0;
	limitReachedFlag = FALSE;
	
	for (red=0; red<displaySpecsPtr->numberPaletteLevels; red++) 
		{
		theColor.red = (UInt16)(redInit + red*increment);
	
		for (green=0; green<displaySpecsPtr->numberPaletteLevels; green++) 
			{
			theColor.green = (UInt16)(greenInit + green*increment);
	
			for (blue=0; blue<displaySpecsPtr->numberPaletteLevels; blue++) 
				{
				theColor.blue  = (UInt16)(blueInit + blue*increment);
				MSetEntryColor (paletteObject, paletteEntry, &theColor);
				paletteEntry++;
				
				if (paletteEntry >= tableSize)
					{
					limitReachedFlag = TRUE;
					break;
					
					}	// end "if (paletteEntry >= tableSize)"
					
				}	// end 'for (blue=0; ...' 
			
			if (limitReachedFlag)
				break;
				
			}	// end 'for (green=0; ...' 
			
		if (limitReachedFlag)
			break; 
				
		}	// end 'for (red=0; ...' 
	
}	// end "Create3CPalette" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DetermineIfFalseColorAvailable
//
//	Software purpose:	The purpose of this routine is to determine if a set of FALSE
//							colors can be determine from the project file available.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:				
// 
// Called By:			CreatePalette
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 04/24/2018

Boolean DetermineIfFalseColorAvailable (
				SInt16								fileFormat,
				UInt32								numberClasses,
				Handle								classDescriptionH)
 
{
	Boolean								returnFlag = FALSE;
	
	
			// Determine if the FALSE color item is to be included. 
				                                        
	if (gProjectInfoPtr != NULL)
		{
		if (fileFormat == kMultiSpecClassificationType)
			{		
			UInt16* classSymbolPtr = (UInt16*)GetHandlePointer (classDescriptionH);
													
			if (classSymbolPtr != NULL)
				{
				classSymbolPtr = &classSymbolPtr[numberClasses*sizeof (Str15)];
				
				if (*classSymbolPtr == 0 &&
						numberClasses == (UInt32)gProjectInfoPtr->numberStatisticsClasses+1)
					returnFlag = TRUE;
				
				if (*classSymbolPtr == ' ' &&
						numberClasses == (UInt32)gProjectInfoPtr->numberStatisticsClasses+1)
					returnFlag = TRUE;
				
				if (*classSymbolPtr != ' ' &&
						numberClasses == (UInt32)gProjectInfoPtr->numberStatisticsClasses)
					returnFlag = TRUE;
					
				}	// end "if (classSymbolPtr != NULL)"
				
			}	// end "if (fileFormat == kMultiSpecClassificationType)"
			
		else if ((fileFormat == kErdas74Type ||
								fileFormat == kTIFFType ||
											fileFormat == kGeoTIFFType) &&
					(numberClasses == (UInt32)gProjectInfoPtr->numberStatisticsClasses ||
						numberClasses == (UInt32)gProjectInfoPtr->numberStatisticsClasses+1))
			returnFlag = TRUE;
				
		else if ((fileFormat == kGAIAType || fileFormat == kGAIA2Type) &&
					numberClasses == (UInt32)gProjectInfoPtr->numberStatisticsClasses+1)
			returnFlag = TRUE;
			
		}	// end "if (gProjectInfoPtr != NULL)"
			
	return (returnFlag); 
			
}	// end "DetermineIfFalseColorAvailable" 


/*
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				DrawCPalette
//
//	Software purpose:	The purpose of this routine is to draw a palette 
//							in the palette window to display the colors being used.
//	
//							This routine originally came from Absoft's MacFortran 
//							to demonstrate the palette manager.  It was converted 
//							to C and then modified again to be a test color 
//							display within MultiSpec.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/21/1988
//	Revised By:			Larry L. Biehl			Date: 04/26/2011	

void DrawCPalette (
				Rect*									rectPtr, 
				DisplaySpecsPtr					displaySpecsPtr)
 
{	
	SInt16								color,
											colorIndexStart,
											numberEntries,
											penWidth,
											x1,
											x2,
											y1,
											y2;
	
	
			// Get the number of entries.														
			
	numberEntries = displaySpecsPtr->numPaletteEntriesUsed;
	colorIndexStart = 0;
	
			// Set width of the pen based on the number of levels 					
			
	penWidth = (rectPtr->bottom)/numberEntries;
	if (penWidth < 1)
		penWidth = 1;
	PenSize (2, penWidth);
	PenMode (8);
	
	x1 = 0;
	y1 = 0;
	x2 = rectPtr->right;
	y2 = 0;
	
			// Allow for the palette drawing to start with an index of 1 if		
			// needed.																				
					
	numberEntries += colorIndexStart;

			// draw some lines with our new palette										

	for (color=colorIndexStart; color<numberEntries; color++) 
		{
		PmForeColor (color);
		MoveTo (x1,y1);
		LineTo (x2,y2);
		y2 += penWidth;
		y1 += penWidth;
		
		}	// end "for (color=colorIndexStart; color<numberEntries; ..." 
	
}	// end "DrawCPalette" 
#endif	// defined multispec_mac
*/


#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawPalettePopUp
//
//	Software purpose:	The purpose of this routine is to draw the palette
//							prompt and palette selection for palette
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DisplayThematicDialog in displayThematic.c
//
//	Coded By:			Larry L. Biehl			Date: 10/31/1990
//	Revised By:			Larry L. Biehl			Date: 10/31/1990	

pascal void DrawPalettePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpPaletteMenu, 
								gPaletteSelection, 
								TRUE);
	
}	// end "DrawPalettePopUp" 
#endif	// defined multispec_mac


/*
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DrawPPalette
//
//	Software purpose:	The purpose of this routine is to draw a pattern 
//							palette in the palette window to display the patterns
//							being used.
//
//	Parameters in:		*rect - Address of rectangle that defines where to
//											drawthe palatte in the palette window.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 02/01/1991	

void DrawPPalette (
				Rect*									rectPtr, 
				DisplaySpecsPtr					displaySpecsPtr)

{
	SInt16								maxNumber,
											pattern,
											patternIndex,
											penWidth,
											x1,
											x2,
											y1,
											y2;

	
			// Initialize local variables.													
			
	maxNumber = displaySpecsPtr->numPaletteEntriesUsed;
	
			// Set width of the pen based on the number of levels 					
			
	penWidth = rectPtr->bottom/displaySpecsPtr->numberPaletteLevels;
	if (penWidth < 1)  penWidth = 1;
	PenSize (2, penWidth);
	PenMode (8);
	ForeColor (blackColor);
		
	x1 = 0;
	y1 = 0;
	x2 = rectPtr->right;
	y2 = 0;

			// draw some lines with our new palette										

	patternIndex = 1;
	for (pattern=1; pattern<=displaySpecsPtr->numberPaletteLevels; pattern++) 
		{
		if (patternIndex > maxNumber)
			patternIndex = 1;
			
		PenPat ((ConstPatternParam)&gPatterns[patternIndex]);
		MoveTo (x1,y1);
		LineTo (x2,y2);
		y2 += penWidth;
		y1 += penWidth;
		
		patternIndex++;
		
		}	// end "for (pattern=1; pattern<=displaySpecsPtr->..." 
	
}	// end "DrawPPalette" 
#endif	// defined multispec_mac   
*/

                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean FalseColorPaletteDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal dialog for
//							the channels for the FALSE color palette.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:			ImageWControlEvent in MControls.c
//							ClassifyDialog in SClassfy.cpp
//							DisplayThematicDialog in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/26/1996
//	Revised By:			Larry L. Biehl			Date: 04/25/2018

Boolean FalseColorPaletteDialog (void)

{
	Boolean								returnFlag = FALSE;                                          
		
#if defined multispec_mac  
	Rect									theBox;
	
	DialogPtr							dialogPtr; 
	Handle								theHandle;
	
	SInt32*								channelPtr;
	
	SInt32								blueChannel,
											greenChannel,
											maxChannelFeatureNum,
											redChannel,
											theNum; 
	
	FileInfoPtr							projectFileInfoPtr;
	LayerInfoPtr						projectLayerInfoPtr;								
	WindowInfoPtr						projectWindowInfoPtr; 
	
	SInt16								blueChannelIndex,
											greenChannelIndex,
											itemHit,
											projectHandleStatus,
											redChannelIndex,
											theType;         
	
	Boolean								modalDone;
												
												
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kFalseColorPaletteDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)											
																						return (FALSE);
																				
			// Get Project window information structure pointer.
			
	GetProjectImageFileInfo (
								kDoNotPrompt, 
								kSetupGlobalInfoPointersIfCan,
								&projectWindowInfoPtr,
								&projectLayerInfoPtr,
								&projectFileInfoPtr,
								&projectHandleStatus);
												
			// Load default project values if needed.
			
	LoadDefaultProjectFalseColorChannels ();
	
			// Get default values.  			
			
	redChannel = 
			gProjectInfoPtr->channelsPtr[gProjectInfoPtr->falseColorPaletteRed] + 1;
	greenChannel = 
			gProjectInfoPtr->channelsPtr[gProjectInfoPtr->falseColorPaletteGreen] + 1;
	blueChannel = 
			gProjectInfoPtr->channelsPtr[gProjectInfoPtr->falseColorPaletteBlue] + 1;
		
	maxChannelFeatureNum = projectWindowInfoPtr->totalNumberChannels;
	
	LoadDItemValue (dialogPtr, 4, redChannel);
	LoadDItemValue (dialogPtr, 6, greenChannel);
	LoadDItemValue (dialogPtr, 8, blueChannel);
		
			// Get a handle to a one column list.											
			
	gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 10);
	
			//	Create the channel list.														
			
	SetDialogItemDrawRoutine (dialogPtr, 10, gCreateOneColumnList1Ptr);
	
	AddChannelsToDialogList (gDialogListHandle,
										0, 
										NULL, 
										projectLayerInfoPtr,
										projectFileInfoPtr,
										kItemsListOnly, 
										(SInt16*)gProjectInfoPtr->channelsPtr, 
										gProjectInfoPtr->numberStatisticsChannels,
										kSubsetMenuItem);
	
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kFalseColorPaletteDialogID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 4, 0, SHRT_MAX);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 4:				// Channel for red color
					channelPtr = &redChannel;	
					break;
					
				case 6:				// Channel for green color
					channelPtr = &greenChannel;		
					break;
					
				case 8:				// Channel for blue color
					channelPtr = &blueChannel;	
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
			if (itemHit == 1)
				itemHit = CheckMaxValue (dialogPtr, 
													4,
													1, 
													maxChannelFeatureNum,
													kDisplayRangeAlert);
				
			if (itemHit == 1)
				itemHit = CheckMaxValue (dialogPtr, 
													6, 
													1,
													maxChannelFeatureNum,
													kDisplayRangeAlert);
				
			if (itemHit == 1)
				itemHit = CheckMaxValue (dialogPtr, 
													8, 
													1,
													maxChannelFeatureNum,
													kDisplayRangeAlert);
				
			if	(itemHit == 1)
				{
				theNum = GetDItemValue (dialogPtr, 4);
				itemHit = FalseColorCheckColorChannel (
														dialogPtr, 4, theNum, &redChannelIndex);
				
				}	// end "if	(itemHit == 1)"
				
			if	(itemHit == 1)
				{                    
				theNum = GetDItemValue (dialogPtr, 6);
				itemHit = FalseColorCheckColorChannel (
														dialogPtr, 6, theNum, &greenChannelIndex);
				
				}	// end "if	(itemHit == 1)"
				
			if	(itemHit == 1)   
				{
				theNum = GetDItemValue (dialogPtr, 8);
				itemHit = FalseColorCheckColorChannel (
														dialogPtr, 8, theNum, &blueChannelIndex);
				
				}	// end "if	(itemHit == 1)"
					
			if	(itemHit == 1)      // User selected OK and channels are okay
				{		
				modalDone = TRUE;
				returnFlag = TRUE;
	
						// Cell Width.	
						
				gProjectInfoPtr->falseColorPaletteRed = redChannelIndex;
				gProjectInfoPtr->falseColorPaletteGreen = greenChannelIndex;
				gProjectInfoPtr->falseColorPaletteBlue = blueChannelIndex;		  
												
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
			// Dispose of the dialog list handle.										
			
	if (gDialogListHandle != NULL)  
		LDispose (gDialogListHandle);
	gDialogListHandle = NULL;
	
	UnlockImageInformationHandles (projectHandleStatus, 
												gProjectInfoPtr->windowInfoHandle);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac

	#if defined multispec_win   	
		CMFalseColorDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMFalseColorDlg ();
			
			returnFlag = dialogPtr->DoDialog (); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL  	
	#endif	// defined multispec_win 
	
	#if defined multispec_lin   
		CMFalseColorDlg*		dialogPtr = NULL;
		
			try
				{ 
				dialogPtr = new CMFalseColorDlg ();
				returnFlag = dialogPtr->DoDialog (); 
				delete dialogPtr;
				}
			catch (int e)
				{
				MemoryMessage (0, kObjectMessage);
				returnFlag = FALSE;
				} 
	#endif	// defined multispec_lin

	return (returnFlag);
	
}	// end "FalseColorPaletteDialog"



SInt16 FalseColorCheckColorChannel (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt32								itemValue,
				SInt16*								channelIndexPtr) 
										
{
	SInt16								itemHit = 1;
							
	Rect									tempRect;
		
	Handle								okHandle;
		
	SInt16								index,
											theType;


			// Verify that the input channel in the dialog box is one of the channels
			// used for the project statistics.
				                                       
	*channelIndexPtr = -1;
	for (index=0; 
			index<gProjectInfoPtr->numberStatisticsChannels; 
				index++)
		{
		if (itemValue == (SInt32)gProjectInfoPtr->channelsPtr[index] + 1)
			*channelIndexPtr = index;
				
		}	// end "for (index=0; ..."
						
	if (*channelIndexPtr < 0)
		{
		NumberErrorAlert (itemValue, 
								dialogPtr, 
								itemNumber);
																			
				// Display an alert.													
		
		GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &tempRect);
		HiliteControl ((ControlHandle)okHandle, 255);
					
		DisplayAlert (kErrorAlertID, 
							kCautionAlert,
							kAlertStrID, 
							IDS_Alert53,
							0, 
							NULL);	
			
		HiliteControl ((ControlHandle)okHandle, 0);

		itemHit = 0;
			
		}	// end "if (redChannelIndex < 0 || ..."		
		
	return (itemHit);
					
}	// end "FalseColorCheckColorChannel"  
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		PaletteHandle GetActivePaletteHandle
//
//	Software purpose:	The purpose of this routine is to get the palette
//							handle for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1997
//	Revised By:			Larry L. Biehl			Date: 01/16/1997	

CMPaletteInfo GetActivePaletteHandle (void)
 
{       
	#if defined multispec_mac || defined multispec_mac_swift
		return (GetPalette (gActiveImageWindow));
	#endif	// defined multispec_mac_swift
				
	#if defined multispec_win || defined multispec_lin	
		CMPaletteInfo paletteHandle = NULL;
		
		Handle displaySpecsHandle = GetActiveDisplaySpecsHandle (); 
		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					displaySpecsHandle);
	
		if (displaySpecsPtr != NULL)
			paletteHandle = displaySpecsPtr->paletteObject;
			
		return (paletteHandle);		
	#endif	// defined multispec_win | lin

}	// end "GetActivePaletteHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetBackgroundIncludedFlag
//
//	Software purpose:	The purpose of this routine is to determine if the first
//							class is the background class. This check was original done
//							for for only ERDAS and MultiSpec ascii files. It has since
//							been changed so be done for all file times for which a
//							classSymbolPtr array is available.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1996
//	Revised By:			Larry L. Biehl			Date: 04/25/2018

Boolean GetBackgroundIncludedFlag (
				FileInfoPtr							fileInfoPtr,
				UInt16*								classSymbolPtr)
 
{
	Boolean								backgroundFlag = FALSE;
	
			
				// Determine if first class is the background class. This check
				// is for MultiSpec ascii and ERDAS classifications.
		
	if (classSymbolPtr != NULL)
		{		
		if (fileInfoPtr->format == kMultiSpecClassificationType &&
																		*classSymbolPtr == ' ')
			backgroundFlag = TRUE;
			
		else if (fileInfoPtr->format != kMultiSpecClassificationType &&
																		*classSymbolPtr == 0)
			backgroundFlag = TRUE;
			
		}	// end "if (classSymbolPtr != NULL)"
		
	return (backgroundFlag); 
					
}	// end "GetBackgroundIncludedFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetClassColorTable
//
//	Software purpose:	This routine get the requested colors for the input classes.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			CreateThematicSupportFile in SFileIO
//							
//	Coded By:			Larry L. Biehl			Date: 07/22/2010
//	Revised By:			Larry L. Biehl			Date: 05/07/2013	

Boolean GetClassColorTable (
				FileInfoPtr							gisFileInfoPtr,
				UInt16								numberClasses, 
				SInt16*								classPtr,
				UInt16								numberListClasses,
				UInt16*								numberTRLClassesPtr,  
				ColorSpec**							inputColorSpecPtrPtr,
				CMPaletteInfo						paletteHandle,
				UInt16*								paletteIndexPtr,
				SInt16								paletteType,
				UInt16								numberPaletteEntriesToRead, 
				SInt16								classNameCode,
				SInt16								thematicListType,
				SInt16								collapseClassCode)

{
	ColorSpec*							colorSpecPtr;
	
	SInt16								numberProjectClasses,
											thematicPaletteType;
	
	UInt16								inputPaletteOffset,
											paletteCode;
	
	Boolean								continueFlag;
	
	
	if (gisFileInfoPtr == NULL)
																							return (FALSE);
	
			// Do not get colors for the classes if there are more classes than the class		
			// limit.																				
	
	if (numberClasses > gClassListLimit)
																							return (TRUE);
	
			// Make certain that we have the proper information for the 			
			// palette information.																
	
	if (paletteHandle != NULL || *inputColorSpecPtrPtr != NULL)
		paletteType = 0;
	
	else	// paletteHandle == NULL && *inputColorSpecPtrPtr == NULL
		if (paletteType <= 0)
																							return (FALSE);
	
	continueFlag = TRUE;
	numberProjectClasses = 0;
	if (gProjectInfoPtr != NULL)
		numberProjectClasses = gProjectInfoPtr->numberStatisticsClasses;
	
			// Check if collapsed classes need to be handled.
	
	if (classNameCode == kFromDescriptionCode && collapseClassCode != kCollapseClass)
		numberClasses = (UInt16)(gisFileInfoPtr->maxClassNumberValue + 1);		
	
			// Determine the number of trailer classes.									
	
	*numberTRLClassesPtr = numberClasses;
	if (classPtr != NULL)
		*numberTRLClassesPtr = numberProjectClasses;
	
			// Make sure that the number of classes is not more than 256.
			// Allow for the background class if needed.
	
	if (classNameCode < kFromDescriptionCode)
		*numberTRLClassesPtr = MIN (kMaxNumberStatClasses-1, *numberTRLClassesPtr);
	*numberTRLClassesPtr = MIN (kMaxNumberStatClasses, *numberTRLClassesPtr);
	
			// Get buffer to load color table into if needed.
	
	colorSpecPtr = *inputColorSpecPtrPtr;
	if (continueFlag && colorSpecPtr == NULL)
		{	
				// Get table for palette information. Allow enough for the
				// number of classes, white, black, and background. The
				// white, black and background will probably not be need for
				// the trailer file, but the routines being called will
				// fix those colors.
		
		colorSpecPtr = (ColorSpec*)MNewPointer (
									(SInt32)(numberPaletteEntriesToRead+3)*sizeof (ColorSpec));
		continueFlag = (colorSpecPtr != NULL);
		
		}	// end "if (continueFlag && colorSpecPtr != NULL)"
	
			// Set up the palette type variable depending upon the source
			// for the palette information.
	
	thematicPaletteType = paletteType;
	inputPaletteOffset = 0;
	if (thematicPaletteType == 0)
		{
		if (*inputColorSpecPtrPtr == NULL)
			{
			if (paletteHandle != NULL)
				{
				thematicPaletteType = kPaletteHandle;
				
				if (gProcessorCode != kMultispecToThematicProcessor)
					inputPaletteOffset = 0;
				
				}	// end "if (paletteHandle != NULL)"
			
			else	// resourcePHandle == NULL 
				thematicPaletteType = kComputedGrays;
			
			}	// end "if (inputColorSpecPtr == NULL)"
		
		}	// end "if (thematicPaletteType == 0)"
	
			// Get the requested color scheme for the classes.
	
	if (continueFlag && thematicPaletteType != 0)
		continueFlag = LoadColorSpecTable (NULL, 
													  NULL,
													  NULL,
													  colorSpecPtr,
													  paletteHandle,
													  inputPaletteOffset,
													  MIN (*numberTRLClassesPtr, 256),
													  *numberTRLClassesPtr,
													  thematicPaletteType,
													  numberPaletteEntriesToRead,
													  thematicListType,
													  &paletteCode);
	
	*inputColorSpecPtrPtr = colorSpecPtr;
	
	return (continueFlag);
	
}	// end "GetClassColorTable"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetDefaultThematicFilePalette
//
//	Software purpose:	The purpose of this routine is to determine if a 
//							default palette exists for the Thematic image file.
//							This will be the case for ERDAS GIS files and
//							SUN screendump files.  If an ERDAS trl file exists for
//							the ERDAS GIS file then the trl file is opened.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			LoadThematicDisplaySpecs in displayPalette.c
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1989
//	Revised By:			Larry L. Biehl			Date: 06/29/2011	

Boolean GetDefaultThematicFilePalette (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)
 
{
	Boolean								returnFlag;
	
			
			// Initialize local variables.													
			
	returnFlag = FALSE;
	
	if (fileInfoPtr->ancillaryInfoformat == kArcViewDefaultSupportType)
				// This overrides other possibilities.
		returnFlag = TRUE;
			
	else	// fileInfoPtr->ancillaryInfoformat != kArcViewDefaultSupportType
		{
				// Branch to appropriate palette setup routine based on the file	
				// format.																				
				
		switch (fileInfoPtr->format)
			{
			case 0:		// No identifying header.
			case kErdas73Type:
			case kErdas74Type:
			case kGAIAType:
			case kGAIA2Type:
			case kMultiSpecClassificationType:
				returnFlag = GetDefaultSupportFile (windowInfoPtr->windowInfoHandle, 
																kITRLFileType);
				break;
				
			case kSunScreenDumpType:		// SUN screen dump format 
				if (fileInfoPtr->numberHeaderBytes > 32)
					returnFlag = TRUE;
				break;
				
			case kTIFFType:		// Tiff file with color table 
			case kGeoTIFFType:	// GeoTiff file with color table 
				if (fileInfoPtr->colorTableOffset > 0)
					returnFlag = TRUE;
				break;
				
			case kImagineType:		// Imagine file with color table 
				returnFlag = CheckIfDefaultImaginePaletteExists (fileInfoPtr);
				break;
				
			#if include_gdal_capability
				case kArcInfoBinaryGridType: 
				case kArcViewType:  
				case kECWType:
				case kGRIBType:
				case kJPEG2000Type:	
				case kPNGType:				 
							// GDAL color table
					returnFlag = (CheckIfDefaultGDALPaletteExists (fileInfoPtr) > 0);
					break;
			#endif	// include_gdal_capability
				
			}	// end "switch (fileInfoPtr->format)"
			
		}	// end "else fileInfoPtr->ancillaryInfoformat != ..."
		
	return (returnFlag);

}	// end "GetDefaultThematicFilePalette" 
 

                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		PaletteHandle GetPaletteHandle
//
//	Software purpose:	The purpose of this routine is to get the palette
//							handle for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/15/1995
//	Revised By:			Larry L. Biehl			Date: 12/08/1995	

CMPaletteInfo GetPaletteHandle (void)
 
{
	DisplaySpecsPtr								displaySpecsPtr;
	Handle											displaySpecsHandle;
	CMPaletteInfo									paletteHandle;


	paletteHandle = NULL;
   
   displaySpecsHandle = GetDisplaySpecsHandle (gImageWindowInfoPtr); 
   
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
												
	if (displaySpecsPtr != NULL)
		paletteHandle = displaySpecsPtr->paletteObject;	

	return (paletteHandle);

}	// end "GetPaletteHandle" 
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetPaletteID
//
//	Software purpose:	The purpose of this routine is to get the palette
//							resource ID for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ChangeFormatToBILorBIS in SReform1.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/14/1995
//	Revised By:			Larry L. Biehl			Date: 11/19/1996	

SInt16 GetPaletteID (
				SInt16								thematicPaletteType)
 
{
	SInt16								paletteId;
	
	
	switch (thematicPaletteType)
		{
		case kDefaultColors:
			paletteId = 1001;
			break;												
	
		case kDefaultGrays:
			paletteId = 1002;
			break;				
		
		case kCorrelationMatrixColors:
			paletteId = 1003;
			break;					
		
		case kAVHRR_NDVI_Colors:
			paletteId = 1006;
			break;
			
		case kMODIS_NDVI_Colors:
			paletteId = 1007;
			break;
			
		case kProbablilityColors:
			paletteId = 1005;
			break;
			
		case kProbablilityColors2:
			paletteId = 1004;
			break;
			
		default:
			paletteId = gDefaultImagePalette;
			
		}	// end "switch (thematicPaletteType)"	
					
	return (paletteId);

}	// end "GetPaletteID" 


                    
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int GetPaletteOffset
//
//	Software purpose:	The purpose of this routine is to get the palette
//							offset for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/15/1995
//	Revised By:			Larry L. Biehl			Date: 06/15/1995	

SInt16 GetPaletteOffset (void)
 
{
	DisplaySpecsPtr								displaySpecsPtr;
	Handle											displaySpecsHandle;
	SInt16											paletteOffset;


	paletteOffset = 0;
   
   displaySpecsHandle = GetDisplaySpecsHandle (gImageWindowInfoPtr); 
   
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
												
	if (displaySpecsPtr != NULL)
		paletteOffset = displaySpecsPtr->paletteOffset;

	return (paletteOffset);

}	// end "GetPaletteOffset"
 

                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetPaletteType
//
//	Software purpose:	The purpose of this routine is to get the palette
//							type for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ChangeFormatToBILorBIS in SReform1.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1996
//	Revised By:			Larry L. Biehl			Date: 11/19/1996	

SInt16 GetPaletteType (void)
 
{
	DisplaySpecsPtr					displaySpecsPtr;
	Handle								displaySpecsHandle;
	SInt16								paletteType;


	paletteType = kDefaultColors;
   
   displaySpecsHandle = GetDisplaySpecsHandle (gImageWindowInfoPtr); 
   
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
												
	if (displaySpecsPtr != NULL)
		paletteType = displaySpecsPtr->thematicClassPaletteType;

	return (paletteType);

}	// end "GetPaletteType"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadColorSpecTable
//
//	Software purpose:	The purpose of this routine is to load the input color 
//							specification structure with the specified colors.
//
//	Parameters in:		window
//
//	Parameters out:	paletteCode	0-bit = white start
//											1-bit = black end
//											2-bit = black wrap around
//
// Value Returned:	None				
// 
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//							CreateThematicColorPalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1996
//	Revised By:			Larry L. Biehl			Date: 05/19/2011	

Boolean LoadColorSpecTable (
				Handle								windowInfoHandle,
				FileInfoPtr							fileInfoPtr, 
				DisplaySpecsPtr					displaySpecsPtr,
				ColorSpec*							colorSpecPtr,
				CMPaletteInfo						inputResourcePHandle,
				UInt16								inputPaletteOffset,
				UInt16								tableSize,
				UInt16								numPaletteEntriesUsed,
				SInt16								thematicPaletteType,
				UInt32								numberClasses,
				SInt16								classGroupCode,
				UInt16*								paletteCodePtr)
 
{
	Handle								imageFileInfoH;
	SignedByte							fileHandleStatus,
											supportHandleStatus;
									
	UInt16*								classSymbolPtr;
	
	Boolean								channelThematicDisplayTypeFlag,
											continueFlag;
	
	
			// Check input variables.
			
	if (colorSpecPtr == NULL)
																							return (FALSE);
	
			// Initialize local variables.													
			
	continueFlag = FALSE;
	*paletteCodePtr = 0;
	classSymbolPtr = NULL;
	imageFileInfoH = NULL; 
		
				// Get pointers to the file information.	
	
	if (fileInfoPtr == NULL)
		{	
		imageFileInfoH = GetFileInfoHandle (windowInfoHandle);
	
				// Check status of file information structure.								
				// The Get pointer to file information.										
				
		fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
															imageFileInfoH, &fileHandleStatus);
											
		}	// end "if (fileInfoPtr == NULL)"
	
	if (fileInfoPtr != NULL)
		{		
		if (GetImageType (windowInfoHandle) == kThematicImageType)
			{
					// Get pointer to class symbol table.										
					// Remember to take into account that classSymbolPtr 					
					// points to a short int vector.	Str15 is used instead of			
					// Str31.																			
		  					
			classSymbolPtr = (UInt16*)GetHandlePointer (
															fileInfoPtr->classDescriptionH, kLock);
			
			if (classSymbolPtr != NULL)
				classSymbolPtr = &classSymbolPtr[fileInfoPtr->numberClasses*sizeof (Str15)];
				
			}	// end "if (GetImageType (windowInfoHandle) == kThematicImageType)"
					
		}	// end "if (fileInfoPtr != NULL)"
	
	channelThematicDisplayTypeFlag = FALSE;
	if (GetImageType (windowInfoHandle) == kMultispectralImageType)
		channelThematicDisplayTypeFlag = TRUE;
	
			// Branch to appropriate palette setup routine based on the 		
			// type of palette to use.	
			
	CMFileStream*		paletteFileStreamPtr;			
	
	switch (thematicPaletteType)
		{
		case kImageDefaultColorTable:		// File default 
			if (fileInfoPtr == NULL)
																							return (FALSE);
																						
			switch (fileInfoPtr->ancillaryInfoformat)
				{
				case kMultiSpecClassificationType:
							// Force the colors to be read from the ERDAS trl file
							// in order beginning at '0'.
							
					classSymbolPtr = NULL;
				
				case 0:
				case kErdas73Type:
				case kErdas74Type:
				case kGAIAType:
				case kGAIA2Type:
					paletteFileStreamPtr = GetSupportFileStreamPointer (
														windowInfoHandle, &supportHandleStatus);
														
							// The trl palette is always 1 byte. Make sure it is set as 
							// such since the trl file may be overriding what is in a tiff
							// file which is 2 bytes.
							
					fileInfoPtr->colorTableValueBytes = 1;
					
					continueFlag = ReadOneBytePalette (colorSpecPtr,
																	fileInfoPtr, 
																	paletteFileStreamPtr, 
																	displaySpecsPtr, 
																	classSymbolPtr, 
																	128, 
																	256, 
																	2,
																	paletteCodePtr);
					
					CloseFile (paletteFileStreamPtr);
					
					UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);
					break;
				
				case kSunScreenDumpType:
					paletteFileStreamPtr = GetFileStreamPointer (fileInfoPtr);
					continueFlag = ReadOneBytePalette (colorSpecPtr,
																	fileInfoPtr,
																	paletteFileStreamPtr, 
																	displaySpecsPtr, 
																	classSymbolPtr, 
																	32, 
																	256, 
																	1,
																	paletteCodePtr);
					break;
				
				case kTIFFType:
				case kGeoTIFFType:
					paletteFileStreamPtr = GetFileStreamPointer (fileInfoPtr);
					continueFlag = ReadOneBytePalette (colorSpecPtr,
																	fileInfoPtr,
																	paletteFileStreamPtr, 
																	displaySpecsPtr, 
																	classSymbolPtr, 
																	fileInfoPtr->colorTableOffset, 
																	512, 
																	1,
																	paletteCodePtr);
					break;
				
				case kImagineType:
					continueFlag = ReadImagineClassPalette (fileInfoPtr,
																			colorSpecPtr, 
																			displaySpecsPtr, 
																			classSymbolPtr,
																			paletteCodePtr);
					break;
					
				case kArcViewDefaultSupportType:
					paletteFileStreamPtr = GetSupportFileStreamPointer (
														windowInfoHandle, &supportHandleStatus);
														
					continueFlag = ReadArcViewColorPalette (colorSpecPtr, 
																			fileInfoPtr, 
																			paletteFileStreamPtr,
																			displaySpecsPtr, 
																			classSymbolPtr, 
																			paletteCodePtr);
												
					CloseFile (paletteFileStreamPtr);
					
					UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);
					break;				

				#if include_gdal_capability					
					case kArcViewType:						
						continueFlag = ReadGDALColorTable (colorSpecPtr,
																		fileInfoPtr, 
																		displaySpecsPtr, 
																		classSymbolPtr, 
																		paletteCodePtr);
						break;
				#endif	// include_gdal_capability
					
				}	// end "switch (fileInfoPtr->ancillaryInfoformat)"
			break;
			
		case kDefaultColors:
		case kDefaultGrays:
		case kCorrelationMatrixColors:
		case kAVHRR_NDVI_Colors:
		case kMODIS_NDVI_Colors:
		case kProbablilityColors:
		case kProbablilityColors2:
		case kPaletteHandle:	
			continueFlag = ReadPaletteFromResource (fileInfoPtr,
																	colorSpecPtr,
																	displaySpecsPtr,
																	channelThematicDisplayTypeFlag,
																	inputResourcePHandle,
																	inputPaletteOffset,
																	numPaletteEntriesUsed, 
																	classSymbolPtr, 
																	thematicPaletteType,
																	numberClasses,
																	classGroupCode,
																	paletteCodePtr);
			break;
			
		case kFalseColors:
			continueFlag = CreateFalseColorPalette (fileInfoPtr,
																	colorSpecPtr, 
																	classSymbolPtr,
																	numberClasses,
																	classGroupCode,
																	paletteCodePtr);
			break;
			
		case kComputedGrays:
			continueFlag = CreateGrayLevelPalette (colorSpecPtr,
																numberClasses);
			break;
			
		case kUserDefinedColors:
			continueFlag = FALSE;
			break;
			
		case kCopyColorsFromClassTable:
			continueFlag = CopyColorsFromClassTableToGroupTable (fileInfoPtr,
																					colorSpecPtr,
																					displaySpecsPtr,
																					numPaletteEntriesUsed,
																					numberClasses,
																					paletteCodePtr);
			
			displaySpecsPtr->thematicGroupPaletteType = kUserDefinedColors;
			break;
			
		}	// end "switch (thematicPaletteType)"
				
				// Unlock handles.
	
	if (fileInfoPtr != NULL)	
		CheckAndUnlockHandle (fileInfoPtr->classDescriptionH);
				
			// Unlock the file information handle if needed.							
	
	MHSetState (imageFileInfoH, fileHandleStatus);
	
	return (continueFlag);

}	// end "LoadColorSpecTable"                                                    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadDefaultProjectFalseColorChannels
//
//	Software purpose:	The purpose of this routine is to load the default
//							FALSE color channels for the current project. This
// 						may be needed if this is the first time to be used
//							or the number of channels in the project changed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1996
//	Revised By:			Larry L. Biehl			Date: 12/09/1996	 

void LoadDefaultProjectFalseColorChannels (void)

{   
	if (gProjectInfoPtr->falseColorPaletteRed < 0)
		{
		if (gProjectInfoPtr->numberStatisticsChannels == 1)		
			{
			gProjectInfoPtr->falseColorPaletteRed = 
					gProjectInfoPtr->falseColorPaletteGreen = 
							gProjectInfoPtr->falseColorPaletteBlue = 0;				
			
			}	// end "if (...->numberStatisticsChannels == 1)" 
		
		else if (gProjectInfoPtr->numberStatisticsChannels == 2)		
			{
			gProjectInfoPtr->falseColorPaletteRed = 1;
			gProjectInfoPtr->falseColorPaletteGreen = 
							gProjectInfoPtr->falseColorPaletteBlue = 0;		
			
			}	// end "else if (...->totalNumberChannels == 2)" 
			
		else if (gProjectInfoPtr->numberStatisticsChannels == 3)
			{
			gProjectInfoPtr->falseColorPaletteRed = 2;
			gProjectInfoPtr->falseColorPaletteGreen = 1;
			gProjectInfoPtr->falseColorPaletteBlue = 0;	
			
			}	// end "else if (...->numberStatisticsChannels == 3)" 
			
		else if (gProjectInfoPtr->numberStatisticsChannels == 12)
			{
			gProjectInfoPtr->falseColorPaletteRed = 10;
			gProjectInfoPtr->falseColorPaletteGreen = 8;
			gProjectInfoPtr->falseColorPaletteBlue = 6;
			
			}	// end "else if ...->numberStatisticsChannels == 12)" 
			
		else if (gProjectInfoPtr->numberStatisticsChannels < 85)
			{
			gProjectInfoPtr->falseColorPaletteRed = 3;
			gProjectInfoPtr->falseColorPaletteGreen = 2;
			gProjectInfoPtr->falseColorPaletteBlue = 1;
			
			}	// end "if (gProjectInfoPtr->numberStatisticsChannels < 85)" 
					
		else	// gProjectInfoPtr->numberStatisticsChannels >= 85 
			{
					// Assume Imaging Spectrometer data.	
					
			gProjectInfoPtr->falseColorPaletteRed = 49;
			gProjectInfoPtr->falseColorPaletteGreen = 26;
			gProjectInfoPtr->falseColorPaletteBlue = 16;	
			
			}	// end "else ...->numberStatisticsChannels >= 85" 
				
		}	// end "if (gProjectInfoPtr->falseColorPaletteRed < 0)"

}	// end "LoadDefaultProjectFalseColorChannels" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadTwoBytePalette
//
//	Software purpose:	The purpose of this routine is to read the color
//							palette from the specified file.  Currently this
//							routine is used to read palettes from ERDAS trl and
//							SUN screendump files.  
//
//							The code for the 'colorOrder' input variable is:
//								=1 RGB
//								=2 GRB
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ReadArcViewColorPalette in SArcView.cpp
//							ReadGDALColorTable in SGDALInterface.cpp
//							ReadOneBytePalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/12/2011
//	Revised By:			Larry L. Biehl			Date: 04/30/2013	

void LoadTwoBytePalette (
				ColorSpec*							colorSpecPtr,
				FileInfoPtr							imageFileInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				UInt16*								classSymbolPtr,
				UInt16*								paletteCodePtr,
				UInt32								colorVectorLength,
				UInt16*								vectorBluePtr,
				UInt16*								vectorGreenPtr,
				UInt16*								vectorRedPtr)

{
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
	
	UInt32								index,
											inputIndex;
	
	SInt16								entry;
	
	UInt16								numberLevels;
	
	Boolean								classCodeFlag;   
	
	
	entry = 1;
	
			// Get the number of classes.  If the number of classes is
			// more than 254 then set the number of colors to be read from 
			// the color table file to 254.  This is so that there is 		
			// space in the 256 entry palette for white and black.			
			// Note that if there is less that 254 classes then space is 	
			// allowed for a background color.	If there is 254 classes or 
			//	more then the background color is forced to be white.		
			// The palette specified by colorSpecsPtr is limited to 256 colors.		
	
	numberLevels = MIN (256, (UInt16)displaySpecsPtr->numberPaletteLevels);
	
	classCodeFlag = (displaySpecsPtr->classGroupCode == kClassDisplay);
	
			// Get the class to group pointer if needed.
	
	if (!classCodeFlag)
		{		
		classToGroupPtr = GetClassToGroupPointer (imageFileInfoPtr);
		groupToPalettePtr = GetGroupToPalettePointer (imageFileInfoPtr);
			
		}	// end "if (!classCodeFlag)"
	
	for (index=0; index<numberLevels; index++)
		{
		if (classSymbolPtr != NULL)
			{
			if (classCodeFlag)
				inputIndex = classSymbolPtr[index];
				
			else	// !classCodeFlag
				{ 
				inputIndex = classToGroupPtr[index];
				inputIndex = groupToPalettePtr[inputIndex];
				inputIndex = classSymbolPtr[inputIndex];
					
				}	// end "else  !classCodeFlag"
			
			}	// end "if (classSymbolPtr != NULL)"
		
		else	// classSymbolPtr == NULL
			inputIndex = index;
		
		if (inputIndex >= colorVectorLength)
			inputIndex = (inputIndex % colorVectorLength);
			
		colorSpecPtr->value     = entry;
		
		colorSpecPtr->rgb.red   = vectorRedPtr[inputIndex];
		colorSpecPtr->rgb.green = vectorGreenPtr[inputIndex];
		colorSpecPtr->rgb.blue  = vectorBluePtr[inputIndex];
		
		/* 
		Start test code
				// List the palette to the text window. This is for testing.
				
		char									string[256];		// for testing only
	
		SInt16								red, green, blue;		// for testing only
	
		
		red = colorSpecPtr->rgb.red/256;
		green = colorSpecPtr->rgb.green/256;
		blue = colorSpecPtr->rgb.blue/256;
										
		sprintf (string, "%d %3d %3d %3d%s",
					inputIndex,
					red,
					green,
					blue,
					gEndOfLine);
						
		OutputString ((CMFileStream*)NULL,
							string,
							0, 
							gOutputForce1Code,
							TRUE);
		// End test code.
		*/
		colorSpecPtr++;
		entry++; 
		
		}	// end "for (index=0; index<numberLevels; index++)"

		// Unlock the class to group handle in case it is locked.

	UnlockGroupTablesHandle (imageFileInfoPtr); 	

}	// end "LoadTwoBytePalette"                                                 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MDisposePalette
//
//	Software purpose:	The purpose of this routine is to dispose of the input 
//							palette object.
//
//	Parameters in:		Palette Object
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1995
//	Revised By:			Larry L. Biehl			Date: 12/19/2018

void MDisposePalette (
				CMPaletteInfo						paletteObject)

{  
	if (paletteObject != NULL)
		{   
		#if defined multispec_mac             		                                             
			DisposePalette (paletteObject);		
		#endif	// defined multispec_mac
				
				      
		#if defined multispec_win		
			delete paletteObject;                                		   
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			//wxDELETE (paletteObject);
			delete paletteObject;
		#endif

		}	// end "if (paletteObject != NULL)"              

}	// end "MDisposePalette"                                                       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MGetEntryColor
//
//	Software purpose:	The purpose of this routine is to set the
//							specified entry in the palette to the 
//							requested color.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1997
//	Revised By:			Larry L. Biehl			Date: 12/19/2018

void MGetEntryColor (
				CMPaletteInfo						paletteObject,
				UInt16								entry, 
				RGBColor*							theColorPtr)

{          
	#if defined multispec_mac             	                                                      
		::GetEntryColor (paletteObject, entry, theColorPtr);	
	#endif	// defined multispec_mac
			
	#if defined multispec_win	
		PALETTEENTRY		paletteEntry; 
		                           
		paletteObject->GetPaletteEntries (entry, 1, &paletteEntry); 
	               
		theColorPtr->red = (UInt16)(paletteEntry.peRed << 8);
		theColorPtr->green = (UInt16)(paletteEntry.peGreen << 8);
		theColorPtr->blue = (UInt16)(paletteEntry.peBlue << 8); 	   
	#endif	// defined multispec_win  

   #if defined multispec_lin
   	/*
      unsigned char red,green,blue;
      paletteObject->GetRGB (entry, &red, &green, &blue);
      theColorPtr->red = (UInt16)red;
      theColorPtr->green = (UInt16)green;
      theColorPtr->blue = (UInt16)blue;
		*/
      theColorPtr->red = paletteObject->mPaletteObject[entry].red;
      theColorPtr->green = paletteObject->mPaletteObject[entry].green;
      theColorPtr->blue = paletteObject->mPaletteObject[entry].blue;
   #endif

}	// end "MGetEntryColor"                                                       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 MGetNumberPaletteEntries
//
//	Software purpose:	The purpose of this routine is to return the number of
//							palette entries in the input palette object.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/13/1995
//	Revised By:			Larry L. Biehl			Date: 12/11/1995	

SInt16 MGetNumberPaletteEntries (
				CMPaletteInfo						paletteObject)

{  
	if (paletteObject != NULL)
		{       
		#if defined multispec_mac             		                                             
			return ((*paletteObject)->pmEntries);		
		#endif	// defined multispec_mac
				
				      
      #if defined multispec_win	|| defined multispec_lin		                           
			return (paletteObject->GetNumberPaletteEntries ());		   
		#endif	// defined multispec_win 
		
		}	// end "if (paletteObject != NULL)"
		
	return (NULL);  
	
}	// end "MGetNumberPaletteEntries" 
                                                                  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean MNewPalette
//
//	Software purpose:	The purpose of this routine is to obtain a handle
//							or pointer to a new palette.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	'Handle' to Palette Object
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/27/1995
//	Revised By:			Larry L. Biehl			Date: 06/05/1995	

Boolean MNewPalette (
				CMPaletteInfo*						paletteObjectPtr,
				SInt16								numberEntries)

{       
	Boolean								continueFlag;
	   
	   
	#if defined multispec_mac                      	                        
		*paletteObjectPtr = ::NewPalette (numberEntries, 0, 2, 0);
		
		continueFlag = (*paletteObjectPtr != NULL);	
	#endif	// defined multispec_mac
			      
	#if defined multispec_win	   
		LOGPALETTE*		logicalPalettePtr; 
		
		
		if (*paletteObjectPtr == NULL)
			{
					// Get new palette class.
					
			*paletteObjectPtr = new CMPalette;
			
			if (*paletteObjectPtr == NULL)
																							return (FALSE);
																					
			}	// end "if (*paletteObjectPtr == NULL)"
		
		
		logicalPalettePtr = (LOGPALETTE*)MNewPointer (
							sizeof (LOGPALETTE) + sizeof (PALETTEENTRY));
							
		if (logicalPalettePtr == NULL)
																							return (FALSE);
																					
		(*paletteObjectPtr)->SetNumberPaletteEntries (1);										
		logicalPalettePtr->palNumEntries =
												(*paletteObjectPtr)->GetNumberPaletteEntries ();
		logicalPalettePtr->palVersion = 0x300;
		logicalPalettePtr->palPalEntry->peRed = 255;
		logicalPalettePtr->palPalEntry->peGreen = 255;
		logicalPalettePtr->palPalEntry->peBlue = 255;
		logicalPalettePtr->palPalEntry->peFlags = 0;
		
		continueFlag = (*paletteObjectPtr)->CPalette::CreatePalette (logicalPalettePtr);
		
		CheckAndDisposePtr ((Ptr)logicalPalettePtr); 
		  
					// Now resize to the requested number of entries.
			
		if (continueFlag)
			continueFlag = (*paletteObjectPtr)->ResizePalette (numberEntries); 
			
		if (continueFlag)
			{
			(*paletteObjectPtr)->SetNumberPaletteEntries (numberEntries);
			
					// Indicate that a palette does exist
					
			(*paletteObjectPtr)->mPaletteObject = (PaletteHandle)1;
			
			}	// end "if (continueFlag)"	   
	#endif	// defined multispec_win  
	                    
   #if defined multispec_lin	
		if (*paletteObjectPtr == NULL)
			{
					// Get new palette class.

			*paletteObjectPtr = new CMPalette (numberEntries);

			if (*paletteObjectPtr == NULL)
																							return (FALSE);

			}	// end "if (*paletteObjectPtr == NULL)"
	
		return (TRUE);
   #endif	// defined multispec_lin
	
	return (continueFlag);
	
}	// end "MNewPalette"                                                        



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean MSetEntryColor
//
//	Software purpose:	The purpose of this routine is to set the
//							specified entry in the palette to the 
//							requested color.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/27/1995
//	Revised By:			Larry L. Biehl			Date: 07/09/2015	

void MSetEntryColor (
				CMPaletteInfo						paletteObject,
				UInt16								entry, 
				RGBColor*							theColorPtr)

{
	if (entry >= 0 && entry < 256)
		{
		#if defined multispec_mac
			::SetEntryColor (paletteObject, entry, theColorPtr);
		#endif	// defined multispec_mac
		
		#if defined multispec_win
			PALETTEENTRY		paletteEntry;
		
		
			paletteEntry.peRed = (UInt8)(theColorPtr->red >> 8);
			paletteEntry.peGreen = (UInt8)(theColorPtr->green >> 8);
			paletteEntry.peBlue = (UInt8)(theColorPtr->blue >> 8);
			//paletteEntry.peFlags = PC_NOCOLLAPSE + PC_RESERVED;
			paletteEntry.peFlags = PC_RESERVED;
			//paletteEntry.peFlags = 0;
		
			paletteObject->SetPaletteEntries (entry, 1, &paletteEntry);
		#endif	// defined multispec_win

		#if defined multispec_lin
			//bool continueFlag;
			//UInt16 numberColors = paletteObject->GetNumberPaletteEntries ();
			/*
			if (entry < numberColors)
				{
				UInt8 redp[numberColors];
				UInt8 greenp[numberColors];
				UInt8 bluep[numberColors];
				for (int index = 0; index < numberColors; index++)
					{
					continueFlag = paletteObject->GetRGB (index,
																		&redp[index],
																		&greenp[index],
																		&bluep[index]);
			 
					}	// end "for (index=0; index<numberColors; index++)"
			 
						// Now make changes in the palette
			 
				if (continueFlag)
					{
					//redp[entry] = (UInt8)(theColorPtr->red >> 8);
					//greenp[entry] = (UInt8)(theColorPtr->green >> 8);
					//bluep[entry] = (UInt8)(theColorPtr->blue >> 8);
					redp[entry] = (UInt8)theColorPtr->red;
					greenp[entry] = (UInt8)theColorPtr->green;
					bluep[entry] = (UInt8)theColorPtr->blue;
					paletteObject->Create (numberColors,redp,greenp,bluep);

					}	// end "if (continueFlag)"
			 
				}	// end if (entry < numberColors)

			else	// entry >= numberColors
				{
				UInt8 redp[entry];
				UInt8 greenp[entry];
				UInt8 bluep[entry];
				for (int index = 0; index < numberColors; index++)
					{
					continueFlag = paletteObject->GetRGB (index,
																		&redp[index],
																		&greenp[index],
																		&bluep[index]);
			 
					}	// end "for (index=0; index<numberColors; index++)"
			 
				for (int index=numberColors; index<(entry+1); index++)
					{
					redp[index] = 255;
					greenp[index] = 255;
					bluep[index] = 255;
			 
					}	// end "for (index=0; index<numberColors; index++)"
			 
						// Now make changes in the palette
			 
				if (continueFlag)
					{
					redp[entry] = (UInt8)theColorPtr->red;
					greenp[entry] = (UInt8)theColorPtr->green;
					bluep[entry] = (UInt8)theColorPtr->blue;
					paletteObject->Create ((entry+1),redp,greenp,bluep);
			 
					}	// end "if (continueFlag)"
			 
				}	// end "else entry >= numberColors"
			*/
			paletteObject->mPaletteObject[entry].red = (UInt8)theColorPtr->red;
			paletteObject->mPaletteObject[entry].green = (UInt8)theColorPtr->green;
			paletteObject->mPaletteObject[entry].blue = (UInt8)theColorPtr->blue;
		#endif	// defined multispec_lin
		
		}	// end "if (entry >= 0 && entry < 256)"

}	// end "MSetEntryColor" 
                                                                  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean MVerifyPaletteSize
//
//	Software purpose:	The purpose of this routine is to verify that the size of the
//							palette is set to the size required.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	'Handle' to Palette Object
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/27/1995
//	Revised By:			Larry L. Biehl			Date: 12/19/2018

Boolean MVerifyPaletteSize (
				CMPaletteInfo						paletteObject,
				UInt32								numberEntries)

{
	Boolean 								continueFlag  = FALSE;
	
	
	#if defined multispec_mac                                     							
		if ((*paletteObject)->pmEntries != numberEntries)
			{
					// Use this routine when new routines can be accessed.	
								 
			::ResizePalette (paletteObject, numberEntries); 
						
			}	// end "if (*(displaySpecsPtr->paletteObject)->pmEntries..."
	
		continueFlag = ((*paletteObject)->pmEntries == numberEntries);
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		continueFlag = paletteObject->ResizePalette ((UInt16)numberEntries);
		
	   if (continueFlag)
	   	paletteObject->SetNumberPaletteEntries ((UInt16)numberEntries);
	#endif	// defined multispec_win  

   #if defined multispec_lin
   			// Note that 256 entries are available in the palette.
		//if (paletteObject != NULL &&
		//						paletteObject->GetNumberPaletteEntries () >= numberEntries)
		if (paletteObject != NULL && numberEntries <= 256)
			continueFlag = TRUE;
	
	   if (continueFlag)
	   	paletteObject->SetNumberPaletteEntries ((UInt16)numberEntries);
		/*
		UInt16 redIndex, greenIndex, blueIndex;
		UInt8 redpalette[numberEntries];
		UInt8 greenpalette[numberEntries];
		UInt8 bluepalette[numberEntries];

		for (int index = 0; index < numberEntries; index++)
			{
			redpalette[index] = (UInt8)(255);
			greenpalette[index] = (UInt8)(255);
			bluepalette[index] = (UInt8)(255);
			
			}	// end "for (index=0; index<numberColors; index++)"
	
		paletteObject->SetNumberPaletteEntries ((UInt16) numberEntries);
		continueFlag = paletteObject->Create (numberEntries,
															redpalette,
															greenpalette,
															bluepalette);
		*/
   #endif
	
	return (continueFlag);
      
}	// end "MVerifyPaletteSize" 
                                                                  


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean PaletteExists
//
//	Software purpose:	The purpose of this routine is to determine whether the
//							palette has been set up for the input CMPaletteInfo.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	'Handle' to Palette Object
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1995
//	Revised By:			Larry L. Biehl			Date: 12/19/2018

Boolean PaletteExists (
				CMPaletteInfo						paletteObject)

{          
	#if defined multispec_mac 
		return (paletteObject != NULL);
	#endif	// defined multispec_mac
	
	#if defined multispec_mac_swift
		return (FALSE);
	#endif	// defined multispec_mac_swift
			      
	#if defined multispec_win 
	   Boolean			paletteExistsFlag = FALSE;

		if (paletteObject != NULL && paletteObject->mPaletteObject != NULL)
			paletteExistsFlag = TRUE;

		return (paletteExistsFlag); 
	#endif	// defined multispec_win
	
   #if defined multispec_lin
		Boolean paletteExistsFlag = FALSE;

		//if (paletteObject != NULL && paletteObject->mPaletteObject != 0)
		if (paletteObject != NULL)
			paletteExistsFlag = TRUE;
		//else // It does not exist. Make sure its NULL
		//	paletteObject = NULL;

		return (paletteExistsFlag);
   #endif

}	// end "PaletteExists"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadOneBytePalette
//
//	Software purpose:	The purpose of this routine is to read the color
//							palette from the specified file.  Currently this
//							routine is used to read palettes from ERDAS trl and
//							SUN screendump files.  
//
//							The code for the 'colorOrder' input variable is:
//								=1 RGB
//								=2 GRB
//
//	Parameters in:		window
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			CreateThematicColorPalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1989
//	Revised By:			Larry L. Biehl			Date: 07/23/2018

Boolean ReadOneBytePalette (
				ColorSpec*							colorSpecPtr, 
				FileInfoPtr							imageFileInfoPtr, 
				CMFileStream*						paletteFileStreamPtr, 
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt32								paletteStart, 
				SInt16								paletteOffset, 
				SInt16								colorOrder,
				UInt16*								paletteCodePtr)
 
{
	UInt16								*red2ByteStringPtr,
											*green2ByteStringPtr,
											*blue2ByteStringPtr;
	
	unsigned char						*tempVector1Ptr,
											*tempVector2Ptr,
											*tempVector3Ptr;
	
	UInt32								count,
											index;
										
	UInt32								posOff;
	
	SInt16								errCode;
										
	UInt16								vector1Ptr[256],
											vector2Ptr[256],
											vector3Ptr[256];
	
	Boolean								closeFileAtEnd;
											
		
	if (paletteFileStreamPtr == NULL ||
				displaySpecsPtr == NULL || 
					colorSpecPtr == NULL ||
						imageFileInfoPtr == NULL)
																						return (FALSE);
	
			// Check that there is a valid class to group handle if needed.		
			
	if (displaySpecsPtr->classGroupCode != kClassDisplay && 
									imageFileInfoPtr->groupTablesHandle == NULL)
																						return (FALSE);
																				
			// If the file is not open, then open a path to the file that			
			// contains the palette. The OpenFileReadOnly routine will
			// check if the file is already opened.														
		
	errCode = noErr;
	closeFileAtEnd = FALSE;
	if (!FileOpen (paletteFileStreamPtr))
		{
		errCode = OpenFileReadOnly (paletteFileStreamPtr, 
												kDontResolveAliasChains,
												kLockFile, 
												kDoNotVerifyFileStream);
	
		IOCheck (errCode, paletteFileStreamPtr);
		
		if (errCode == noErr)
			closeFileAtEnd = TRUE;
			
		}	// end "if (!FileOpen (paletteFileStreamPtr))"
		
			// Continue if okay.																	

	if (errCode == noErr)
		{
				// Put the colors into the palette.											
				
		if (imageFileInfoPtr->colorTableValueBytes == 1)
			{
			tempVector1Ptr = gTextString;
			tempVector2Ptr = gTextString2;
			tempVector3Ptr = gTextString3;
			
					// Get the number of bytes to read.
						
			count = MIN (256, paletteOffset); 
			
			}	// end "if (...->colorTableValueBytes == 1)" 
			
		else	// imageFileInfoPtr->colorTableValueBytes != 1 
			{
			tempVector1Ptr = (unsigned char*)vector1Ptr;
			tempVector2Ptr = (unsigned char*)vector2Ptr;
			tempVector3Ptr = (unsigned char*)vector3Ptr;
			
					// Get the number of bytes to read.
						
			count = MIN (512, paletteOffset); 
			
			}	// end "else ...->colorTableValueBytes != 1" 
	
				// Read the colors for the classes from the file.						
				// Read the values for the first color										
  	
		posOff = (SInt32)paletteStart;                               
		errCode = MSetMarker (paletteFileStreamPtr, 
										fsFromStart, 
										posOff,
										kErrorMessages);
		
		if (errCode == noErr)
			{                    
			errCode = MReadData (paletteFileStreamPtr, 
										&count,
										tempVector1Ptr,
										kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
				// Read the values for the second color.									
  	
  		if (errCode == noErr)
  			{
			posOff += paletteOffset;                                     
			errCode = MSetMarker (paletteFileStreamPtr, 
											fsFromStart,
											posOff,
											kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
	
  		if (errCode == noErr)
  			{                                                              
			errCode = MReadData (paletteFileStreamPtr, 
											&count, 
											tempVector2Ptr,
											kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
		
				// Read the values for the third color.									
  	
  		if (errCode == noErr)
  			{
			posOff += paletteOffset;                                      
			errCode = MSetMarker (paletteFileStreamPtr, 
											fsFromStart,
											posOff,
											kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
	
  		if (errCode == noErr)
  			{                                                              
			errCode = MReadData (paletteFileStreamPtr, 
										&count, 
										tempVector3Ptr,
										kErrorMessages);
			
			}	// end "if (errCode == noErr)" 
			
				// Loop for the number of classes.  Convert one byte value to		
				// a two byte value.																
				
  		if (errCode == noErr)
  			{
					// Make certain that the color table is stored in a two-byte format.
				
			if (imageFileInfoPtr->colorTableValueBytes == 1)
				{
						// 'count' is the number of entries read in the color table.
					
				for (index=0; index<count; index++)
					{
					#if defined multispec_mac || defined multispec_win
						vector1Ptr[index] = (UInt16)tempVector1Ptr[index] * 256;
						vector2Ptr[index] = (UInt16)tempVector2Ptr[index] * 256;
						vector3Ptr[index] = (UInt16)tempVector3Ptr[index] * 256;
					#endif	// defined multispec_mac || defined multispec_win
					
					#if defined multispec_lin
						vector1Ptr[index] = (UInt16)tempVector1Ptr[index];
						vector2Ptr[index] = (UInt16)tempVector2Ptr[index];
						vector3Ptr[index] = (UInt16)tempVector3Ptr[index];
					#endif	// defined multispec_lin
				
					}	// end "for (index=0; index<=numberColors; index++)" 
				
				}	// end "if (...->colorTableValueBytes == 1)" 
				
			else	// imageFileInfoPtr->colorTableValueBytes != 1 
				{
						// Swap the bytes if needed.											
						
				if (imageFileInfoPtr->swapColorTableBytes)
					{
					count /= 2;
					
					for (index=0; index<count; index++)
						{
						vector1Ptr[index] = Swap2Bytes (vector1Ptr[index]);
						vector2Ptr[index] = Swap2Bytes (vector2Ptr[index]);
						vector3Ptr[index] = Swap2Bytes (vector3Ptr[index]);
					
						}	// end "for (index=0; index<=numberColors; index++)" 
				
					}	// end "if (imageFileInfoPtr->swapColorTableBytes)" 
					
				}	// end "else ...->colorTableValueBytes != 1" 
					
						// Set up the char string pointers relative to the order	of	
						// the colors.																
						
  			if (colorOrder == 1)		// For Sun Screen Dump and TIFF type  files.												
  				{
  						// Order of colors is Red, Green, Blue.							
					
				red2ByteStringPtr = vector1Ptr;
				green2ByteStringPtr = vector2Ptr;
				blue2ByteStringPtr = vector3Ptr;
				
				}	// end "if (colorOrder == 1)" 
				
			else	// colorOrder == 2 
				{
  						// Order of colors is Green, Red, Blue.							
					
				red2ByteStringPtr = vector2Ptr;
				green2ByteStringPtr = vector1Ptr;
				blue2ByteStringPtr = vector3Ptr;
  						
  				}	// end "else colorOrder == 2" 
			
			LoadTwoBytePalette (colorSpecPtr, 
										imageFileInfoPtr, 
										displaySpecsPtr, 
										classSymbolPtr, 
										paletteCodePtr,
										count,
										blue2ByteStringPtr,
										green2ByteStringPtr,
										red2ByteStringPtr);

			}	// end "if (errCode == noErr)"
		
		}	// end "if (errCode == noErr)"
		
	if (closeFileAtEnd)
		CloseFile (paletteFileStreamPtr);
	
	return (errCode == noErr);
	
}	// end "ReadOneBytePalette"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadPaletteFromResource
//
//	Software purpose:	The purpose of this routine is to read the color
//							palette from the default resource file. 
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1990
//	Revised By:			Larry L. Biehl			Date: 05/03/2011

Boolean ReadPaletteFromResource (
				FileInfoPtr							fileInfoPtr, 
				ColorSpec*							colorSpecPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				Boolean								channelThematicDisplayTypeFlag,
				CMPaletteInfo						inputResourcePHandle,
				UInt16								inputPaletteOffset,
				UInt16								numberPaletteEntriesUsed, 
				UInt16*								classSymbolPtr, 
				UInt16								thematicPaletteType,
				UInt32								numberClasses,
				SInt16								classGroupCode,
				UInt16*								paletteCodePtr)
 
{
	RGBColor								theColor;
	
	CMPaletteInfo						resourcePHandle;
	
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
											
	double								doublePaletteIndex,
											paletteIndexSkip;
											
	SInt16								paletteId;
	
	UInt16								maxPaletteValue;
	
	UInt16								backgroundOffset,
											classValueRange,
											index,
											numberColors,
											numberResourcePaletteEntries,
											offset,
											paletteIndex,
											paletteResourceIndex;
	
	Boolean								backgroundFlag,
											classCodeFlag,
											invertPaletteFlag,
											useClassSymbolPtrFlag;
											
											
   #ifdef multispec_lin
      maxPaletteValue = 255;
   #else
      maxPaletteValue = 65535;
   #endif
			// Check that there is a valid class to group handle if needed.		
			
	if (classGroupCode != kClassDisplay && (fileInfoPtr == NULL ||
											fileInfoPtr->groupTablesHandle == NULL))
																							return (FALSE);
		
			// Get the number of colors.						
			
	numberColors = (UInt16)MIN (256, numberClasses);
	
	if (thematicPaletteType == kPaletteHandle)
		{
		resourcePHandle = inputResourcePHandle;
		offset = inputPaletteOffset;
		
		}	// end "if (thematicPaletteType == kPaletteHandle)"
		
	else	// thematicPaletteType != kPaletteHandle
		{
		paletteId = GetPaletteID (thematicPaletteType);																							
		resourcePHandle = GetNewPalette (paletteId);
		offset = 2;
		
		}	// end "else thematicPaletteType != kPaletteHandle"
		
	if (resourcePHandle == NULL)
																							return (FALSE);
																				
			// Get the number of palette entries less white and black for
			// the non-paletteHandle case.				
	
	numberResourcePaletteEntries = MGetNumberPaletteEntries (resourcePHandle) - offset;
	
	classCodeFlag = (classGroupCode == kClassDisplay);	
	
			// Get the class to group pointer if needed.									
	
	if (!classCodeFlag)
		{
		classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
		groupToPalettePtr = GetGroupToPalettePointer (fileInfoPtr);
		
		}	// end "if (!classCodeFlag)" 

			// Determine if the class symbol pointer vector needs to be used.
			// It is used for color ramps.
			// Also get the palette index skip so that the colors will be scattered
			// throughout the palette when the number of classes is less than
			// the number of available palette entries.
	
	useClassSymbolPtrFlag = FALSE;
	paletteIndexSkip = 0;		
	if (thematicPaletteType == kCorrelationMatrixColors ||
						thematicPaletteType == kAVHRR_NDVI_Colors ||
										thematicPaletteType == kMODIS_NDVI_Colors)
		{
		classValueRange = numberColors;
		if (classSymbolPtr != NULL)
			{
			useClassSymbolPtrFlag = TRUE;
			classValueRange = classSymbolPtr[numberColors-1];
			
			}	// end "if (classSymbolPtr != NULL)"
		
		paletteIndexSkip = (double)(numberResourcePaletteEntries-2)/classValueRange;
		if (displaySpecsPtr != NULL &&
				displaySpecsPtr->displayType == k1_ChannelThematicDisplayType &&
					classValueRange > 2)
			paletteIndexSkip = 
							(double)(numberResourcePaletteEntries-2)/(classValueRange-2);
			
		}	// end "if (thematicPaletteType == kCorrelationMatrixColors || ..."
			
			// Determine if first class is the background class. This check
			// is for MultiSpec ascii, ERDAS and GAIA classifications.
	
	backgroundFlag = GetBackgroundIncludedFlag (fileInfoPtr, classSymbolPtr);
											
			// Determine if the palette is to be inverted.  This can occur for 
			// multispectral data being displayed in a thematic window.
			
	invertPaletteFlag = FALSE;
	if (displaySpecsPtr != NULL)
		invertPaletteFlag = displaySpecsPtr->invertValuesFlag[2];
		
			// If called from reformat processor and the number of palette entries
			// being used is one more than the number of classes then a class was added
			// for the background. One needs to include this first in the color table.
			
	if (gProcessorCode == kRefChangeFileFormatProcessor &&
						numberPaletteEntriesUsed == numberClasses + 1)
		{
		colorSpecPtr->value     = 0;
		colorSpecPtr->rgb.red   = maxPaletteValue;
		colorSpecPtr->rgb.green = maxPaletteValue;
		colorSpecPtr->rgb.blue  = maxPaletteValue;
		colorSpecPtr++;
		
		}	// end "if (gProcessorCode == kRefChangeFileFormatProcessor && ..."
																				
			// Copy the number of colors need from the resource palette to the	
			// display palette.			
	
	backgroundOffset = 0;
	
	doublePaletteIndex = 0;	
	for (index=0; index<numberColors; index++)
		{
				// Get the palette index for a class or group display.

		if (classCodeFlag)
			{
			if (useClassSymbolPtrFlag)
				{
				if (paletteIndexSkip > 1)
					paletteIndex = 
							(UInt16)((double)classSymbolPtr[index] * paletteIndexSkip);
				
				else	// paletteIndexSkip <= 1
					paletteIndex = classSymbolPtr[index];
					
				}	// end "if (useClassSymbolPtrFlag)"
			
			else	// !useClassSymbolPtrFlag
				{
				if (index != 0 && paletteIndexSkip > 1)
					{
					doublePaletteIndex += paletteIndexSkip;
					paletteIndex = (UInt16)doublePaletteIndex;
					
					}	// end "if (index != 0 && paletteIndexSkip > 1)"
					
				else	// index == 0 || paletteIndexSkip <= 1
					paletteIndex = index;
					
				}	// end "else !useClassSymbolPtrFlag"

			}	// end "if (classCodeFlag)"	
					
		else	// !classCodeFlag
			{ 
			paletteIndex = classToGroupPtr[index];
			paletteIndex = groupToPalettePtr[paletteIndex];
			if (useClassSymbolPtrFlag)
				{
				if (paletteIndexSkip > 1)
					paletteIndex = 
						(UInt16)((double)classSymbolPtr[paletteIndex] * paletteIndexSkip);
				
				else	// paletteIndexSkip <= 1
					paletteIndex = classSymbolPtr[paletteIndex];
				
				}	// end "if (useClassSymbolPtrFlag)"
			
			else	// !useClassSymbolPtrFlag
				{
				if (index != 0 && paletteIndexSkip > 1)
					{
					paletteIndex = (UInt16)((double)paletteIndex * paletteIndexSkip);
					
					}	// end "if (index != 0 && paletteIndexSkip > 1)"
					
				}	// end "else !useClassSymbolPtrFlag"
			
			}	// end "else !classCodeFlag"
		
		if (paletteIndex == 0 && backgroundFlag)
			{	
			GetEntryColor (resourcePHandle, 0, &theColor);
			backgroundFlag = FALSE;
			backgroundOffset = 1;
			
			}	// end "if (paletteIndex == 0 && backgroundFlag)"
			
		else	// paletteIndex != 0 || !backgroundFlag
			{
			if (channelThematicDisplayTypeFlag &&
											(index == 0 || index == numberColors - 1))
				{
				if (index == 0)
					paletteResourceIndex = 1;
				
				else	// index == numberColors - 1
					paletteResourceIndex = 0;
				
				}	// end "if (channelThematicDisplayTypeFlag && ..."
				
			else	// ...->displayType != k1_ChannelThematicDisplayType || ...
				{
						// Allow for a background offset. In otherwords the color
						// for the first class in the palette may have been set to
						// the background color of white.
				
				paletteIndex -= backgroundOffset;
				
						// Make certain that the palette index is within the proper		
						// range.																		
						
				if (paletteIndex >= numberResourcePaletteEntries)
					paletteIndex = (paletteIndex % numberResourcePaletteEntries);
				
				paletteResourceIndex = paletteIndex;
				if (invertPaletteFlag)
					paletteResourceIndex = numberResourcePaletteEntries - paletteIndex;
				
				paletteResourceIndex += offset;
					
				}	// end "else ...->displayType != k1_ChannelThematicDisplayType || ..."
			
			GetEntryColor (resourcePHandle, paletteResourceIndex, &theColor);
			
			}	// end "else paletteIndex != 0 || !backgroundFlag"
		
				// Adjust palette index assuming a class display.					
				
		paletteIndex++;
		
		colorSpecPtr->value     = index;
		colorSpecPtr->rgb.red   = theColor.red;
		colorSpecPtr->rgb.green = theColor.green;
		colorSpecPtr->rgb.blue  = theColor.blue;
		
		colorSpecPtr++;
		
		}	// end "for (index=1; index<=numberColors; index++)" 
	/*
			// This is to list the palette for use for uses such as copying to the 
			// Windows version.
			
	sprintf ((char*)gTextString, "paletteId = %d%s", paletteId, gEndOfLine);
	ListString ((char*)gTextString,  strlen ((char*)gTextString), gOutputTextH);

	UInt32 i;
	for (i=0; i<numberResourcePaletteEntries+2; i++)
		{
		GetEntryColor (resourcePHandle, i, &theColor);
		sprintf ((char*)gTextString, "%3ld %5ld, %5ld, %5ld%s", i, theColor.red, theColor.green, theColor.blue, gEndOfLine);
		ListString ((char*)gTextString,  strlen ((char*)gTextString), gOutputTextH);
	
		}	// end "for (i=0; i<=255; i++)"
		
	ListString ((char*)gEndOfLine,  strlen ((char*)gEndOfLine), gOutputTextH);
	*/	  				
			// If the number of classes is more than the number of colors in 
			// the palette, then check if first wrap around color is black. 		
			// If so indicate that the last black color will be used to display				
			// a class.																	
	
	if (numberClasses > numberPaletteEntriesUsed)
		{
		GetEntryColor (resourcePHandle, 2, &theColor);

		if (theColor.red == 0 && theColor.green == 0 && theColor.blue == 0)
			*paletteCodePtr |= 0x0004;

		}	// end "if (numberClasses > ...)"
			
	UnlockGroupTablesHandle (fileInfoPtr);
	
			// Dispose of the resource palette if needed.												
	
	if (thematicPaletteType != kPaletteHandle)	
		MDisposePalette (resourcePHandle);

	return (TRUE);
	
}	// end "ReadPaletteFromResource" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetBackgroundPaletteEntries
//
//	Software purpose:	The purpose of this routine is to set specified
//							palette entries to the background color based on
//							which classes or groups are not to be displayed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			CreateThematicColorPalette in paletteProc.c
//
//	Coded By:			Larry L. Biehl			Date: 01/22/1991
//	Revised By:			Larry L. Biehl			Date: 09/11/2006	

Boolean SetBackgroundPaletteEntries (
				CTabHandle							cTableHandle, 
				DisplaySpecsPtr					displaySpecsPtr, 
				FileInfoPtr							fileInfoPtr)
 
{
	CTabPtr								cTabPtr;
	
	SInt16*								displayClassGroupsPtr;
	
	CMPaletteInfo						paletteObject;
	
	UInt32								classIndex,
											numberClasses,
											numberEntries,
											paletteIndex;
	
	SInt16								setting;
	
		
			// Check if palette handles make sense.										
			
	paletteObject = displaySpecsPtr->paletteObject;
	numberEntries = MGetNumberPaletteEntries (paletteObject);
	cTabPtr = (CTabPtr)GetHandlePointer ((Handle)cTableHandle);
	if (numberEntries != (UInt16)(cTabPtr->ctSize + 1))
																						return (FALSE);
																				
			// Copy color table to the window palette.									
			// Make all palette entries animated except for black, white and		
			// the background (if a separate color).
		
	#if defined multispec_mac	
		if (gBlinkProcedure == kBlink2)
			{
			CTab2Palette (cTableHandle, paletteObject, pmAnimated+pmTolerant, 0);
			SetEntryUsage (paletteObject, 0, pmTolerant, 0);
			SetEntryUsage (paletteObject, numberEntries-1, pmTolerant, 0);
			
				//if (displaySpecsPtr->numberPaletteLevels <= 253)
				SetEntryUsage (paletteObject, numberEntries-2, pmTolerant, 0);
			
			}	// end "if (gBlinkProcedure == kBlink2)" 
			
		else	// gBlinkProcedure == kBlink1 
			CTab2Palette (cTableHandle, paletteObject, pmTolerant, 0);
			
		displaySpecsPtr->blinkProcedure = gBlinkProcedure;			
	#endif	// defined multispec_mac
	
	#if defined multispec_win	
		//SInt16 pmTolerant	= 0x0002;
		//SInt16 pmTolerant	= PC_RESERVED;
		CTab2Palette (cTableHandle, paletteObject, PC_RESERVED, 0);
		CTab2Palette (cTableHandle, displaySpecsPtr->backgroundPaletteObject, 0, 0);			
	#endif	// defined multispec_win

   #if defined multispec_lin
		CTab2Palette (cTableHandle, paletteObject, 0, 0);
		CTab2Palette (cTableHandle, displaySpecsPtr->backgroundPaletteObject, 0, 0);			
	#endif	// defined multispec_win
																						
			// Set classes/groups that are not to be displayed to the background	
			// color.																				
	
	displayClassGroupsPtr = (SInt16*)GetHandlePointer (
													displaySpecsPtr->displayClassGroupsHandle,
													kLock);
	numberClasses = fileInfoPtr->numberClasses;
		
	if (displaySpecsPtr->classGroupCode == kClassDisplay)
		setting = 0x0001;
		
	else	// displaySpecsPtr->classGroupCode != kClassDisplay 
		setting = 0x0100;
		
			// Set color for any class that is not to be displayed to the			
			// background color and indicate that it is not to be animated.		
	
	if (displayClassGroupsPtr != NULL)
		{
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
			if (!(displayClassGroupsPtr[classIndex] & setting))
				{
				paletteIndex = classIndex + displaySpecsPtr->paletteOffset;
				
				if (paletteIndex > numberEntries)
					break;
				
				MSetEntryColor (
					paletteObject, (SInt16)paletteIndex, &displaySpecsPtr->backgroundColor); 
				
            #if defined multispec_win || defined multispec_lin                                           
					MSetEntryColor (displaySpecsPtr->backgroundPaletteObject, 
											(SInt16)paletteIndex, 
											&displaySpecsPtr->backgroundColor);
				#endif	// defined multispec_win
					                   
				#if defined multispec_mac 
					SetEntryUsage (paletteObject, (SInt16)paletteIndex, pmTolerant, 0); 
				#endif	// defined multispec_mac
				
				#if defined multispec_win 
					SetEntryUsage (paletteObject, (SInt16)paletteIndex, 0, 0);
				#endif	// defined multispec_win
				
				}	// end "if (!(displayClassGroupsPtr[classIndex] & setting))" 
			
			}	// end "for (classIndex=0; classIndex<..." 
			
		}	// end "if (displayClassGroupsPtr != NULL)"
		
	CheckAndUnlockHandle (displaySpecsPtr->displayClassGroupsHandle);
		 
	return (TRUE);
		
}	// end "SetBackgroundPaletteEntries" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetPaletteSpecification
//
//	Software purpose:	The purpose of this routine is to set the palette
//							information for the specified window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ImageWControlEvent in controls.c
//							DisplayThematicDialogOK in SDisThem.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1996
//	Revised By:			Larry L. Biehl			Date: 06/22/2015	

void SetPaletteSpecification (
				DisplaySpecsPtr					displaySpecsPtr,
				SInt16								classGroupCode,
				SInt16								paletteSelection,
				Boolean								paletteUpToDateFlag)
 
{
			// Palette to be used.										
				
	if (classGroupCode == kClassDisplay)
		{	
		if (displaySpecsPtr->thematicClassPaletteType != paletteSelection)
			displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
			
		if (paletteSelection == kFalseColors && !paletteUpToDateFlag)
			displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
					
		displaySpecsPtr->thematicClassPaletteType = paletteSelection;
					
		if (displaySpecsPtr->displayType == k1_ChannelThematicDisplayType &&
												gDisplaySpecsDefault.structureLoadedFlag)
			gDisplaySpecsDefault.oneChannelThematicClassPaletteType = paletteSelection;
					
		}	// end "if (classGroupCode == kClassDisplay)" 
				
	else	// classGroupCode == kGroupDisplay 
		{	
		if (displaySpecsPtr->thematicGroupPaletteType != paletteSelection)
			displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
			
		if (paletteSelection == kFalseColors && !paletteUpToDateFlag)
			displaySpecsPtr->readPaletteFromDiskFlag = TRUE;
					
		displaySpecsPtr->thematicGroupPaletteType = paletteSelection;
					
		}	// end "else classGroupCode == kGroupDisplay" 								
						
	if (displaySpecsPtr->readPaletteFromDiskFlag)
		displaySpecsPtr->paletteUpToDateFlag = FALSE;
	
			// If palette is not up to date and this is not the first time
			// through for the image then indicate that the class or group
			// information has changed. Note that the displayedLineStart
			// parameter will be 0 if this is the first time that the
			// window has been displayed.
				
	if (!displaySpecsPtr->paletteUpToDateFlag && 
													displaySpecsPtr->displayedLineStart > 0)
		{
		Handle fileInfoHandle = GetActiveImageFileInfoHandle ();
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		
		if (classGroupCode == kClassDisplay)										
			fileInfoPtr->classChangedFlag = TRUE;
			
		else	// classGroupCode == kGroupDisplay								
			fileInfoPtr->groupChangedFlag = TRUE;
			
		#if defined multispec_lin
				// Make sure that wxWidgets knows the document has changed.
			gActiveImageViewCPtr->GetDocument ()->Modify (TRUE);
		#endif // defined multispec_lin
		
		}	// end "if (!displaySpecsPtr->paletteUpToDateFlag && ..."
		
}	// end "SetPaletteSpecification"  


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpPalettePopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the
//							palette popup menu according to the given Thematic image
//								file specifications.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	paletteCode to be used.
//
// Called By:			ImageWControlEvent in controls.c
//							DisplayThematicDialog in SDisThem.cpp
//							CMDisplayThematicDlg::OnInitDialog in WDistDlg.cpp
//							CMDisplayThematicDlg::OnDropdownPaletteCombo in WDistDlg.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/31/1990
//	Revised By:			Larry L. Biehl			Date: 04/27/2018

SInt16 SetUpPalettePopUpMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpPaletteMenu,
				SInt16								fileFormat,
				SInt16								ancillaryInfoFormat,
				UInt32								numberClasses,
				Handle								classDescriptionH, 
				UInt32								colorTableOffset, 
				DisplaySpecsPtr					displaySpecsPtr, 
				SInt16								classGroupCode,
				SInt16								paletteCode)

{    
	#if defined multispec_mac	
		EnableMenuItem (popUpPaletteMenu, kImageDefaultColorTable);
		if ((fileFormat == 0 && displaySpecsPtr->filePaletteFlag) || 
					fileFormat == kErdas73Type || 
					fileFormat == kErdas74Type ||
					fileFormat == kMultiSpecClassificationType ||
					fileFormat == kArcViewType ||
					((fileFormat == kGeoTIFFType || fileFormat == kTIFFType) && 
						(ancillaryInfoFormat == kArcViewDefaultSupportType || 
															ancillaryInfoFormat == kErdas74Type)))
			{
			if (ancillaryInfoFormat == kArcViewDefaultSupportType)
				SetMenuItemText (popUpPaletteMenu, 
										kImageDefaultColorTable, 
										"\pArcView .clr file");
			
			else	// ancillaryInfoFormat != kArcViewDefaultSupportType
				SetMenuItemText (popUpPaletteMenu, 
										kImageDefaultColorTable, 
										"\pERDAS .trl file");
		
					// Determine if .clr or .trl file is present.										
					
			if (!displaySpecsPtr->filePaletteFlag)
				DisableMenuItem (popUpPaletteMenu, kImageDefaultColorTable);
	 		
	 		}	// end "if (fileFormat == kErdas73Type || ...)" 
	 		
		else if (fileFormat == kSunScreenDumpType) 
	 		SetMenuItemText (popUpPaletteMenu, 
									kImageDefaultColorTable, 
									"\pSUN screendump file");
	 		
		else if (fileFormat == kTIFFType) 
			{
	 		SetMenuItemText (popUpPaletteMenu, 
									kImageDefaultColorTable, 
									"\pTIFF file");
							
			if (colorTableOffset == 0)		// Implies no table exist in TIFF file
				DisableMenuItem (popUpPaletteMenu, kImageDefaultColorTable);
			
			}	// end "else if (fileFormat == kTIFFType)"
	 		
		else if (fileFormat == kGeoTIFFType)
			{ 
	 		SetMenuItemText (popUpPaletteMenu, 
									kImageDefaultColorTable, 
									"\pGeoTIFF file");
							
			if (colorTableOffset == 0)		// Implies no table exist in GeoTIFF file
				DisableMenuItem (popUpPaletteMenu, kImageDefaultColorTable);
			
			}	// end "else if (fileFormat == kTIFFType)"
	 		
		else if (fileFormat == kImagineType) 
	 		SetMenuItemText (popUpPaletteMenu, 
									kImageDefaultColorTable, 
									"\pImagine file");
	 		
	 	else	// fileFormat is not one of above but has color table. 
	 		{
	 		SetMenuItemText (popUpPaletteMenu, 
									kImageDefaultColorTable, 
									"\pImage file default");
			DisableMenuItem (popUpPaletteMenu, kImageDefaultColorTable);
	 		
	 		}	// end "else fileFormat != kSunScreenDumpType"
	 		
		EnableMenuItem (popUpPaletteMenu, kDefaultColors);
		EnableMenuItem (popUpPaletteMenu, kDefaultGrays);
		EnableMenuItem (popUpPaletteMenu, kCorrelationMatrixColors);
		EnableMenuItem (popUpPaletteMenu, kAVHRR_NDVI_Colors);
		EnableMenuItem (popUpPaletteMenu, kMODIS_NDVI_Colors);
		
		if (DetermineIfFalseColorAvailable (fileFormat, numberClasses, classDescriptionH)) 
			EnableMenuItem (popUpPaletteMenu, kFalseColors);
		
		else	// !DetermineIfFalseColorAvailable (fileFormat)	
			DisableMenuItem (popUpPaletteMenu, kFalseColors);
		
		EnableMenuItem (popUpPaletteMenu, kUserDefinedColors);
		if (classGroupCode == kClassDisplay &&
						displaySpecsPtr->thematicClassPaletteType != kUserDefinedColors)
			DisableMenuItem (popUpPaletteMenu, kUserDefinedColors);
		
		if (classGroupCode != kClassDisplay &&
						displaySpecsPtr->thematicGroupPaletteType != kUserDefinedColors)
			DisableMenuItem (popUpPaletteMenu, kUserDefinedColors);
	
		return paletteCode;
	#endif	// defined multispec_mac 
		
	#if defined multispec_win		
		SInt16								nextStringIndex;
		 
	
				// Set user defined values for palette combo box.
				
		CComboBox* comboBoxPtr = (CComboBox*)(dialogPtr->GetDlgItem (IDC_PaletteCombo));
		 
		comboBoxPtr->SetItemData (0, kDefaultColors);
		comboBoxPtr->SetItemData (1, kDefaultGrays);
		comboBoxPtr->SetItemData (2, kCorrelationMatrixColors);
		comboBoxPtr->SetItemData (3, kAVHRR_NDVI_Colors);  
		comboBoxPtr->SetItemData (4, kMODIS_NDVI_Colors);
		
				// Make sure that the 6th, 7th & 8th strings do not exist. 
		                             
		comboBoxPtr->DeleteString (8);
		comboBoxPtr->DeleteString (7);
		comboBoxPtr->DeleteString (6);
		comboBoxPtr->DeleteString (5);
		nextStringIndex = 5;    
		
				// Determine if the FALSE color item is to be included. 
				                                   
		if (DetermineIfFalseColorAvailable (fileFormat, numberClasses, classDescriptionH))
			{                                                     
		 	comboBoxPtr->InsertString (nextStringIndex, (LPCTSTR)_T("False Color..."));
			comboBoxPtr->SetItemData (nextStringIndex, kFalseColors);
			nextStringIndex++;
			
			}	// end "if (DetermineIfFalseColorAvailable (fileFormat))" 
			
				// Determine if image default color is to be include in the list.
	
		if ((fileFormat == 0 && displaySpecsPtr->filePaletteFlag) || 
					fileFormat == kErdas73Type || 
					fileFormat == kErdas74Type ||
					fileFormat == kMultiSpecClassificationType ||
					fileFormat == kArcViewType ||
					((fileFormat == kGeoTIFFType || fileFormat == kTIFFType) && 
						(ancillaryInfoFormat == kArcViewDefaultSupportType || 
															ancillaryInfoFormat == kErdas74Type)))
			{                                                                                 
					// Determine if .trl file is present.										
					
			if (displaySpecsPtr->filePaletteFlag) 
				{                    
				if (ancillaryInfoFormat == kArcViewDefaultSupportType)
					comboBoxPtr->InsertString (
											nextStringIndex, (LPCTSTR)_T("ArcView .clr file"));
				
				else	// ancillaryInfoFormat != kArcViewDefaultSupportType
					comboBoxPtr->InsertString (
											nextStringIndex, (LPCTSTR)_T("ERDAS .trl file")); 
					
				comboBoxPtr->SetItemData (nextStringIndex, kImageDefaultColorTable);
				nextStringIndex++;
				
				}	// end "if (displaySpecsPtr->filePaletteFlag)"
	 		
	 		}	// end "if (fileFormat == kErdas73Type || ...)" 
	 		
		else if (fileFormat == kSunScreenDumpType)
			{                                  
	 		comboBoxPtr->InsertString (
										nextStringIndex, (LPCTSTR)_T("SUN screendump file")); 
			comboBoxPtr->SetItemData (nextStringIndex, kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kSunScreenDumpType)"  
	 		
		else if (fileFormat == kTIFFType && colorTableOffset > 0)
			{                                
	 		comboBoxPtr->InsertString (nextStringIndex, (LPCTSTR)_T("TIFF file")); 
			comboBoxPtr->SetItemData (nextStringIndex, kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kTIFFType && ..."  
	 		
		else if (fileFormat == kGeoTIFFType && colorTableOffset > 0)
			{                                  
	 		comboBoxPtr->InsertString (nextStringIndex, (LPCTSTR)_T("GeoTIFF file")); 
			comboBoxPtr->SetItemData (nextStringIndex, kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kGeoTIFFType && ..."  
	 		
		else if (fileFormat == kImagineType)
			{                                  
	 		comboBoxPtr->InsertString (nextStringIndex, (LPCTSTR)_T("Imagine file")); 
			comboBoxPtr->SetItemData (nextStringIndex, kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kImagineType)"  
	 		
		else	// fileFormat not one of above but has a color table
			{                                  
	 		comboBoxPtr->InsertString (
										nextStringIndex, (LPCTSTR)_T("Image file default")); 
			comboBoxPtr->SetItemData (nextStringIndex, kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kImagineType)" 
		 	
		if (nextStringIndex == 6)    
			comboBoxPtr->SetItemData (6, kUserDefinedColors);   
	 		
	 			// Determine if user defined colors is to be included in the list.               
		
		if ((classGroupCode == kClassDisplay &&
					displaySpecsPtr->thematicClassPaletteType == kUserDefinedColors) ||
			  (classGroupCode != kClassDisplay &&
					displaySpecsPtr->thematicGroupPaletteType == kUserDefinedColors)) 
			{
		 	comboBoxPtr->InsertString (nextStringIndex, (LPCTSTR)_T("User Defined Colors"));  
			comboBoxPtr->SetItemData (nextStringIndex, kUserDefinedColors);
			
			}	// end "if ((classGroupCode == kClassDisplay && ..."
	
		return paletteCode;
	#endif	// defined multispec_win 

   #if defined multispec_lin	
		wxComboBox*							comboBoxPtr;
	
		int									comboItemCount,
												j;
	
		SInt16								nextStringIndex,
												paletteSelection;
	
	
				// Set user defined values for palette combo box.
				
		comboBoxPtr = (wxComboBox*)(dialogPtr->FindWindow (IDC_PaletteCombo));
	
				// Make sure that the 6th, 7th & 8th strings do not exist.
				 
		comboItemCount = comboBoxPtr->GetCount ();
		if (comboItemCount > 5)
			{
         for (j=comboItemCount; j>=6; j--)
            comboBoxPtr->Delete (j-1);
		
			}	// end "if (comboItemCount > 5)"
			
		nextStringIndex = 5;    
		
				// Determine if the FALSE color item is to be included. 
				                                   
		if (DetermineIfFalseColorAvailable (
													fileFormat, numberClasses, classDescriptionH))
			{                                                     
		 	comboBoxPtr->Append (wxT("False Color..."));
			comboBoxPtr->SetClientData (nextStringIndex, (void*)kFalseColors);
			nextStringIndex++;
			
			}	// end "if (DetermineIfFalseColorAvailable (fileFormat))" 
			
				// Determine if image default color is to be include in the list.
	
		if ((fileFormat == 0 && displaySpecsPtr->filePaletteFlag) || 
					fileFormat == kErdas73Type || 
					fileFormat == kErdas74Type ||
					fileFormat == kMultiSpecClassificationType ||
					fileFormat == kArcViewType ||
					((fileFormat == kGeoTIFFType || fileFormat == kTIFFType) && 
						(ancillaryInfoFormat == kArcViewDefaultSupportType || 
															ancillaryInfoFormat == kErdas74Type)))
			{                                                                                 
					// Determine if .trl file is present.										
					
			if (displaySpecsPtr->filePaletteFlag) 
				{                    
				if (ancillaryInfoFormat == kArcViewDefaultSupportType)
					comboBoxPtr->Append (wxT("ArcView .clr file"));
				
				else	// ancillaryInfoFormat != kArcViewDefaultSupportType
					comboBoxPtr->Append (wxT("ERDAS .trl file"));
					
				comboBoxPtr->SetClientData (
												nextStringIndex, (void*)kImageDefaultColorTable);
				nextStringIndex++;
				
				}	// end "if (displaySpecsPtr->filePaletteFlag)"
	 		
	 		}	// end "if (fileFormat == kErdas73Type || ...)" 
	 		
		else if (fileFormat == kSunScreenDumpType)
			{                                  
	 		comboBoxPtr->Append (wxT("SUN screendump file"));
			comboBoxPtr->SetClientData (nextStringIndex, (void*)kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kSunScreenDumpType)"  
	 		
		else if (fileFormat == kTIFFType && colorTableOffset > 0)
			{                                
	 		comboBoxPtr->Append (wxT("TIFF file"));
			comboBoxPtr->SetClientData (nextStringIndex, (void*)kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kTIFFType && ..."  
	 		
		else if (fileFormat == kGeoTIFFType && colorTableOffset > 0)
			{                                  
	 		comboBoxPtr->Append (wxT("GeoTIFF file"));
			comboBoxPtr->SetClientData (nextStringIndex, (void*)kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kGeoTIFFType && ..."  
	 		
		else if (fileFormat == kImagineType)
			{                                  
	 		comboBoxPtr->Append (wxT("Imagine file"));
			comboBoxPtr->SetClientData (nextStringIndex, (void*)kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kImagineType)"  
	 		
		else	// fileFormat not one of above but has a color table
			{                                  
	 		comboBoxPtr->Append (wxT("Image file default"));
			comboBoxPtr->SetClientData (nextStringIndex, (void*)kImageDefaultColorTable);
			nextStringIndex++;
				
			}	// end "else if (fileFormat == kImagineType)" 
		 	
		//if (nextStringIndex == 6)    
		//	comboBoxPtr->SetClientData (nextStringIndex, (void*)gUserDefinedColors);   
	 		
	 			// Determine if user defined colors is to be included in the list.               
		
		if ((classGroupCode == kClassDisplay &&
					displaySpecsPtr->thematicClassPaletteType == kUserDefinedColors) ||
			  (classGroupCode != kClassDisplay &&
					displaySpecsPtr->thematicGroupPaletteType == kUserDefinedColors)) 
			{
		 	comboBoxPtr->Append (wxT("User Defined Colors"));
			comboBoxPtr->SetClientData (nextStringIndex, (void*)kUserDefinedColors);
			
			}	// end "if ((classGroupCode == kClassDisplay && ..."	

				// Now try to reset the selection back to that which represents the
				// original selection code. If not possible set to the last
				// selection in the list.
	
		paletteSelection = ::GetComboListSelection (dialogPtr,
																	IDC_PaletteCombo,
																	paletteCode);
	
		if (paletteSelection < 0)
			{
					// This implies that the False Color option is not available now.
					// Set paletteSelection to the last option available which is the
					// the original
			paletteSelection = comboBoxPtr->GetCount () - 1;
			
			SInt64 palette64 =
					(SInt64)((int*)comboBoxPtr->GetClientData (paletteSelection));
			paletteCode = (SInt16)palette64;
			
			}	// end "if (paletteSelection < 0)"

		comboBoxPtr->SetSelection (paletteSelection);
	
		return paletteCode;
	#endif	// defined multispec_lin 
			
}	// end Routine "SetUpPalettePopUpMenu"          



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateActiveImageLegend
//
//	Software purpose:	The purpose of this routine is to update the palette for
//							the active image window..
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ImageWControlEvent in controls.c
//							Menus in menus.c
//							LoadThematicClasses in SSaveWrt.cpp
//							LoadThematicGroups in SSaveWrt.cpp
//							EditGroupClassDialogOK in SThemWin.cpp
//							EditGroups in SThemWin.cpp
//							LegendPopUpMenu in thematicWindow.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/04/1996
//	Revised By:			Larry L. Biehl			Date: 06/08/2011	

void UpdateActiveImageLegend (
				SInt16								classGroupCode,
				Boolean								callCreatePaletteFlag)
 
{
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsH,
											windowInfoHandle;
	
	
	if (classGroupCode == kClassDisplay || 
				classGroupCode == kGroupDisplay ||
						classGroupCode == kGroupClassDisplay)
		{
		windowInfoHandle = GetActiveImageWindowInfoHandle ();
		
		displaySpecsPtr = GetActiveDisplaySpecsPtr (&displaySpecsH);
		
		if (displaySpecsPtr != NULL)
			{                                    
			if (displaySpecsPtr->classGroupCode != classGroupCode)
				{ 
				displaySpecsPtr->classGroupCode = classGroupCode;
				displaySpecsPtr->paletteUpToDateFlag = FALSE;
				
				}	// end "if (...->classGroupCode != classGroupCode)"
				
			LoadThematicLegendList (GetActiveLegendListHandle (),
											FALSE);
		
			if (callCreatePaletteFlag)
				{
				if (CreatePalette (windowInfoHandle, 
											displaySpecsPtr, 
											GetWindowType (windowInfoHandle)))
					{
					ActivateImageWindowPalette (displaySpecsPtr->paletteObject);
								
					}	// end "if (CreatePalette (..."
					
				}	// end "if (callCreatePaletteFlag)"
				
			}	// end "if (displaySpecsPtr != NULL)" 
							
				// Unlock the display specification handle if needed.				

		CheckAndUnlockHandle (displaySpecsH);
		
		}	// end "if (classGroupCode == kClassDisplay || ..."
		
}	// end "UpdateActiveImageLegend"
