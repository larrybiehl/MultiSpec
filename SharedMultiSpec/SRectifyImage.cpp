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
//	File:						SRectifyImage.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/16/2020
//
//	Include files:			"MultiSpecHeaders"
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routine to handle the processor Reformat->
//								rectify image processor.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx
	 #include "xImageView.h"
	 #include "xMultiSpec.h"
	 #include "xReformatRectifyDialog.h"
#endif	// defined multispec_wx 
	
#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_LineInterval						11
	#define	IDC_ColumnInterval					14
	#define	IDC_NonSelectedPixels				18
	#define	IDC_TranslateScaleRotateRadio		21
	#define	IDC_TranslateScaleTitle				22
	#define	IDC_LineTranslateScalePrompt		23
	#define	IDC_ColumnTranslateScalePrompt	24
	#define	IDC_LineOffset							25
	#define	IDC_ColumnOffset						26
	#define	IDC_LineScale							27
	#define	IDC_ColumnScale						28
	#define	IDC_RotationTitle						29
	#define	IDC_RotationClockwisePrompt		30
	#define	IDC_RotationClockwise				31
	#define	IDC_ChannelsPrompt					32
	#define	IDC_Channels							33
	#define	IDC_UseMapOrientationAngle			34
	#define	IDC_ReprojectToRadio					35
	#define	IDC_ReferenceFileListPrompt		36
	#define	IDC_ReferenceFileList				37
	#define	IDC_ResampleMethodPrompt			40
	#define	IDC_ResampleMethod					41
#endif	// defined multispec_mac || defined multispec_mac_swift   
                             
#if defined multispec_win
	 #include "WReformatRectifyDialog.h"
#endif	// defined multispec_win 	

#define	kNearestNeighbor				1
#define	kMajorityRule					2
//#define	kMean							3

SInt16							gResampleSelection;

								

			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
			
void ConcatenateMapping (
				TransMapMatrix* 					dstMapMatrixPtr,
				TransMapMatrix* 					mapMatrixPtr);

Boolean DetermineIfIdentityMatrix (
				TransMapMatrix* 					mapMatrixPtr);

#if defined multispec_mac
	pascal void DrawResamplePopUp (
					DialogPtr							dialogPtr,
					SInt16								itemNumber);
#endif

void ExtendRealRect (
				DoubleRect* 						realRectPtr,
				double 								line, 
				double 								column);

void GetMappingMatrix (
				RectifyImageOptionsPtr			rectifyImageOptionsPtr);

SInt16 GetReprojectToImageList (
				DialogPtr							dialogPtr,
				Handle								windowInfoHandle, 
				Boolean								loadListFlag,
				Handle*								referenceWindowInfoHandlePtr, 
				SInt16*								listCountPtr);

void InitializeLineToBackgroundValue (
				FileInfoPtr							outFileInfoPtr,
				HUInt8Ptr							outputBufferPtr,
				UInt32								countOutBytes,
				double								backgroundValue);

Boolean InvertMappingMatrix (
				TransMapMatrix* 					mapMatrixPtr,
				TransMapMatrix* 					inverseMapMatrixPtr);

Boolean IsPointInPolygon (
				UInt32								nvert,
				double*								vertx, 
				double*								verty, 
				double								testx, 
				double								testy);

Boolean ListRectifyResultsInformation (
				ReformatOptionsPtr				reformatOptionsPtr,
				FileInfoPtr							outFileInfoPtr);

void MapControlPoints (
				FileInfoPtr 						fileInfoPtr,
				TransMapMatrix*					mapMatrixPtr);

void MapLineColumn (
				TransMapMatrix* 					mapMatrixPtr,
				double* 								linePtr, 
				double* 								columnPtr);

void MapNearestNeighborLineColumn (
				TransMapMatrix* 					mapMatrixPtr,
				SInt32	 							line, 
				SInt32	 							column, 
				SInt32* 								inputLinePtr, 
				SInt32* 								inputColumnPtr);

void MapOutputImageRectangle (
				TransMapMatrix* 					mapMatrixPtr,
				LongRect* 							inputRectanglePtr, 
				LongRect* 							outputRectanglePtr);

void OffsetMappingMatrix (
				TransMapMatrix* 					mapMatrixPtr,
				SInt32	 							columnOffset, 
				SInt32	 							lineOffset);

Boolean RectifyImage (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr 						outFileInfoPtr, 
				ReformatOptionsPtr 				reformatOptionsPtr);

Boolean RectifyImageDialog (
				FileInfoPtr 						fileInfoPtr,
				FileInfoPtr 						outFileInfoPtr, 
				ReformatOptionsPtr 				reformatOptionsPtr,
				double								minBackgroundValue,
				double								maxBackgroundValue);

void RectifyUpdateMapProjectionStructure (
				Handle								inputWindowInfoHandle,
				FileInfoPtr							outFileInfoPtr,
				SInt32								columnStart, 
				SInt32								lineStart,
				SInt16								procedureCode,
				Handle								referenceWindowInfoHandle,
				SInt32								columnShift, 
				SInt32								lineShift,
				double								columnScaleFactor,
				double								lineScaleFactor,
				TransMapMatrix*					mapMatrixPtr,
				TransMapMatrix*					inverseMapMatrixPtr,
				double								rotationAngle);

Boolean ReprojectImage (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr);

void ReprojectNearestNeighborLineColumn (
				MapProjectionInfoPtr				referenceMapProjectionInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				DoubleRect*							boundingRectPtr,
				SInt32								line, 
				SInt32								column, 
				SInt32*								inputLinePtr, 
				SInt32*								inputColumnPtr);

void ReprojectWithMajorityRule (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				referenceMapProjectionInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				UInt32*								histogramVector,
				SInt32								outputLine, 
				SInt32								outputColumn,
				SInt32								maxNumberInputColumns,
				SInt32								maxNumberInputLines,
				SInt16								backgroundValue, 
				SInt32*								outputPixelValuePtr);

void ScaleMappingMatrix (
				TransMapMatrix* 					mapMatrixPtr,
				TransMapMatrix* 					scaleMapMatrixPtr,
				double 								columnScale, 
				double 								lineScale, 
				double 								aboutColumn, 
				double 								aboutLine);

void SetIdentityMappingMatrix (
				TransMapMatrix* 					mapMatrixPtr);

void SetUpResampleMethodPopupMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpResampleSelectionMenu,
				Boolean								thematicTypeFlag);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ConcatenateMapping
//
//	Software purpose:	The purpose of this routine is to concatenate two input matrices.
//							The new matrix is returned in the dstMapMatrix location.
//
//	Parameters in:		two input matrices
//
//	Parameters out:	the output of the concatenate process.
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/12/1992
//	Revised By:			Larry L. Biehl			Date: 08/12/1992	

void ConcatenateMapping (
				TransMapMatrix*					dstMapMatrixPtr,
				TransMapMatrix*					mapMatrixPtr)				

{
	TransMapMatrix						concat;
	

	concat.map[0][0] = dstMapMatrixPtr->map[0][0] * mapMatrixPtr->map[0][0] + 
										dstMapMatrixPtr->map[0][1] * mapMatrixPtr->map[1][0];
	concat.map[0][1] = dstMapMatrixPtr->map[0][0] * mapMatrixPtr->map[0][1] + 
										dstMapMatrixPtr->map[0][1] * mapMatrixPtr->map[1][1];
	concat.map[1][0] = dstMapMatrixPtr->map[1][0] * mapMatrixPtr->map[0][0] + 
										dstMapMatrixPtr->map[1][1] * mapMatrixPtr->map[1][0];
	concat.map[1][1] = dstMapMatrixPtr->map[1][0] * mapMatrixPtr->map[0][1] + 
										dstMapMatrixPtr->map[1][1] * mapMatrixPtr->map[1][1];
	concat.map[2][0] = dstMapMatrixPtr->map[2][0] * mapMatrixPtr->map[0][0] + 
									dstMapMatrixPtr->map[2][1] * mapMatrixPtr->map[1][0] + 
																				mapMatrixPtr->map[2][0];
	concat.map[2][1] = dstMapMatrixPtr->map[2][0] * mapMatrixPtr->map[0][1] + 
									dstMapMatrixPtr->map[2][1] * mapMatrixPtr->map[1][1] + 
																				mapMatrixPtr->map[2][1];

	*dstMapMatrixPtr = concat;
	
}	// end "ConcatenateMapping"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfIdentityMatrix
//
//	Software purpose:	The purpose of this routine is to determine if the
//							input mapping matrix is the identity matrix.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/12/1992
//	Revised By:			Larry L. Biehl			Date: 08/12/1992	

Boolean DetermineIfIdentityMatrix (
				TransMapMatrix*					mapMatrixPtr)

{
	Boolean								identityFlag;
	
												
					// Determine if this mapping is just a shift in the lines		
					// and/or columns.															
			
	identityFlag = FALSE;
	if (mapMatrixPtr->map[0][0] == 1 && 
			mapMatrixPtr->map[1][1] == 1 &&
				mapMatrixPtr->map[0][1] == 0 && 
					mapMatrixPtr->map[1][0] == 0)
		identityFlag = TRUE;
		
	return (identityFlag);
	
}	// end "DetermineIfIdentityMatrix" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawResamplePopUp
//
//	Software purpose:	The purpose of this routine is to draw the resample
//							prompt and selection for the resample options 
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/25/2012
//	Revised By:			Larry L. Biehl			Date: 05/25/2012	

pascal void DrawResamplePopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpResampleMenu, 
								gResampleSelection, 
								TRUE);
	
}	// end "DrawResamplePopUp" 
#endif	// defined multispec_mac



void ExtendRealRect (
				DoubleRect*							realRectPtr, 
				double 								line, 
				double 								column)
				
{
	if (column < realRectPtr->left)
		realRectPtr->left = column;
		
	else if (column > realRectPtr->right)
		realRectPtr->right = column;

	if (line < realRectPtr->top)
		realRectPtr->top = line;
	else if (line > realRectPtr->bottom)
		realRectPtr->bottom = line;
		
}	// end "ExtendRealRect" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetMappingMatrix
//
//	Software purpose:	This routine determines the mapping matrix based on the input 
//							user parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:		
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/07/1992
//	Revised By:			Larry L. Biehl			Date: 02/14/2012

void GetMappingMatrix (
				RectifyImageOptionsPtr			rectifyImageOptionsPtr)

{
	double								rotationAngle;
	
	TransMapMatrix						*mapMatrixPtr,
											*rotateMapMatrixPtr;
	
	SInt32								aboutColumn,
											aboutLine;
	
	
	mapMatrixPtr			= &rectifyImageOptionsPtr->mapMatrix;
	rotateMapMatrixPtr	= &rectifyImageOptionsPtr->tempMapMatrix;
	aboutColumn 			= (SInt32)rectifyImageOptionsPtr->rotationColumnCenter;
	aboutLine 				= (SInt32)rectifyImageOptionsPtr->rotationLineCenter;
	rotationAngle			= -rectifyImageOptionsPtr->rotationAngle;
	
	SetIdentityMappingMatrix (mapMatrixPtr);

	ScaleMappingMatrix (mapMatrixPtr,
								&rectifyImageOptionsPtr->tempMapMatrix,
								rectifyImageOptionsPtr->columnScaleFactor, 
								rectifyImageOptionsPtr->lineScaleFactor, 
								aboutColumn, 
								aboutLine);
	
			// Get rotation matrix.															
			
	rotateMapMatrixPtr->map[0][0] = cos (kDegreesToRadians*rotationAngle);
	if (rotationAngle == 90 || rotationAngle == -90)
		rotateMapMatrixPtr->map[0][0] = 0;
	
	rotateMapMatrixPtr->map[0][1] = -sin (kDegreesToRadians*rotationAngle);
	
	rotateMapMatrixPtr->map[1][0] = -rotateMapMatrixPtr->map[0][1];
	rotateMapMatrixPtr->map[1][1] =  rotateMapMatrixPtr->map[0][0];
	
			// Get the offsets.																
			
	rotateMapMatrixPtr->map[2][0] = aboutColumn - 
								rotateMapMatrixPtr->map[0][0] * aboutColumn - 
													rotateMapMatrixPtr->map[1][0] * aboutLine;
	rotateMapMatrixPtr->map[2][1] = aboutLine - 
								rotateMapMatrixPtr->map[0][1] * aboutColumn - 
													rotateMapMatrixPtr->map[1][1] * aboutLine;
												
	if (DetermineIfIdentityMatrix (mapMatrixPtr) && 
			mapMatrixPtr->map[2][0] == 0 && 
				mapMatrixPtr->map[2][1] == 0)
		*mapMatrixPtr = *rotateMapMatrixPtr;
		
	else	// *mapMatrixPtr is not an identity matrix
		ConcatenateMapping (mapMatrixPtr, rotateMapMatrixPtr);

}	// end "GetMappingMatrix"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetReprojectToImageList
//
//	Software purpose:	The purpose of this routine is to determine the number of image 
//							windows that the file represented by the active image window can
//							be reprojected to. If the input flag is set, a list of the
//							possible images will be made.
//
//							The active image window needs to represent a file that contains
//							map description information such that the latitude-longitude of 
//							each pixel can be calculated and the list of re-project to images
//							must contain map information such that the latitude-longitude
//							can be calculated.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/15/2007
//	Revised By:			Larry L. Biehl			Date: 04/16/2020

SInt16 GetReprojectToImageList (
				DialogPtr							dialogPtr, 
				Handle								windowInfoHandle, 
				Boolean								loadListFlag,
				Handle*								referenceWindowInfoHandlePtr, 
				SInt16*								listCountPtr)

