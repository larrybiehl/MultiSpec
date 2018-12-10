//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SShapeToThematicFile.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/27/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//								
//	Brief description:	The purpose of the routines in this file is to convert the
//								requested ArcView shape file into a Thematic type file with 
//								0 for all non-selected pixels and the class number for all 
//								pixels that the shapes include.
//
//	Functions in file:	SInt16			ConvertPolygonShapeToClassNumber
//								Boolean 			ConvertShapeToClassNumber
//								Boolean			IsPointInPolygon
//								Boolean			ListShapeDBFFieldNames
//								void				LoadShapeToThematicClassNames
//								Boolean 			LoadShapeToThematicDialog
//								void 				LoadShapeToThematicInitialize
//								void 				LoadShapeToThematicDialogOK
//								Boolean 			LoadShapeToThematicSpecs
//								Boolean			PointInRectangle
//								SInt16			SetClassInMaskImage
//								void 				ShapeToThematicFileControl
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*  Following is template used for testing/debugging
	int numberChars = sprintf ((char*)gTextString3,
										" SShapeToThematicFile: (test): %d%s",
										test,
										gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif

#if defined multispec_mac 
#endif	// defined multispec_mac    
  
#if defined multispec_win 
#endif	// defined multispec_win 



extern void 		LoadShapeToThematicDialogInitialize (
							DialogPtr							dialogPtr,
							ReformatOptionsPtr				reformatOptionsPtr,
							DialogSelectArea*					dialogSelectAreaPtr,
							Boolean*								trainTypeFlagPtr,
							Boolean*								testTypeFlagPtr,
							SInt16*								classSelectionPtr,
							UInt16**								localClassPtrPtr,
							UInt32*								localNumberClassesPtr,
							Boolean*								includeHeaderFlagPtr);

extern void 		LoadShapeToThematicDialogOK (
							ReformatOptionsPtr				reformatOptionsPtr,
							DialogSelectArea*					dialogSelectAreaPtr,
							Boolean								trainTypeFlag,
							Boolean								testTypeFlag,
							SInt16								classSelection,
							UInt32								localNumberClasses,
							Boolean								includeHeaderFlag);
	 
#if defined multispec_win || defined multispec_lin
	#pragma pack(4)
#endif	// defined multispec_win
	
typedef struct ArcViewMultiPoint
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
//	SDoubleRect					box;
	DoubleRect					box;
	UInt32						numPoints; 
	ArcViewDoublePoint		points[1];
	
	} ArcViewMultiPoint, *ArcViewMultiPointPtr;
	 
	
typedef struct ArcViewPoint
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	ArcViewDoublePoint		point;
	
	} ArcViewPoint, *ArcViewPointPtr;
	 
	
typedef struct ArcViewPolyLine
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
//	SDoubleRect					box;
	DoubleRect					box;
	UInt32						numParts;
	UInt32						numPoints; 
	UInt32						parts[1];
	
	} ArcViewPolyLine, *ArcViewPolyLinePtr;
	 
	
typedef struct ArcViewRecordHeader
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	
	} ArcViewRecordHeader, *ArcViewRecordHeaderPtr;
	 
#if defined multispec_win || defined multispec_lin
	#pragma pack()
#endif	// defined multispec_win

				

		// Prototypes for routines in this file that are only called by			
		// other routines in this file.	
		
SInt16					ConvertPolygonShapeToClassNumber (
								ReformatOptionsPtr				reformatOptionsPtr,
								FileIOInstructionsPtr			fileIOInstructionsPtr,
								MapProjectionInfoPtr				mapProjectionInfoPtr,
								ShapeInfoPtr						shapeInfoPtr,
								Ptr									vectorDataPtr,
								CMFileStream*						outFileStreamPtr,
								Handle								windowInfoHandle,
								Boolean								maskInMemoryFlag);												
		
Boolean 					ConvertShapeToClassNumber (
								FileInfoPtr							outFileInfoPtr,
								ShapeInfoPtr						shapeInfoPtr,
								ReformatOptionsPtr				reformatOptionsPtr);
/*
UInt32					GetValueForRecordIndex (
								DBFHandle							dbfInfoPtr,
								UInt32								featureNumber,
								UInt32								recordIndex);
*/
Boolean					IsPointInPolygon (
								UInt32								nvert, 
								ArcViewDoublePoint*				vert,  
								double								testx, 
								double								testy);
/*
Boolean					IsPointInPolygon2 (
								UInt32								nvert, 
								ArcViewDoublePoint*				vert,  
								double								testx, 
								double								testy);
*/                       
Boolean					ListShapeDBFFieldNames (
								ShapeInfoPtr						shapeInfoPtr);
				
Boolean 					LoadShapeToThematicSpecs (
								Handle*								reformatOptionsHPtr);

Boolean 					LoadShapeToThematicDialog (
								ReformatOptionsPtr				reformatOptionsPtr);		

Boolean					PointInRectangle (
								DoublePoint*						doublePointPtr,
								DoubleRect*							boundingRectanglePtr);

SInt16					SetClassInMaskImage (
								FileIOInstructionsPtr			fileIOInstructionsPtr,
								HUCharPtr				 			ioOutBufferPtr,
								Boolean								maskInMemoryFlag,
								UInt32*								currentLinePtr,
								Boolean*								lineToBeWrittenFlagPtr,
								UInt32								line, 
								UInt32								column, 
								UInt32								classNumber);
								
		// Quick fix for now to allow a parameter setting to be shared across more
		// than one routine.
		
Boolean c_phenoSorgProjectFlag = false;


/* Code found on the web to determine if point is within polygon
// http://alienryderflex.com/polygon/
//  Globals which should be set before calling this function:
//
//  int    polySides  =  how many corners the polygon has
//  float  polyX[]    =  horizontal coordinates of corners
//  float  polyY[]    =  vertical coordinates of corners
//  float  x, y       =  point to be tested
//
//  (Globals are used in this example for purposes of speed.  Change as
//  desired.)
//
//  The function will return YES if the point x,y is inside the polygon, or
//  NO if it is not.  If the point is exactly on the edge of the polygon,
//  then the function may return YES or NO.
//
//  Note that division by zero is avoided because the division is protected
//  by the "if" clause which surrounds it.

bool IsPointInPolygon2 (
				UInt32								nvert, 
				ArcViewDoublePoint*				vert,  
				double								testx, 
				double								testy) 
				
{
	int   i, j=polySides-1;
	bool  oddNodes = NO;

	for (i=0; i<polySides; i++) 
		{
		if ((polyY[i]< y && polyY[j]>=y
				|| polyY[j]< y && polyY[i]>=y)
				&& (polyX[i]<=x || polyX[j]<=x)) 
			{
			oddNodes ^= (polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x); 
			}
		
		j=i; 
	 
		}

  return oddNodes; 
  
}	// end "IsPointInPolygon2"



private bool IsPointInPolygon (
				PointF[]								polygon, 
				PointF								point)
				
{
	bool									isInside = false;
	
	
	for (int i=0, j=polygon.Length-1; i<polygon.Length; j=i++)
		{
		if (((polygon[i].Y > point.Y) != (polygon[j].Y > point.Y)) &&
									(point.X < (polygon[j].X - polygon[i].X) * 
											(point.Y - polygon[i].Y) / 
												(polygon[j].Y - polygon[i].Y) + polygon[i].X))
			{
			isInside = !isInside;
			}
			
		}	// end "for (int i=0, j=polygon.Length-1; i<polygon.Length; j=i++)"
		
	return isInside;

}	// end "IsPointInPolygon"
*/



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ConvertPolygonShapeToClassNumber
//
//	Software purpose:	The purpose of this routine is to convert all pixels within
//							the input polygon shape to class number in the output thematic
//							file.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			AreasToThematicFileControl in fieldsToThematicFile.c
//
//	Coded By:			Larry L. Biehl			Date: 04/11/2013
//	Revised By:			Larry L. Biehl			Date: 04/26/2016

SInt16 ConvertPolygonShapeToClassNumber (
				ReformatOptionsPtr				reformatOptionsPtr,
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				ShapeInfoPtr						shapeInfoPtr,
				Ptr									vectorDataPtr,
				CMFileStream*						outFileStreamPtr,
				Handle								windowInfoHandle,
				Boolean								maskInMemoryFlag)
			
{
	DoublePoint							mapPoint;
	
	LongPoint							lineColumnPoint;
	
	LongRect								lineColumnRect,
											tempLineColumnRect;
	
	ArcViewDoublePoint*				doublePointPtr;
	ArcViewPolyLinePtr				arcViewPolyLinePtr;
	
	UInt32								classNumber,
											column,
											currentLine,
											featureNumber,
											lastClassNumber,
											lastRecordIndex,
											line,
											lineCount,
											numberPoints,
											partIndex,
											pointIndex,
											pointStart,
											pointStop,
											recordIndex,
											vectorDataIndex;
											
	SInt16								errCode;
											
	Boolean								lineToBeWrittenFlag;


	errCode = noErr;
	    
			// Get the rectangle in lines and columns that includes all of the
			// shape file records.
									
	lineColumnRect.top = lineColumnRect.left = SInt32_MAX;
	lineColumnRect.bottom = lineColumnRect.right = 0 - 2147483647; // SInt32_MIN;
		
	vectorDataIndex = 0;
	featureNumber = 8;
	for (recordIndex=0; recordIndex<shapeInfoPtr->numberRecords; recordIndex++)
		{
		arcViewPolyLinePtr = (ArcViewPolyLinePtr)&vectorDataPtr[vectorDataIndex];
			
		ConvertMapRectToLCRect (windowInfoHandle, 
										&arcViewPolyLinePtr->box,
										&tempLineColumnRect, 
										1);
										
				// Note that since the vertical component in arcViewPolyLinePtr->box 
				// increase from bottom of box to the top of the box and line-column
				// rectangle goes the other way, the bottom and top need to be reversed.
										
		lineColumnRect.top = MIN (lineColumnRect.top, tempLineColumnRect.bottom);
		lineColumnRect.bottom = MAX (lineColumnRect.bottom, tempLineColumnRect.top);
		lineColumnRect.left = MIN (lineColumnRect.left, tempLineColumnRect.left);
		lineColumnRect.right = MAX (lineColumnRect.right, tempLineColumnRect.right);
				
		vectorDataIndex += arcViewPolyLinePtr->recordLength;
	
		if (recordIndex == shapeInfoPtr->numberRecords-1)
			{
			lineColumnRect.top = MAX (reformatOptionsPtr->lineStart, lineColumnRect.top);
			lineColumnRect.bottom =  MIN (
										reformatOptionsPtr->lineEnd, lineColumnRect.bottom);
			lineColumnRect.left = MAX (
										reformatOptionsPtr->columnStart, lineColumnRect.left);
			lineColumnRect.right = MIN (
										reformatOptionsPtr->columnEnd, lineColumnRect.right);
					
			}		// end "if (recordIndex == shapeInfoPtr->numberRecords-1)"

      }		// end "for (recordIndex=0; ..."
	
			// Load the total number of lines into the status dialog.				
				
	gNextStatusTime = TickCount ();
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status20, 
							lineColumnRect.bottom-lineColumnRect.top+1);
	lineCount = 0;
	
			// Initialize variable needed if entire thematic file is not in memory.
			
	currentLine = 0;
	lineToBeWrittenFlag = FALSE;
		
			// Now loop through these lines and columns and fill in the class numbers
			// looping through the polygons to see which one the line/column falls
			// within.
	
	for (line=(UInt32)lineColumnRect.top; line<=(UInt32)lineColumnRect.bottom; line++)
		{
		lineCount++;	
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
			CheckSomeEvents (updateMask);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}		// end "if (TickCount () >= gNextStatusTime)"
		
		lastRecordIndex = shapeInfoPtr->numberRecords + 1;
		lastClassNumber = 0;
		for (column=(UInt32)lineColumnRect.left; column<=(UInt32)lineColumnRect.right; column++)
			{
			lineColumnPoint.h = column;
			lineColumnPoint.v = line;
			
			ConvertLCPointToMapPoint (mapProjectionInfoPtr, 
												&lineColumnPoint, 
												&mapPoint);
												
			classNumber = 0;																	
			vectorDataIndex = 0;
			for (recordIndex=1; recordIndex<=shapeInfoPtr->numberRecords; recordIndex++)
				{
				arcViewPolyLinePtr = (ArcViewPolyLinePtr)&vectorDataPtr[vectorDataIndex];
					
				if (arcViewPolyLinePtr->shapeType == 3 || 
																arcViewPolyLinePtr->shapeType == 5)
					{
					if (PointInRectangle (&mapPoint, &arcViewPolyLinePtr->box))
						{
						pointIndex = 0;
						doublePointPtr = (ArcViewDoublePoint*)
									&arcViewPolyLinePtr->parts[arcViewPolyLinePtr->numParts];
						
						for (partIndex=0; 
								partIndex<arcViewPolyLinePtr->numParts; 
								partIndex++)
							{	
							pointStart = arcViewPolyLinePtr->parts[partIndex];
							
							if (partIndex+1 < arcViewPolyLinePtr->numParts)
								pointStop = arcViewPolyLinePtr->parts[partIndex+1];
							else		// partIndex+1 == areaViewPolyLinePtr->numParts
								pointStop = arcViewPolyLinePtr->numPoints;
								
							numberPoints = pointStop - pointStart;
																			
							if (IsPointInPolygon (numberPoints, 
															doublePointPtr, 
															mapPoint.h, 
															mapPoint.v))
								{
								//classNumber = arcViewPolyLinePtr->recordNumber;
								classNumber = recordIndex;
								/*
								if (recordIndex == lastRecordIndex)
									classNumber = lastClassNumber;
								
								else		// recordIndex != lastRecordIndex
									{
									classNumber = GetValueForRecordIndex (
																				shapeInfoPtr->dbfInfoPtr,
																				featureNumber,
																				recordIndex);
									lastRecordIndex = recordIndex;
									lastClassNumber = classNumber;
									
									}		// end "else recordIndex != lastRecordIndex"
								*/
								}		// end "if (IsPointInPolygon (numberPoints, ..."
															
							doublePointPtr += numberPoints;

							}		// end "for (partIndex=0; partIndex<..."
							
						}		// end "if (PointInRectangle (&mapPoint, ..."
						
					if (classNumber > 0)
						break;
						
					}		// end "if (arcViewPolyLinePtr->shapeType == 3 || ..."
				
				vectorDataIndex += arcViewPolyLinePtr->recordLength;
											
				}		// end "for (recordIndex=0; recordIndex<..."
				
			if (classNumber > 0)
				{
				errCode = SetClassInMaskImage (fileIOInstructionsPtr,
															reformatOptionsPtr->ioOutBufferPtr,
															maskInMemoryFlag,
															&currentLine,
															&lineToBeWrittenFlag,
															line-reformatOptionsPtr->lineStart,
															column-reformatOptionsPtr->columnStart,
															classNumber);
															
				if (errCode != noErr)
					break;
					
				}		// end "if (classNumber > 0)"
															
			}		// end "for (column=lineColumnRect.left; column<=..."
			
		if (errCode != noErr)
			break;
		
		}		// end "for (line=lineColumnRect.top; line<=lineColumnRect.bottom; ..."
		
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
		
	return (errCode);
		
}		// end "ConvertPolygonShapeToClassNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ConvertShapeToClassNumber
//
//	Software purpose:	The purpose of this routine is to convert all pixels
//							in the specified image file that belong to a selected
//							class to the respective class number.  The pixels in
//							the input image file will have been set to a background
//							value.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			AreasToThematicFileControl in fieldsToThematicFile.c
//
//	Coded By:			Larry L. Biehl			Date: 04/02/2001
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean ConvertShapeToClassNumber (
				FileInfoPtr							outFileInfoPtr,
				ShapeInfoPtr						shapeInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)
			