{
	#if defined multispec_win                                
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win

   #if defined multispec_wx
		wxChoice*							referenceListCtrl;
   #endif	// defined multispec_wx
	
	FileInfoPtr							fileInfoPtr;
	
	WindowInfoPtr						compareWindowInfoPtr;
											
	WindowPtr							windowPtr;
	
	DoubleRect		 					boundingInputRect;
	
	Handle								compareWindowInfoHandle;
	
	SInt16								firstEnabledMenuItem,
											imageListLength,
								 			window,
											windowIndex;
	
	Boolean								includeFlag;
	
	SignedByte							handleStatus2,
											handleStatus3;
	
	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

			// Initialize local variables.													
	
	imageListLength = 0;
	*listCountPtr = 0;
	firstEnabledMenuItem = SHRT_MAX;
	
	if (gNumberOfIWindows >= 2 && DetermineIfLatLongPossible (windowInfoHandle))
		{
		boundingInputRect.top = gImageWindowInfoPtr->boundingLatLongRectangle.top;
		boundingInputRect.left = gImageWindowInfoPtr->boundingLatLongRectangle.left;
		boundingInputRect.bottom = gImageWindowInfoPtr->boundingLatLongRectangle.bottom;
		boundingInputRect.right = gImageWindowInfoPtr->boundingLatLongRectangle.right;
		
		#if defined multispec_wx
			referenceListCtrl =
								(wxChoice*)wxWindow::FindWindowById (IDC_ReferenceFileList);
		#endif

		window = 0;
		windowIndex = kImageWindowStart;

		do
			{
			includeFlag = FALSE;
			
			windowPtr = gWindowList[windowIndex];
			compareWindowInfoHandle = GetWindowInfoHandle (windowPtr);
			
			compareWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																compareWindowInfoHandle,
																&handleStatus2);
			
			if (compareWindowInfoPtr != NULL &&
						windowInfoHandle != compareWindowInfoHandle && 
									DetermineIfLatLongPossible (compareWindowInfoHandle) &&
											WindowBoundingAreaAndRectIntersect (
																				compareWindowInfoPtr,
																				&boundingInputRect,
																				FALSE))
				includeFlag = TRUE;
				
		
			if (includeFlag)
				(*listCountPtr)++;
			
			if (loadListFlag)
				{													
				fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																compareWindowInfoPtr->fileInfoHandle,
																&handleStatus3);
					
				#if defined multispec_mac  
					imageListLength++;
					AppendMenu (gPopUpTemporaryMenu, "\pNewFile");
					UCharPtr fileNamePtr =
										(UCharPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
					MSetMenuItemText (gPopUpTemporaryMenu,
											imageListLength, 
											fileNamePtr,
											kASCIICharString);
				#endif	// defined multispec_mac   
		
				#if defined multispec_win
					if (includeFlag) 
						{
						imageListLength++;
						comboBoxPtr =
									(CComboBox*)dialogPtr->GetDlgItem (IDC_ReferenceFileList);
						comboBoxPtr->AddString ((LPCTSTR)_T("\0NewFile\0"));
						FileStringPtr fileNamePtr =
									(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
						dialogPtr->SetComboItemText (IDC_ReferenceFileList, 
																imageListLength-1, 
																&fileNamePtr[2],
																kUTF8CharString);
						comboBoxPtr->SetItemData (imageListLength-1, windowIndex);
																
						}	// end "if (includeFlag)"
				#endif	// defined multispec_win   
				
				#if defined multispec_wx
					if (includeFlag) 
						{
						imageListLength++;
						FileStringPtr fileNamePtr =
								(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
						referenceListCtrl->Append (&fileNamePtr[2]);
						SInt64 windowIndex64 = windowIndex;
						referenceListCtrl->SetClientData (
														imageListLength-1, (void*)windowIndex64);

						}
				#endif
		
				if (includeFlag)
					{
					if (firstEnabledMenuItem == SHRT_MAX)
						{
						firstEnabledMenuItem = imageListLength;
						*referenceWindowInfoHandlePtr = compareWindowInfoHandle;
						
						}	// end "if (firstEnabledMenuItem == SHRT_MAX)"
						
					}	// end "if (includeFlag)" 
				
				#if defined multispec_mac  
					else	// !includeFlag 
						DisableMenuItem (gPopUpTemporaryMenu, imageListLength);
				#endif	// defined multispec_mac   
			
				MHSetState (compareWindowInfoPtr->fileInfoHandle, handleStatus3);
					
				}	// end "if (loadListFlag)" 
			
			MHSetState (compareWindowInfoHandle, handleStatus2);
			
			window++;
			windowIndex++;
														
			}	while (window<gNumberOfIWindows);
			
		}	// end "if (gNumberOfIWindows >= 2 && ..."
		
	if (loadListFlag && firstEnabledMenuItem != SHRT_MAX)
		imageListLength = firstEnabledMenuItem;
		
	return (imageListLength);
			
}	// end "GetReprojectToImageList" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeLineToBackgroundValue
//
//	Software purpose:	This routine initializes the input line to the requested
//							background value.  It takes into account the type of data value.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/20/2006
//	Revised By:			Larry L. Biehl			Date: 01/23/2006

void InitializeLineToBackgroundValue (
				FileInfoPtr							outFileInfoPtr, 
				HUInt8Ptr							outputBufferPtr,
				UInt32								countOutBytes,
				double								backgroundValue)

{
	HDoublePtr							outputBufferDoublePtr;
	HUInt16Ptr							outputBufferUInt16Ptr;
	HUInt32Ptr							outputBufferUInt32Ptr;
	
	float									backgroundFloatValue;
	
	UInt32								count,
											numberSamples;
	
	SInt32								backgroundSInt32Value;
	UInt32								backgroundUInt32Value;
	SInt16								backgroundSInt16Value;
	UInt16								backgroundUInt16Value;
	SInt8									backgroundSInt8Value;
	UInt8									backgroundUInt8Value;
	
	
		
			// Initialize a background line that will be copied from as			
			// needed.																			
	
	if (outFileInfoPtr->numberBytes == 1)
		{
		if (outFileInfoPtr->signedDataFlag)
			{
			backgroundSInt8Value = (SInt8)backgroundValue;
			memcpy (&backgroundUInt8Value, &backgroundSInt8Value, 1);
			
			}	// end "if (outFileInfoPtr->signedDataFlag)"
			
		else	// !outFileInfoPtr->signedDataFlag
			backgroundUInt8Value = (UInt8)backgroundValue;
			
		for (count=0; count<countOutBytes; count++)
			{
			*outputBufferPtr = backgroundUInt8Value;
			outputBufferPtr++;
			
			}	// end "for (count=0; count<countOutBytes; count++)" 
			
		}	// end "if (outFileInfoPtr->numberBytes == 1)"
		
	else if (outFileInfoPtr->numberBytes == 2)
		{
		if (outFileInfoPtr->signedDataFlag)
			{
			backgroundSInt16Value = (SInt16)backgroundValue;
			memcpy (&backgroundUInt16Value, &backgroundSInt16Value, 2);
			
			}	// end "if (outFileInfoPtr->signedDataFlag)"
			
		else	// !outFileInfoPtr->signedDataFlag
			backgroundUInt16Value = (UInt16)backgroundValue;
			
		numberSamples = countOutBytes/2;
		outputBufferUInt16Ptr = (HUInt16Ptr)outputBufferPtr;
		for (count=0; count<numberSamples; count++)
			{
			*outputBufferUInt16Ptr = backgroundUInt16Value;
			outputBufferUInt16Ptr++;
			
			}	// end "for (count=0; count<countOutBytes; count++)" 
			
		}	// end "else outFileInfoPtr->numberSamples == 2"
		
	else if (outFileInfoPtr->numberBytes == 4)
		{
		if (outFileInfoPtr->dataTypeCode == kIntegerType)
			{
			if (outFileInfoPtr->signedDataFlag)
				{
				backgroundSInt32Value = (SInt32)backgroundValue;
				memcpy (&backgroundUInt32Value, &backgroundSInt32Value, 4);
				
				}	// end "if (outFileInfoPtr->signedDataFlag)"
				
			else	// !outFileInfoPtr->signedDataFlag
				backgroundUInt32Value = (UInt32)backgroundValue;
				
			}	// end "if (outFileInfoPtr->dataTypeCode == kIntegerType)"
			
		else	// outFileInfoPtr->dataTypeCode == kRealType
			{
			backgroundFloatValue = (float)backgroundValue;
			memcpy (&backgroundUInt32Value, &backgroundFloatValue, 4);
			
			}	// end "else outFileInfoPtr->dataTypeCode == kRealType"
			
		numberSamples = countOutBytes/4;
		outputBufferUInt32Ptr = (HUInt32Ptr)outputBufferPtr;
		for (count=0; count<numberSamples; count++)
			{
			*outputBufferUInt32Ptr = backgroundUInt32Value;
			outputBufferUInt32Ptr++;
			
			}	// end "for (count=0; count<numberSamples; count++)" 
			
		}	// end "else outFileInfoPtr->numberBytes == 4"
		
	else if (outFileInfoPtr->numberBytes == 8)
		{
		numberSamples = countOutBytes/8;
		outputBufferDoublePtr = (HDoublePtr)outputBufferPtr;
		for (count=0; count<numberSamples; count++)
			{
			*outputBufferDoublePtr = backgroundValue;
			outputBufferDoublePtr++;
			
			}	// end "for (count=0; count<numberSamples; count++)" 
			
		}	// end "else outFileInfoPtr->numberBytes == 8"

}	// end "InitializeLineToBackgroundValue"



Boolean InvertMappingMatrix (
				TransMapMatrix*					mapMatrixPtr, 
				TransMapMatrix*					inverseMapMatrixPtr)
		
{
	double								determinant;

	
			// Get the inverse mapping.													

	determinant = mapMatrixPtr->map[0][0] * mapMatrixPtr->map[1][1] - 
								mapMatrixPtr->map[0][1] * mapMatrixPtr->map[1][0];

	if (determinant != 0)
		{
		inverseMapMatrixPtr->map[0][0] = mapMatrixPtr->map[1][1]/determinant;
		inverseMapMatrixPtr->map[0][1] = - mapMatrixPtr->map[0][1]/determinant;
		inverseMapMatrixPtr->map[1][0] = - mapMatrixPtr->map[1][0]/determinant;
		inverseMapMatrixPtr->map[1][1] = mapMatrixPtr->map[0][0]/determinant;
		
		inverseMapMatrixPtr->map[2][0] = 
				- mapMatrixPtr->map[2][0] * inverseMapMatrixPtr->map[0][0]
						- mapMatrixPtr->map[2][1] * inverseMapMatrixPtr->map[1][0];
								
		inverseMapMatrixPtr->map[2][1] = 
				- mapMatrixPtr->map[2][0] * inverseMapMatrixPtr->map[0][1]
						- mapMatrixPtr->map[2][1] * inverseMapMatrixPtr->map[1][1];
						
		return (TRUE);
						
		}	// end "if (determinant != 0)" 
		
	else	// determinant == 0 
		return (FALSE);
		
}	// end "InvertMappingMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean IsPointInPolygon
//
//	Software purpose:	The purpose of this routine is determine if the input point
//							is within the polygon described by the input vertices.
//							The code comes from 
//							"http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html".
//							And is provided by W. Randolph Franklin
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	TRUE if point is within the polygon
//							FALSE if the point is not within the polygon
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/22/2012
//	Revised By:			Larry L. Biehl			Date: 05/22/2012

Boolean IsPointInPolygon (
				UInt32								nvert, 
				double*								vertx, 
				double*								verty, 
				double								testx, 
				double								testy)

	{
	UInt32								i, 
											j;
	
	Boolean								inPolygonFlag = FALSE;
	
	
	for (i = 0, j = nvert-1; i < nvert; j = i++) 
		{
		if (((verty[i]>testy) != (verty[j]>testy)) &&
				(testx < 
					(vertx[j]-vertx[i])*(testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]))
			inPolygonFlag = !inPolygonFlag;
			
		}	// end "for (i = 0, j = nvert-1; i < nvert; j = i++)"
		
  return (inPolygonFlag);
		
}	// end "IsPointInPolygon"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				Boolean ListRectifyResultsInformation
//
//	Software purpose:	The purpose of this routine is to list the results
//							of the reformat rectify operation.
//
//	Parameters in:		Pointer to reformat instruction structure.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2007
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean ListRectifyResultsInformation (
				ReformatOptionsPtr				reformatOptionsPtr, 
				FileInfoPtr							outFileInfoPtr)

{  
	char									string[16];
	CharPtr								fileNamePtr;
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	SInt16								*rectifyChannelPtr;
	
	UInt32								channelIndex,
											index;
											
	SInt16								stringLength;
											
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;

	continueFlag = ListLineColumnIntervalString (NULL, 
																gOutputForce1Code,
																reformatOptionsPtr->lineStart, 
																reformatOptionsPtr->lineEnd, 
																reformatOptionsPtr->lineInterval,
																reformatOptionsPtr->columnStart, 
																reformatOptionsPtr->columnEnd, 
																reformatOptionsPtr->columnInterval, 
								 								continueFlag);
	
	if (rectifyImageOptionsPtr->procedureCode == kTranslateScaleRotate)
		{				
		sprintf ((char*)gTextString,
					"%s"
					"    Rectify option: Translate, scale, and/or rotate%s",
					gEndOfLine,
					gEndOfLine);
		
		if (continueFlag)		
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString), 
												gOutputTextH);
					
		sprintf ((char*)gTextString,
					"%s"
					#ifndef multispec_wx
						"    Column shift:        %ld%s"
						"    Line shift:          %ld%s"
					#endif
					#if defined multispec_wx				
						"    Column shift:        %d%s"
						"    Line shift:          %d%s"
					#endif
					"    Column scale factor: %g%s"
					"    Line scale factor:   %g%s"
					"    Rotation angle:      %g%s",
					gEndOfLine,
					rectifyImageOptionsPtr->columnShift,
					gEndOfLine,
					rectifyImageOptionsPtr->lineShift,
					gEndOfLine,
					rectifyImageOptionsPtr->columnScaleFactor,
					gEndOfLine,
					rectifyImageOptionsPtr->lineScaleFactor,
					gEndOfLine,
					rectifyImageOptionsPtr->rotationAngle,
					gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
		}	// end "if (...->procedureCode == kTranslateScaleRotate)"
	
	else if (rectifyImageOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{		
		sprintf ((char*)gTextString,
					"%s"
					"    Rectify option: Re-project to reference image%s",
					gEndOfLine,
					gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString), 
												gOutputTextH);
		
		stringLength = sprintf ((char*)gTextString,
					"      Resample option: ");
					
		if (rectifyImageOptionsPtr->resampleCode == kNearestNeighbor)
			sprintf ((char*)&gTextString[stringLength],
									"Nearest Neighbor%s",
									gEndOfLine);
					
		else if (rectifyImageOptionsPtr->resampleCode == kMajorityRule)
			sprintf ((char*)&gTextString[stringLength],
									"Majority%s",
									gEndOfLine);
				
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,
												(UInt32)strlen ((char*)gTextString),
												gOutputTextH);
		
		}	// end "else if (...->procedureCode == kReprojectToReferenceImage)"
								 								
	sprintf (string, "No");
	if (rectifyImageOptionsPtr->blankOutsideSelectedAreaFlag)							
		sprintf (string, "Yes");						
		
	sprintf ((char*)gTextString,
				"    Background value:    %g%s"
				"    Set non-selected pixels to background: %s%s%s",
				rectifyImageOptionsPtr->backgroundValue,
				gEndOfLine,
				(char*)string,
				gEndOfLine,
				gEndOfLine);
			
	if (continueFlag)	
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString), 
											gOutputTextH);
				
				// List the channels to be rectified if the active image is
				// not a thematic image file
	
	if (continueFlag && !outFileInfoPtr->thematicType)
		{
				//	Need to change the channel list to represent list of channels
				// not 1 or 0 to represent whether the channel is to be used.
		
		rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;
		channelIndex = 0;		
		for (index=0; 
				index<reformatOptionsPtr->numberChannels; 
				index++)
			{
			if (rectifyChannelPtr[index] == 1)
				{
				rectifyChannelPtr[channelIndex] = (SInt16)index;
				channelIndex++;
				
				}	// end "if (rectifyChannelPtr[index] == 1)"
			
			}	// end "for (index=0; ..."									
			
		continueFlag = ListChannelsUsed (
				gImageFileInfoPtr,
				NULL,
				rectifyChannelPtr,
				rectifyImageOptionsPtr->numberChannelsToRectify, 
				NULL,
				&gOutputForce1Code,
				kNoTransformation);
				
		}	// end "if (continueFlag && !outFileInfoPtr->thematicType)"
		
	if (rectifyImageOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{
		continueFlag = ListMapProjectionString (gImageFileInfoPtr->mapProjectionHandle, 
																(char*)"    ",
																continueFlag);	
		
				// Insert a blank line
		
		if (continueFlag)	
			continueFlag = ListString ((char*)&gEndOfLine,
												(UInt32)strlen ((char*)&gEndOfLine),
												gOutputTextH);
										
		}	// end "if (...->procedureCode == kReprojectToReferenceImage)"
										
			// List "  Output Information:"
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared8, 
																(unsigned char*)gTextString, 
																NULL, 
																gOutputForce1Code, 
																continueFlag);
	
	fileNamePtr = (char*)GetFileNameCPointerFromFileInfo (outFileInfoPtr);
	sprintf ((char*)gTextString,
						"    New output image file name: '%s'%s",
						fileNamePtr,
						gEndOfLine);
				
	if (continueFlag)	
		continueFlag = ListString ((char*)gTextString,
											(UInt32)strlen ((char*)gTextString),
											gOutputTextH,
											kUTF8CharString);

	continueFlag = ListLineColumnIntervalString (NULL, 
																gOutputForce1Code,
																1, 
																outFileInfoPtr->numberLines, 
																1,
																1, 
																outFileInfoPtr->numberColumns, 
																1, 
								 								continueFlag);
		
			// Insert a blank line
	
	if (continueFlag)	
		continueFlag = ListString ((char*)&gEndOfLine,
											(UInt32)strlen ((char*)&gEndOfLine), 
											gOutputTextH);
	
	if (continueFlag && !outFileInfoPtr->thematicType)
		{
		continueFlag = ListChannelsUsed (outFileInfoPtr,
													NULL,
													NULL,
													outFileInfoPtr->numberChannels, 
													NULL,
													&gOutputForce1Code,
													kNoTransformation);
		
		}	// end "if (continueFlag && !outFileInfoPtr->thematicType)"
	
	if (rectifyImageOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{		
		continueFlag = ListMapProjectionString (outFileInfoPtr->mapProjectionHandle, 
																(char*)"    ",
																continueFlag);		
		
		}	// end "if (...->procedureCode == kReprojectToReferenceImage)"
		
	return (continueFlag);
	
}	// end "ListRectifyResultsInformation" 
							


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MapControlPoints
//
//	Software purpose:	This routine maps the control points for the files to reflect
//							the new line column values in the output image.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/06/2007
//	Revised By:			Larry L. Biehl			Date: 03/06/2007

void MapControlPoints (
				FileInfoPtr 						fileInfoPtr,
				TransMapMatrix*					mapMatrixPtr)

{
	double								columnValue,
											lineValue;
											
	ControlPointsPtr					controlPointsPtr;
	
	UInt32								count,
											numberControlPoints;
	
				
	if (fileInfoPtr != NULL)
		{
		controlPointsPtr = GetControlPointVectorPointers (
											fileInfoPtr->controlPointsHandle,
											kNoLock);
							
		numberControlPoints = 0;								
		if (controlPointsPtr != NULL)
			numberControlPoints = controlPointsPtr->count;
		
		for (count = 0; count<numberControlPoints; count++)
			{
			columnValue = controlPointsPtr->easting1Ptr[count];
			lineValue = controlPointsPtr->northing1Ptr[count];
			
			MapLineColumn (mapMatrixPtr, 
									&lineValue, 
									&columnValue);
									
			controlPointsPtr->easting1Ptr[count] = columnValue;
			controlPointsPtr->northing1Ptr[count] = lineValue;
			
			}	// end "for (count = 0; count<numberControlPoints; count++)"
			
		CloseControlPointVectorPointers (fileInfoPtr->controlPointsHandle);
		
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "MapControlPoints" 



//------------------------------------------------------------------------------------
//	[x  y] [a  b] = [newX  newY]
//		    [c  d]

void MapLineColumn (
				TransMapMatrix*					mapMatrixPtr, 
				double*								linePtr, 
				double*								columnPtr)
				
{
	double* 								mx = &mapMatrixPtr->map[0][0];
	double* 								my = &mapMatrixPtr->map[1][0];
	double* 								mt = &mapMatrixPtr->map[2][0];
	
	double 								tmpCol = *columnPtr;
	double 								tmpLine = *linePtr;

	*columnPtr = *mx++ * tmpCol + *my++ * tmpLine + *mt++;
	*linePtr = *mx * tmpCol + *my * tmpLine + *mt;
	
}	// end "MapLineColumn" 



//------------------------------------------------------------------------------------
//	[x  y] [a  b] = [newX  newY]
//		    [c  d]

void MapNearestNeighborLineColumn (
				TransMapMatrix*					mapMatrixPtr, 
				SInt32								line, 
				SInt32								column, 
				SInt32*								inputLinePtr, 
				SInt32*								inputColumnPtr)
				
{
	double* 								mx = &mapMatrixPtr->map[0][0];
	double* 								my = &mapMatrixPtr->map[1][0];
	double* 								mt = &mapMatrixPtr->map[2][0];
	

	*inputColumnPtr = (SInt32)(*mx++ * column + *my++ * line + *mt++ + .5);
	*inputLinePtr = (SInt32)(*mx * column + *my * line + *mt + .5);
	
}	// end "MapNearestNeighborLineColumn" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MapOutputImageRectangle
//
//	Software purpose:	The purpose of this routine is to determine the size
//							of the output image that contains the requested input
// 						image to be rectified.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1992
//	Revised By:			Larry L. Biehl			Date: 08/11/1992	

void MapOutputImageRectangle (
				TransMapMatrix*					mapMatrixPtr, 
				LongRect*							inputRectanglePtr, 
				LongRect*							outputRectanglePtr)

{
	DoubleRect 							bounds;
	
	double								column,
											line;
	
	
	line = inputRectanglePtr->top;
	column = inputRectanglePtr->left;
	MapLineColumn (mapMatrixPtr, &line, &column);
	bounds.top = bounds.bottom = line;
	bounds.left = bounds.right = column;
	
	line = inputRectanglePtr->top;
	column = inputRectanglePtr->right;
	MapLineColumn (mapMatrixPtr, &line, &column);
	ExtendRealRect (&bounds, line, column);
	
	line = inputRectanglePtr->bottom;
	column = inputRectanglePtr->right;
	MapLineColumn (mapMatrixPtr, &line, &column);
	ExtendRealRect (&bounds, line, column);
	
	line = inputRectanglePtr->bottom;
	column = inputRectanglePtr->left;
	MapLineColumn (mapMatrixPtr, &line, &column);
	ExtendRealRect (&bounds, line, column);

	outputRectanglePtr->left = (SInt32)floor (bounds.left);
	outputRectanglePtr->top = (SInt32)floor (bounds.top);
	outputRectanglePtr->right = (SInt32)ceil (bounds.right);
	outputRectanglePtr->bottom = (SInt32)ceil (bounds.bottom);

}	// end "MapOutputImageRectangle" 



//------------------------------------------------------------------------------------

void OffsetMappingMatrix (
				TransMapMatrix*					mapMatrixPtr, 
				SInt32								columnOffset, 
				SInt32								lineOffset)

{
	mapMatrixPtr->map[2][0] += columnOffset;
	mapMatrixPtr->map[2][1] += lineOffset;
	
}	// end "OffsetMappingMatrix" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean RectifyImage
//
//	Software purpose:	This routine does the rectification of the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1992
//	Revised By:			Larry L. Biehl			Date: 02/26/2013

Boolean RectifyImage (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
			// Declare local variables & structures
			
	double								backgroundValue;
												
	CMFileStream*						outFileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	TransMapMatrix*					inverseMapMatrixPtr;
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	
	unsigned char						*ioOutBufferPtr,
					 						*columnOutBufferPtr,
					 						*savedOutBufferPtr;
					 					
	SInt16								*rectifyChannelPtr;
	
	unsigned char 						*ioOut1ByteBufferPtr,
								 			*ioIn1ByteBufferPtr;
	
	SInt32								column,
											columnByteSkip,
											countInBytes,
											countOutBytes,
											inputColumn,
											inputMapLine,
											lastOutputWrittenLine,
											limitIoOutBytes,
											line,
											lineShift,
											mapColumnShift,
											mapLineShift,
											nonRectifiedColumnOffset,
											numberOutputColumns,
											percentComplete,
											preLineBytes,
											rectifiedInputOffset,
											rectifiedLine,
											rectifiedOutputOffset,
											totalIOOutBytes,
											writePosOff;
	
	UInt32								channelCount,
											columnEnd,
											columnStart,
											inOffsetBytes,
											inOffsetIncrement,
											inputLine,
											lastLineRead,
											numberBytes,
											numberColumnsChannels,
											numberOutputLines,
											outChannelByteIncrement,
											outNumberBytesPerLineAndChannel,
											outOffsetIncrement,
											rectifyBytesToMove,
											startColumn,
											startLine,
											stopColumn,
											stopLine,
											supportFileType;
	
	SInt16								errCode,
											GAIAPrelineString[7] = 
													{	0x0300, 0x0600, 0x0000, 0x0100, 
														0x0000, 0x0000, 0x0000},
											lastPercentComplete,
											numberReadChannels;
												
	UInt16								forceByteCode,
											numberOutChannels;										
	
	Boolean								continueFlag,
											forceBISFlag,
											loadAllColumnsAtOnceFlag,
											shiftOnlyFlag,
											someNoRectifyChannelsFlag;
	
		
			// Initialize local variables.													
			
	errCode = noErr;
	writePosOff = 0;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	forceByteCode = reformatOptionsPtr->forceByteCode;
	forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
	preLineBytes = outFileInfoPtr->numberPreLineBytes;
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	inverseMapMatrixPtr = &rectifyImageOptionsPtr->inverseMapMatrix;
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;
	backgroundValue = rectifyImageOptionsPtr->backgroundValue;
	
	someNoRectifyChannelsFlag = (reformatOptionsPtr->numberChannels !=
										rectifyImageOptionsPtr->numberChannelsToRectify);
	shiftOnlyFlag = rectifyImageOptionsPtr->shiftOnlyFlag;
	
	lineShift = MAX (0, rectifyImageOptionsPtr->lineShift);
	
	mapColumnShift = MIN (0, rectifyImageOptionsPtr->columnShift);
	mapLineShift = MIN (0, rectifyImageOptionsPtr->lineShift);
					
	ioOutBufferPtr = (unsigned char*)reformatOptionsPtr->ioOutBufferPtr;
				
	lastPercentComplete = -1;
	
	columnStart = reformatOptionsPtr->columnStart;
	columnEnd =  reformatOptionsPtr->columnEnd;
	
	if (rectifyImageOptionsPtr->blankOutsideSelectedAreaFlag)
		{
		startLine = reformatOptionsPtr->startLine;
		stopLine = reformatOptionsPtr->stopLine;
		startColumn = reformatOptionsPtr->startColumn;
		stopColumn = reformatOptionsPtr->stopColumn;
		
		rectifiedInputOffset = MIN (
						(SInt32)(startColumn-1), rectifyImageOptionsPtr->columnShift);
		rectifiedInputOffset = MAX (0, rectifiedInputOffset);
		rectifiedOutputOffset = MAX (0, rectifyImageOptionsPtr->columnShift);
		
		}	// end "if (...->blankOutsideSelectedAreaFlag)" 
		
	else	// !...->blankOutsideSelectedAreaFlag 
		{
		startLine = reformatOptionsPtr->lineStart;
		stopLine = reformatOptionsPtr->lineEnd;
		startColumn = columnStart;
		stopColumn = columnEnd;
		
		rectifiedInputOffset = 0;
		rectifiedOutputOffset = 0;
		
		}	// end "else !...->blankOutsideSelectedAreaFlag"
		
	numberBytes = outFileInfoPtr->numberBytes;
	
	nonRectifiedColumnOffset = rectifyImageOptionsPtr->nonRectifiedInputOffset;
	nonRectifiedColumnOffset *= numberBytes;
	
	rectifiedInputOffset *= numberBytes;
	rectifiedOutputOffset *= numberBytes;
															
	numberOutChannels = reformatOptionsPtr->numberChannels;
	numberOutputColumns = outFileInfoPtr->numberColumns;
	numberOutputLines = outFileInfoPtr->numberLines;
	
	numberReadChannels = reformatOptionsPtr->numberChannels;
		
			// Determine if the section of code which handles all columns			
			// for a given channel at once needs to be called.							
			
	loadAllColumnsAtOnceFlag = 
					!forceBISFlag && (someNoRectifyChannelsFlag || shiftOnlyFlag);
	
			// Write a header if requested.													
			
	continueFlag = WriteNewImageHeader (fileInfoPtr,
													outFileInfoPtr, 
													(char*)ioOutBufferPtr,
													NULL,
													kFromReformat,
													reformatOptionsPtr->outputFileCode, 
													reformatOptionsPtr->lineInterval,
													reformatOptionsPtr->columnInterval,
													kClassDisplay,
													kNoPaletteColorsDefined);
	
						// Return if there was an IO error									
			
	if (continueFlag)
		{
				// Set up limit of output buffer to know when to write buffer to	
				// the disk file. Make certain which buffer is the output buffer.	
				
		countOutBytes = reformatOptionsPtr->countOutBytes;
		limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
		totalIOOutBytes = 0;
		
				// Initialize a background line that will be copied from as			
				// needed.	
								
		InitializeLineToBackgroundValue (outFileInfoPtr, 
													(HUInt8Ptr)ioOutBufferPtr,
													countOutBytes,
													backgroundValue);	
		
				// Set up output working buffer pointer.  This pointer will			
				// change as the output buffer fills up									
		
		savedOutBufferPtr = &ioOutBufferPtr[countOutBytes];
		
				// Get bytes to skip each time through the column loop for the		
				// output buffer.																	
				
		columnByteSkip = 1;
		if (forceBISFlag)
			columnByteSkip = numberOutChannels;
		columnByteSkip *= numberBytes;
		
				// Get total number of input bytes for one channel.					
				// Make certain that this is not more than the number of bytes		
				// allowed for in the output file.											
				
		countInBytes = stopColumn - startColumn + 1;
		countInBytes *= numberBytes;
		
		countInBytes = MIN (countInBytes,
									(SInt32)(numberBytes * outFileInfoPtr->numberColumns));
						
				// Get increment to skip each time through the channel loop
				// for the input buffer.
				
		inOffsetIncrement = columnEnd - columnStart + 1;
		if (forceBISFlag || fileInfoPtr->bandInterleave == kBIS)
			inOffsetIncrement = 1;
		
				// Get count of bytes to skip each time through the channel			
				// loop for the input buffer.	
				
		inOffsetBytes = inOffsetIncrement * numberBytes;
		
				// Get count of bytes to skip each time through the channel			
				// loop for the output buffer.
										
		outChannelByteIncrement = (countOutBytes-preLineBytes)/numberOutChannels;
		
		if (outFileInfoPtr->bandInterleave == kBIS)
			outChannelByteIncrement /= numberOutputColumns;
		
		else if (outFileInfoPtr->bandInterleave == kBSQ)
			outChannelByteIncrement = 
										reformatOptionsPtr->ioOutBufferBytes/numberOutChannels;
						
				// Get increment to skip each time through the channel loop
				// for the output buffer. I.E. allow for the byte count of
				// the output data.
				// Note that this could  be cleaned up some.  outOffsetIncrement and
				// outChannelByteIncrement are really the same thing. One is in values 
				// and the other is in bytes.  They are used in two different places.
				
		outOffsetIncrement = outChannelByteIncrement/numberBytes;
		
				// Get the number of bytes per line (all columns) and one band 
				// of data.
		
		outNumberBytesPerLineAndChannel =
												(countOutBytes-preLineBytes)/numberOutChannels;
		if (outFileInfoPtr->bandInterleave == kBIS)
			outNumberBytesPerLineAndChannel /= numberOutputColumns;
	
		rectifyBytesToMove = (stopColumn - startColumn + 1) * numberBytes;
		rectifyBytesToMove = MIN (outNumberBytesPerLineAndChannel, rectifyBytesToMove);
					
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		
				// Get the total number of samples a line of data.						
				// It will be used when adjusting the data if request.				
				
		numberColumnsChannels = 
								((SInt32)columnEnd - columnStart + 1) * numberReadChannels;
			
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used.
				
		errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
														NULL,
														startLine,
														stopLine,
														1,
														columnStart,
														columnEnd,
														1,
														numberReadChannels,
														NULL,
														kDetermineSpecialBILFlag);		
												
		if (errCode != noErr)	
			continueFlag = FALSE;			
		
		line = 1;
		lastLineRead = -1;
		inputLine = reformatOptionsPtr->startLine;
		lastOutputWrittenLine = 0;

				// Turn spin cursor on

		gPresentCursor = kSpin;
		
		while (line <= (SInt32)numberOutputLines && continueFlag)
			{
			ioOut1ByteBufferPtr = savedOutBufferPtr;
			
					// Initialize output line to the background value.	
					
			if (outFileInfoPtr->bandInterleave == kBSQ)
				{
				channelCount = 0;
				while (channelCount<numberOutChannels)
					{	
					BlockMoveData (ioOutBufferPtr, 
										&ioOut1ByteBufferPtr[preLineBytes], 
										outNumberBytesPerLineAndChannel);
										
					ioOut1ByteBufferPtr += outChannelByteIncrement;
					channelCount++;
										
					}	// end "while (channelCount<numberOutChannels)"
					
				ioOut1ByteBufferPtr = savedOutBufferPtr;
					
				}	// end "if (outFileInfoPtr->bandInterleave == kBSQ)"
			
			else	// ...->bandInterleave != kBSQ	
				BlockMoveData (ioOutBufferPtr, 
									&ioOut1ByteBufferPtr[preLineBytes], 
									countOutBytes-preLineBytes);
		
					// Add the preline calibration bytes if any.  For now this is	
					// only handled for GAIA data.											
					
			if (preLineBytes > 0)
				{
				BlockMoveData (&GAIAPrelineString, ioOut1ByteBufferPtr, preLineBytes);
				ioOut1ByteBufferPtr += preLineBytes;
				
				}	// end "if (preLineBytes > 0)" 
				
			if (loadAllColumnsAtOnceFlag)
				{
						// For output files which are in BIL and BSQ formats,			
						// load all columns of data for those channels that do not	
						// need to be rectified.												
						
				ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
				ioOut1ByteBufferPtr = &savedOutBufferPtr[preLineBytes];
				channelCount = 0;
						
				if (inputLine >= startLine && inputLine <= stopLine)
					{ 
					if (inputLine != lastLineRead)
						{
								// Get all requested channels for line of image data. 
								// Return if there is a file IO error.						
					 
						errCode = GetLineOfData (fileIOInstructionsPtr,
															inputLine, 
															columnStart,
															columnEnd,
															1,
												 			gInputBufferPtr,  
												 			gOutputBufferPtr);
												
						if (errCode != noErr)			
							{
							continueFlag = FALSE;
																									break;
							
							}	// end "if (errCode != noErr)"	
																					
						lastLineRead = inputLine;
																					
						}	// end "if (inputLine != lastLineRead)" 
				
					while (channelCount<numberOutChannels)
						{
						if (!rectifyChannelPtr[channelCount])
							{
							if (fileInfoPtr->bandInterleave != kBIS)
								BlockMoveData (&ioIn1ByteBufferPtr[nonRectifiedColumnOffset], 
													ioOut1ByteBufferPtr, 
													countInBytes);
													
							else	// fileInfoPtr->bandInterleave == kBIS
								{
								ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
								ioIn1ByteBufferPtr =
												&ioIn1ByteBufferPtr[channelCount*numberBytes];
									
								for (column=0; column<countInBytes; column+=numberBytes)
									{
									memcpy (
										ioOut1ByteBufferPtr, ioIn1ByteBufferPtr, numberBytes);
									ioIn1ByteBufferPtr += numberReadChannels;
									ioOut1ByteBufferPtr += numberBytes;
								
									}	// end "for (column=0; column<=..."	
											
								ioOut1ByteBufferPtr -= countInBytes;
								
								}	// end "else fileInfoPtr->bandInterleave == kBIS"
								
							}	// end "if (!rectifyChannelPtr[channelCount])"
						
						channelCount++;
						
						 		// Not sure about this. It is change above for 1-byte data but
						 		// not for 2-byte data.  Is it really working correctly.
						 	
						ioIn1ByteBufferPtr += inOffsetBytes; 
						ioOut1ByteBufferPtr += outChannelByteIncrement;
					
						}	// end "while (channelCount<numberOutChannels)" 
				
					}	// end "if (inputLine >= startLine && ..." 
					
				if (continueFlag && shiftOnlyFlag)
					{
							// Load all columns of data for those channels for 		
							// which rectification is only a line and/or column 		
							// shift.																
							
					ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
					ioOut1ByteBufferPtr = &savedOutBufferPtr[preLineBytes];
					channelCount = 0;
					
					rectifiedLine = inputLine - lineShift;
					if (rectifiedLine >= (SInt32)startLine && 
															rectifiedLine <= (SInt32)stopLine)
						{
								// Line is within input image file range.					
								
						if (rectifiedLine != (SInt32)lastLineRead)
							{
									// Get all requested channels for line of image 	
									// data.  Return if there is a file IO error.		
					 
							errCode = GetLineOfData (fileIOInstructionsPtr,
																rectifiedLine,
																columnStart,
																columnEnd,
																1,
													 			gInputBufferPtr,  
													 			gOutputBufferPtr);
														
							if (errCode != noErr)		
								{
								continueFlag = FALSE;
																									break;
								
								}	// end "if (errCode != noErr)"	
																					
							lastLineRead = rectifiedLine;
																					
							}	// end "if (rectifiedLine != lastLineRead)" 
							
						while (channelCount<numberOutChannels)
							{
							if (rectifyChannelPtr[channelCount])
								{
								if (fileInfoPtr->bandInterleave != kBIS)
									BlockMoveData (&ioIn1ByteBufferPtr[rectifiedInputOffset], 
													&ioOut1ByteBufferPtr[rectifiedOutputOffset], 
													rectifyBytesToMove);
													
								else	// fileInfoPtr->bandInterleave == kBIS
									{
									ioIn1ByteBufferPtr = (unsigned char*)gOutputBufferPtr;
									ioIn1ByteBufferPtr =
													&ioIn1ByteBufferPtr[channelCount*numberBytes];
										
									for (column=0; column<countInBytes; column+=numberBytes)
										{
										memcpy (
											ioOut1ByteBufferPtr, ioIn1ByteBufferPtr, numberBytes);
										ioIn1ByteBufferPtr += numberReadChannels;
										ioOut1ByteBufferPtr += numberBytes;
									
										}	// end "for (column=0; column<=..."	
										
									ioOut1ByteBufferPtr -= countInBytes;
									
									}	// end "else fileInfoPtr->bandInterleave == kBIS"
											
								}	// end "if (rectifyChannelPtr[channelCount])" 
							
							channelCount++;
							ioIn1ByteBufferPtr += inOffsetBytes;
							ioOut1ByteBufferPtr += outChannelByteIncrement;
						
							}	// end "while (channelCount<numberOutChannels)" 
							
						}	// end "if (rectifiedLine >= startLine && ..."
						
					}	// end "if (continueFlag && shiftOnlyFlag)" 
			
						// Check if user wants to abort processing.							
						
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						continueFlag = FALSE; 
							
					}	// end "if (TickCount () >= nextTime)" 
					
				}	// end "if (loadAllColumnsAtOnceFlag)" 
				
			if (continueFlag && (!shiftOnlyFlag || forceBISFlag))
				{
				columnOutBufferPtr = &savedOutBufferPtr[preLineBytes];
										
				for (column=1; column<=numberOutputColumns; column++)
					{
							// Get the input line and column that matches the 			
							// output line and column.											
				
					MapNearestNeighborLineColumn (inverseMapMatrixPtr,
															line + mapLineShift, 
															column + mapColumnShift, 
															&inputMapLine, 
															&inputColumn);
					
					if (inputMapLine >= (SInt32)startLine && 
								inputColumn >= (SInt32)startColumn &&
									inputMapLine <= (SInt32)stopLine && 
											inputColumn <= (SInt32)stopColumn)
						{
						if (inputMapLine != (SInt32)lastLineRead)
							{
									// Get all requested channels for line of 			
									// image data. Return if there is a file IO error.	
						 
							errCode = GetLineOfData (fileIOInstructionsPtr,
																inputMapLine, 
																columnStart,
																columnEnd,
																1,
													 			gInputBufferPtr,  
													 			gOutputBufferPtr);
													 					
							if (errCode != noErr)		
								{
								continueFlag = FALSE;
																										break;
								
								}	// end "if (errCode != noErr)"	
																						
							lastLineRead = inputMapLine;
																						
							}	// end "if (inputMapLine != lastLineRead)" 
							
						channelCount = 0;
						inputColumn -= columnStart;
						
								// Adjust column index to allow for band interleave.
								
						if (forceBISFlag || fileInfoPtr->bandInterleave == kBIS)
							inputColumn *= numberReadChannels;
							
								// Set input buffer pointer.												
				
						ioIn1ByteBufferPtr = (unsigned char*)
														&gOutputBufferPtr[inputColumn*numberBytes];
						
							// Set output buffer pointers.								
		
						ioOut1ByteBufferPtr = columnOutBufferPtr;
					
						while (channelCount<numberOutChannels)
							{
							if (rectifyChannelPtr[channelCount])
								memcpy (ioOut1ByteBufferPtr, ioIn1ByteBufferPtr, numberBytes);
						
							channelCount++;
						
							ioIn1ByteBufferPtr += inOffsetBytes;
							ioOut1ByteBufferPtr += outChannelByteIncrement;
					
							}	// end "while (channelCount<..." 
							
						}	// end "else inputMapLine && inputColumn ... 
						
					columnOutBufferPtr += columnByteSkip;
			
							// Check if user wants to abort processing.							
							
					if (TickCount () >= gNextTime)
						{
						if (!CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
							continueFlag = FALSE;
							
						}	// end "if (TickCount () >= nextTime)" 
						
					}	// end "for (column=1; column<=numberOutputColumns; ..." 
					
				}	// end "if (... && (!shiftOnlyFlag || forceBISFlag))" 
			
			if (outFileInfoPtr->bandInterleave == kBSQ)
				savedOutBufferPtr = &savedOutBufferPtr[outNumberBytesPerLineAndChannel];
			
			else	// outFileInfoPtr->bandInterleave != kBSQ
				savedOutBufferPtr = &savedOutBufferPtr[countOutBytes];
	
					// Write line(s), channel(s) of data when needed.
			
			totalIOOutBytes += countOutBytes;
			
			if (continueFlag && (totalIOOutBytes > limitIoOutBytes))
				{
				errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															&ioOutBufferPtr [countOutBytes],
															reformatOptionsPtr->channelPtr,
															numberOutChannels,
															lastOutputWrittenLine,
															outNumberBytesPerLineAndChannel,
															numberOutputLines,
															outChannelByteIncrement,
															totalIOOutBytes,
															reformatOptionsPtr,
															1);
				
				totalIOOutBytes = 0;
				savedOutBufferPtr = (unsigned char*)&ioOutBufferPtr[countOutBytes];
				
				lastOutputWrittenLine = line;
																
				if (errCode != noErr)
					{
					continueFlag = FALSE;
																									break;
					
					}	// end "if (errCode != noErr)"
				
				}	// end "if (... && (totalIOOutBytes > limitIoOutBytes))" 
			
					// Update status dialog box.												
					
			percentComplete = 100 * line/numberOutputLines;
			if (percentComplete != lastPercentComplete)
				{
				LoadDItemValue (gStatusDialogPtr, 
										IDC_ShortStatusValue, 
										(SInt32)percentComplete);
				lastPercentComplete = (SInt16)percentComplete;
				
				}	// end "if (percentComplete != lastPercentComplete)" 
			
					// Adjust line.																
					
			line++;
			inputLine++;
				
			}	// end "while (line < numberOutputLines && continueFlag)" 
			
				// Flush output buffer if needed.											
		
		if (continueFlag && totalIOOutBytes > 0)
			{				
			errCode = WriteOutputDataToFile (outFileInfoPtr,
														outFileStreamPtr,
														&ioOutBufferPtr [countOutBytes],
														reformatOptionsPtr->channelPtr,
														numberOutChannels,
														lastOutputWrittenLine,
														outNumberBytesPerLineAndChannel,
														numberOutputLines,
														outChannelByteIncrement,
														totalIOOutBytes,
														reformatOptionsPtr,
														1);
															
			if (errCode != noErr)	
				continueFlag = FALSE;							
			
			}	// end "if (continueFlag && (totalIOOutBytes > 0))" 
		
				// Close up any File IO Instructions structure that pertain to the 
				// specific area used for rectification.
				
		CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

				// Turn spin cursor off
	
		gPresentCursor = kWait;
		
				// Write channel discriptions at the end of the file if needed.	
		
		if (continueFlag && reformatOptionsPtr->channelDescriptions)	
			WriteChannelDescriptionsAndValues (
									outFileInfoPtr, 
									NULL, 
									outFileInfoPtr->numberChannels,
									(reformatOptionsPtr->outputFileCode == kAppendToMenuItem));

		if (continueFlag && outFileInfoPtr->thematicType)
			{                        
			supportFileType = kITRLFileType;
			if (outFileInfoPtr->format != kErdas74Type)
				supportFileType = kICLRFileType;
				                   
			continueFlag = CreateThematicSupportFile (
														outFileInfoPtr,
														NULL, 
														(UInt16)outFileInfoPtr->numberClasses, 
														NULL,
														0,
														NULL, 
														GetPaletteHandle (),
														NULL,		// newPaletteIndexPtr, 
														GetPaletteType (),
														(UInt16)outFileInfoPtr->numberClasses, 
														kFromDescriptionCode,
														kPaletteHistogramClassNames,
														kClassDisplay,
														kCollapseClass,
														supportFileType);
				
			}	// end "if (continueFlag && outFileInfoPtr->thematicType)" 
			
		}	// end "if (continueFlag)" 
		
	return (!continueFlag);
			
}	// end "RectifyImage" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RectifyImageControl
//
//	Software purpose:	This routine controls the process of rectifying the
//							input image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1992
//	Revised By:			Larry L. Biehl			Date: 01/31/2013