{
	DoubleRect							boundingWindowBox;
	LongRect								lineColRect;
	
	LongPoint							nextPoint;
	
	ArcViewPointPtr					arcViewPointPtr;
	ArcViewRecordHeaderPtr			arcViewRecordHeaderPtr;
	CMFileStream*						outFileStreamPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr = NULL;
	HUCharPtr				 			ioOutBufferPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	Ptr									vectorDataPtr;
	
	SInt64								writePosOff;
	
	Handle								mapProjectionHandle;
	
	UInt32								count,
											currentLine,
											recordIndex,
											shapeType = 0,
											vectorDataIndex;
											
	SInt16								errCode;
											
	UInt16								classNumber;
											
	Boolean								lineToBeWrittenFlag,
											maskInMemoryFlag,
											returnFlag = FALSE;
											
	
	vectorDataPtr = NULL;
	
	if (outFileInfoPtr != NULL && shapeInfoPtr != NULL) 
		vectorDataPtr = (Ptr)GetHandlePointer (shapeInfoPtr->vectorDataHandle,
																kLock);
								
	{
				// Set up status dialog.  Load in number of classes.						
					
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status5, 
								(SInt32)reformatOptionsPtr->numberClasses);
	
		maskInMemoryFlag = FALSE;
		if (reformatOptionsPtr->numberOutputBufferLines == outFileInfoPtr->numberLines)
			maskInMemoryFlag = TRUE;
			 
		SetUpGeneralFileIOInstructions (&gFileIOInstructions[0],
													NULL,	
													NULL,
													outFileInfoPtr,
													1,
													NULL,
													(HUCharPtr)reformatOptionsPtr->ioOutBufferPtr,
													(HUCharPtr)reformatOptionsPtr->ioOutBufferPtr,
													NULL,
													0,
													kDoNotPackData,
													kDoNotForceBISFormat,	
													kDoNotForceBytes,
													&fileIOInstructionsPtr);  
		
		SetUpFileIOInstructions (fileIOInstructionsPtr,
											NULL,
											reformatOptionsPtr->lineStart,
											reformatOptionsPtr->lineEnd,
											reformatOptionsPtr->lineInterval,
											reformatOptionsPtr->columnStart,
											reformatOptionsPtr->columnEnd,
											reformatOptionsPtr->columnInterval,
											1,
											NULL,
											kSetSpecialBILFlagFalse);	
											
				// Get pointer to the map projection information
			
		mapProjectionHandle = GetFileMapProjectionHandle2 (
															gImageWindowInfoPtr->windowInfoHandle);
	
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
													mapProjectionHandle, kLock);
		
			// Get the rectangle in map units that is going to be converted to
			// a raster thematic image file.
	
		lineColRect.top = reformatOptionsPtr->lineStart;
		lineColRect.left = reformatOptionsPtr->columnStart;
		lineColRect.bottom = reformatOptionsPtr->lineEnd;
		lineColRect.right = reformatOptionsPtr->columnEnd;

		ConvertLCRectToMapRect (gImageWindowInfoPtr->windowInfoHandle, 
											&lineColRect, 
											&boundingWindowBox);
				
		vectorDataIndex = 0;
		currentLine = 0;
		lineToBeWrittenFlag = FALSE;
		errCode = noErr;
		classNumber = 1;
		ioOutBufferPtr = (HUCharPtr)reformatOptionsPtr->ioOutBufferPtr;
		
				// Get the shape type for use later. Not the code will not have mix types 
				// as it is written now.
				
		arcViewRecordHeaderPtr = (ArcViewRecordHeaderPtr)vectorDataPtr;
		shapeType = arcViewRecordHeaderPtr->shapeType;

		outFileStreamPtr = GetFileStreamPointer (outFileInfoPtr);					
																
		for (recordIndex=0; 
					recordIndex<shapeInfoPtr->numberRecords;
							recordIndex++)
			{	
			arcViewRecordHeaderPtr = 
								(ArcViewRecordHeaderPtr)&vectorDataPtr[vectorDataIndex];
								
			switch (arcViewRecordHeaderPtr->shapeType)
				{			
      		case 1:	// Point shape
      			arcViewPointPtr = (ArcViewPointPtr)arcViewRecordHeaderPtr;
      			
      			if (PointInRectangle (&arcViewPointPtr->point, 
													&boundingWindowBox))
      				{
						ConvertMapPointToLC (mapProjectionInfoPtr, 
													&arcViewPointPtr->point, 
													&nextPoint);
													
						errCode = SetClassInMaskImage (
														fileIOInstructionsPtr,
														reformatOptionsPtr->ioOutBufferPtr,
														maskInMemoryFlag,
														&currentLine,
														&lineToBeWrittenFlag,
														nextPoint.v-reformatOptionsPtr->lineStart, 
														nextPoint.h-reformatOptionsPtr->columnStart, 
														classNumber);
					
						}		// end "if (PointInBox (&areaViewPolyLinePtr->box, ..."
						
      			break;
      			
      		case 3:	// PolyLine shape	
      		case 5:	// Polygon shape
					errCode = ConvertPolygonShapeToClassNumber (
																reformatOptionsPtr,
																fileIOInstructionsPtr,
																mapProjectionInfoPtr,
																shapeInfoPtr,
																vectorDataPtr,
																outFileStreamPtr,
																gImageWindowInfoPtr->windowInfoHandle,
																maskInMemoryFlag);
								
					recordIndex = shapeInfoPtr->numberRecords;
      			break;
      			
      		default:
      			break;
				
				}		// end "switch (areaViewRecordHeaderPtr->shapeType)"
				
			vectorDataIndex += arcViewRecordHeaderPtr->recordLength;
							
			if (errCode != noErr)
				break;
			
			}		// end "for (recordIndex=0; ..."
			
		CheckAndUnlockHandle (mapProjectionHandle);
		
		if (errCode == noErr)
			{			
			if (lineToBeWrittenFlag)
				{
						// Set output file pointer to correct position for 		
						// writing the output data.				
				
				writePosOff = outFileInfoPtr->numberHeaderBytes +
									(currentLine-1)*outFileInfoPtr->numberColumns;	
														
						// Set pointer to correct location for writing.			
			
				errCode = MSetMarker (outFileStreamPtr, 
												fsFromStart, 
												writePosOff,
												kErrorMessages);

						// Write buffer to appropriate location in the 			
						// output disk file.												
						
				ioOutBufferPtr = 
							(HUCharPtr)reformatOptionsPtr->ioOutBufferPtr;
				count = outFileInfoPtr->numberColumns;
				
				if (errCode == noErr)
					errCode = MWriteData (outFileStreamPtr,
													&count, 
													ioOutBufferPtr,
													kErrorMessages);
													
				lineToBeWrittenFlag = FALSE;
				maskInMemoryFlag = FALSE;
													
				}		// end "if (lineToBeWrittenFlag)"
			
			if (maskInMemoryFlag)
				{
				errCode = MSetMarker (outFileStreamPtr, 
												fsFromStart, 
												outFileInfoPtr->numberHeaderBytes,
												kErrorMessages);
												
				count = outFileInfoPtr->numberLines * outFileInfoPtr->numberColumns *
																			outFileInfoPtr->numberBytes;
				
				if (errCode == noErr)
					errCode = MWriteData (outFileStreamPtr,
													&count, 
													(HUCharPtr)reformatOptionsPtr->ioOutBufferPtr,
													kErrorMessages);
				
				}		// end "if (maskInMemoryFlag)"
				
			}		// end "if (errCode == noErr)"
		
		CheckAndUnlockHandle (shapeInfoPtr->vectorDataHandle);
		
		if (errCode == noErr)
			returnFlag = TRUE;
		
		}		// end "if (vectorDataPtr != NULL)"
		
	if (shapeType == 1)
		{
		reformatOptionsPtr->numberClasses = 2;
		outFileInfoPtr->numberClasses = reformatOptionsPtr->numberClasses;
		
		}		// end "if (shapeType == 1)"
	
	CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);	
	CloseUpGeneralFileIOInstructions (fileIOInstructionsPtr);
	
	return (returnFlag);
		
}		// end "ConvertShapeToClassNumber"