void RectifyImageControl (void)

{
			// Declare local variables & structures
			
	double								minBackgroundValue,
											maxBackgroundValue;
														
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	
	Handle								outFileInfoHandle,
											reformatOptionsH;
	
	LongRect								inputRectangle,
											outputRectangle;
	
	FileInfoPtr							outFileInfoPtr;	
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	
	time_t								startTime;
	
	SInt32								index,
											savedOutputRectangleStartColumn,
											savedOutputRectangleStartLine;				
	
	UInt32								numberBytes;					
	
	Boolean								continueFlag,
											returnFlag,
											updateMapProjectionFlag;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																								return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
			// Initialize local variables.													
			
	continueFlag = TRUE;
	rectifyImageOptionsPtr = NULL;
	outFileInfoHandle = NULL;
	reformatOptionsH = NULL;
	fileIOInstructionsPtr = NULL;
	reformatOptionsPtr = NULL; 
	
			// Get a handle to a block of memory to be used for						
			// file information for the new image file.									
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
	
	continueFlag = GetReformatAndFileInfoStructures (
														&outFileInfoHandle, &reformatOptionsH);
			
			// Get a handle to a block of memory to use for the reformatting		
			// options.  Then lock it and get a pointer to the block.				
	
	if (continueFlag)
		{		
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
																			reformatOptionsH, kLock);
														
		outFileInfoPtr =  (FileInfoPtr)GetHandlePointer (outFileInfoHandle, kLock);
			
		reformatOptionsPtr->swapBytesFlag = gImageFileInfoPtr->swapBytesFlag;
			
		reformatOptionsPtr->signedOutputDataFlag = gImageFileInfoPtr->signedDataFlag;
			
		outFileInfoHandle = NULL;
		if (!gImageFileInfoPtr->thematicType && gImageWindowInfoPtr->descriptionCode)
			reformatOptionsPtr->channelDescriptions = TRUE;
		reformatOptionsPtr->numberChannels = gImageWindowInfoPtr->totalNumberChannels;
													
		}	// end "if (continueFlag)" 
			
			// Get a block of memory to use for the reformatting rectification options.
			
	if (continueFlag)
		{		
		rectifyImageOptionsPtr = 
						(RectifyImageOptionsPtr)MNewPointer (sizeof (RectifyImageOptions));
		if (rectifyImageOptionsPtr)
			{
			rectifyImageOptionsPtr->columnScaleFactor = 1.0;
			rectifyImageOptionsPtr->lineScaleFactor = 1.0;
			rectifyImageOptionsPtr->rotationAngle = 0.0;
			rectifyImageOptionsPtr->rotationColumnCenter = 
						((double)reformatOptionsPtr->columnStart + 
																	reformatOptionsPtr->columnEnd)/2.;
			rectifyImageOptionsPtr->rotationLineCenter = 
						((double)reformatOptionsPtr->lineStart + 
																	reformatOptionsPtr->lineEnd)/2.;
			rectifyImageOptionsPtr->rectifyChannelPtr = NULL;
			rectifyImageOptionsPtr->numberChannelsToRectify = 
														gImageWindowInfoPtr->totalNumberChannels;
			
			if (gImageWindowInfoPtr->dataTypeCode == kIntegerType)
				{
				rectifyImageOptionsPtr->backgroundValue = 
															gImageWindowInfoPtr->minUsableDataValue;
				
				if (gImageWindowInfoPtr->numberBytes == 4)
					rectifyImageOptionsPtr->backgroundValue = 0;
				
				}	// end "if (gImageWindowInfoPtr->dataTypeCode == kIntegerType)"
				
			else	// gImageWindowInfoPtr->dataTypeCode == kRealType
				rectifyImageOptionsPtr->backgroundValue = 0;
				
			minBackgroundValue = gImageWindowInfoPtr->minUsableDataValue;
			maxBackgroundValue = gImageWindowInfoPtr->maxUsableDataValue;
	
					// Get handle to window information for reference image
			
			rectifyImageOptionsPtr->referenceWindowInfoHandle = NULL;
				
			rectifyImageOptionsPtr->columnShift = 0;
			rectifyImageOptionsPtr->lineShift = 0;
			rectifyImageOptionsPtr->nonRectifiedInputOffset = 0;
			rectifyImageOptionsPtr->rectifiedInputOffset = 0;
			rectifyImageOptionsPtr->procedureCode = kTranslateScaleRotate;
			rectifyImageOptionsPtr->blankOutsideSelectedAreaFlag = TRUE;
			rectifyImageOptionsPtr->shiftOnlyFlag = FALSE;
			
			reformatOptionsPtr->rectifyImageOptionsPtr = rectifyImageOptionsPtr;
			
			}	// end "if (rectifyImageOptionsPtr)" 
			
		else	// !rectifyImageOptionsPtr 
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
					
			// Set up memory for rectification channels vector.						
	
	if (continueFlag)
		{
		numberBytes = (SInt32)gImageWindowInfoPtr->totalNumberChannels * 
																						sizeof (SInt16);
																				
		Boolean tempFlag;
		continueFlag = CheckPointerSize (
											(Ptr*)&rectifyImageOptionsPtr->rectifyChannelPtr,
											numberBytes, 
											&tempFlag);
		
		}	// end "if (continueFlag)" 
	
			// If memory is okay to this point then initialize the vectors.		
			
	if (continueFlag)
		{
				// Fill in vector of all rectification channels.						
		
		for (index=0; index<rectifyImageOptionsPtr->numberChannelsToRectify; index++)
			rectifyImageOptionsPtr->rectifyChannelPtr[index] = (SInt16)index;
			
				// Initialize the format information for the output file		
				
		InitializeOutputFileInformation (gImageWindowInfoPtr,
													gImageFileInfoPtr,
													reformatOptionsPtr,
													outFileInfoPtr,
													kKeepSameFormatAsInput);
													
				// The output file will not need to have the bytes swapped.  If the input
				// file contains data with bytes swapped, they will be swapped when read
				// in before writing to the new file.  The user does not have the option
				// to force the output file to have bytes swapped.
													
		outFileInfoPtr->swapBytesFlag = FALSE;
													
				// Get Changes that the user wants to make in the file format		
				
		if (RectifyImageDialog (gImageFileInfoPtr, 
											outFileInfoPtr, 
											reformatOptionsPtr,
											minBackgroundValue,
											maxBackgroundValue))
			{
					// List the processor name, date and time.							
				
			gOutputForce1Code = 0x0001;
			continueFlag = ListHeaderInfo (NULL, 
														kLImageInfo, 
														&gOutputForce1Code, 
														kNoStatisticsUsed, 
														continueFlag);
														
			if (rectifyImageOptionsPtr->procedureCode == kTranslateScaleRotate)
				{
						// Get the mapping matrix.					
						
				GetMappingMatrix (rectifyImageOptionsPtr);
		
					// Determine the size of the output image that contains the 		
					// requested input image to be rectified.									
				
				inputRectangle.left = reformatOptionsPtr->columnStart; 
				inputRectangle.top = reformatOptionsPtr->lineStart; 
				inputRectangle.right = reformatOptionsPtr->columnEnd; 
				inputRectangle.bottom = reformatOptionsPtr->lineEnd;
				MapOutputImageRectangle (&rectifyImageOptionsPtr->mapMatrix, 
													&inputRectangle,
													&outputRectangle);
					
						// Adjust the mapping so that the output rectangle starts from	
						// line 1 and column 1.														
										
				OffsetMappingMatrix (&rectifyImageOptionsPtr->mapMatrix,
											1 - outputRectangle.left,
											1 - outputRectangle.top);
											
				savedOutputRectangleStartLine = outputRectangle.top;
				savedOutputRectangleStartColumn = outputRectangle.left;
											
						// Now apply any line and column shifts that the user wants 	
						// to apply for the rectified data.										
										
				OffsetMappingMatrix (&rectifyImageOptionsPtr->mapMatrix,
											rectifyImageOptionsPtr->columnShift,
											rectifyImageOptionsPtr->lineShift);
											
						// Now adjust the output rectangle so that it includes all of the
						// requested input rectangle allowing for any user requested line
						// and column shifts.
						
				if (rectifyImageOptionsPtr->lineShift > 0)
					outputRectangle.bottom += rectifyImageOptionsPtr->lineShift;
					
				else if (rectifyImageOptionsPtr->lineShift < 0)
					outputRectangle.top += rectifyImageOptionsPtr->lineShift;
						
				if (rectifyImageOptionsPtr->columnShift > 0)
					outputRectangle.right += rectifyImageOptionsPtr->columnShift;
					
				else if (rectifyImageOptionsPtr->columnShift < 0)
					outputRectangle.left += rectifyImageOptionsPtr->columnShift;
													
						// Determine if this mapping is just a shift in the lines		
						// and/or columns.															
				
				rectifyImageOptionsPtr->shiftOnlyFlag = 
						DetermineIfIdentityMatrix (&rectifyImageOptionsPtr->mapMatrix);
				
						// Now get the inverse mapping.											
						
				continueFlag = InvertMappingMatrix (
													&rectifyImageOptionsPtr->mapMatrix,
													&rectifyImageOptionsPtr->inverseMapMatrix);
					
				if (continueFlag)
					{					
							// Keep the user selected lines and columns of the input  	
							// image that are to be used in the rectification process.	
							
					reformatOptionsPtr->startLine = reformatOptionsPtr->lineStart;
					reformatOptionsPtr->stopLine =  reformatOptionsPtr->lineEnd;
					reformatOptionsPtr->startColumn = reformatOptionsPtr->columnStart;
					reformatOptionsPtr->stopColumn =  reformatOptionsPtr->columnEnd;
					
							// Make certain that these parameters reflect the 				
							// output data to be read so that the updating of the			
							// output file structure works properly.							
					
					reformatOptionsPtr->lineStart = 1;
					reformatOptionsPtr->lineEnd =  
											outputRectangle.bottom - outputRectangle.top + 1;
					reformatOptionsPtr->columnStart = 1;
					reformatOptionsPtr->columnEnd =  
											outputRectangle.right - outputRectangle.left + 1;
											
					}	// end "if (continueFlag)"
													
				}	// end "if (...->procedureCode == kTranslateScaleRotate)"	

			if (continueFlag)
				{														
						// Update parameters in the structure for the output file.	
				
				UpdateOutputFileStructure (outFileInfoPtr, 
													reformatOptionsPtr, 
													1, 
													1, 
													NULL);
														
				if (rectifyImageOptionsPtr->procedureCode == kTranslateScaleRotate)
					{
							// Make certain that these ReformatOption parameters now 	
							// reflect the input data to be read that will include		
							// that needed for rectified and non-rectified channels.		
											
							// Copy the adjusted output image size back to the output	
							// rectangle if the data is only being shifted.
							
					if (rectifyImageOptionsPtr->shiftOnlyFlag)
						{
						outputRectangle.left -= reformatOptionsPtr->startColumn - 1; 
						outputRectangle.top -= reformatOptionsPtr->startLine - 1; 
						outputRectangle.right -= reformatOptionsPtr->startColumn - 1; 
						outputRectangle.bottom -= reformatOptionsPtr->startLine - 1;
						
						}	// end "if (rectifyImageOptionsPtr->shiftOnlyFlag)"
						
					else	// !rectifyImageOptionsPtr->shiftOnlyFlag
						{
								// The '-1's keep the output rectangle relative to 1
								// based instead of 0 based.
								
						outputRectangle.left -= (savedOutputRectangleStartColumn-1); 
						outputRectangle.top -= (savedOutputRectangleStartLine-1); 
						outputRectangle.right -= (savedOutputRectangleStartColumn-1); 
						outputRectangle.bottom -= (savedOutputRectangleStartLine-1);
						
						}	// end "else !rectifyImageOptionsPtr->shiftOnlyFlag"
					
							// Determine the input data that is needed for the rectified	
							// output data.															
							
					MapOutputImageRectangle (&rectifyImageOptionsPtr->inverseMapMatrix, 
														&outputRectangle, &inputRectangle);
					
					reformatOptionsPtr->lineStart = MAX (1, inputRectangle.top);
					reformatOptionsPtr->lineEnd = MIN (
						(SInt32)gImageWindowInfoPtr->maxNumberLines, inputRectangle.bottom);
					reformatOptionsPtr->columnStart = MAX (1,inputRectangle.left);
					reformatOptionsPtr->columnEnd = MIN (
						(SInt32)gImageWindowInfoPtr->maxNumberColumns, inputRectangle.right);
					
							// Determine if the map projection stucture should be updated.
						
					updateMapProjectionFlag = TRUE;
					if (rectifyImageOptionsPtr->numberChannelsToRectify != 
														gImageWindowInfoPtr->totalNumberChannels)
						updateMapProjectionFlag = FALSE;
						
					}	// end "if (...->procedureCode == kTranslateScaleRotate"
					
				else	// ...->procedureCode == kReprojectToReferenceImage
					{
							// Now make sure that these parameters reflect the data to be
							// read from the input file.  The parameters for the size of
							// the output file are reflected in the outFileInfoPtr
							// structure.
							
					reformatOptionsPtr->columnStart = reformatOptionsPtr->startColumn;
					reformatOptionsPtr->columnEnd = reformatOptionsPtr->stopColumn;
					reformatOptionsPtr->lineStart = reformatOptionsPtr->startLine;
					reformatOptionsPtr->lineEnd = reformatOptionsPtr->stopLine;
					
					reformatOptionsPtr->startColumn = 1;
					reformatOptionsPtr->startLine = 1;
					
							// Indicate that the map projection stucture should be updated.
							
					updateMapProjectionFlag = TRUE;
												
					}	// end "else ...->procedureCode == kReprojectToReferenceImage"
				
						// Update the map information for the output structure if all
						// channels of data are only going to be shifted and/or scaled.  
						// Otherwise do not provide map information for the output file.	
				
				if (updateMapProjectionFlag)										
					RectifyUpdateMapProjectionStructure (
												gActiveImageWindowInfoH,
												outFileInfoPtr,
												reformatOptionsPtr->startColumn,
												reformatOptionsPtr->startLine,
												rectifyImageOptionsPtr->procedureCode,
												rectifyImageOptionsPtr->referenceWindowInfoHandle,
												rectifyImageOptionsPtr->columnShift, 
												rectifyImageOptionsPtr->lineShift,
												rectifyImageOptionsPtr->columnScaleFactor,
												rectifyImageOptionsPtr->lineScaleFactor,
												&rectifyImageOptionsPtr->mapMatrix,
												&rectifyImageOptionsPtr->inverseMapMatrix,
												rectifyImageOptionsPtr->rotationAngle);
				
				if (rectifyImageOptionsPtr->procedureCode == kTranslateScaleRotate)
					{		
							// Save the input rectified start column.							
							
					inputRectangle.left = reformatOptionsPtr->columnStart;
					
					if (reformatOptionsPtr->numberChannels > 
												rectifyImageOptionsPtr->numberChannelsToRectify)
						{
						reformatOptionsPtr->lineStart = MIN (
																	reformatOptionsPtr->lineStart,
																	reformatOptionsPtr->startLine);
						reformatOptionsPtr->lineEnd = MAX (
																	reformatOptionsPtr->lineEnd,
																	reformatOptionsPtr->stopLine);
						reformatOptionsPtr->columnStart = MIN (
																	reformatOptionsPtr->columnStart,
																	reformatOptionsPtr->startColumn);
						reformatOptionsPtr->columnEnd = MAX (
																	reformatOptionsPtr->columnEnd,
																	reformatOptionsPtr->stopColumn);
							
						}	// end "if (reformatOptionsPtr->numberChannels > ..." 
						
							// Get the offset to start reading data from the input		
							// for non-rectified channels.										
							
					rectifyImageOptionsPtr->nonRectifiedInputOffset =
							reformatOptionsPtr->startColumn - inputRectangle.left;
					rectifyImageOptionsPtr->nonRectifiedInputOffset = MAX (
										rectifyImageOptionsPtr->nonRectifiedInputOffset, 0);
						
							// Get the offset to start reading data from the input		
							// for rectified channels.												
							
					rectifyImageOptionsPtr->rectifiedInputOffset =
							inputRectangle.left - reformatOptionsPtr->startColumn;
					rectifyImageOptionsPtr->rectifiedInputOffset = MAX (
											rectifyImageOptionsPtr->rectifiedInputOffset, 0);
											
					}	// end "if (...->procedureCode == kTranslateScaleRotate)"
				
						// Get pointer to memory to use to read an image file line
						// into.
			 						
				continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
																gImageWindowInfoPtr,
																gImageLayerInfoPtr,
																gImageFileInfoPtr,
																&gInputBufferPtr, 
																&gOutputBufferPtr,
																reformatOptionsPtr->columnStart,
										 						reformatOptionsPtr->columnEnd,
										 						1,
										 						reformatOptionsPtr->numberChannels,
										 						NULL, 
																kPackData, 
																outFileInfoPtr->bandInterleave == kBIS, 
																reformatOptionsPtr->forceByteCode,
																kDoNotAllowForThreadedIO,
																&fileIOInstructionsPtr);
										
				}	// end "if (continueFlag)" 
	
					// Get buffer for output data.											
					
			if (continueFlag)
				continueFlag = 
							GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
		
				// Branch to end of routine if memory is full and do cleanup of handles.
				
			if (continueFlag)
				{		
						// Set up the disk file to be used for the new reformatted	
						// image file.																
						
				continueFlag = 
						!GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
						
				}	// end "if (continueFlag)"  
						
			if (continueFlag)
				{
						// Change cursor to watch cursor until done with process.	
				
				MSetCursor (kWait);
					
						// Get status dialog. Also call routine to do					
						// activate/update events.												
					
				gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);
				if (gStatusDialogPtr)
					{
					MGetString (gTextString, kReformatStrID, IDS_PercentComplete);
					LoadDItemString (
								gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
					LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)0);
					ShowStatusDialogWindow (kShortStatusInfoID);
					
							// Everything has now been set up.								
							// Do reformatting for BIL output files here.				
							
					startTime = time (NULL);
					
					if (rectifyImageOptionsPtr->procedureCode == kTranslateScaleRotate)									
						returnFlag = RectifyImage (
								fileIOInstructionsPtr, outFileInfoPtr, reformatOptionsPtr);
					
					else if (rectifyImageOptionsPtr->procedureCode ==
																			kReprojectToReferenceImage)
						returnFlag = ReprojectImage (
								fileIOInstructionsPtr, outFileInfoPtr, reformatOptionsPtr);
					
							// List Reformat information.										
							
					continueFlag = ListRectifyResultsInformation (
																reformatOptionsPtr, outFileInfoPtr);
				
							// List the CPU time taken for reformatting.					
								
					continueFlag = ListCPUTimeInformation (NULL, continueFlag, startTime);
								
							// Dispose of status dialog										
					
					CloseStatusDialog (TRUE);
						
					}	// end "if (gStatusDialogPtr)" 
		  		
		  				// Close the output file
		  				
				CloseFile (outFileInfoPtr);
			
						// Reset cursor back to arrow to indicate that process		
						// is done																	
					
				MInitCursor ();
					
				}	// end "if (continueFlag)" 
				
					// Dispose of the IO buffer.		
										
			DisposeIOBufferPointers (fileIOInstructionsPtr,
												&gInputBufferPtr, 
												&gOutputBufferPtr);
				
			}	// end "if (ChangeImageFormatDialog (gImageFileInfoPtr, ... " 
						
		}	// end "if (continueFlag)" 
		
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, gImageFileInfoPtr);				
			
	//ReleaseReformatSpecsMemory (&reformatOptionsH, gImageFileInfoPtr);
					
}	// end "RectifyImageControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		RectifyImageDialog
//
//	Software purpose:	This routine handle the rectify image dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1992
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

Boolean RectifyImageDialog (
				FileInfoPtr							fileInfoPtr, 
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr,
				double								minBackgroundValue,
				double								maxBackgroundValue)

{
	Boolean								OKFlag;
										
#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
	
	Rect									headerBox,
											resampleBox,
											theBox;
	
	DialogPtr							dialogPtr;
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	
	SInt16								*rectifyChannelPtr;
									
	UserItemUPP							drawHeaderOptionsPopUpPtr,
											drawResamplePopUpPtr;
	
	double								lastBackgroundValue,
											lastColumnScaleFactor,
											lastLineScaleFactor,
											lastRotationAngle,
											mapOrientationAngle,
											realValue;
	
	Handle								okHandle,
											referenceWindowInfoHandle,
											theHandle;
	
	SInt32								lastColumnShift,
											lastLineShift,
											theNum;
	
	SInt16								itemHit,
											itemHit2,
											numberDecimalPlaces,
											procedureCode,
											theType;
											
	//SignedByte							handleStatus;
	
	Boolean								blankOutsideSelectedAreaFlag,
											dialogWindowShownFlag,
											modalDone;
											

			// Get the modal dialog for the image file format description			
				
	dialogPtr = LoadRequestedDialog (kRectifyImageDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																						return (FALSE);
	
	referenceWindowInfoHandle = NULL;
		
			// Intialize local user item proc pointers.	
			
	drawHeaderOptionsPopUpPtr = NewUserItemUPP (DrawHeaderOptionsPopUp);		
			
	drawResamplePopUpPtr = NewUserItemUPP (DrawResamplePopUp);	
												
	SetDialogItemDrawRoutine (dialogPtr, 37, gDrawImageFilePopUpPtr);
	
	RectifyImageDialogInitialize (dialogPtr,
												fileInfoPtr,
												&dialogSelectArea,
												reformatOptionsPtr,
												&gHeaderOptionsSelection,
												&gChannelSelection,
												&blankOutsideSelectedAreaFlag,
												&lastBackgroundValue,
												&procedureCode,
												&gResampleSelection,
												&gFileNamesSelection,
												&referenceWindowInfoHandle,
												&lastLineShift,
												&lastColumnShift,
												&lastLineScaleFactor,
												&lastColumnScaleFactor,
												&lastRotationAngle,
												&mapOrientationAngle);
												
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;

			// To entire image icon.															
			//	Reformat area																		
			
	LoadLineColumnItems (&dialogSelectArea, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	
			// "Image Header" popup box.														
				
	SetDialogItemDrawRoutine (dialogPtr, 17, &headerBox, drawHeaderOptionsPopUpPtr);
	
			// Make sure that all header choices are enabled to start with.
			
	InitializeHeaderPopupMenu (dialogPtr, gPopUpHeaderOptionsMenu);
	DisableMenuItem (gPopUpHeaderOptionsMenu, kMatlabMenuItem);
	
	if (fileInfoPtr->format != kGAIAType && fileInfoPtr->format != kGAIA2Type)
		DisableMenuItem (gPopUpHeaderOptionsMenu, kGAIAMenuItem);
		
			// Blank pixels outside of selected area check box.						
   
	SetDLogControl (dialogPtr, 18, blankOutsideSelectedAreaFlag);				
   
	SetDLogControl (dialogPtr, 
							IDC_TranslateScaleRotateRadio, 
							(procedureCode == kTranslateScaleRotate));	
							
	SetDLogControl (dialogPtr, 
							IDC_ReprojectToRadio, 
							(procedureCode == kReprojectToReferenceImage));
			
	lastBackgroundValue = rectifyImageOptionsPtr->backgroundValue;

	LoadDItemRealValue (dialogPtr, 20, lastBackgroundValue, 0);
   
   		// Load the default rectification settings.									
			
	LoadDItemValue (dialogPtr, 25, lastLineShift);
	
	LoadDItemValue (dialogPtr, 26, lastColumnShift);
			
	LoadDItemRealValue (dialogPtr, 27, lastLineScaleFactor, 2);
			
	LoadDItemRealValue (dialogPtr, 28, lastColumnScaleFactor, 2);
	
	LoadDItemRealValue (dialogPtr, 31, lastRotationAngle, 1);

			//	Set the draw routine for the rectification channel popup box.		
			
	if (!fileInfoPtr->thematicType)
		{
		SetDialogItemDrawRoutine (dialogPtr, 33, gDrawChannelsPopUpPtr);
		
		}	// end "if (!fileInfoPtr->thematicType)"														
				
	SetDialogItemDrawRoutine (
							dialogPtr, 41, &resampleBox, drawResamplePopUpPtr);
	//DisableMenuItem (gPopUpResampleMenu, kMean);
		
			// Center the dialog and then show it.											
	
	dialogWindowShownFlag = FALSE;	
	if (gMultiSpecWorkflowInfo.workFlowCode != 1 && 
														gMultiSpecWorkflowInfo.workFlowCode != 2)
		{
		ShowDialogWindow (dialogPtr, kRectifyImageDialogID, kSetUpDFilterTable);
		
		dialogWindowShownFlag = TRUE;	
		
		gDialogItemDescriptorPtr[20] = kDItemReal + kDItemMinus;
		gDialogItemDescriptorPtr[25] = kDItemMinus;
		gDialogItemDescriptorPtr[26] = kDItemMinus;
		gDialogItemDescriptorPtr[27] = kDItemReal;
		gDialogItemDescriptorPtr[28] = kDItemReal;
		gDialogItemDescriptorPtr[31] = kDItemReal + kDItemMinus;
		
				// Set default text selection to first edit text item						
				
		SelectDialogItemText (dialogPtr, 25, 0, INT16_MAX);
	
		itemHit = 0;
		
		}	// end "if (gMultiSpecWorkflowInfo.workFlowCode != 1 && ..."
		
	else	// gMultiSpecWorkflowInfo.workFlowCode == 1 || ...
		itemHit = gMultiSpecWorkflowInfo.workFlowCode;
								
	modalDone = FALSE;
	do 
		{
		if (dialogWindowShownFlag)
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			
		if (itemHit > 2)
			{
					// If itemHit was a radio button make appropriate control		
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.										
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 6:			// Entire area to selected area switch.
				case 42:			// Entire area to selected area switch. (Appearance Manager)
				case 9:			//	 lineStart
				case 10:			//	 lineEnd
				case 12:			//	 columnStart
				case 13:			//	 columnEnd
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 17:				// "Write Erdas Header" popup box
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpHeaderOptionsMenu, 
															gHeaderOptionsSelection,
															kPopUpHeaderOptionsMenuID);
													
					if (itemHit2 != 0)
						gHeaderOptionsSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the	
							// header options pop up box.										
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 18:				// "nonselected pixels to background" Check box 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					blankOutsideSelectedAreaFlag = GetDLogControl (dialogPtr, 18);
					break;
					
				case 20:				//	 background value 
					realValue = GetDItemRealValue (dialogPtr, itemHit); 
					if (realValue < minBackgroundValue || realValue > maxBackgroundValue)
						RealNumberErrorAlert (lastBackgroundValue, dialogPtr, itemHit, 0);
										
					else	// lastBackgroundValue >=0 && ... 
						lastBackgroundValue = realValue;
					break;
					
				case 21:				// Translate, scale, rotate image 
					SetDLogControl (dialogPtr, IDC_TranslateScaleRotateRadio, 1);
					SetDLogControl (dialogPtr, IDC_ReprojectToRadio, 0);
					procedureCode = kTranslateScaleRotate;
		
					RectifyImageDialogOnRectifyCode (dialogPtr, 
																	procedureCode,
																	blankOutsideSelectedAreaFlag,
																	mapOrientationAngle);
					
					RectifyImageDialogOnReferenceFile (dialogPtr,
																	procedureCode,
																	gFileNamesSelection,
																	&referenceWindowInfoHandle,
																	&dialogSelectArea);
	
							// Set default text selection to first edit text item						
							
					SelectDialogItemText (dialogPtr, 25, 0, INT16_MAX);
					break;
					
				case 25:				//	 line shift  
					if (labs (theNum) >
										kMaxNumberLines - gImageWindowInfoPtr->maxNumberLines)
						NumberErrorAlert (lastLineShift, dialogPtr, itemHit);
										
					else	// labs (theNum) <= ...->maxNumberColumns 
						lastLineShift = theNum;
					break;
					
				case 26:				//	 column shift  
					if (labs (theNum) >
									kMaxNumberLines - gImageWindowInfoPtr->maxNumberColumns)
						NumberErrorAlert (lastColumnShift, dialogPtr, itemHit);
										
					else	// labs (theNum) <= ...->maxNumberColumns 
						lastColumnShift = theNum;
					break;
					
				case 27:				//	 line scale factor  
					lastLineScaleFactor = GetDItemRealValue (dialogPtr, itemHit);
					break;
					
				case 28:				//	 column scale factor  
					lastColumnScaleFactor = GetDItemRealValue (dialogPtr, itemHit);
					break;
					
				case 31:				//	 rotation angle  
					realValue = GetDItemRealValue (dialogPtr, itemHit);
					if (realValue > 180 || realValue < -180)
						RealNumberErrorAlert (lastRotationAngle, dialogPtr, itemHit, 1);
					
					else	// realValue <= 180 && realValue >= -180 
						lastRotationAngle = realValue;
					
					if (mapOrientationAngle != 0)
						{
						if (lastRotationAngle == mapOrientationAngle)
							SetDItemValue (dialogPtr, 34, 1);
						
						else	// lastRotationAngle != mapOrientationAngle
							SetDItemValue (dialogPtr, 34, 0);
						
						}	// end "if (mapOrientationAngle != 0)"
					break;
					
				case 33:				//	 Channels  	
					itemHit = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpAllSubsetMenu, 
															gChannelSelection, 
															kPopUpAllSubsetMenuID);
					if (itemHit == 2)
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						ChannelsDialog (
								(SInt16*)&rectifyImageOptionsPtr->numberChannelsToRectify,
								rectifyChannelPtr,
								gImageLayerInfoPtr,
								fileInfoPtr,
								1,
								kNoTransformation,
								NIL,
								gImageWindowInfoPtr->totalNumberChannels,
								gChannelSelection);
													
						if (rectifyImageOptionsPtr->numberChannelsToRectify ==
														gImageWindowInfoPtr->totalNumberChannels)
							itemHit = 1;
								
						HiliteControl ((ControlHandle)okHandle, 0);
						
						}	// end "if (itemHit == 2)" 
					
					if (itemHit != 0)
						gChannelSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// channel pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 34:				// "use map orientation angle" Check box 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					lastRotationAngle = 0;
					numberDecimalPlaces = 1;
					if (GetDLogControl (dialogPtr, 34))
						{
						lastRotationAngle = mapOrientationAngle;
						numberDecimalPlaces = 6;
						
						}	// end "if (GetDLogControl (dialogPtr, 34))"
						
					LoadDItemRealValue (
									dialogPtr, 31, lastRotationAngle, numberDecimalPlaces);
					SelectDialogItemText (dialogPtr, 31, 0, INT16_MAX);
					break;
					
				case 35:				// Rectify to reference image 
					SetDLogControl (dialogPtr, IDC_TranslateScaleRotateRadio, 0);
					SetDLogControl (dialogPtr, IDC_ReprojectToRadio, 1);
					procedureCode = kReprojectToReferenceImage;
		
					RectifyImageDialogOnRectifyCode (dialogPtr, 
																	procedureCode,
																	TRUE,
																	mapOrientationAngle);
					
					RectifyImageDialogOnReferenceFile (dialogPtr,
																	procedureCode,
																	gFileNamesSelection,
																	&referenceWindowInfoHandle,
																	&dialogSelectArea);
	
							// Set default text selection to first edit text item						
							
					SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);
					break;
					
				case 37:				// Reference image to register against popup box 
					if (gFileNamesSelection<0)
						break;
						
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpTemporaryMenu, 
															gFileNamesSelection,
															kColorMenuID);
													
					if (itemHit2 != gFileNamesSelection && itemHit2 != 0)
						gFileNamesSelection = RectifyImageDialogOnReferenceFile (
																		dialogPtr,
																		procedureCode,
																		itemHit2,
																		&referenceWindowInfoHandle,
																		&dialogSelectArea);
	
							// Make certain that the correct label is drawn in the	
							// reference image pop up box.										
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 41:				// "Resample Code" popup box 
					
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpResampleMenu, 
															gResampleSelection,
															kPopUpResampleMenuID);
													
					if (itemHit2 != 0)
						gResampleSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the	
							// resample code pop up box.										
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
					// If item hit is 1, check if display line-column values make 	
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1)
				itemHit = CheckLineColValues (&dialogSelectArea,
														reformatOptionsPtr->lineStart, 
														reformatOptionsPtr->columnStart,
														dialogPtr);
			
			if (itemHit == 1)
				{
				itemHit2 = 0;
				if (lastLineScaleFactor <= 0)
					itemHit2 = 27;
				
				if (itemHit2 ==0 && lastColumnScaleFactor <= 0)
					itemHit2 = 28;
					
				if (itemHit2 != 0)
					{
					RealNumberErrorAlert (0, dialogPtr, itemHit2, -1);
													
					itemHit = 0;
					
					}	// end "if (itemHit2 != 0)" 
				
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				OKFlag = TRUE;
				
				RectifyImageDialogOK (dialogPtr,
												outFileInfoPtr,
												fileInfoPtr,
												gImageWindowInfoPtr,
												gImageLayerInfoPtr,
												&dialogSelectArea,
												reformatOptionsPtr,
												gHeaderOptionsSelection,
												GetDLogControl (dialogPtr, 18),
												gChannelSelection,
												lastBackgroundValue,
												procedureCode,
												gResampleSelection,
												referenceWindowInfoHandle,
												lastLineShift,
												lastColumnShift,
												lastLineScaleFactor,
												lastColumnScaleFactor,
												lastRotationAngle);
										
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				OKFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone);
		
			// Remove file names from temporary popup menu.								

	for (itemHit=0; itemHit<=gNumberOfIWindows; itemHit++)
		DeleteMenuItem (gPopUpTemporaryMenu, 1);
		
	DisposeUserItemUPP (drawHeaderOptionsPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac
	
	#if defined multispec_win 
		CMReformatRectifyDlg*		dialogPtr = NULL; 
		
		TRY
			{ 
			dialogPtr = new CMReformatRectifyDlg (); 
			                                                                     
			OKFlag = dialogPtr->DoDialog (outFileInfoPtr,
													fileInfoPtr,
													gImageWindowInfoPtr,
													gImageLayerInfoPtr,
													reformatOptionsPtr,
													minBackgroundValue,
													maxBackgroundValue); 
			
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL 
	#endif	// defined multispec_win

	#if defined multispec_wx
		CMReformatRectifyDlg*		dialogPtr = NULL;
	
		dialogPtr = new CMReformatRectifyDlg (NULL);
		
		OKFlag = dialogPtr->DoDialog (outFileInfoPtr,
														fileInfoPtr,
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														reformatOptionsPtr,
														minBackgroundValue,
														maxBackgroundValue); 
			
		delete dialogPtr;
	#endif	// defined multispec_wx
	
	return (OKFlag);
	
}	// end "RectifyImageDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RectifyImageDialogInitialize
//
//	Software purpose:	This routine handle the rectify image dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/13/2005
//	Revised By:			Larry L. Biehl			Date: 09/05/2017

void RectifyImageDialogInitialize (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				SInt16*								headerOptionsSelectionPtr,
				SInt16*								channelSelectionPtr,
				Boolean*								blankOutsideSelectedAreaFlagPtr,
				double*								lastBackgroundValuePtr,
				SInt16*								procedureCodePtr,
				SInt16*								resampleCodePtr,
				SInt16*								fileNamesSelectionPtr,
				Handle*								referenceWindowInfoHandlePtr,
				SInt32*								lastLineShiftPtr,
				SInt32*								lastColumnShiftPtr,
				double*								lastLineScaleFactorPtr,
				double*								lastColumnScaleFactorPtr,
				double*								lastRotationAnglePtr,
				double*								mapOrientationAnglePtr)

{
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	SInt16								*rectifyChannelPtr;
	
	SInt16								entireIconItem,
											listCount;
	
				
			// Initialize selected area structure.	

	entireIconItem = 6;
	if (gAppearanceManagerFlag)
		entireIconItem = 42;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											reformatOptionsPtr->columnStart,
											reformatOptionsPtr->columnEnd,
											reformatOptionsPtr->columnInterval,
											reformatOptionsPtr->lineStart,
											reformatOptionsPtr->lineEnd,
											reformatOptionsPtr->lineInterval,
											9,
											entireIconItem,
											kDontAdjustToBaseImage);
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;

			// To entire image icon.															
			//	Reformat area																		

	#if defined multispec_mac 
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 6);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 42);
	#endif	// defined multispec_mac  
		
	LoadLineColumnItems (dialogSelectAreaPtr, 
								dialogPtr,
								kInitializeLineColumnValues, 
								kIntervalEditBoxesExist,
								1);
	HideDialogItem (dialogPtr, IDC_LineInterval);
	HideDialogItem (dialogPtr, IDC_ColumnInterval);
	
			// Adjust header format from the default if needed.
			
	*headerOptionsSelectionPtr = kERDAS74MenuItem;
	if (reformatOptionsPtr->headerFormat == kTIFFType)
		*headerOptionsSelectionPtr = kTIFFGeoTIFFMenuItem;
	else if (fileInfoPtr->format == kGAIAType || fileInfoPtr->format == kGAIA2Type)
		*headerOptionsSelectionPtr = kGAIAMenuItem;
	
	*blankOutsideSelectedAreaFlagPtr =
											rectifyImageOptionsPtr->blankOutsideSelectedAreaFlag;
			
	*lastBackgroundValuePtr = rectifyImageOptionsPtr->backgroundValue;
	
			// Get the procedure to be used.
	
	*procedureCodePtr = rectifyImageOptionsPtr->procedureCode;
   
   		// Load the default rectification settings.									
			
	*lastLineShiftPtr = rectifyImageOptionsPtr->lineShift;
	
	*lastColumnShiftPtr = rectifyImageOptionsPtr->columnShift;
			
	*lastLineScaleFactorPtr = rectifyImageOptionsPtr->lineScaleFactor;
			
	*lastColumnScaleFactorPtr = rectifyImageOptionsPtr->columnScaleFactor;
	
	*lastRotationAnglePtr = rectifyImageOptionsPtr->rotationAngle;
	
	*mapOrientationAnglePtr =
		GetMapOrientationAngle (fileInfoPtr->mapProjectionHandle) * kRadiansToDegrees;

			//	Set the draw routine for the rectification channel popup box.		
			
	if (fileInfoPtr->thematicType)
		{
		HideDialogItem (dialogPtr, IDC_ChannelsPrompt);
		HideDialogItem (dialogPtr, IDC_Channels);
		
		}	// end "if (fileInfoPtr->thematicType)" 
						
	*channelSelectionPtr = kAllMenuItem;
	
			// Check if any images can be used for reference images.
			
	*fileNamesSelectionPtr = GetReprojectToImageList (dialogPtr, 
																		gActiveImageWindowInfoH, 
																		TRUE, 
																		referenceWindowInfoHandlePtr,
																		&listCount);
																		
	if (listCount <= 0)
		{
		*procedureCodePtr = kTranslateScaleRotate;
		HideDialogItem (dialogPtr, IDC_ReprojectToRadio);
			
				// The initial reference image for now is the active image for use with
				// translate, scale and rotate.
		
		*referenceWindowInfoHandlePtr = gActiveImageWindowInfoH;
		
		}	// end "if (listCount <= 0)"
		
	else	// listCount > 0
		{
		*procedureCodePtr = kReprojectToReferenceImage;
		ShowDialogItem (dialogPtr, IDC_ReprojectToRadio);
		
		}	// else "listCount > 0"
		
			// Set sampling procedure
			
	*resampleCodePtr = kNearestNeighbor;
	SetUpResampleMethodPopupMenu (dialogPtr,
											gPopUpResampleMenu,
											fileInfoPtr->thematicType);
					
	RectifyImageDialogOnRectifyCode (dialogPtr, 
												*procedureCodePtr,
												*blankOutsideSelectedAreaFlagPtr,
												*mapOrientationAnglePtr);
	
}	// end "RectifyImageDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RectifyImageDialogOK
//
//	Software purpose:	This routine handle the rectify image dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2005
//	Revised By:			Larry L. Biehl			Date: 05/25/2012