/*
// Currently not used; was done for a test.
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetValueForRecordIndex
//
//	Software purpose:	The purpose of this routine is to load the input class name
//							buffer with a default set of class names for a thematic image
//							generated by the conversion of a shape file to a thematic image.
//							The info in the record(s) of the dbf file will be used if
//							available.
//
//	Parameters in:
//
//	Parameters out:	None.
//
//	Value Returned:	None
//
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/24/2016
//	Revised By:			Larry L. Biehl			Date: 04/24/2016

UInt32 GetValueForRecordIndex (
				DBFHandle							dbfInfoPtr,
				UInt32								featureNumber,
				UInt32								recordIndex)

{
	#if include_gdal_capability
		char									fieldName[32];
	
		double								doubleValue;
	
		DBFFieldType						fieldType;
		
		int									fieldWidth,
												numberDecimals;
	#else
		Handle								dbfInfoPtr = NULL;
	#endif	// include_gdal_capability
	
	UInt32								classNumber;
	
	
	#if include_gdal_capability
				// Get type for field
		
		fieldType = DBFGetFieldInfo (dbfInfoPtr, featureNumber, fieldName, &fieldWidth, &numberDecimals);
	#endif	// include_gdal_capability
	
	classNumber = recordIndex;
	#if include_gdal_capability
		switch (fieldType)
			{
			case FTInteger:
				classNumber = DBFReadIntegerAttribute (
															dbfInfoPtr, recordIndex, featureNumber);
				break;
					
			case FTDouble:
				doubleValue = DBFReadDoubleAttribute (
															dbfInfoPtr, recordIndex, featureNumber);
				classNumber = (UInt32)doubleValue;
				break;
					
			default:
				break;
					
			}		// end "switch (fieldType)"
	#endif	// include_gdal_capability
	
	return (classNumber);

}		// end "GetValueForRecordIndex"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  c Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:			Larry L. Biehl			Date: 04/11/2013

Boolean IsPointInPolygon (
				UInt32								nvert, 
				ArcViewDoublePoint*				vert,  
				double								testx, 
				double								testy)

	{
	UInt32								i, 
											j;
	
	Boolean								inPolygonFlag = FALSE;
	
	
	for (i =0, j=nvert-1; i<nvert; j=i++) 
		{
		if (((vert[i].y>testy) != (vert[j].y>testy)) &&
			(testx < (vert[j].x-vert[i].x)*(testy-vert[i].y) / 
																(vert[j].y-vert[i].y) + vert[i].x))
			inPolygonFlag = !inPolygonFlag;
			
		}		// end "for (i = 0, j = nvert-1; i < nvert; j = i++)"
		
  return (inPolygonFlag);
		
}		// end "IsPointInPolygon"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean IsPointInPolygon2
//
//	Software purpose:	The purpose of this routine is determine if the input point
//							is within the polygon described by the input vertices.
//							The code comes from 
//							"http://alienryderflex.com/polygon/".
//
//	Parameters in:		
//  int    polySides  =  how many corners the polygon has
//  float  polyX[]    =  horizontal coordinates of corners
//  float  polyY[]    =  vertical coordinates of corners
//  float  x, y       =  point to be tested
//
//	Parameters out:	None
//
// Value Returned:	TRUE if point is within the polygon
//							FALSE if the point is not within the polygon

//  The function will return YES if the point x,y is inside the polygon, or
//  NO if it is not.  If the point is exactly on the edge of the polygon,
//  then the function may return YES or NO.
//
//  Note that division by zero is avoided because the division is protected
//  by the "if" clause which surrounds it.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/22/2012
//	Revised By:			Larry L. Biehl			Date: 04/11/2013

Boolean IsPointInPolygon2 (
				UInt32								nvert, 
				ArcViewDoublePoint*				vert,  
				double								testx, 
				double								testy) 
{
	int									i, 
											j=nvert-1;
	
	bool									oddNodes = FALSE;

	for (i=0; i<nvert; i++) 
		{
		if ((vert[i].y < testy && vert[j].y >= testy 
				|| vert[j].y < testy && vert[i].y >= testy)
					&& (vert[i].x <= testx || vert[j].y <= testx)) 
			{
			oddNodes ^= (vert[i].x+(testy-vert[i].y)/
											(vert[j].y-vert[i].y)*(vert[j].x-vert[i].x) < testx); 
			}
		
		j = i; 
	 
		}		// end "for (i=0; i<nvert; i++)"

  return oddNodes; 
}
*/



#if include_gdal_capability
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListShapeDBFFieldNames
//
//	Software purpose:	The purpose of this routine is to list the shape dbf field names
//							used as class name descriptions in the output .clr file if a
//							dbf file is used to generate those names.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/05/2013
//	Revised By:			Larry L. Biehl			Date: 04/27/2016
                       
Boolean ListShapeDBFFieldNames (
				ShapeInfoPtr						shapeInfoPtr)

{  
	char									fieldName[32],
											textString[128];
	
	DBFHandle							dbfInfoPtr = NULL;
	
	int									fieldWidth, 
											numberDecimals;
											
	UInt32								fieldNumber[3],
											numberFields,
											totalLength;
											
	SInt16								length;
	
	Boolean								continueFlag = TRUE;
	
															
	if (shapeInfoPtr->dbfInfoPtr != NULL)
		{
		dbfInfoPtr = shapeInfoPtr->dbfInfoPtr;			
		numberFields = DBFGetFieldCount (dbfInfoPtr);
		
		fieldNumber[0] = 0;
		fieldNumber[1] = 1;
		fieldNumber[2] = 2;
		
		if (c_phenoSorgProjectFlag)
			{
			fieldNumber[0] = 8;
			fieldNumber[1] = 9;
			fieldNumber[2] = 10;
					
			}		// end "if (c_phenoSorgProjectFlag)"
		
				// Get type for first field
				
		DBFGetFieldInfo (dbfInfoPtr, 
								fieldNumber[0], 
								fieldName, 
								&fieldWidth, 
								&numberDecimals);
		length = sprintf (textString,
								"%s    Shape dbf fields used for output support file: %s",
								gEndOfLine,
								fieldName);
		totalLength = length;

		if (numberFields > 1)
			{
			DBFGetFieldInfo (dbfInfoPtr, 
									fieldNumber[1], 
									fieldName, 
									&fieldWidth, 
									&numberDecimals);
			length = sprintf (&textString[totalLength],
								", _%s",
								fieldName);
			totalLength += length;
								
			}		// end "if (numberFields > 1)"
			
		if (numberFields > 2)
			{
			DBFGetFieldInfo (dbfInfoPtr, 
									fieldNumber[2], 
									fieldName, 
									&fieldWidth, 
									&numberDecimals);
			length = sprintf (&textString[totalLength],
								", _%s",
								fieldName);
			totalLength += length;
								
			}		// end "if (numberFields > 2)"
			
		length = sprintf (&textString[totalLength], "%s", gEndOfLine);
		totalLength += length;
						
		continueFlag = OutputString ((CMFileStream*)NULL,
												textString, 
												totalLength, 
												gOutputForce1Code,
												TRUE);
		
		}		// end "if (shapeInfoPtr->dbfInfoPtr != NULL)" 	
		
	return (continueFlag);
		
}		// end "ListShapeDBFFieldNames"
#endif	// include_gdal_capability



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadShapeToThematicClassNames
//
//	Software purpose:	The purpose of this routine is to load the input class name
//							buffer with a default set of class names for a thematic image
//							generated by the conversion of a shape file to a thematic image.
//							The info in the record(s) of the dbf file will be used if
//							available.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/05/2013
//	Revised By:			Larry L. Biehl			Date: 04/10/2017 
                       
void LoadShapeToThematicClassNames (
				ShapeInfoPtr						shapeInfoPtr,
				char*									classNameTablePtr,
				UInt32								numberClasses)

{  	
	ArcViewRecordHeaderPtr			arcViewRecordHeaderPtr = NULL;
   UInt32								fieldNumber[3];
	
	#if include_gdal_capability
		UInt8									fieldName[32],
												textString[128];
		
		double								doubleLabel;
	
		DBFFieldType						fieldType[3];
		
	
		DBFHandle							dbfInfoPtr = NULL;
	
		Ptr									charLabelPtr;
	
		int									fieldWidth, 
												integerLabel,
												numberDecimals[3];
		
		SInt32								numberFields;
		
		UInt32								field,
												fieldLimit;
	
		SInt16								length;
	#else
		Handle								dbfInfoPtr = NULL;
	#endif	// include_gdal_capability
	
	Ptr									vectorDataPtr;

	SInt32								numberRecords = 0;
	
	UInt32								identifier,
											index,
											nameLength,
											vectorDataIndex = 0;	
	
			
	vectorDataPtr = (Ptr)GetHandlePointer (shapeInfoPtr->vectorDataHandle,
														kLock);
		
	if (vectorDataPtr != NULL)
		arcViewRecordHeaderPtr = (ArcViewRecordHeaderPtr)&vectorDataPtr[vectorDataIndex];
		
	fieldNumber[0] = 0;
	fieldNumber[1] = 1;
	fieldNumber[2] = 2;
	
	#if include_gdal_capability									
		if (shapeInfoPtr->dbfInfoPtr != NULL && shapeInfoPtr->shapeType != 1)
			{
			dbfInfoPtr = shapeInfoPtr->dbfInfoPtr;
			numberRecords = DBFGetRecordCount (dbfInfoPtr);				
			numberFields = DBFGetFieldCount (dbfInfoPtr);
			fieldLimit = MIN (numberFields, 3);
			/*
			int numberChars = sprintf (
				(char*)gTextString3,
				" SShapeToThematicFile:LoadShapeToThematicClassNames (numberFields): %d%s",
				numberFields,
				gEndOfLine);
			ListString ((char*)gTextString3, numberChars, gOutputTextH);
			*/
					// Include code to determine if this is a shape file for the PhenoSorg 
					// project. It may very well change but do this for the initial version 
					// of the shape file for processing work on the project.
					// 4/28/2016.
					
			if (numberFields == 11)
				{
				c_phenoSorgProjectFlag = true;
				fieldType[0] = DBFGetFieldInfo (dbfInfoPtr, 
															8, 
															(char*)fieldName, 
															&fieldWidth, 
															&numberDecimals[0]);
				if (CompareStringsNoCase (fieldName, (UCharPtr)"Plot", 4))
					c_phenoSorgProjectFlag = false;
					
				if (c_phenoSorgProjectFlag)
					{
					fieldType[0] = DBFGetFieldInfo (dbfInfoPtr, 
																9, 
																(char*)fieldName, 
																&fieldWidth, 
																&numberDecimals[0]);
					if (CompareStringsNoCase (fieldName, (UCharPtr)"Ent", 3))
						c_phenoSorgProjectFlag = false;
					}
						
				if (c_phenoSorgProjectFlag)
					{
					fieldType[0] = DBFGetFieldInfo (dbfInfoPtr, 
																10, 
																(char*)fieldName, 
																&fieldWidth, 
																&numberDecimals[0]);
					if (CompareStringsNoCase (fieldName, (UCharPtr)"K", 1))
						c_phenoSorgProjectFlag = false;
					}
				
				if (c_phenoSorgProjectFlag)
					{
					fieldNumber[0] = 8;
					fieldNumber[1] = 9;
					fieldNumber[2] = 10;
					
					}		// end "if (c_phenoSorgProjectFlag)"
				
				}		// end "if (numberFields == 11)"
			
					// Get type for first field
					
			fieldType[0] = DBFGetFieldInfo (dbfInfoPtr, 
														fieldNumber[0], 
														(char*)fieldName, 
														&fieldWidth, 
														&numberDecimals[0]);

			if (fieldLimit > 1)
				fieldType[1] = DBFGetFieldInfo (dbfInfoPtr, 
															fieldNumber[1], 
															(char*)fieldName, 
															&fieldWidth, 
															&numberDecimals[1]);
				
			if (fieldLimit > 2)
				fieldType[2] = DBFGetFieldInfo (dbfInfoPtr, 
															fieldNumber[2], 
															(char*)fieldName, 
															&fieldWidth, 
															&numberDecimals[2]);
		
			}		// end "if (shapeInfoPtr->dbfInfoPtr != NULL && ...)"
	#endif	// include_gdal_capability
		
	for (index=1; index<=numberClasses; index++)
		{
		if (vectorDataPtr != NULL)
			{
			arcViewRecordHeaderPtr = 
									(ArcViewRecordHeaderPtr)&vectorDataPtr[vectorDataIndex];
			identifier = arcViewRecordHeaderPtr->recordNumber - 1; 
			vectorDataIndex += arcViewRecordHeaderPtr->recordLength;
			
			}		// end "if (vectorDataPtr != NULL)"
			
		else		// vectorDataPtr == NULL
			identifier = index - 1;
			
		if (dbfInfoPtr != NULL && identifier < (UInt32)numberRecords)
			{
			#if include_gdal_capability
				nameLength = 0;
				for (field=0; field<fieldLimit; field++)
					{
					length = 0;
					switch (fieldType[field])
						{
						case FTString:
							charLabelPtr = (char*)DBFReadStringAttribute (
																				shapeInfoPtr->dbfInfoPtr, 
																				identifier, 
																				fieldNumber[field]);
							length = sprintf ((char*)&textString[nameLength], 
													"%s", 
													charLabelPtr);
							break;
							
						case FTInteger:
							integerLabel = DBFReadIntegerAttribute (shapeInfoPtr->dbfInfoPtr, 
																					identifier, 
																					fieldNumber[field]);
							length = sprintf ((char*)&textString[nameLength], 
													"%d", 
													integerLabel);
							break;
						
						case FTDouble:
							doubleLabel = DBFReadDoubleAttribute (shapeInfoPtr->dbfInfoPtr, 
																				identifier, 
																				fieldNumber[field]);
							length = sprintf ((char*)&textString[nameLength], 
													"%.*f", numberDecimals[field], 
													doubleLabel);
							break;
						
						default:
							break;
						
						}		// end "switch (fieldType)"
						
					if (field == 0 || !c_phenoSorgProjectFlag)
						nameLength += length;
						
					if (c_phenoSorgProjectFlag)
						{
								// For PhenoSorg case	
						if (field == 1 && integerLabel > 0)
							{
							length = sprintf ((char*)&textString[nameLength], 
													"ent%d", 
													integerLabel);
							nameLength += length;
							}
							
						else if (field == 2 && integerLabel > 0)
							{
							length = sprintf ((char*)&textString[nameLength], 
													"k%d", 
													integerLabel);
							nameLength += length;
							}
							
						else if (field > 0 && integerLabel == 0)
							{
							field = fieldLimit;
									// Remove the '_'
							nameLength--;
							
							}		// end "else if (field > 0 && integerLabel == 0)"
							
						}		// end "if (c_phenoSorgProjectFlag)"
					
					if (nameLength < 25 && field < fieldLimit-1)
						{
						sprintf ((char*)&textString[nameLength], "_");
						nameLength++;
					
						}		// end "if (nameLength < 25 && field < fieldLimit-1)"
					
					if (nameLength >= 25 || field >= fieldLimit-1)
						field = fieldLimit;
						
					}		// end "for (field=0; field<fieldLimit; field++)"
					
				nameLength = MIN (nameLength, 31);
				textString[nameLength] = 0;
				nameLength = sprintf ((char*)classNameTablePtr, "%s", textString);
			#endif	// include_gdal_capability
			
			}		// end "if (dbfInfoPtr != NULL && identifier < numberRecords)"
			
		else if (shapeInfoPtr->shapeType == 1)
			{
			nameLength = sprintf ((char*)classNameTablePtr, "Class %d", (unsigned int)identifier);
			
			}		// end "else dbfInfoPtr == NULL || identifier >= numberRecords"
			
		else		// (dbfInfoPtr == NULL || identifier >= numberRecords) && ->shapeType != 1
			{
			nameLength = sprintf ((char*)classNameTablePtr, "Shape Record %d", (unsigned int)identifier);
			
			}		// end "else dbfInfoPtr == NULL || identifier >= numberRecords"
				
		classNameTablePtr[nameLength] = 0;
		classNameTablePtr += 32;
		
		}		// end "for (index=1; index<=numberClasses; index++)"
						
	CheckAndUnlockHandle (shapeInfoPtr->vectorDataHandle);
		
}		// end "LoadShapeToThematicClassNames"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadShapeToThematicDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal
//							dialog for confirming the specification for creating
//							a Thematic image file from the top most shape file.  The
//							modal dialog template is in the resource file.
//							This dialog has not been completed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/02/2001
//	Revised By:			Larry L. Biehl			Date: 12/16/2016	