void RectifyImageDialogOK (
				DialogPtr							dialogPtr,
				FileInfoPtr							outFileInfoPtr,
				FileInfoPtr							fileInfoPtr,
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				SInt16								headerOptionsSelection,
				Boolean								blankOutsideSelectedAreaFlag,
				SInt16								channelSelection,
				double								lastBackgroundValue,
				SInt16								procedureCode,
				SInt16								resampleCode,
				Handle								referenceWindowInfoHandle,
				SInt32								lastLineShift,
				SInt32								lastColumnShift,
				double								lastLineScaleFactor,
				double								lastColumnScaleFactor,
				double								lastRotationAngle)

{		
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	SInt16								*rectifyChannelPtr;
	
	SInt32								index,
											rectifyIndex;
											
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;
	
			// Rectify procedure code.
											
	rectifyImageOptionsPtr->procedureCode = procedureCode;
	rectifyImageOptionsPtr->referenceWindowInfoHandle = referenceWindowInfoHandle;
	
			// Selected area for output file.									
	
	reformatOptionsPtr->lineStart = (SInt32)dialogSelectAreaPtr->lineStart;
	reformatOptionsPtr->lineEnd = (SInt32)dialogSelectAreaPtr->lineEnd;
	reformatOptionsPtr->lineInterval = 1;
	reformatOptionsPtr->columnStart = (SInt32)dialogSelectAreaPtr->columnStart;
	reformatOptionsPtr->columnEnd = (SInt32)dialogSelectAreaPtr->columnEnd;
	reformatOptionsPtr->columnInterval = 1;
	
	rectifyImageOptionsPtr->rotationColumnCenter = 
				((double)dialogSelectAreaPtr->columnStart + 
												dialogSelectAreaPtr->columnEnd)/2.;
	rectifyImageOptionsPtr->rotationLineCenter = 
				((double)dialogSelectAreaPtr->lineStart + 
												dialogSelectAreaPtr->lineEnd)/2.;

			// Item 17: "Write Erdas Header" popup box.	
						
	reformatOptionsPtr->headerFormat = 
								GetHeaderFormatFromPopUpSelection (headerOptionsSelection);
						
	if (reformatOptionsPtr->headerFormat == kNoneType)
		outFileInfoPtr->numberHeaderBytes = 0;
	
			// Set the band interleave depending on the file format.
						
	if (reformatOptionsPtr->headerFormat == kArcViewType)
		outFileInfoPtr->bandInterleave = fileInfoPtr->bandInterleave;
			
	else if (reformatOptionsPtr->headerFormat == kErdas74Type)
		outFileInfoPtr->bandInterleave = kBIL;
			
	else if (reformatOptionsPtr->headerFormat == kTIFFType)
		outFileInfoPtr->bandInterleave = kBSQ;

			// Item 21: Nonselected pixels to background.					
		
	rectifyImageOptionsPtr->blankOutsideSelectedAreaFlag = 
																		blankOutsideSelectedAreaFlag;
		
			// Items 25, 26, 27, 28, & 31.										
			
	rectifyImageOptionsPtr->backgroundValue = lastBackgroundValue;
	rectifyImageOptionsPtr->lineShift = lastLineShift;
	rectifyImageOptionsPtr->lineScaleFactor = lastLineScaleFactor;
	rectifyImageOptionsPtr->columnShift = lastColumnShift;
	rectifyImageOptionsPtr->columnScaleFactor = lastColumnScaleFactor;
	rectifyImageOptionsPtr->rotationAngle = lastRotationAngle;

			// Item 33; Channels to be rectified before copying to 		
			// output file.															
			// Make the rectifyChannel Vector a vector of 0's or 1's		
			// to indicate which channel is to be rectified.				
			
	if (channelSelection == kAllMenuItem)
		{
		rectifyImageOptionsPtr->numberChannelsToRectify = 
																reformatOptionsPtr->numberChannels;
		for (index=0; 
				index<rectifyImageOptionsPtr->numberChannelsToRectify; 
					index++)
			rectifyChannelPtr[index] = 1;
			
		}	// end "if (channelSelection == kAllMenuItem)" 
				
	if (channelSelection == kSubsetMenuItem)
		{
				// Now set the rectify channels vector to be 0 for 		
				// channel not to be rectified and 1 for channel to be 	
				// rectified.															
		
		rectifyIndex = rectifyImageOptionsPtr->numberChannelsToRectify - 1;
		for (index = reformatOptionsPtr->numberChannels-1;
				index >= 0; 
					index--)
			{
			if (rectifyChannelPtr[rectifyIndex] == index)
				{
				rectifyChannelPtr[index] = 1;
				
				if (rectifyIndex > 0)
					rectifyIndex--;
				
				}	// end "if (rectifyChannelPtr... == index)" 
				
			else	// rectifyChannelPtr[rectifyIndex] != index 
				rectifyChannelPtr[index] = 0;
				
			}	// end "for (index=...->numberChannels-1; ..." 
			
		}	// end "if (channelSelection == kSubsetMenuItem)" 
		
			// Get the file information for the selected channels.  The	
			// information will be loaded into the window information	
			// structure.																
		
	GetFileInformationForChannelList (windowInfoPtr,
													layerInfoPtr, 
													fileInfoPtr,
													NULL, 
													(SInt16)reformatOptionsPtr->numberChannels);
	
			// Set the flag indicating byte count per data value to force the data to.								
			
	reformatOptionsPtr->forceByteCode = kDoNotForceBytes;
	if (windowInfoPtr->localMaxNumberBytes == 2 && 
														windowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForce2Bytes;
		
	else if (windowInfoPtr->localMaxNumberBytes == 4 && 
														windowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForce4Bytes;
		
	else if (windowInfoPtr->localMaxNumberBytes == 8 && 
														windowInfoPtr->localBytesDifferFlag)
		reformatOptionsPtr->forceByteCode = kForceReal8Bytes;
		
	if (rectifyImageOptionsPtr->procedureCode == kReprojectToReferenceImage)
		{
				// Save the selected area for the input image.
				
		reformatOptionsPtr->startColumn = reformatOptionsPtr->columnStart;
		reformatOptionsPtr->stopColumn = reformatOptionsPtr->columnEnd;
		reformatOptionsPtr->startLine = reformatOptionsPtr->lineStart;
		reformatOptionsPtr->stopLine = reformatOptionsPtr->lineEnd;
		
				// Set the area for the output image to represent the size of the 
				// reference image.
		
		reformatOptionsPtr->columnStart = 1;
		reformatOptionsPtr->columnEnd = GetMaxNumberColumns (
											rectifyImageOptionsPtr->referenceWindowInfoHandle);
		reformatOptionsPtr->lineStart = 1;
		reformatOptionsPtr->lineEnd = GetMaxNumberLines (
											rectifyImageOptionsPtr->referenceWindowInfoHandle);
	
		rectifyImageOptionsPtr->columnScaleFactor = 1.0;
		rectifyImageOptionsPtr->lineScaleFactor = 1.0;
		rectifyImageOptionsPtr->rotationAngle = 0.0;
		rectifyImageOptionsPtr->lineShift = 0;
		rectifyImageOptionsPtr->columnShift = 0;
		
		rectifyImageOptionsPtr->resampleCode = resampleCode;
		
		}	// end "if (...->procedureCode == kReprojectToReferenceImage)"
	
}	// end "RectifyImageDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RectifyImageDialogOnRectifyCode
//
//	Software purpose:	This routine handles showing and hiding options relative to
//							the rectification code that the user has selected.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/24/2007
//	Revised By:			Larry L. Biehl			Date: 07/10/2015

void RectifyImageDialogOnRectifyCode (
				DialogPtr							dialogPtr,
				SInt16								rectifyCode,
				Boolean								blankOutsideSelectedAreaFlag,
				double								mapOrientationAngle)

{		
	SetDLogControl (dialogPtr, 
							IDC_NonSelectedPixels, 
							blankOutsideSelectedAreaFlag);
																				
	if (rectifyCode == kTranslateScaleRotate)
		{
		ShowDialogItem (dialogPtr, IDC_TranslateScaleTitle);
		#if defined multispec_wx
			ShowDialogItem (dialogPtr, IDC_TranslateScaleTitle2);
			ShowDialogItem (dialogPtr, IDC_TranslateScaleTitle3);
		#endif	// defined multispec_wx
		ShowDialogItem (dialogPtr, IDC_LineTranslateScalePrompt);
		ShowDialogItem (dialogPtr, IDC_ColumnTranslateScalePrompt);
		ShowDialogItem (dialogPtr, IDC_LineOffset);
		ShowDialogItem (dialogPtr, IDC_ColumnOffset);
		ShowDialogItem (dialogPtr, IDC_LineScale);
		ShowDialogItem (dialogPtr, IDC_ColumnScale);
		ShowDialogItem (dialogPtr, IDC_RotationTitle);
		ShowDialogItem (dialogPtr, IDC_RotationClockwisePrompt);
		ShowDialogItem (dialogPtr, IDC_RotationClockwise);
		
		if (mapOrientationAngle == 0)
			HideDialogItem (dialogPtr, IDC_UseMapOrientationAngle);
			
		else	// mapOrientationAngle != 0
			ShowDialogItem (dialogPtr, IDC_UseMapOrientationAngle);
		
		SetDLogControlHilite (dialogPtr, IDC_NonSelectedPixels, 0); 
		
		HideDialogItem (dialogPtr, IDC_ReferenceFileListPrompt);
		HideDialogItem (dialogPtr, IDC_ReferenceFileList);
		HideDialogItem (dialogPtr, IDC_ResampleMethodPrompt);
		HideDialogItem (dialogPtr, IDC_ResampleMethod);
		
		}	// end "if (listCount <= 0)"
		
	else	// rectifyCode != kTranslateScaleRotate
		{
		HideDialogItem (dialogPtr, IDC_TranslateScaleTitle);
		#if defined multispec_wx
			HideDialogItem (dialogPtr, IDC_TranslateScaleTitle2);
			HideDialogItem (dialogPtr, IDC_TranslateScaleTitle3);
		#endif	// defined multispec_wx
		HideDialogItem (dialogPtr, IDC_LineTranslateScalePrompt);
		HideDialogItem (dialogPtr, IDC_ColumnTranslateScalePrompt);
		HideDialogItem (dialogPtr, IDC_LineOffset);
		HideDialogItem (dialogPtr, IDC_ColumnOffset);
		HideDialogItem (dialogPtr, IDC_LineScale);
		HideDialogItem (dialogPtr, IDC_ColumnScale);
		HideDialogItem (dialogPtr, IDC_RotationTitle);
		HideDialogItem (dialogPtr, IDC_RotationClockwisePrompt);
		HideDialogItem (dialogPtr, IDC_RotationClockwise);
		HideDialogItem (dialogPtr, IDC_UseMapOrientationAngle);
		
		SetDLogControlHilite (dialogPtr, IDC_NonSelectedPixels, 255); 
		
		ShowDialogItem (dialogPtr, IDC_ReferenceFileListPrompt);
		ShowDialogItem (dialogPtr, IDC_ReferenceFileList);
		ShowDialogItem (dialogPtr, IDC_ResampleMethodPrompt);
		ShowDialogItem (dialogPtr, IDC_ResampleMethod);
		
		}	// else "else rectifyCode != kTranslateScaleRotate"
	
}	// end "RectifyImageDialogOnRectifyCode"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 RectifyImageDialogOnReferenceFile
//
//	Software purpose:	The purpose of this routine is to handle a user change in the 
//							target file dialog selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:			RectifyImageDialog
//
//	Coded By:			Larry L. Biehl			Date: 02/20/2007
//	Revised By:			Larry L. Biehl			Date: 11/08/2019
	                
SInt16 RectifyImageDialogOnReferenceFile (
				DialogPtr							dialogPtr,
				SInt16								procedureCode,
				SInt16								fileNamesSelection,
				Handle*								referenceWindowInfoHandlePtr,
				DialogSelectArea*					dialogSelectAreaPtr)
	
{
	#if defined multispec_win                                
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win
   #if defined multispec_wx                                
		wxChoice* 							referenceListCtrl;
	#endif	// defined multispec_wx
	
	UInt32								windowIndex;
	
	  
	if (procedureCode == kTranslateScaleRotate)
		{
		*referenceWindowInfoHandlePtr = gActiveImageWindowInfoH;

		}	// end "if (procedureCode == kTranslateScaleRotate)" 
		
	else	// procedureCode != kTranslateScaleRotate 
		{
		if (fileNamesSelection > 0)
			{
			#if defined multispec_mac  
				windowIndex = kImageWindowStart + fileNamesSelection - 1;
			#endif	// defined multispec_mac   

			#if defined multispec_win  
				comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_ReferenceFileList);
				windowIndex = (UInt32)comboBoxPtr->GetItemData (fileNamesSelection - 1);
			#endif	// defined multispec_win 
			
         #if defined multispec_wx
				referenceListCtrl =
								(wxChoice*)wxWindow::FindWindowById (IDC_ReferenceFileList);
				SInt64 windowIndex64 =
						(SInt64)((int*)referenceListCtrl->GetClientData (
																				fileNamesSelection - 1));
				windowIndex = (UInt32)windowIndex64;
			#endif  // defined multispec_wx
					
			*referenceWindowInfoHandlePtr =
													GetWindowInfoHandle (gWindowList[windowIndex]);
			
			}	// end "if (fileNamesSelection > 0)"
			
		else	// fileNamesSelection <= 0
				// This is an error.  For now use the active image window.
			*referenceWindowInfoHandlePtr = gActiveImageWindowInfoH;
			
		}	// end "else procedureCode != kTranslateScaleRotate"
	/*			
			// Decided to use the input line and column display in the dialog to always
			// represent that to be used is that is selected in the active image window
			// not that selected in the reference image.  It seems more intuitive this
			// way.
			// Use the selection information from the reference image.
			
	dialogSelectAreaPtr->windowPtr = GetWindowPtr (*referenceWindowInfoHandlePtr);
	dialogSelectAreaPtr->adjustToBaseImageFlag = kDontAdjustToBaseImage;

	dialogSelectAreaPtr->imageWindowInfoPtr = 
				(WindowInfoPtr)GetHandleStatusAndPointer (*referenceWindowInfoHandlePtr,
																		&handleStatus);	
		
			// Get any selected area for the target image.																	
	
	if (!GetSelectedAreaInfo (dialogSelectAreaPtr->windowPtr,
										dialogSelectAreaPtr->imageWindowInfoPtr,
										(SInt32*)&dialogSelectAreaPtr->lineStart,
										(SInt32*)&dialogSelectAreaPtr->lineEnd,
										(SInt32*)&dialogSelectAreaPtr->columnStart,
										(SInt32*)&dialogSelectAreaPtr->columnEnd,
										kDontClearSelectionArea,
										kUseThreshold,
										dialogSelectAreaPtr->adjustToBaseImageFlag))
   	{
		dialogSelectAreaPtr->lineStart = 1;
		dialogSelectAreaPtr->lineEnd = 
										GetMaxNumberLines (*referenceWindowInfoHandlePtr);
		dialogSelectAreaPtr->columnStart = 1;
		dialogSelectAreaPtr->columnEnd = 
										GetMaxNumberColumns (*referenceWindowInfoHandlePtr);
							
		}	// end "if (!GetSelectedAreaInfo (..."

	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	
	MHSetState (*referenceWindowInfoHandlePtr, handleStatus);
	
	dialogSelectAreaPtr->imageWindowInfoPtr = NULL;
	
	#if defined multispec_mac 
		InvalWindowRect (
						GetDialogWindow (dialogPtr), &dialogSelectAreaPtr->toSelectionBox);
	#endif	// defined multispec_mac   
	*/	
	return (fileNamesSelection);
	
}	// end "RectifyImageDialogOnReferenceFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RectifyUpdateMapProjectionStructure
//
//	Software purpose:	The purpose of this routine is to update parameters in the map 
//							projection structure based on the input arguments.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/13/2005
//	Revised By:			Larry L. Biehl			Date: 03/28/2017

void RectifyUpdateMapProjectionStructure (
				Handle								inputWindowInfoHandle,
				FileInfoPtr							outFileInfoPtr,
				SInt32								columnStart, 
				SInt32								lineStart,
				SInt16								procedureCode,
				Handle								referenceWindowInfoHandle,
				SInt32								columnShift, 
				SInt32								lineShift,
				double								columnScaleFactor,
				double								lineScaleFactor,
				TransMapMatrix*					mapMatrixPtr,
				TransMapMatrix*					inverseMapMatrixPtr,
				double								rotationAngle)

{
	double								cosOrientAngle,
				  							doubleColumn,
				  							doubleLine,
				  							leftEdgeOfPixel11,
				  							sinOrientAngle,
				  							topEdgeOfPixel11,
											xOffset,
											yOffset;
											
	Handle								inputMapProjectionHandle,
											referenceFileInfoHandle;
									
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	SInt16								errCode;
	
	
	inputMapProjectionHandle = GetFileMapProjectionHandle2 (inputWindowInfoHandle);
	if (procedureCode == kReprojectToReferenceImage)
		inputMapProjectionHandle =
										GetFileMapProjectionHandle2 (referenceWindowInfoHandle);
	
	if (inputMapProjectionHandle != NULL && outFileInfoPtr != NULL)
		{
				// Put copy of the input map projection structure in the output
				// map projection structure.
				
		CopyMapProjectionHandle (inputMapProjectionHandle,
											&outFileInfoPtr->mapProjectionHandle);
											
		mapProjectionInfoPtr = (MapProjectionInfoPtr)
										GetHandlePointer (outFileInfoPtr->mapProjectionHandle);
     
		if (mapProjectionInfoPtr != NULL)
     		{
			/*
					This is just for some special processing. Take out or comment out 
					when done.
									
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = 
																				gCentralLongitudeSetting;
			gCentralLongitudeSetting += 15;
			if (gCentralLongitudeSetting > 180)
				gCentralLongitudeSetting -= 360;
			*/
			if (procedureCode == kTranslateScaleRotate)
				{
	     		if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0 &&
	     																				rotationAngle == 0)
		     		{
		     				// Take care of the area being a selected portion of the entire
							// image. Note that the column/line shift is relative to the new
							// pixels size.
		     				                                                                   
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 
							(columnStart - 1) *
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
		                                                                           
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 
							(lineStart - 1)  *
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
										
					leftEdgeOfPixel11 =
							mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 -
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize/2;
										
					topEdgeOfPixel11 =
							mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 +
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize/2;
					
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize /= 
																						columnScaleFactor;
					
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize /= 
																						lineScaleFactor;
					
							// Now take into account the new center of pixel because of the 
							// scale change.  We are assuming that any translation of the
							// pixels are relative to the fixed map coordinates for the image.
									
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = 
							leftEdgeOfPixel11 +
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize/2;
									
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = 
							topEdgeOfPixel11 -
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize/2;
					/*
					if (columnScaleFactor < 1)
						mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +=
							(-0.5 - columnShift) *
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
					
					if (lineScaleFactor < 1)
						mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -=
							(-0.5 - lineShift) *
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
					
					if (columnScaleFactor > 1)
						mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +=
							(0.5 - columnShift) * /
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
					
					if (lineScaleFactor > 1)
						mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -=
							(0.5 - lineShift) * /
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
		     		*/
		     		}	// end "if (...mapOrientationAngle == 0 && rotationAngle == 0)"
		     		
		     	else	// ...mapOrientationAngle != 0 || rotationAngle != 0
		     		{	 	     				
					if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0 &&
																						rotationAngle != 0)
						{
								// Update the control points.
								
						MapControlPoints (outFileInfoPtr, mapMatrixPtr);
						
						}	// end "...->planarCoordinate.polynomialOrder > 0 &&"
						
							// Determine the input line/column needed for the upper left
							// pixel in the new image to be created taking the image
							// rotation angle into account.
					
					doubleColumn = 1;
					doubleLine = 1;		
					MapLineColumn (inverseMapMatrixPtr, &doubleLine, &doubleColumn);
					
					cosOrientAngle = 
							cos (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
					sinOrientAngle = 
							sin (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
							
					xOffset = (doubleColumn - 1) * 
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
							
					yOffset = -(doubleLine - 1) * 
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
																													 
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 
												xOffset*cosOrientAngle + yOffset*sinOrientAngle;
																											
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 
												xOffset*sinOrientAngle - yOffset*cosOrientAngle;
					
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize /= 
																						columnScaleFactor;
					
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize /= 
																						lineScaleFactor;
					
					rotationAngle *= kDegreesToRadians;
					mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle -=
																							rotationAngle;
					if (fabs (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle) < 
																										0.00001)
						mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = 0.;
							
		     		}	// end "else ...mapOrientationAngle != 0"
		     		
		     	}	// end "if (procedureCode == kTranslateScaleRotate)"
		     	
		   else	// procedureCode == kReprojectToReferenceImage
		   	{
						// If the rectify procedure is to rectify to a reference image, we
						// need to be sure that any control points for the output image 
						// reflects those for the reference image.
							
				if (outFileInfoPtr->controlPointsHandle != 
															gImageFileInfoPtr->controlPointsHandle)
														
					UnlockAndDispose (outFileInfoPtr->controlPointsHandle);
				
				referenceFileInfoHandle = GetFileInfoHandle (referenceWindowInfoHandle);

				outFileInfoPtr->controlPointsHandle = 
												GetControlPointsHandle (referenceFileInfoHandle);
											
				if (outFileInfoPtr->controlPointsHandle != NULL)
					{
					errCode = HandToHand (&outFileInfoPtr->controlPointsHandle);
					if (errCode != noErr)
						outFileInfoPtr->controlPointsHandle = NULL;
				
					}	// end "if (outFileInfoPtr->controlPointsHandle != NULL)"
		   	
		   	}	// end "else procedureCode == kReprojectToReferenceImage"
			/*
			if (rightToLeftFlag)
				{
				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +=
						(outFileInfoPtr->numberColumns - 1) *
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
				
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
								-mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
							
				}	// end "if (rightToLeftFlag)"
															
			if (bottomToTopFlag)
				{
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -=
						(outFileInfoPtr->numberLines - 1) *
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
									
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
									-mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
							
				}	// end "if (bottomToTopFlag)"
			*/
			}	// end "if (mapProjectionInfoPtr != NULL)"
			
		}	// end "if (inputMapProjectionHandle != NULL && outFileInfoPtr != NULL)"
				 										
}	// end "RectifyUpdateMapProjectionStructure"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReprojectImage
//
//	Software purpose:	This routine does the rectification of the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2006
//	Revised By:			Larry L. Biehl			Date: 07/16/2018

Boolean ReprojectImage (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{
			// Declare local variables & structures
			
	DoubleRect							boundingRect;
			
	double								backgroundValue;
												
	CMFileStream*						outFileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr,
											referenceMapProjectionInfoPtr;
											
	RectifyImageOptionsPtr			rectifyImageOptionsPtr;
	
	unsigned char						*ioOutBufferPtr,
					 						*columnOutBufferPtr,
					 						*savedOutBufferPtr;
	
	UInt32								*histogramVector;
					 					
	SInt16								//*ioIn2ByteBufferPtr,
											*rectifyChannelPtr;
	
	UInt16								*savedOutBuffer2BytePtr;
	
	unsigned char 						*ioOut1ByteBufferPtr,
								 			*ioIn1ByteBufferPtr;
								 			
	Handle								mapProjectionHandle,
											referenceMapProjectionHandle,
											referenceWindowInfoHandle,
											windowInfoHandle;
	
	SInt32								column,
											columnByteSkip,
											countInBytes,
											countOutBytes,
											inputColumn,
											inputLine,
											lastOutputWrittenLine,
											limitIoOutBytes,
											line,
											numberOutputColumns,
											outputPixelValue,
											percentComplete,
											preLineBytes,
											//rectifiedInputOffset,
											//rectifiedOutputOffset,
											totalIOOutBytes,
											writePosOff;
	
	UInt32								channelCount,
											columnEnd,
											columnStart,
											inOffsetBytes,
											inOffsetIncrement,
											lastLineRead,
											lineEnd,
											lineStart,
											numberBytes,
											numberColumnsChannels,
											numberOutputLines,
											outChannelByteIncrement,
											outNumberBytesPerLineAndChannel,
											outOffsetIncrement,
											rectifyBytesToMove,
											startColumn,
											startLine,
											stopColumn,
											stopLine,
											supportFileType;
	
	SInt16								errCode,
											GAIAPrelineString[7] = 
													  {0x0300, 0x0600, 0x0000, 0x0100,
														0x0000, 0x0000, 0x0000},
											lastPercentComplete,
											numberReadChannels,
											resampleCode;
												
	UInt16								forceByteCode,
											numberOutChannels;										
	
	Boolean								continueFlag,
											forceBISFlag;
	
		
			// Initialize local variables.													
			
	errCode = noErr;
	writePosOff = 0;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	forceByteCode = reformatOptionsPtr->forceByteCode;
	forceBISFlag = (outFileInfoPtr->bandInterleave == kBIS);
	preLineBytes = outFileInfoPtr->numberPreLineBytes;
	outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);
	histogramVector = NULL;
	
	rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
	rectifyChannelPtr = rectifyImageOptionsPtr->rectifyChannelPtr;
	backgroundValue = rectifyImageOptionsPtr->backgroundValue;
	resampleCode = rectifyImageOptionsPtr->resampleCode;
					
	ioOutBufferPtr = (unsigned char*)reformatOptionsPtr->ioOutBufferPtr;
				
	lastPercentComplete = -1;
	
	columnStart = reformatOptionsPtr->columnStart;
	columnEnd =  reformatOptionsPtr->columnEnd;
	lineStart = reformatOptionsPtr->lineStart;
	lineEnd =  reformatOptionsPtr->lineEnd;
	
	startColumn = reformatOptionsPtr->startColumn;
	stopColumn = gImageFileInfoPtr->numberColumns;
	startLine = reformatOptionsPtr->startLine;
	stopLine = gImageFileInfoPtr->numberLines;
		
	numberBytes = outFileInfoPtr->numberBytes;
															
	numberOutChannels = reformatOptionsPtr->numberChannels;
	numberOutputColumns = outFileInfoPtr->numberColumns;
	numberOutputLines = outFileInfoPtr->numberLines;
	
	numberReadChannels = reformatOptionsPtr->numberChannels;
	
			// Get handle to window information for the active image
			
	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	referenceWindowInfoHandle = rectifyImageOptionsPtr->referenceWindowInfoHandle;
		
	if (referenceWindowInfoHandle == NULL || windowInfoHandle == NULL)
																						return (FALSE);
			
			// Set up pointers to map structure information for the active image
			// window
	
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																		mapProjectionHandle, kLock);
	
	GetCoefficientsVectorPointers (mapProjectionInfoPtr);					
			
			// Set up pointers to map structure information for the reference image
			
	referenceMapProjectionHandle = 
								GetFileMapProjectionHandle2 (referenceWindowInfoHandle);
	
	referenceMapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
															referenceMapProjectionHandle, kLock);
	
	GetCoefficientsVectorPointers (referenceMapProjectionInfoPtr);
	
			// Write a header if requested.													
			
	continueFlag = WriteNewImageHeader (fileInfoPtr,
													outFileInfoPtr, 
													(char*)ioOutBufferPtr,
													NULL,
													kFromReformat,
													reformatOptionsPtr->outputFileCode, 
													reformatOptionsPtr->lineInterval,
													reformatOptionsPtr->columnInterval,
													kClassDisplay,
													kNoPaletteColorsDefined);
	
			// Return if there was an IO error
			
	if (continueFlag)
		{
				// Set up limit of output buffer to know when to write buffer to	
				// the disk file. Make certain which buffer is the output buffer.	
				
		countOutBytes = reformatOptionsPtr->countOutBytes;
		limitIoOutBytes = reformatOptionsPtr->ioOutBufferBytes - countOutBytes;
		totalIOOutBytes = 0;
		
				// Initialize a background line that will be copied from as			
				// needed.	
								
		InitializeLineToBackgroundValue (outFileInfoPtr, 
													(HUInt8Ptr)ioOutBufferPtr,
													countOutBytes,
													backgroundValue);		
													
				// Set up output working buffer pointer.  This pointer will	 change as
				// the output buffer fills up
		
		savedOutBufferPtr = &ioOutBufferPtr[countOutBytes];
		
				// Get bytes to skip each time through the column loop for the		
				// output buffer.																	
				
		columnByteSkip = 1;
		if (forceBISFlag)
			columnByteSkip = numberOutChannels;
		columnByteSkip *= numberBytes;
		
				// Get total number of input bytes for one channel.					
				// Make certain that this is not more than the number of bytes		
				// allowed for in the output file.											
				
		countInBytes = stopColumn - startColumn + 1;
		countInBytes *= numberBytes;
		
		countInBytes = MIN (countInBytes,
			(SInt32)(numberBytes * outFileInfoPtr->numberColumns));
						
				// Get increment to skip each time through the channel loop
				// for the input buffer.
				
		inOffsetIncrement = columnEnd - columnStart + 1;
		if (forceBISFlag || fileInfoPtr->bandInterleave == kBIS)
			inOffsetIncrement = 1;
		
				// Get count of bytes to skip each time through the channel			
				// loop for the input buffer.	
				
		inOffsetBytes = inOffsetIncrement * numberBytes;
		
				// Get count of bytes to skip each time through the channel			
				// loop for the output buffer.
										
		outChannelByteIncrement = (countOutBytes-preLineBytes)/numberOutChannels;
		
		if (outFileInfoPtr->bandInterleave == kBIS)
			outChannelByteIncrement /= numberOutputColumns;
		
		else if (outFileInfoPtr->bandInterleave == kBSQ)
			outChannelByteIncrement = 
										reformatOptionsPtr->ioOutBufferBytes/numberOutChannels;
						
				// Get increment to skip each time through the channel loop
				// for the output buffer. I.E. allow for the byte count of
				// the output data.
				// Note that this could  be cleaned up some.  outOffsetIncrement and
				// outChannelByteIncrement are really the same thing. One is in values 
				// and the other is in bytes.  They are used in two different places.
				
		outOffsetIncrement = outChannelByteIncrement/numberBytes;
		
				// Get the number of bytes per line (all columns) and one band 
				// of data.
		
		outNumberBytesPerLineAndChannel = (countOutBytes-preLineBytes)/numberOutChannels;
		if (outFileInfoPtr->bandInterleave == kBIS)
			outNumberBytesPerLineAndChannel /= numberOutputColumns;
	
		rectifyBytesToMove = (stopColumn - startColumn + 1) * numberBytes;
		rectifyBytesToMove = MIN (outNumberBytesPerLineAndChannel, rectifyBytesToMove);
		
				// Get the bounding latitude-longitude rectangle for the active
				// image window.
				// Note that as of 7/16/2018 this is not being used. Will leave in for
				// a while in case it is found it is needed again. This was used to try
				// to save time by catching when the nearest neighbor calculation did not
				// need to be done; but found this check caused problems by leaving out
				// pixels that could really be used. The bounding rectangle is based on
				// the upper left and lower right point. It is not guaranteed that
				// the top or bottom coordinated does not vary across the width. The
				// same for the left and right.
		
		//if (referenceMapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
		//																				kGeographicRSCode)
		//	{
			boundingRect.top = gImageWindowInfoPtr->boundingLatLongRectangle.top;
			boundingRect.left = gImageWindowInfoPtr->boundingLatLongRectangle.left;
			boundingRect.bottom = gImageWindowInfoPtr->boundingLatLongRectangle.bottom;
			boundingRect.right = gImageWindowInfoPtr->boundingLatLongRectangle.right;
			
		//	}	// end "...->gridCoordinate.referenceSystemCode == kGeographicRSCode"
			
		//else	// ...->gridCoordinate.referenceSystemCode != kGeographicRSCode
		//	{
		//	boundingRect.top = gImageWindowInfoPtr->boundingMapRectangle.top;
		//	boundingRect.left = gImageWindowInfoPtr->boundingMapRectangle.left;
		//	boundingRect.bottom = gImageWindowInfoPtr->boundingMapRectangle.bottom;
		//	boundingRect.right = gImageWindowInfoPtr->boundingMapRectangle.right;
			
		//	}	// end "else ...->gridCoordinate.referenceSystemCode != kGeographicRSCode"
					
				// Intialize the nextTime variable to indicate when the next 		
				// check should occur for a command-.										
				
		gNextTime = TickCount ();
		
				// Get the total number of samples a line of data.						
				// It will be used when adjusting the data if request.				
				
		numberColumnsChannels = 
								((SInt32)columnEnd - columnStart + 1) * numberReadChannels;
						
		if (resampleCode == kMajorityRule)
			{
					// Get histogram vector to be used.
					
			histogramVector = (UInt32*)MNewPointerClear (256*sizeof (UInt32));
			
			if (histogramVector == NULL)
				continueFlag = FALSE;
						
			}	// end "if (resampleCode == kMajorityRule)"
			
				// Load some of the File IO Instructions structure that pertain
				// to the specific area being used.
		
		if (continueFlag)		
			errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
															NULL,
															startLine,
															stopLine,
															1,
															columnStart,
															columnEnd,
															1,
															numberReadChannels,
															NULL,
															kDetermineSpecialBILFlag);		
												
		if (errCode != noErr)	
			continueFlag = FALSE;			
		
		line = 1;
		lastLineRead = -1;
		lastOutputWrittenLine = 0;

				// Turn spin cursor on

		gPresentCursor = kSpin;
		
		while (line <= (SInt32)numberOutputLines && continueFlag)
			{
			ioOut1ByteBufferPtr = savedOutBufferPtr;
			
					// Initialize output line to the background value.	
					
			if (outFileInfoPtr->bandInterleave == kBSQ)
				{
				channelCount = 0;
				while (channelCount<numberOutChannels)
					{	
					BlockMoveData (ioOutBufferPtr, 
										&ioOut1ByteBufferPtr[preLineBytes], 
										outNumberBytesPerLineAndChannel);
										
					ioOut1ByteBufferPtr += outChannelByteIncrement;
					channelCount++;
										
					}	// end "while (channelCount<numberOutChannels)"
					
				ioOut1ByteBufferPtr = savedOutBufferPtr;
					
				}	// end "if (outFileInfoPtr->bandInterleave == kBSQ)"
			
			else	// ...->bandInterleave != kBSQ	
				BlockMoveData (ioOutBufferPtr, 
									&ioOut1ByteBufferPtr[preLineBytes], 
									countOutBytes-preLineBytes);
		
					// Add the preline calibration bytes if any.  For now this is	
					// only handled for GAIA data.											
					
			if (preLineBytes > 0)
				{
				BlockMoveData (&GAIAPrelineString, ioOut1ByteBufferPtr, preLineBytes);
				ioOut1ByteBufferPtr += preLineBytes;
				
				}	// end "if (preLineBytes > 0)" 
				
			if (continueFlag)
				{
				columnOutBufferPtr = &savedOutBufferPtr[preLineBytes];
										
				for (column=1; column<=numberOutputColumns; column++)
					{
					if (resampleCode == kNearestNeighbor)
						{
								// Get the input line and column that matches the 			
								// output line and column.										
					
						ReprojectNearestNeighborLineColumn (
													referenceMapProjectionInfoPtr,
													mapProjectionInfoPtr,
													&boundingRect,
													line, 
													column, 
													&inputLine, 
													&inputColumn);
							
						if (inputLine >= (SInt32)lineStart && 
									inputColumn >= (SInt32)columnStart &&
										inputLine <= (SInt32)lineEnd &&
											inputColumn <= (SInt32)columnEnd)
							{
							if (inputLine != (SInt32)lastLineRead)
								{
										// Get all requested channels for line of 			
										// image data. Return if there is a file IO error.	
							 
								errCode = GetLineOfData (fileIOInstructionsPtr,
																	inputLine, 
																	columnStart,
																	columnEnd,
																	1,
																	gInputBufferPtr,  
																	gOutputBufferPtr);
																			
								if (errCode != noErr)		
									{
									continueFlag = FALSE;
																										break;
									
									}	// end "if (errCode != noErr)"	
																							
								lastLineRead = inputLine;
																							
								}	// end "if (inputLine != lastLineRead)" 
								
							channelCount = 0;
							inputColumn -= columnStart;
							
									// Adjust column index to allow for band interleave.
									
							if (forceBISFlag || fileInfoPtr->bandInterleave == kBIS)
								inputColumn *= numberReadChannels;
								
									// Set input buffer pointer.												
					
							ioIn1ByteBufferPtr = (unsigned char*)
														&gOutputBufferPtr[inputColumn*numberBytes];
							
									// Set output buffer pointers.								
			
							ioOut1ByteBufferPtr = columnOutBufferPtr;
						
							while (channelCount<numberOutChannels)
								{
								if (rectifyChannelPtr[channelCount])
									memcpy (ioOut1ByteBufferPtr,
												ioIn1ByteBufferPtr,
												numberBytes);
							
								channelCount++;
							
								ioIn1ByteBufferPtr += inOffsetBytes;
								ioOut1ByteBufferPtr += outChannelByteIncrement;
						
								}	// end "while (channelCount<..." 
								
							}	// end "else inputLine && inputColumn ... 
						
						}	// end "if (resampleCode == kNearestNeighbor)"
						
					else if (resampleCode == kMajorityRule)
						{
								// Note that this is for thematic images only.
								
						ReprojectWithMajorityRule (fileIOInstructionsPtr,
															referenceMapProjectionInfoPtr,
															mapProjectionInfoPtr,
															histogramVector,
															line, 
															column,
															fileInfoPtr->numberColumns,
															fileInfoPtr->numberLines,
															(SInt16)backgroundValue, 
															&outputPixelValue);
																
						if (numberBytes == 1)
							savedOutBufferPtr[column-1] = (UInt8)outputPixelValue;
																
						else if (numberBytes == 2)
							savedOutBuffer2BytePtr[column-1] = (UInt16)outputPixelValue;
						
						}	// end "else if (resampleCode == kMajorityRule)"
						
					columnOutBufferPtr += columnByteSkip;
			
							// Check if user wants to abort processing.							
							
					if (TickCount () >= gNextTime)
						{
						if (!CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
							continueFlag = FALSE;
							
						}	// end "if (TickCount () >= nextTime)" 
						
					}	// end "for (column=1; column<=numberOutputColumns; ..." 
					
				}	// end "if (continueFlag)" 
			
			if (outFileInfoPtr->bandInterleave == kBSQ)
				savedOutBufferPtr = &savedOutBufferPtr[outNumberBytesPerLineAndChannel];
			
			else	// outFileInfoPtr->bandInterleave != kBSQ
				savedOutBufferPtr = &savedOutBufferPtr[countOutBytes];
				
			savedOutBuffer2BytePtr = (UInt16*)savedOutBufferPtr;
	
					// Write line(s), channel(s) of data when needed.					
			
			totalIOOutBytes += countOutBytes;
			
			if (continueFlag && (totalIOOutBytes > limitIoOutBytes))
				{
				errCode = WriteOutputDataToFile (outFileInfoPtr,
																outFileStreamPtr,
																&ioOutBufferPtr [countOutBytes],
																reformatOptionsPtr->channelPtr,
																numberOutChannels,
																lastOutputWrittenLine,
																outNumberBytesPerLineAndChannel,
																numberOutputLines,
																outChannelByteIncrement,
																totalIOOutBytes,
																reformatOptionsPtr,
																1);
				
				totalIOOutBytes = 0;
				savedOutBufferPtr = (unsigned char*)&ioOutBufferPtr[countOutBytes];
				
				lastOutputWrittenLine = line;
																
				if (errCode != noErr)
					{
					continueFlag = FALSE;
																									break;
					
					}	// end "if (errCode != noErr)"
				
				}	// end "if (... && (totalIOOutBytes > limitIoOutBytes))" 
			
					// Update status dialog box.												
					
			percentComplete = 100 * line/numberOutputLines;
			if (percentComplete != lastPercentComplete)
				{
				LoadDItemValue (gStatusDialogPtr, 
										IDC_ShortStatusValue, 
										(SInt32)percentComplete);
				lastPercentComplete = (SInt16)percentComplete;
				
				}	// end "if (percentComplete != lastPercentComplete)" 
			
					// Adjust line.																
					
			line++;
			//inputLine++;
				
			}	// end "while (line < numberOutputLines && continueFlag)" 
			 
		CheckAndDisposePtr (histogramVector);
			
				// Flush output buffer if needed.											
		
		if (continueFlag && totalIOOutBytes > 0)
			{				
			errCode = WriteOutputDataToFile (outFileInfoPtr,
															outFileStreamPtr,
															&ioOutBufferPtr [countOutBytes],
															reformatOptionsPtr->channelPtr,
															numberOutChannels,
															lastOutputWrittenLine,
															outNumberBytesPerLineAndChannel,
															numberOutputLines,
															outChannelByteIncrement,
															totalIOOutBytes,
															reformatOptionsPtr,
															1);
															
			if (errCode != noErr)	
				continueFlag = FALSE;							
			
			}	// end "if (continueFlag && (totalIOOutBytes > 0))" 
		
				// Close up any File IO Instructions structure that pertain to the 
				// specific area used for rectification.
				
		CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);

				// Turn spin cursor off
	
		gPresentCursor = kWait;
		
				// Write channel discriptions at the end of the file if needed.	
		
		if (continueFlag && reformatOptionsPtr->channelDescriptions)	
			WriteChannelDescriptionsAndValues (
									outFileInfoPtr, 
									NULL, 
									outFileInfoPtr->numberChannels,
									(reformatOptionsPtr->outputFileCode == kAppendToMenuItem));

		if (continueFlag && outFileInfoPtr->thematicType)
			{           
			supportFileType = kITRLFileType;
			if (outFileInfoPtr->format != kErdas74Type)
				supportFileType = kICLRFileType;
				
			//outFileInfoPtr->asciiSymbols = TRUE;
			                                
			continueFlag = CreateThematicSupportFile (
															outFileInfoPtr,
															NULL, 
															(UInt16)outFileInfoPtr->numberClasses, 
															NULL,
															0,
															NULL, 
															GetPaletteHandle (),
															NULL,		// newPaletteIndexPtr, 
															GetPaletteType (),
															(UInt16)outFileInfoPtr->numberClasses, 
															kFromDescriptionCode,
															kPaletteHistogramClassNames,
															kClassDisplay,
															kCollapseClass,
															supportFileType);
				
			}	// end "if (continueFlag && outFileInfoPtr->thematicType)" 
			
		}	// end "if (continueFlag)"
												
	CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
	CloseCoefficientsVectorPointers (referenceMapProjectionInfoPtr);
	
	CheckAndUnlockHandle (mapProjectionHandle);
	CheckAndUnlockHandle (referenceMapProjectionHandle);
		
	return (!continueFlag);
			
}	// end "ReprojectImage" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReprojectWithMajorityRule
//
//	Software purpose:	This routine maps the input line column in the reference image
//							to the same line-column (latitude-longitude) in the input image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/22/2012
//	Revised By:			Larry L. Biehl			Date: 05/24/2012