Boolean LoadShapeToThematicDialog (
				ReformatOptionsPtr				reformatOptionsPtr)

{
	Boolean								returnFlag = TRUE;
				
#if defined multispec_mac							
	DialogSelectArea					dialogSelectArea;
	Rect									theBox;
	
	DialogPtr							dialogPtr;
	
	UInt16								*localClassPtr = NULL;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								theNum;
	
	UInt32								localNumberClasses;
	
	SInt16								itemHit,
											theType;
	
	Boolean								includeHeaderFlag,
											modalDone,
											testTypeFlag,
											trainTypeFlag;
	
																							return (TRUE);
			// Initialize local variables.													
			
	dialogPtr = NULL;
		
			// Get the modal dialog for the image display specification				
				
	dialogPtr = LoadRequestedDialog (kfieldsToImageID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																							return (FALSE);
	
			// Set Procedure pointers for those dialog items that need them.			
	
	SetDialogItemDrawRoutine (dialogPtr, 18, gDrawDialogClassPopUpPtr);
			
	SetDialogItemDrawRoutine (dialogPtr, 21, gHiliteOKButtonPtr);
	
			// Initialize dialog variables.

	LoadShapeToThematicDialogInitialize (dialogPtr,
														reformatOptionsPtr,
														&dialogSelectArea,
														&trainTypeFlag,
														&testTypeFlag,
														&gClassSelection,
														&localClassPtr,
														&localNumberClasses,
														&includeHeaderFlag);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Use training and/or testing fields.						
					
	SetDLogControl (dialogPtr, 15, trainTypeFlag);
	SetDLogControl (dialogPtr, 16, testTypeFlag);
	
			// Write ERDAS header record and trailer file.					
	
	SetDLogControl (dialogPtr, 20, includeHeaderFlag);
	
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kfieldsToImageID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 8, 0, INT16_MAX);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.			
					// If itemHit was a radio button make appropriate control		
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.										
					// For number value items, get the string and convert it to		
					// a number.																	

			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}		// end "if (theType == 16)" 
				
			switch (itemHit)
				{
				case 6:				// Entire area to selected area switch.			
				case 8:				//	 Display start line of image  
				case 9:				//	 Display end line of image  
				case 10:				//	 Display line interval of image  
				case 11:				//	 Display start column of image  
				case 12:				//	 Display end column of image  
				case 13:				//	 Display column interval of image  

					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 15:			// Check box for 'Use training fields/areas'.		
				case 16:			// Check box for 'Use testing fields/areas'.			
				case 20:			// Check box for 'Write ERDAS header record ...		
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
					
				case 18:		// Classes 
					itemHit = StandardPopUpMenu (dialogPtr, 
															17, 
															18, 
															gPopUpAllSubsetMenu, 
															gClassSelection, 
															kPopUpAllSubsetMenuID);
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of classes to be used.								
								
						itemHit = ClassDialog (
											&localNumberClasses,
											(SInt16*)localClassPtr, 
											1,
											gProjectInfoPtr->numberStatisticsClasses,
											gClassSelection,
											okHandle);
							
						}		// end "if (itemHit == kSubsetMenuItem)" 
					
					if (itemHit != 0)
						gClassSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
						
				}		// end "switch (itemHit)" 
				
			}		// end "if (itemHit > 2)" 
			
		else if (itemHit > 0) 	// and itemHit <= 2 
			{
					// If item hit is 1, check if line-column values make 			
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1)
				itemHit = CheckLineColValues (
										&dialogSelectArea,
										reformatOptionsPtr->lineStart, 
										reformatOptionsPtr->columnStart, 
										dialogPtr);
				
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;

				LoadShapeToThematicDialogOK (reformatOptionsPtr,
														&dialogSelectArea,
														(GetDLogControl (dialogPtr, 15) == 1),
														(GetDLogControl (dialogPtr, 16) == 1),
														gClassSelection,
														localNumberClasses,
														(GetDLogControl (dialogPtr, 20) == 1));
														
				}		// end "if	(itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;		
				returnFlag = FALSE;
				
				}		// end "if	(itemHit == 2)" 
				
			}		// end "else if (itemHit > 0) and itemHit <= 2" 
				
		} while (!modalDone);                    
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac

	#if defined multispec_win   
		/*	
		CMFieldsToThematicDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMFieldsToThematicDialog ();
			
			returnFlag = dialogPtr->DoDialog (reformatOptionsPtr); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL
		*/	
	#endif	// defined multispec_win  
	
	return (returnFlag);
	
}		// end "LoadShapeToThematicDialog"



void LoadShapeToThematicDialogInitialize (
				DialogPtr							dialogPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				Boolean*								trainTypeFlagPtr,
				Boolean*								testTypeFlagPtr,
				SInt16*								classSelectionPtr,
				UInt16**								localClassPtrPtr,
				UInt32*								localNumberClassesPtr,
				Boolean*								includeHeaderFlagPtr)

{	
			// Update the specification dialog with the information given in 		
			// the reformat structure.													
				
			// Initialize selected area structure.	
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											NULL,
											gProjectSelectionWindow,
											reformatOptionsPtr->columnStart,
											reformatOptionsPtr->columnEnd,
											reformatOptionsPtr->columnInterval,
											reformatOptionsPtr->lineStart,
											reformatOptionsPtr->lineEnd,
											reformatOptionsPtr->lineInterval,
											8,
											6,
											kAdjustToBaseImage);										
			
	dialogSelectAreaPtr->imageWindowInfoPtr = gImageWindowInfoPtr;
								
	#if defined multispec_mac 
		LoadLineColumnItems (dialogSelectAreaPtr, 
										dialogPtr, 
										kInitializeLineColumnValues, 
										kIntervalEditBoxesExist,
										1);
	#endif	// defined multispec_mac             
				
			// Display all or subset of classes/groups.
	
	*classSelectionPtr = reformatOptionsPtr->classSet;
	*localClassPtrPtr = (UInt16*)reformatOptionsPtr->classPtr;
	*localNumberClassesPtr = reformatOptionsPtr->numberClasses;
	
			// Write ERDAS header record and trailer file.
			
	*includeHeaderFlagPtr = (reformatOptionsPtr->headerFormat != 0);	
	
}		// end "LoadShapeToThematicDialogInitialize"



void LoadShapeToThematicDialogOK (
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				Boolean								trainTypeFlag,
				Boolean								testTypeFlag,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				Boolean								includeHeaderFlag)

{	
			// Selected reformat area.												
	
	reformatOptionsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	reformatOptionsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	reformatOptionsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	reformatOptionsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	reformatOptionsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	reformatOptionsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
	
			// Use training and/or testing fields/areas.	
			
	reformatOptionsPtr->fieldType = 0;
	if (trainTypeFlag)
		reformatOptionsPtr->fieldType += kTrainingType;
	if (testTypeFlag)
		reformatOptionsPtr->fieldType += kTestingType;
		
			// Classes				

	reformatOptionsPtr->classSet = classSelection;
	reformatOptionsPtr->numberClasses = localNumberClasses;
	if (classSelection == kAllMenuItem)		// All classes 
		LoadClassVector (&reformatOptionsPtr->numberClasses,
								reformatOptionsPtr->classPtr);

			// "Write Erdas Header" check box.						

	reformatOptionsPtr->headerFormat = 0;
	if (includeHeaderFlag)
	 	reformatOptionsPtr->headerFormat = kErdas74Type;
	
}		// end "LoadShapeToThematicDialogOK"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadShapeToThematicSpecs
//
//	Software purpose:	The purpose of this routine is to load the Reformat 
//							Specification structure with the default set of
//							parameters for converting project train/test area
//							to a thematic image file.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			AreasToThematicFileControl
//
//	Coded By:			Larry L. Biehl			Date: 04/02/2001
//	Revised By:			Larry L. Biehl			Date: 01/31/2013