void ReprojectWithMajorityRule (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				referenceMapProjectionInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				UInt32*								histogramVector,
				SInt32								outputLine, 
				SInt32								outputColumn,
				SInt32								maxNumberInputColumns,
				SInt32								maxNumberInputLines,
				SInt16								backgroundValue,
				SInt32*								outputPixelValuePtr)
				
{	
	DoublePoint								mapPoint,
												llPoint,
												lrPoint,
												ulPoint,
												urPoint;
												
	DoubleRect								outputPixelInsideLimit;
												
	LongPoint								lineColumnPoint,
												llLineColumn,
												lrLineColumn,
												ulLineColumn,
												urLineColumn;
												
	double									horizontalCoordinates[4],
												halfHorizontalPixelSize,
												halfVerticalPixelSize,
												verticalCoordinates[4];
												
	FileInfoPtr								fileInfoPtr;
	UInt16*									output2ByteBufferPtr;
												
	SInt32									column,
												columnEnd,
												columnStart,
												line,
												lineEnd,
												lineStart,
												valueWithMaxCount;
												
	UInt32									index,
												maxCount;
												
	SInt16									errCode;
												
	Boolean									twoByteFlag,
												validPointFlag;
	
	
	valueWithMaxCount = backgroundValue;
	
	lineColumnPoint.h = outputColumn;
	lineColumnPoint.v = outputLine;
	
	ConvertLCPointToMapPoint (referenceMapProjectionInfoPtr, 
										&lineColumnPoint, 
										&mapPoint);
										
			// Find the vertices for the four corners of this pixel in
			// map units
			
	halfHorizontalPixelSize = 
				referenceMapProjectionInfoPtr->planarCoordinate.horizontalPixelSize/2;
	halfVerticalPixelSize = 
				referenceMapProjectionInfoPtr->planarCoordinate.verticalPixelSize/2;
			
	ulPoint.h = mapPoint.h - halfHorizontalPixelSize;
	ulPoint.v = mapPoint.v + halfVerticalPixelSize;
	
	urPoint.h = mapPoint.h + halfHorizontalPixelSize;
	urPoint.v = ulPoint.v;
	
	lrPoint.h = urPoint.h;
	lrPoint.v = mapPoint.v - halfVerticalPixelSize;
	
	llPoint.h = ulPoint.h;
	llPoint.v = lrPoint.v;
	
			// Find the vertices for the four corners of this pixel in
			// lat-long units.
	
	validPointFlag = ConvertMapPointToLatLongPoint (referenceMapProjectionInfoPtr,
																	&ulPoint);
	
	if (validPointFlag)
		validPointFlag = ConvertMapPointToLatLongPoint (
																	referenceMapProjectionInfoPtr,
																	&urPoint);
	
	if (validPointFlag)
		validPointFlag = ConvertMapPointToLatLongPoint (
																	referenceMapProjectionInfoPtr,
																	&lrPoint);
	
	if (validPointFlag)
		validPointFlag = ConvertMapPointToLatLongPoint (
																	referenceMapProjectionInfoPtr,
																	&llPoint);
	
	if (validPointFlag)
		{
				// Set up limits to use to check if input pixel is within the internal
				// rectangle that fits within the output pixel polygon.
				 
		outputPixelInsideLimit.left = MAX (ulPoint.h, llPoint.h);
		outputPixelInsideLimit.top = MIN (ulPoint.v, urPoint.v);
		outputPixelInsideLimit.right = MIN (urPoint.h, lrPoint.h);
		outputPixelInsideLimit.bottom = MAX (llPoint.v, lrPoint.v);
		
				// Set up vectors for checking (when needed) if input pixel is within
				// output pixel polygon
												
		horizontalCoordinates[0] = ulPoint.h;
		horizontalCoordinates[1] = urPoint.h;
		horizontalCoordinates[2] = lrPoint.h;
		horizontalCoordinates[3] = llPoint.h;
													
		verticalCoordinates[0] = ulPoint.v;
		verticalCoordinates[1] = urPoint.v;
		verticalCoordinates[2] = lrPoint.v;
		verticalCoordinates[3] = llPoint.v;
		
				// Now find the lines and columns in the target image that
				// these four vertices represent
			
		ConvertLatLongPointToMapPoint (mapProjectionInfoPtr,
													&ulPoint);
		
		ConvertMapPointToLC (mapProjectionInfoPtr, 
									&ulPoint,
									&ulLineColumn);
										
		ConvertLatLongPointToMapPoint (mapProjectionInfoPtr,
													&urPoint);
		
		ConvertMapPointToLC (mapProjectionInfoPtr, 
									&urPoint,
									&urLineColumn);
										
		ConvertLatLongPointToMapPoint (mapProjectionInfoPtr,
													&lrPoint);
		
		ConvertMapPointToLC (mapProjectionInfoPtr, 
									&lrPoint,
									&lrLineColumn);
										
		ConvertLatLongPointToMapPoint (mapProjectionInfoPtr,
													&llPoint);
		
		ConvertMapPointToLC (mapProjectionInfoPtr, 
									&llPoint,
									&llLineColumn);
										
				// Now find the columns and lines in the target image to check if they
				// are to be included in the input reference pixel.
				
		columnStart = MIN (ulLineColumn.h, llLineColumn.h);		
		columnEnd = MAX (urLineColumn.h, lrLineColumn.h);		
		lineStart = MIN (ulLineColumn.v, urLineColumn.v);		
		lineEnd = MAX (llLineColumn.v, lrLineColumn.v);
		
		if (columnStart <= maxNumberInputColumns && 
				columnEnd >= 1 &&
					lineStart <= maxNumberInputLines &&
						lineEnd >= 1)
			{		
			columnStart = MAX (columnStart, 1);
			columnEnd = MIN (columnEnd, maxNumberInputColumns);
			lineStart = MAX (lineStart, 1);
			lineEnd = MIN (lineEnd, maxNumberInputLines);
			
					// Now loop through these input pixels to see if they are included
					// within the output pixel
			
			errCode = noErr;
			
			fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
			twoByteFlag = FALSE;
			if (fileInfoPtr->numberBytes == 2)
				twoByteFlag = TRUE;
			
			output2ByteBufferPtr = (UInt16*)gOutputBufferPtr;
			for (line=lineStart; line<=lineEnd; line++)
				{
						// Get thematic data for this line of image data. 
						// Return if there is a file IO error.	
			 
				errCode = GetLineOfData (fileIOInstructionsPtr,
													line, 
													columnStart,
													columnEnd,
													1,
													gInputBufferPtr,  
													gOutputBufferPtr);
															
				if (errCode != noErr)
																									break;
									
				for (column=columnStart; column<=columnEnd; column++)
					{
					lineColumnPoint.h = column;
					lineColumnPoint.v = line;
		
					ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
														&lineColumnPoint, 
														&mapPoint);
														
					ConvertMapPointToLatLongPoint (mapProjectionInfoPtr,
																&mapPoint);
					
					if ((mapPoint.h > outputPixelInsideLimit.left &&
							mapPoint.h < outputPixelInsideLimit.right &&
								mapPoint.v < outputPixelInsideLimit.top &&
									mapPoint.v > outputPixelInsideLimit.bottom) ||
										(IsPointInPolygon (4,
																	horizontalCoordinates,
																	verticalCoordinates, 
																	mapPoint.h, 
																	mapPoint.v)))
						{
						if (twoByteFlag)
							histogramVector[output2ByteBufferPtr[column-columnStart]]++;
							
						else
							histogramVector[gOutputBufferPtr[column-columnStart]]++;
						
						}	// end "if (IsPointInPolygon (4, ..."
					
					}	// end "for (column=columnStart; column<=columnEnd; column++)"
				
				}	// end "for (line=lineStart; line<=lineEnd; line++)"
				
			if (errCode == noErr)
				{
						// Find the value with the highest count. While doing this
						// zero the vector out to be ready for the next pixel.
						
				maxCount = 0;
				for (index=0; index<fileInfoPtr->maxClassNumberValue; index++)
					{
					if (histogramVector[index] > maxCount)
						{
						maxCount = histogramVector[index];
						valueWithMaxCount = index;
						
						}	// end "if (histogramVector[index] > maxNumber)"
						
					histogramVector[index] = 0;
					
					}	// end "for (index=0; index<fileInfoPtr->maxClassNumberValue; ..."
									
				}	// end "if (errCode == noErr)"
				
			}	// end "if (columnStart <= maxNumberInputColumns && ..."
													
		}	// end "if (validPointFlag)"
		
	*outputPixelValuePtr = valueWithMaxCount;
		
}	// end "ReprojectWithMajorityRule" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReprojectNearestNeighborLineColumn
//
//	Software purpose:	This routine maps the input line column in the reference image
//							to the same line-column (latitude-longitude) in the input image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2006
//	Revised By:			Larry L. Biehl			Date: 07/16/2018