Boolean LoadShapeToThematicSpecs (
				Handle*								reformatOptionsHPtr)

{
	FileInfoPtr							outFileInfoPtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	
	Handle								outFileInfoHandle;
	
	Boolean								continueFlag;

	
			// Check input parameters and any globals that will be used.	
																				
	outFileInfoHandle = NULL;
	reformatOptionsPtr = NULL;
	
			// Get a handle to a block of memory to be used for						
			// file information for the new image file.									
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
			
			// Get a handle to a block of memory to use for the reformatting		
			// options.  Then lock it and get a pointer to the block.				
		
	continueFlag = GetReformatAndFileInfoStructures (
													&outFileInfoHandle, reformatOptionsHPtr);
	
	if (continueFlag)
		{														
		reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
												*reformatOptionsHPtr, kLock);

		outFileInfoPtr = (FileInfoPtr)GetHandlePointer (
												outFileInfoHandle, kLock);
												
		reformatOptionsPtr->numberClasses = 1;
		reformatOptionsPtr->numberChannels = 1;
			
				// Initialize the format information for the output file.			
				
		outFileInfoPtr->bandInterleave		= kBIL;    			// kBIL  
		outFileInfoPtr->numberHeaderBytes 	= 128;
		outFileInfoPtr->numberTrailerBytes 	= 0;
		outFileInfoPtr->numberPreLineBytes 	= 0;
		outFileInfoPtr->numberPostLineBytes = 0;
		outFileInfoPtr->numberPreChannelBytes 	= 0;
		outFileInfoPtr->numberPostChannelBytes = 0;
		
		outFileInfoPtr->numberChannels 		= 1;
		outFileInfoPtr->numberClasses  		= 2;
										
		outFileInfoPtr->numberBytes  			= 1;
		outFileInfoPtr->numberBits 			= 8;
		if (outFileInfoPtr->numberClasses > 256)
			{
			outFileInfoPtr->numberBytes = 2;
			outFileInfoPtr->numberBits = 16;
			
			}		// end "if (outFileInfoPtr->numberClasses > 256)" 
			
		outFileInfoPtr->swapBytesFlag 		= FALSE;
		outFileInfoPtr->descriptionsFlag 	= FALSE;
		outFileInfoPtr->channelDescriptionH = NULL;
		outFileInfoPtr->classDescriptionH 	= NULL;
		outFileInfoPtr->thematicType 			= TRUE;
			
		}		// end "if (continueFlag)" 
	
			// If unable to set up information for reformat								
			// specifications, release memory allocated to it (if any).				
			
	else		// !continueFlag
		{
		ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL); 
		ReleaseReformatSpecsMemory (reformatOptionsHPtr);
		
		}		// end "else !continueFlag"
		
	return (continueFlag);

}		// end "LoadShapeToThematicSpecs" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean PointInRectangle
//
//	Software purpose:	The purpose of this routine is to determine if the input double
//							point is within the input rectangle.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/12/2013
//	Revised By:			Larry L. Biehl			Date: 04/12/2013			

Boolean PointInRectangle (
				DoublePoint*						doublePointPtr,
				DoubleRect*							boundingRectanglePtr)

{	
	if (doublePointPtr->v > boundingRectanglePtr->bottom ||
			doublePointPtr->v < boundingRectanglePtr->top ||
				doublePointPtr->h > boundingRectanglePtr->right ||
					doublePointPtr->h < boundingRectanglePtr->left)
		return (FALSE);
		
	return (TRUE);

}		// end "PointInRectangle"

 

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetClassInMaskImage
//
//	Software purpose:	The purpose of this routine is to set the input class number for
//							the input line and column in the mask image file. This will handle
//							case for entire image being loaded into memory and the case when
//							one needs to read and write each line of the mask image out. If
//							the latter case, the line will be written out only when a new line
//							needs to be used. Current working line is written and next line
//							is read in and the point in the line is set to the specified
//							class number.
//
//	Parameters in:		column is 0 based.
//							line is 0 based. Need to change to 1 based for mask file on disk operations.
//
//	Parameters out:	None
//
// Value Returned:	errCode from the disk file write operation.				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/12/2013
//	Revised By:			Larry L. Biehl			Date: 04/26/2016

SInt16 SetClassInMaskImage (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				HUCharPtr				 			ioOutBufferPtr,
				Boolean								maskInMemoryFlag,
				UInt32*								currentLinePtr,
				Boolean*								lineToBeWrittenFlagPtr,
				UInt32								line, 
				UInt32								column, 
				UInt32								classNumber)

{
	CMFileStream*						outFileStreamPtr;
	FileInfoPtr							outFileInfoPtr;
	UInt16*								ioOut2ByteBufferPtr;
	
	SInt64								writePosOff;
	
	UInt32								count,
											index;
	
	SInt16								errCode = noErr;
	
	
	outFileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	
			// Set classNumber to background if larger than max class number
	
	if (outFileInfoPtr->numberBytes == 1)
		{
		if (classNumber > 255)
			classNumber = 0;
		
		}		// end "if (outFileInfoPtr->numberBytes == 1)"
	
	else		// outFileInfoPtr->numberBytes == 2
		{
		ioOut2ByteBufferPtr = (UInt16*)ioOutBufferPtr;
		if (classNumber > UInt16_MAX)
			classNumber = 0;
		
		}		// end "else outFileInfoPtr->numberBytes == 2"
			
	if (maskInMemoryFlag)
		{
		index = line*outFileInfoPtr->numberColumns + column;
		
		if (outFileInfoPtr->numberBytes == 1)
			ioOutBufferPtr[index] = (UInt8)classNumber;
			
		else		// outFileInfoPtr->numberBytes != 1
			ioOut2ByteBufferPtr[index] = (UInt16)classNumber; 
		
		}		// end "if (maskInMemoryFlag)"
		
	else		// !maskInMemoryFlag
		{		
					// Change line to 1 based for disk based operations.
		line++;	
		if (*currentLinePtr != line)
			{
			outFileStreamPtr = fileIOInstructionsPtr->fileStreamPtr;
			
			if (*lineToBeWrittenFlagPtr)
				{
						// Set output file pointer to correct position for 		
						// writing the output data.				
				
				writePosOff = outFileInfoPtr->numberHeaderBytes +
					(*currentLinePtr-1)*outFileInfoPtr->numberColumns * 
																		outFileInfoPtr->numberBytes;
														
						// Set pointer to correct location for writing.			
			
				if (errCode == noErr)	
					errCode = MSetMarker (outFileStreamPtr, 
													fsFromStart, 
													writePosOff,
													kErrorMessages);
	
						// Write buffer to appropriate location in the 			
						// output disk file.												
						
				count = outFileInfoPtr->numberColumns * outFileInfoPtr->numberBytes;
				
				if (errCode == noErr)
					errCode = MWriteData (outFileStreamPtr, 
													&count, 
													ioOutBufferPtr,
													kErrorMessages);
													
				*lineToBeWrittenFlagPtr = FALSE;
													
				}		// end "if (lineToBeWrittenFlag)"
				
			if (errCode == noErr)
				errCode = GetLine (fileIOInstructionsPtr,
											outFileInfoPtr,
											line,
											0,
											1,
											outFileInfoPtr->numberColumns,
											&count,
											(HUCharPtr)ioOutBufferPtr);
							
			}		// end "if (currentLine != nextPoint.v)" 
		
		if (errCode == noErr)	
			{							
			if (outFileInfoPtr->numberBytes == 1)
				ioOutBufferPtr[column] = (UInt8)classNumber;
				
			else		// outFileInfoPtr->numberBytes != 1
				ioOut2ByteBufferPtr[column] = (UInt16)classNumber; 
			
			*currentLinePtr = line;	
			*lineToBeWrittenFlagPtr = TRUE;
			
			}		// end "if (errCode == noErr)"
		
		}		// end "!maskInMemoryFlag"
		
	return (errCode);
	
}		// end "SetClassInMaskImage"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							  (c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShapeToThematicFileControl
//
//	Software purpose:	The purpose of this routine is to control the 
//							conversion of the project training/test areas to a
//							Thematic image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/02/2001
//	Revised By:			Larry L. Biehl			Date: 12/08/2014	

void ShapeToThematicFileControl (void)

{
	FileInfoPtr							outFileInfoPtr;
	Handle*								shapeHandlePtr;
	ReformatOptionsPtr				reformatOptionsPtr;
	ShapeInfoPtr						shapeInfoPtr;
	
	Handle								reformatOptionsHandle,
											shapeInfoHandle;
	
	time_t								startTime;
	
	UInt32								supportFileType;
	
	SInt16								shapeFileIndex,
											shapeType;
	
	Boolean								continueFlag = TRUE;

	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
	
	shapeInfoHandle = NULL;
	shapeInfoPtr = NULL;
	
			// Get the shape file index for the window. For now we will use the
			// first one in the list.
			
	shapeFileIndex = abs (gImageWindowInfoPtr->overlayList[0].index) - 1;
																								
			// Get the reformat structure and initialize it.							
			
	if (!LoadShapeToThematicSpecs (&reformatOptionsHandle))
																									return;
																							
			// Verify that the shape file index makes sense.
			
	if (shapeFileIndex < 0 || shapeFileIndex >= (SInt16)gShapeHandleListLength)
																									return;
	
			// Note that the following handles were locked in the call to
			// 'LoadShapeToThematicSpecs'
			
	reformatOptionsPtr = (ReformatOptionsPtr)GetHandlePointer (
										reformatOptionsHandle);

	outFileInfoPtr = (FileInfoPtr)GetHandlePointer (
					reformatOptionsPtr->outFileInfoHandle);
	
			// Present the user with a dialog box to allow a change in the default	
			// specifications.																	
			
	if (LoadShapeToThematicDialog (reformatOptionsPtr))
		{						
				// List the processor name, date and time.								

		gOutputForce1Code = 0x0001;
		continueFlag = ListHeaderInfo (NULL, 
													0, 
													&gOutputForce1Code, 
													kNoStatisticsUsed, 
													continueFlag);
						
		if (FindIfMapInformationExists (gImageFileInfoPtr->mapProjectionHandle))
			reformatOptionsPtr->headerFormat = kGeoTIFFType;
											
				// Get potential number of classes. Allow for background class.
				
		reformatOptionsPtr->numberClasses = 
			GetShapeFileNumberRecordsAndType (
												gShapeFilesHandle, shapeFileIndex, &shapeType);
					
		if (shapeType == 1)		// point type
			reformatOptionsPtr->numberClasses = 2;
			
		else		// shapeType != 1
			{
			if (reformatOptionsPtr->numberClasses < UInt32_MAX)
				reformatOptionsPtr->numberClasses++;
				
			reformatOptionsPtr->numberClasses = 
										MIN (reformatOptionsPtr->numberClasses, UInt16_MAX);
			
			}		// end "else shapeType != 1"
		
		outFileInfoPtr->numberClasses = reformatOptionsPtr->numberClasses;
		
		if (outFileInfoPtr->numberClasses > 255)
			{
			outFileInfoPtr->numberBytes = 2;
			outFileInfoPtr->numberBits = 16;
			
			}		// end "if (outFileInfoPtr->numberClasses > 255)"
			
				// Update parameters in the structure for the output file.			
		
		UpdateOutputFileStructure (outFileInfoPtr, 
											reformatOptionsPtr, 
											gImageFileInfoPtr->startLine, 
											gImageFileInfoPtr->startColumn,
											gImageFileInfoPtr->mapProjectionHandle);
		
				// Get buffer for output data.													
				
		continueFlag = GetReformatOutputBuffer (outFileInfoPtr, reformatOptionsPtr);
					
				// Set up the disk file to be used for the new reformatted				
				// image file.																			
		
		if (continueFlag)	
			continueFlag = !GetReformatOutputFile (outFileInfoPtr, reformatOptionsPtr);
		
				// Change cursor to watch cursor until done with process.				
		
		MSetCursor (kWait);
													
		startTime = time (NULL);
		
				// Get the status dialog window.													
		
		gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
		if (continueFlag)	
			continueFlag = (gStatusDialogPtr != NULL);
	
				//	Title for creating background image.	
				//		"\pCreating Thematic Image Background"									
	
		LoadDItemStringNumber (kReformatStrID,
											IDS_Reform13, 
											gStatusDialogPtr, 
											IDC_Status11,
											(Str255*)gTextString);
				
				// Create the base Thematic Image file with all background values.	
				
		if (continueFlag)	
			continueFlag = CreateBackgroundImageFile (
													outFileInfoPtr, 
													reformatOptionsPtr->ioOutBufferPtr,
													kFromReformat,
													reformatOptionsPtr->numberOutputBufferLines,
													(UInt8)reformatOptionsPtr->backgroundValue,
													(reformatOptionsPtr->numberOutputBufferLines < 
																		outFileInfoPtr->numberLines)); 
	
				//	Set title and dialog items for changing pixels to class numbers.	
	
		if (continueFlag)
			{
					// "\pShape File to Thematic Image"
					
			LoadDItemStringNumber (kReformatStrID,
											IDS_Reform32,  	
											gStatusDialogPtr, 
											IDC_Status11,
											(Str255*)gTextString); 
											
			ShowStatusDialogItemSet (kStatusLine);
			gNextStatusTime = TickCount ();
			
			CheckSomeEvents (updateMask);
			
					// Now get a pointer to the overlay list and the number of overlays.
					
			shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle);
																	
			if (shapeHandlePtr != NULL)
				{
				shapeInfoHandle = shapeHandlePtr[shapeFileIndex];
				shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeInfoHandle,
																				kLock);
            													
				continueFlag = (shapeInfoPtr != NULL);
				
				}		// end "if (shapeHandlePtr != NULL)"
			
			}		// end "if (continueFlag)"
				
				// Now change those pixels that belong to one of the requested			
				// project classes.																	
		
		if (continueFlag)	
			continueFlag = ConvertShapeToClassNumber (
												outFileInfoPtr, shapeInfoPtr, reformatOptionsPtr);
																
				// Create the ERDAS Trailer file if needed.									
		
		if (continueFlag)
			{			
			supportFileType = kITRLFileType;
			if (outFileInfoPtr->format != kErdas74Type)
				supportFileType = kICLRFileType;
				
					// Note: Do not include the background class in the count for
					// creating the background class. It will be handled separately
					// in this routine.
			
			continueFlag = CreateThematicSupportFile (
														outFileInfoPtr,
														shapeInfoPtr,
														(UInt16)reformatOptionsPtr->numberClasses-1,
														NULL,
														0,
														NULL,
														NULL,
														NULL,
														kDefaultColors,
														(UInt16)reformatOptionsPtr->numberClasses-1,
														kFromShapeToThematicCode,
														kPaletteHistogramClassNames,
														kClassDisplay,
														kCollapseToHighest,
														supportFileType);
										
			}		// end "if (continueFlag)"	
		
				// List Reformat information.												
		
		if (continueFlag)		
			continueFlag = ListReformatResultsInformation (
																reformatOptionsPtr, outFileInfoPtr);
			
		#if include_gdal_capability				
			if (continueFlag)								
				continueFlag = ListShapeDBFFieldNames (shapeInfoPtr); 
		#endif	// include_gdal_capability	
			
		CheckAndUnlockHandle (shapeInfoHandle);
					
				// List the CPU time taken for reformatting.					
					
		continueFlag = ListCPUTimeInformation (NULL, 
															continueFlag, 
															startTime);
				  		
	  					// Close the output file.													
	  				
		CloseFile (outFileInfoPtr);
		
		}		// end "if (LoadAreasToThematicDialog (reformatOptionsPtr))"
			
			// Release memory used for the reformat structure.	
							
	ReleaseReformatOutputFileInfoAndBuffers (reformatOptionsPtr, NULL);									
					
			// Dispose of updating statistics status dialog box.						
		
	CloseStatusDialog (TRUE);
			
	MInitCursor ();
		
}		// end "ShapeToThematicFileControl" 