void ReprojectNearestNeighborLineColumn (
				MapProjectionInfoPtr				referenceMapProjectionInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				DoubleRect*							boundingRectPtr,
				SInt32								line, 
				SInt32								column, 
				SInt32*								inputLinePtr, 
				SInt32*								inputColumnPtr)
				
{
	DoublePoint							mapPoint,
											referenceMapPoint;
												
	LongPoint							inputLineColumn,
											lineColumnPoint;
												
	Boolean								validPointFlag;
	
	
	lineColumnPoint.h = column;
	lineColumnPoint.v = line;
	
	
	ConvertLCPointToMapPoint (referenceMapProjectionInfoPtr, 
										&lineColumnPoint, 
										&referenceMapPoint);
	
	mapPoint = referenceMapPoint;
	validPointFlag = ConvertMapPointToLatLongPoint (referenceMapProjectionInfoPtr,
																	&mapPoint);
	
	//if (referenceMapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
	//																					kGeographicRSCode)
	//	referenceMapPoint = mapPoint;
	/*
	if (validPointFlag &&
			(referenceMapPoint.h < boundingRectPtr->left ||
				referenceMapPoint.h > boundingRectPtr->right ||
					referenceMapPoint.v < boundingRectPtr->top ||
						referenceMapPoint.v > boundingRectPtr->bottom))
		validPointFlag = FALSE;
	
	*/
	if (validPointFlag)
		{
		ConvertLatLongPointToMapPoint (mapProjectionInfoPtr,
													&mapPoint);
		
		ConvertMapPointToLC (mapProjectionInfoPtr, 
										&mapPoint, 
										&inputLineColumn);
										
		*inputLinePtr = inputLineColumn.v;
		*inputColumnPtr = inputLineColumn.h;
			
		}	// end "if (validPointFlag)"
		
	else	// !validPointFlag
		{
				// Indicate that point is not valid.  Negative line and column
				// values indicate this.
										
		*inputLinePtr = -1;
		*inputColumnPtr = -1;
			
		}	// end "if (validPointFlag)"
		
}	// end "ReprojectNearestNeighborLineColumn" 



void ScaleMappingMatrix (
				TransMapMatrix*					mapMatrixPtr, 
				TransMapMatrix*					scaleMapMatrixPtr,
				double								columnScale,
				double								lineScale,
				double								aboutColumn,
				double								aboutLine)

{
	SetIdentityMappingMatrix (scaleMapMatrixPtr);
	scaleMapMatrixPtr->map[0][0] = columnScale;
	scaleMapMatrixPtr->map[1][1] = lineScale;
	scaleMapMatrixPtr->map[2][0] = aboutColumn - columnScale * aboutColumn;
	scaleMapMatrixPtr->map[2][1] = aboutLine - lineScale * aboutLine;

	ConcatenateMapping (mapMatrixPtr, scaleMapMatrixPtr);
	
}	// end "ScaleMappingMatrix" 



void SetIdentityMappingMatrix (
				TransMapMatrix*					mapMatrixPtr)

{
	mapMatrixPtr->map[0][0] = 1;
	mapMatrixPtr->map[0][1] = 0;
	mapMatrixPtr->map[1][0] = 0;
	mapMatrixPtr->map[1][1] = 1;
	mapMatrixPtr->map[2][0] = 0;
	mapMatrixPtr->map[2][1] = 0;
	
}	// end "SetIdentityMapping"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpResampleMethodPopupMenu
//
//	Software purpose:	The purpose of this routine is to set up the resample
//							method popup menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/07/2012
//	Revised By:			Larry L. Biehl			Date: 11/08/2019

void SetUpResampleMethodPopupMenu (
				DialogPtr							dialogPtr,
				MenuHandle							popUpResampleSelectionMenu,
				Boolean								thematicTypeFlag)

{	
	#if defined multispec_mac
		if (popUpResampleSelectionMenu != NULL)
			{
					// The mean resample method has not been initiated yet.
					
			DeleteMenuItem (popUpResampleSelectionMenu, 2);
			//DisableMenuItem (popUpResampleSelectionMenu, kMean);

			if (!thematicTypeFlag)
						// The majority rule is only for thematic images.
				DisableMenuItem (popUpResampleSelectionMenu, kMajorityRule);
		
			}	// end "if (popUpResampleSelectionMenu != NULL)"
	#endif	// defined multispec_mac
					
	#if defined multispec_win
		CComboBox* comboBoxPtr =
								(CComboBox*)(dialogPtr->GetDlgItem (IDC_ResampleMethod));

		comboBoxPtr->SetItemData (0, kNearestNeighbor);
			
				// The "mean" option is not implemented yet.
				
		comboBoxPtr->DeleteString (1);
		 
		if (thematicTypeFlag)
			comboBoxPtr->SetItemData (1, kMajorityRule);
			
		else	// !thematicTypeFlag
			comboBoxPtr->DeleteString (1);
	#endif	// defined multispec_win

	#if defined multispec_wx
		wxChoice* resampleCtrl =
								(wxChoice*)wxWindow::FindWindowById (IDC_ResampleMethod);
		
		resampleCtrl->Append ("Nearest Neighbor");

      int m_NearestNeighbor = kNearestNeighbor;
		resampleCtrl->SetClientData (0, (void*)(SInt64)m_NearestNeighbor);
		
		if (thematicTypeFlag)
			{
			int m_kMajorityRule = kMajorityRule;
			resampleCtrl->Append ("Majority");
			resampleCtrl->SetClientData (1, (void*)(SInt64)m_kMajorityRule);
			
			}	// end "if (thematicTypeFlag)"
   #endif	// defined multispec_wx
	
}	// end "SetUpResampleMethodPopupMenu" 

 
