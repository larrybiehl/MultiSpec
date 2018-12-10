//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SFileInfo.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			07/30/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide routines to support the MFileInfo structure.
//
//	Functions in file:	CMFileStream* 	GetFileStreamPointer
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
 
#if defined multispec_lin
	#include "CFileStream.h"
	#define FALSE false
	#define TRUE true
   #if defined multispec_lin
		#if defined NetBeansProject
				// The size of a 'unsigned long', as computed by sizeof.
			#define SIZEOF_UNSIGNED_LONG 8
				// The size of a 'void p', as computed by sizeof.
			#define SIZEOF_VOIDP 8
		#endif
	#endif
#endif	// defined multispec_lin

#if defined multispec_win 
	#include	"CFileStream.h"
	#include	"CImageWindow.h"
	#include "SExternalGlobals.h"
#endif	// defined multispec_win 

#if include_gdal_capability
	#include "gdal.h"
#endif	// include_gdal_capability



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseControlPointVectorPointers
//
//	Software purpose:	The purpose of this routine is to indicate that the pointers to 
//							the control point vectors may not be valid any more by setting 
//							them to NULL.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/12/2007
//	Revised By:			Larry L. Biehl			Date: 02/12/2007

void CloseControlPointVectorPointers (
				Handle								controlPointHandle)
{  
	ControlPointsPtr					controlPointsPtr;
	
	
	controlPointsPtr = (ControlPointsPtr)GetHandlePointer (controlPointHandle);
														
	if (controlPointsPtr != NULL)
		{						
		controlPointsPtr->easting1Ptr = NULL;
		controlPointsPtr->northing1Ptr = NULL;
		controlPointsPtr->easting2Ptr = NULL;
		controlPointsPtr->northing2Ptr = NULL;
		controlPointsPtr->statusPtr = NULL;
		
		}	// end "if (controlPointsPtr != NULL)"
		
	CheckAndUnlockHandle (controlPointHandle);
	
}	// end "CloseControlPointVectorPointers"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseImageFile
//
//	Software purpose:	This routine closes the open image file and disposes
//							of the description handle and file information handle
//							associated with the image file.
//
//	Parameters in:		Handle to file information
//							Number of files	
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			DeactivateProject in project.c
//							ChangeProjectBaseImage in project.c
//							AppendFile in refomat.c
//							CloseImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 06/21/1990
//	Revised By:			Larry L. Biehl			Date: 06/04/1996

void CloseImageFile (
				Handle								fileInfoHandle)

{                                   
	if (fileInfoHandle != NULL)
		{                       
		FileInfoPtr				fileInfoPtr;
		SignedByte				handleStatus;


		fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																fileInfoHandle, &handleStatus);
													
		CloseImageFile (fileInfoPtr);
		
		MHSetState (fileInfoHandle, handleStatus);	
			
		}	// end "if (fileInfoHandle != NULL)" 
			
}	// end "CloseImageFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseImageFile
//
//	Software purpose:	This routine closes the open image file and disposes
//							of the description handle and file information handle
//							associated with the image file.
//
//	Parameters in:		Handle to file information
//							Number of files	
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			DeactivateProject in project.c
//							ChangeProjectBaseImage in project.c
//							AppendFile in refomat.c
//							CloseImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 12/01/1995
//	Revised By:			Larry L. Biehl			Date: 07/30/2018

void CloseImageFile (
				FileInfoPtr							fileInfoPtr)

{  
	if (fileInfoPtr != NULL)
		{                                                               
		CloseFile (fileInfoPtr);
		
				// Make sure that file elements in any hdf file are closed also.
		
		#if include_hdf_capability
			if (fileInfoPtr->hdf4FileID > 0 && 
						fileInfoPtr->format != kHDF5Type &&
								fileInfoPtr->format != kNETCDF2Type)
				CloseHDF4File (fileInfoPtr);
		#endif		// include_hdf_capability
		
		#if include_gdal_capability
			#if include_hdf5_capability
				if (fileInfoPtr->hdfHandle != NULL && 
						(fileInfoPtr->format == kHDF5Type ||
							fileInfoPtr->format == kNETCDF2Type ||
								fileInfoPtr->format == kHDF4Type2 ||
									fileInfoPtr->format == kNITFType))
					CloseHDF5DataSetInfo (fileInfoPtr);
			#endif	// include_hdf5_capability
			
			if (fileInfoPtr->gdalDataSetH != 0)
				{
				GDALClose (fileInfoPtr->gdalDataSetH);
				gNumberOfOpenFiles--;
				
				}	// end "if (fileInfoPtr->gdalDataSetH != 0)"
				
			fileInfoPtr->gdalDataSetH = 0;
		#endif	// include_gdal_capability
			
		fileInfoPtr->channelValuesHandle = 
										UnlockAndDispose (fileInfoPtr->channelValuesHandle);
		fileInfoPtr->channelDescriptionH = 
										UnlockAndDispose (fileInfoPtr->channelDescriptionH);
		fileInfoPtr->classDescriptionH = 
										UnlockAndDispose (fileInfoPtr->classDescriptionH);
		fileInfoPtr->controlPointsHandle = 
										UnlockAndDispose (fileInfoPtr->controlPointsHandle);
		fileInfoPtr->groupNameHandle = 
										UnlockAndDispose (fileInfoPtr->groupNameHandle);
		fileInfoPtr->groupTablesHandle = 
										UnlockAndDispose (fileInfoPtr->groupTablesHandle);
		fileInfoPtr->channelToHdfDataSetHandle = 
										UnlockAndDispose (fileInfoPtr->channelToHdfDataSetHandle);
		fileInfoPtr->hdfHandle = UnlockAndDispose (fileInfoPtr->hdfHandle);
		fileInfoPtr->hfaHandle = UnlockAndDispose (fileInfoPtr->hfaHandle);
		
		fileInfoPtr->mapProjectionHandle = 
									DisposeMapProjectionHandle (fileInfoPtr->mapProjectionHandle);
												
		fileInfoPtr->hfaPtr = NULL;
		fileInfoPtr->blockFormatPtr = NULL;
			
		}	// end "if (fileInfoPtr != NULL)" 
			
}	// end "CloseImageFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DisposeFileInfoHandle
//
//	Software purpose:	The purpose of this routine is to all of the image files
//							and memory associated with the input file information
//							structure(s) represented by the input handle.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1995
//	Revised By:			Larry L. Biehl			Date: 12/01/1995
                                                                
void DisposeFileInfoHandle (
				Handle								fileInfoHandle)
				
{		                         
	if (fileInfoHandle != NULL)
		{                      
		FileInfoPtr						fileInfoPtr,
											localFileInfoPtr;
							
		UInt32 							index,
											numberFiles;  
	
	
				// Determine how many file info structures are store within
				// this handle. (I.E. check for linked file case.
				
		SInt32 handleSize = MGetHandleSize (fileInfoHandle);
		numberFiles = handleSize/sizeof (MFileInfo);
		
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
		              
		for (index=0; index<numberFiles; index++)
			{           						
			localFileInfoPtr = &fileInfoPtr[index];
						
			CloseImageFile (localFileInfoPtr);
		
			#if defined multispec_win || defined multispec_lin
				if (localFileInfoPtr->fileStreamCPtr != NULL)
					delete localFileInfoPtr->fileStreamCPtr;
			#endif	// defined multispec_win 
			
			}	// end "for (index=0; index<numberFiles; index++)"
			
				// Make sure that the non-Project processor specification structures 
				// know that this fileInfoHandle is not to be referenced any more.
				
		ClearNonProjProcessorFileInfoHandleReferences (fileInfoHandle);

		UnlockAndDispose (fileInfoHandle); 
		
		}	// end "if (fileInfoHandle != NULL)"
	
}	// end "DisposeFileInfoHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetActiveNumberClasses
//
//	Software purpose:	The purpose of this routine is to return the number of
//							classes for the active image window.
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

UInt32 GetActiveNumberClasses (void)

{  
	UInt32 numberClasses = 0;
	
	                            
	Handle fileInfoHandle = GetActiveImageFileInfoHandle ();
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	if (fileInfoPtr != NULL)
		numberClasses = fileInfoPtr->numberClasses;
									
	return (numberClasses);
	
}	// end "GetActiveNumberClasses"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetBandInterleave
//
//	Software purpose:	The purpose of this routine is to return the band interleave of 
//							the file for the input file information structure handle. Note
//							that for now it just returns the format for the first file
//							in any list of linked files.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/2013
//	Revised By:			Larry L. Biehl			Date: 01/10/2013	

SInt16 GetBandInterleave (
				Handle								fileInfoHandle)

{
	SInt16								bandInterleave = 0;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		bandInterleave = fileInfoPtr->bandInterleave;			
									
	return (bandInterleave);
	
}	// end "GetBandInterleave"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetBlockInformation
//
//	Software purpose:	The purpose of this routine is to return the current settings
//							of the file block height and width. The routine assumes that
//							all channels are like the first. 0 is returned for both if
//							the data in the file are not blocked.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			FileSpecificationDialogInitialize in SOpenDlg.cpp
//							FileSpecificationDialogOK in SOpenDlg.cpp
//							ListDescriptionInformation in SOther.cp
//
//	Coded By:			Larry L. Biehl			Date: 10/27/1999
//	Revised By:			Larry L. Biehl			Date: 07/14/2012	

void GetBlockInformation (
				FileInfoPtr							fileInfoPtr,
				UInt32*								blockHeightPtr,
				UInt32*								blockWidthPtr,
				UInt32*								blockSizePtr,
				UInt32*								blockOffsetPtr)

{
	HierarchalFileFormatPtr			hfaPtr;
	
	
	if (fileInfoPtr->blockHeight == 0)
		{
				// fileInfoPtr->blockHeight of 0 implies that this image file is
				// not being read with gdal routines.
				
				// Get the block size used in the image file. Assume that
				// the first channel is just like the rest of the channels.
				
		hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (fileInfoPtr->hfaHandle);	
		
		if (hfaPtr != NULL)
			{
			*blockHeightPtr = hfaPtr->blockHeight;
			*blockWidthPtr = hfaPtr->blockWidth;
			*blockSizePtr = hfaPtr->blockSize;
			*blockOffsetPtr = hfaPtr->blockOffset;
			
			}	// end "if (hfaPtr != NULL)"
			
		else	// hfaPtr == NULL
			{
			*blockHeightPtr = 0;
			*blockWidthPtr = 0;
			*blockSizePtr = 0;
			
			}	// end "if (hfaPtr != NULL)"
			
		}	// end "if (fileInfoPtr->blockHeight == 0)"
		
	else	// fileInfoPtr->blockHeight > 0
		{
		*blockHeightPtr = fileInfoPtr->blockHeight;
		*blockWidthPtr = fileInfoPtr->blockWidth;
		*blockSizePtr = 0;
		*blockOffsetPtr = 0;
		
		}	// end "else fileInfoPtr->blockHeight > 0

}	// end "GetBlockInformation"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16* GetClassToGroupPointer
//
//	Software purpose:	The purpose of this routine is to obtain the class to
//							group pointer for the input image file information
//							handle or pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/05/1996
//	Revised By:			Larry L. Biehl			Date: 04/26/2016	

SInt16* GetClassToGroupPointer (
				Handle								imageFileInfoHandle)

{
	SignedByte							handleStatus;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
							imageFileInfoHandle, &handleStatus, kNoMoveHi);
							
	SInt16* classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
		
	MHSetState (imageFileInfoHandle, handleStatus);	
									
	return (classToGroupPtr);
	
}	// end "GetClassToGroupPointer"  

 

SInt16* GetClassToGroupPointer (
				FileInfoPtr							imageFileInfoPtr)

{
	SInt16* 								classToGroupPtr = NULL;
	
	UInt32								index;
	
	
	if (imageFileInfoPtr != NULL)
		classToGroupPtr = (SInt16*)GetHandlePointer (
															imageFileInfoPtr->groupTablesHandle, 
															kLock, 
															kNoMoveHi);

	if (classToGroupPtr != NULL)
		{
		//classToGroupPtr = &classToGroupPtr [imageFileInfoPtr->numberClasses];
		index = MAX (256, imageFileInfoPtr->numberClasses);
		classToGroupPtr = &classToGroupPtr[index];
		
		}
									
	return (classToGroupPtr);
	
}	// end "GetClassToGroupPointer" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetChannelDescriptionHandle
//
//	Software purpose:	The purpose of this routine is to obtain the description
//							handle for the class information related to the input
//							file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1999
//	Revised By:			Larry L. Biehl			Date: 09/18/2006	

Handle GetChannelDescriptionHandle (
				Handle								imageFileInfoHandle)

{
	Handle								channelDescriptionH = NULL;
	
		
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (imageFileInfoHandle);
	
	if (fileInfoPtr != NULL)
		channelDescriptionH = fileInfoPtr->channelDescriptionH;			
									
	return (channelDescriptionH);
	
}	// end "GetChannelDescriptionHandle"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetClassDescriptionHandle
//
//	Software purpose:	The purpose of this routine is to obtain the description
//							handle for the class information related to the input
//							file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1999
//	Revised By:			Larry L. Biehl			Date: 09/18/2006	

Handle GetClassDescriptionHandle (
				Handle								imageFileInfoHandle)

{
	Handle								classDescriptionH = NULL;
	
		
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (imageFileInfoHandle);
	
	if (fileInfoPtr != NULL)
		classDescriptionH = fileInfoPtr->classDescriptionH;			
									
	return (classDescriptionH);
	
}	// end "GetClassDescriptionHandle"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetControlPointsHandle
//
//	Software purpose:	The purpose of this routine is to return the handle to the 
//							control points stored in the input file information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2007
//	Revised By:			Larry L. Biehl			Date: 02/27/2007	

Handle GetControlPointsHandle (
				Handle		 						fileInfoHandle)

{  
	FileInfoPtr							fileInfoPtr;
	
	Handle								controlPointsHandle = NULL;
	
	
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
															
	if (fileInfoPtr != NULL)
		controlPointsHandle = fileInfoPtr->controlPointsHandle;
																		
	return (controlPointsHandle);
	
}	// end "GetControlPointsHandle"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		ControlPointsPtr GetControlPointVectorPointers
//
//	Software purpose:	The purpose of this routine is to set the pointers to the
// 						control point vectors.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/12/2007
//	Revised By:			Larry L. Biehl			Date: 02/12/2007

ControlPointsPtr GetControlPointVectorPointers (
				Handle								controlPointHandle,
				Boolean								lockHandleFlag)
{  
	ControlPointsPtr					controlPointsPtr;
	
	SInt32								numberControlPoints;
	
	
	controlPointsPtr = (ControlPointsPtr)GetHandlePointer (
														controlPointHandle,
														lockHandleFlag);
														
	if (controlPointsPtr != NULL)
		{					
		numberControlPoints = controlPointsPtr->count;
			
		controlPointsPtr->easting1Ptr = (double*)&controlPointsPtr[1];
		controlPointsPtr->northing1Ptr = 
							&controlPointsPtr->easting1Ptr[numberControlPoints];
		controlPointsPtr->easting2Ptr = 
							&controlPointsPtr->easting1Ptr[2*numberControlPoints];
		controlPointsPtr->northing2Ptr = 
							&controlPointsPtr->easting1Ptr[3*numberControlPoints];
		controlPointsPtr->statusPtr = (SInt16*)
							&controlPointsPtr->easting1Ptr[4*numberControlPoints];
		
		}	// end "if (controlPointsPtr != NULL)"
						
	return (controlPointsPtr);
	
}	// end "GetControlPointVectorPointers"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMFileStream* GetFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to return the pointer
//							to the file stream structure or class.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/23/1995
//	Revised By:			Larry L. Biehl			Date: 01/03/1996

CMFileStream* GetFileStreamPointer (
				FileInfoPtr							fileInfoPtr)
{  
	CMFileStream*						fileStreamPtr = NULL;
	
	if (fileInfoPtr != NULL)
		{
		#if defined multispec_mac
			fileStreamPtr = &fileInfoPtr->fileStream;
		#endif	// defined multispec_mac
				
		#if defined multispec_win | defined multispec_lin		
			fileStreamPtr = fileInfoPtr->fileStreamCPtr;
		#endif	// defined multispec_win | lin		
		}	// end "if (fileInfoPtr != NULL)"
		
	return (fileStreamPtr);

}	// end "GetFileStreamPointer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetFileFormat
//
//	Software purpose:	The purpose of this routine is to return the format of the
//							file for the input file information structure handle. Note
//							that for now it just returns the format for the first file
//							in any list of linked files.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/2013
//	Revised By:			Larry L. Biehl			Date: 01/10/2013	

SInt16 GetFileFormat (
				Handle								fileInfoHandle)

{
	SInt16								fileFormat = 0;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		fileFormat = fileInfoPtr->format;			
									
	return (fileFormat);
	
}	// end "GetFileFormat"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetFileFormatFromWindowHandle
//
//	Software purpose:	The purpose of this routine is to return the format of the
//							file for the input window information structure handle. Note
//							that for now it just returns the format for the first file
//							in any list of linked files.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			GetDefaultSupportFile in SOpnImag.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 01/10/2013	

SInt16 GetFileFormatFromWindowHandle (
				Handle								windowInfoHandle)

{
	Handle								fileInfoHandle;
	
	
	fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		
	return (GetFileFormat (fileInfoHandle));
	
}	// end "GetFileFormatFromWindowHandle"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetFileMapProjectionHandle
//
//	Software purpose:	The purpose of this routine is to return the map projection
//							handle for the input file information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	map projection handle
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/07/2000
//	Revised By:			Larry L. Biehl			Date: 12/01/2000

Handle GetFileMapProjectionHandle (
				Handle								fileInfoHandle)

{	
			// Get pointer to the support file information.
												
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		return (fileInfoPtr->mapProjectionHandle); 
		
	return (NULL);
												
}	// end "GetFileMapProjectionHandle" 



Handle GetFileMapProjectionHandle (
				FileInfoPtr							fileInfoPtr)

{	
	if (fileInfoPtr != NULL)
		{
		if (fileInfoPtr->mapProjectionHandle == NULL)
			fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
	
		return (fileInfoPtr->mapProjectionHandle);
		
		}	// end "if (fileInfoPtr != NULL)" 
		
	return (NULL);
												
}	// end "GetFileMapProjectionHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16* GetGroupToPalettePointer
//
//	Software purpose:	The purpose of this routine is to obtain the group to
//							palette pointer for the input image file information
//							handle or pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/05/1996
//	Revised By:			Larry L. Biehl			Date: 12/12/1996

SInt16* GetGroupToPalettePointer (
				Handle								imageFileInfoHandle)

{
	SInt16* 								groupToPalettePtr;
	
	SignedByte							handleStatus;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
														imageFileInfoHandle, &handleStatus);
	
	groupToPalettePtr = GetGroupToPalettePointer (fileInfoPtr);
		
	MHSetState (imageFileInfoHandle, handleStatus);	
									
	return (groupToPalettePtr);
	
}	// end "GetGroupToPalettePointer"



SInt16* GetGroupToPalettePointer (
				FileInfoPtr							imageFileInfoPtr)

{	
	SInt16* groupToPalettePtr = NULL;
	
	
	if (imageFileInfoPtr != NULL)
		groupToPalettePtr = (SInt16*)GetHandlePointer (
													imageFileInfoPtr->groupTablesHandle, 
													kLock);
									
	return (groupToPalettePtr);
	
}	// end "GetGroupToPalettePointer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHdfDataSetSelection
//
//	Software purpose:	The purpose of this routine is to return the map projection
//							handle for the input file information structure handle.
//
//	Parameters in:		Window information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	map projection handle
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	07/11/2006
//	Revised By:			Larry L. Biehl			Date: 07/11/2006

SInt16 GetHdfDataSetSelection (
				Handle								fileInfoHandle)

{	
			// Get pointer to the support file information.
												
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		return (fileInfoPtr->hdfDataSetSelection); 
		
	return (0);
												
}	// end "GetHdfDataSetSelection"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHDF_FilePointers
//
//	Software purpose:	This routine locks the channel to hdf data set index vector handle 
//							and sets the pointers to those structures in the input file info
//							stucture(s).
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 11/26/2007
//	Revised By:			Larry L. Biehl			Date: 11/26/2007	

void GetHDF_FilePointers (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)

{
	FileInfoPtr							localFileInfoPtr;
	
	UInt32								index,
											numberImageFiles;
											
	
	if (fileInfoPtr != NULL)
		{
		numberImageFiles = 1;
		if (windowInfoPtr != NULL)
			numberImageFiles = windowInfoPtr->numberImageFiles;
		
		for (index=0; index<numberImageFiles; index++)
			{	
			localFileInfoPtr = &fileInfoPtr[index];
					
					// Get pointer to the hfa structure.
					
			localFileInfoPtr->channelToHdfDataSetPtr = 
										(UInt16*)GetHandlePointer (
														localFileInfoPtr->channelToHdfDataSetHandle,
														kLock,
														kMoveHi);
				
			}	// end "for (index=0; index<numberImageFiles; index++)"
			
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "GetHierarchalFilePointers"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetHierarchalFilePointers
//
//	Software purpose:	This routine locks the hierarchal structure handle and sets
//							the pointers to those structures in the input file info
//							stucture(s).
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 02/07/2007	

void GetHierarchalFilePointers (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)

{
	FileInfoPtr							localFileInfoPtr;
	Ptr									ptr;
	
	UInt32								index,
											numberImageFiles;
											
	
	if (fileInfoPtr != NULL)
		{
		numberImageFiles = 1;
		if (windowInfoPtr != NULL)
			numberImageFiles = windowInfoPtr->numberImageFiles;
		
		for (index=0; index<numberImageFiles; index++)
			{	
			localFileInfoPtr = &fileInfoPtr[index];
					
					// Get pointer to the hfa structure.
					
			localFileInfoPtr->hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (
																		localFileInfoPtr->hfaHandle,
																		kLock);
					
			ptr = (Ptr)localFileInfoPtr->hfaPtr;
			localFileInfoPtr->blockFormatPtr = (BlockFormatPtr)
														&ptr[localFileInfoPtr->blockFormatIndex];
				
			}	// end "for (index=0; index<numberImageFiles; index++)"
			
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "GetHierarchalFilePointers"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetNumberClasses
//
//	Software purpose:	The purpose of this routine is to obtain the description
//							handle for the class information related to the input
//							file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1999
//	Revised By:			Larry L. Biehl			Date: 01/10/1999	

UInt32 GetNumberClasses (
				Handle								fileInfoHandle)

{
	UInt32								numberClasses = 0;
	
		
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		numberClasses = fileInfoPtr->numberClasses;			
									
	return (numberClasses);
	
}	// end "GetNumberClasses"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetNumberControlPoints
//
//	Software purpose:	The purpose of this routine is to return the number of control
//							points stored in the input file information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2007
//	Revised By:			Larry L. Biehl			Date: 02/27/2007	

UInt32 GetNumberControlPoints (
				FileInfoPtr 						fileInfoPtr)

{  
	ControlPointsPtr					controlPointsPtr = NULL;
	
	UInt32								numberControlPoints = 0;
	
	
	if (fileInfoPtr != NULL)
		controlPointsPtr = (ControlPointsPtr)GetHandlePointer (
															fileInfoPtr->controlPointsHandle);
															
	if (controlPointsPtr != NULL)
		numberControlPoints = controlPointsPtr->count;
																		
	return (numberControlPoints);
	
}	// end "GetNumberControlPoints"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetThematicType
//
//	Software purpose:	The purpose of this routine is to obtain the thematic type
//							flag for input file information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1999
//	Revised By:			Larry L. Biehl			Date: 01/10/1999	

Boolean GetThematicType (
				Handle								fileInfoHandle)

{
	Boolean								thematicTypeFlag = FALSE;
	
		
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		thematicTypeFlag = fileInfoPtr->thematicType;			
									
	return (thematicTypeFlag);
	
}	// end "GetThematicType" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle InitializeFileInfoStructure
//
//	Software purpose:	The purpose of this routine is to initialize memory for
//							a new MFileInfo structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			OpenImageFile in menus.c
//							GetSpecifiedImageFile in project.c
//							ChangeProjectBaseImage in project.c
//							LoadAreasToThematicSpecs in fieldsToThematicFile.c
//							CreateOutputWindow in initialize.c
//							CreateResultsDiskFile in multiSpecUtilities.c
//							CreateERDAS_TRLFile in projectUtilities.c
//							ChangeImageFileFormat in reformat.c
//
//	Coded By:			Larry L. Biehl			Date: 03/05/1991
//	Revised By:			Larry L. Biehl			Date: 07/10/2018

Handle InitializeFileInfoStructure (
				Handle								fileInfoHandle,
				Boolean								inputPtrFlag)

{
	FileInfoPtr							fileInfoPtr;
	SignedByte							handleStatus;
								
	
	if (fileInfoHandle == NULL)
		{                               
		fileInfoHandle = MNewHandle (sizeof (MFileInfo));
		inputPtrFlag = FALSE;
		 
		#if defined multispec_win || defined multispec_lin
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
														
			if (fileInfoPtr != NULL)
				fileInfoPtr->fileStreamCPtr = NULL;
		#endif	// defined multispec_win || defined multispec_lin
		
		}	// end "if (fileInfoHandle == NULL)"
	
	if (fileInfoHandle != NULL)
		{
		if (inputPtrFlag)
			{
			fileInfoPtr = (FileInfoPtr)fileInfoHandle;
			
			#if defined multispec_win	
				fileInfoPtr->fileStreamCPtr = NULL;
			#endif	// defined multispec_win

         #if defined multispec_lin
            fileInfoPtr->fileStreamCPtr = NULL;
         #endif

			}	// end "if (inputPtrFlag)"
			
		else	// !inputPtrFlag
			fileInfoPtr = (FileInfoPtr)
				GetHandleStatusAndPointer (fileInfoHandle, &handleStatus);
		
				// Initialize the some variables and handles in the					
				// structure.			
				
		#if defined multispec_mac
			InitializeFileStream (&fileInfoPtr->fileStream);
		#endif	// defined multispec_mac
		
		#if defined multispec_win || defined multispec_lin
			fileInfoPtr->fileStreamCPtr = 
		    						InitializeFileStream (fileInfoPtr->fileStreamCPtr);
		    						
		   if (fileInfoPtr->fileStreamCPtr == NULL)
		   	{
		   	DisposeFileInfoHandle (fileInfoHandle);
		   																			return (NULL);
		   	
		   	}
		#endif	// defined multispec_win || defined multispec_lin
										
		fileInfoPtr->hfaPtr = NULL;
		fileInfoPtr->blockFormatPtr = NULL;
		fileInfoPtr->channelToHdfDataSetPtr = NULL;
		fileInfoPtr->gdalDataSetH = 0;
		
		fileInfoPtr->channelDescriptionUnitString[0] = 0;

		fileInfoPtr->maxDataValue = 255;
		fileInfoPtr->minDataValue = 0;

		fileInfoPtr->maxUsableDataValue = 255;
		fileInfoPtr->minUsableDataValue = 0;
			
		fileInfoPtr->noDataValue = 0;

		fileInfoPtr->channelToHdfDataSetHandle = NULL;
		fileInfoPtr->channelValuesHandle = NULL;
		fileInfoPtr->channelDescriptionH = NULL;
		fileInfoPtr->classDescriptionH = NULL;
		fileInfoPtr->controlPointsHandle = NULL;
		fileInfoPtr->groupTablesHandle = NULL;
		fileInfoPtr->groupNameHandle = NULL;
		fileInfoPtr->hdfHandle = NULL;
		fileInfoPtr->hfaHandle = NULL;
		fileInfoPtr->mapProjectionHandle = NULL;
		
		fileInfoPtr->hdf4FileID = 0;
		fileInfoPtr->hdf5FileID = 0;
		
		fileInfoPtr->signedValueOffset = 0;

		fileInfoPtr->blockFormatIndex = 0;
		fileInfoPtr->bytesPer1chanAllLines = 0;
		fileInfoPtr->bytesPer1lineAllChans = 0;
		fileInfoPtr->bytesPer1line1chan = 0;
		fileInfoPtr->colorTableOffset = 0;
		fileInfoPtr->dataConversionCode = 0;
		fileInfoPtr->maxClassNumberValue = 0;
		fileInfoPtr->numberBins = 256;
		fileInfoPtr->numberClasses = 0;
		fileInfoPtr->numberColumns = 1;
		fileInfoPtr->numberHeaderBytes = 0;
		fileInfoPtr->numberLines = 1;
		fileInfoPtr->numberPreChannelBytes = 0;
		fileInfoPtr->numberPreLineBytes = 0;
		fileInfoPtr->numberPostChannelBytes = 0;
		fileInfoPtr->numberPostLineBytes = 0;
		fileInfoPtr->numberTrailerBytes = 0;
		fileInfoPtr->startColumn = 1;
		fileInfoPtr->startLine = 1;
		
		
		fileInfoPtr->blockHeight = 0;
		fileInfoPtr->blockWidth = 0;

		fileInfoPtr->bandInterleave = kBIL;
		fileInfoPtr->colorTableValueBytes = 1;
		fileInfoPtr->fileExists = -1;	
		fileInfoPtr->format = 0; 
		fileInfoPtr->ancillaryInfoformat = 0;
		fileInfoPtr->channelsInWavelengthOrderCode = kNotApplicable;
		fileInfoPtr->collapseClassSelection = kDoNotCollapseClass;
		fileInfoPtr->descriptionCode = 0;
		fileInfoPtr->hdfDataSetSelection = 0;
		fileInfoPtr->numberHdfDataSets = 0;
		fileInfoPtr->instrumentCode = 0;
		fileInfoPtr->maxNumberDescriptionCharacters = 0;
		fileInfoPtr->dataTypeCode = kIntegerType;
		fileInfoPtr->dataCompressionCode = kNoCompression;
		fileInfoPtr->gdalBandInterleave = 0;
		fileInfoPtr->gdalDataTypeCode = 0;
		fileInfoPtr->maxNumberEDecimalDigits = 0;
		fileInfoPtr->maxNumberFDecimalDigits = 0;
		fileInfoPtr->signedBitMask = 0x0000;
		fileInfoPtr->signedValueMask = 0x0000;
		fileInfoPtr->numberBits = 8;
		fileInfoPtr->numberBytes = 1;
		fileInfoPtr->numberChannels = 1;
		fileInfoPtr->numberGroups = 0;

		fileInfoPtr->asciiSymbols = FALSE;
		fileInfoPtr->blockedFlag = FALSE;
		fileInfoPtr->callGetHDFLineFlag = FALSE;
		fileInfoPtr->classChangedFlag = FALSE;
		fileInfoPtr->classesComputedFlag = FALSE;
		fileInfoPtr->classNamesLoadedFlag = FALSE;
		fileInfoPtr->descriptionsFlag = FALSE;
		fileInfoPtr->treatLinesAsBottomToTopFlag = FALSE;
		fileInfoPtr->groupChangedFlag = FALSE;
		fileInfoPtr->nonContiguousStripsFlag = FALSE;
		fileInfoPtr->noDataValueFlag = FALSE;
		fileInfoPtr->signedDataFlag = FALSE;
		fileInfoPtr->swapBytesFlag = FALSE;
		fileInfoPtr->swapColorTableBytes = FALSE;
		fileInfoPtr->swapHeaderBytesFlag = FALSE;
		fileInfoPtr->thematicType = FALSE;
		
		if (!inputPtrFlag)
			MHSetState (fileInfoHandle, handleStatus);
		
		}	// end "fileInfoHandle != NULL"
		
	return (fileInfoHandle);
			
}	// end "InitializeFileInfoStructure" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle InitializeHierarchalFileStructure
//
//	Software purpose:	The purpose of this routine is to initialize memory for
//							a new MFileInfo structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ReadImagineHeader in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1999
//	Revised By:			Larry L. Biehl			Date: 12/17/2009

Handle InitializeHierarchalFileStructure (
				Handle								hfaHandle,
				FileInfoPtr							fileInfoPtr,
				UInt32								numberBlocks)

{
	BlockFormatPtr						blockFormatPtr;
	HierarchalFileFormatPtr			hfaPtr;
	Ptr									ptr;
	
	UInt32								index,
											numberBytes;
	
	SignedByte							handleStatus;
	
	
	if (hfaHandle == NULL)  
		{					
		numberBytes = fileInfoPtr->numberChannels*sizeof (HierarchalFileFormat);
		fileInfoPtr->blockFormatIndex = numberBytes;
		
		numberBytes += numberBlocks*sizeof (BlockFormat);
		
		hfaHandle = MNewHandle (numberBytes);          
		
		}	// end "if (hfaHandle == NULL)"
		
	hfaPtr = (HierarchalFileFormatPtr)
				GetHandleStatusAndPointer (hfaHandle, &handleStatus);
	
	if (hfaPtr != NULL)
		{
		ptr = (Ptr)hfaPtr;
		blockFormatPtr = (BlockFormatPtr)&ptr[fileInfoPtr->blockFormatIndex];
		
		for (index=0; index<fileInfoPtr->numberChannels; index++)
			{
			hfaPtr->firstColumnStartByte = 0;
			hfaPtr->layerOffsetBytes = 0;
			
			hfaPtr->tiledBufferPtr = NULL;
			
			hfaPtr->blockHeight = 0;
			hfaPtr->blockSize = 0;
			hfaPtr->blockSizeRead = 0;
			hfaPtr->blocksPerChannel = 0;
			hfaPtr->blockWidth = 0;
			hfaPtr->blockWidthRead = 0;
			hfaPtr->numberBlockHeights = 0;
			hfaPtr->numberBlockWidths = 0;
			hfaPtr->blockOffset = 0;
	
			hfaPtr->firstLineRead = 0;
			hfaPtr->lastLineRead = 0;
			
			hfaPtr->numberBlocksRead = 0;
			hfaPtr->firstColumnRead = 0;
			hfaPtr->lastColumnRead = 0;
			
			hfaPtr++;
			
			}	// end "for (index=0; index<..."
			
		for (index=0; index<numberBlocks; index++)
			{
			blockFormatPtr->blockOffsetBytes = 0;
			blockFormatPtr->blockSize = 0;
			
			blockFormatPtr++;
			
			}	// end "for (index=0; index<..."
		
		MHSetState (hfaHandle, handleStatus);
				
		}	// end "if (hfaPtr != NULL)"
				
	return (hfaHandle);
			
}	// end "InitializeHierarchalFileStructure"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetFileInfoHandle
//
//	Software purpose:	This routine sets the file information handle
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
//	Revised By:			Larry L. Biehl			Date: 11/29/1995			

void SetFileInfoHandle (
				WindowInfoPtr						windowInfoPtr,
				Handle								fileInfoHandle)

{ 
	if (windowInfoPtr != NULL)
	  	windowInfoPtr->fileInfoHandle = fileInfoHandle;
	
}	// end "SetFileInfoHandle"       



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetFileInstrumentCode
//
//	Software purpose:	The purpose of this routine is to set the map projection
//							handle in the input file information structure.
//
//	Parameters in:		File information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	map projection handle
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	01/20/2013
//	Revised By:			Larry L. Biehl			Date: 08/22/2017

void SetFileInstrumentCode (
				Handle								fileInfoHandle,
				SInt16								instrumentCode)

{	
			// Get pointer to the support file information.
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		fileInfoPtr->instrumentCode = instrumentCode;
	
}	// end "SetFileInstrumentCode" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetFileMapProjectionHandle
//
//	Software purpose:	The purpose of this routine is to set the map projection
//							handle in the input file information structure.
//
//	Parameters in:		File information structure handle
//
//	Parameters out:	None
//
// Value Returned:  	map projection handle
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	08/02/2010
//	Revised By:			Larry L. Biehl			Date: 08/02/2010

void SetFileMapProjectionHandle (
				Handle								fileInfoHandle,
				Handle								mapProjectionHandle)

{	
			// Get pointer to the support file information.
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	
	if (fileInfoPtr != NULL)
		fileInfoPtr->mapProjectionHandle = mapProjectionHandle;
	
}	// end "SetFileMapProjectionHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnlockGroupTablesHandle
//
//	Software purpose:	The purpose of this routine is to unlock the group tables
//							handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/05/1996
//	Revised By:			Larry L. Biehl			Date: 12/12/1996	
 
void UnlockGroupTablesHandle (
				Handle								imageFileInfoHandle)

{
	SignedByte							handleStatus;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
															imageFileInfoHandle, &handleStatus);
	
	UnlockGroupTablesHandle (fileInfoPtr);
		
	MHSetState (imageFileInfoHandle, handleStatus);	
	
}	// end "UnlockGroupTablesHandle" 



void UnlockGroupTablesHandle (
				FileInfoPtr							imageFileInfoPtr)

{
	if (imageFileInfoPtr != NULL)
		CheckAndUnlockHandle (imageFileInfoPtr->groupTablesHandle);
	
}	// end "UnlockGroupTablesHandle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnlockHDF_FileHandles
//
//	Software purpose:	This routine unlocks the HDF structure handle and sets
//							the pointers to those structures in the input file info
//							stucture(s) to NULL.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 11/26/2007
//	Revised By:			Larry L. Biehl			Date: 11/27/2007	

void UnlockHDF_FileHandles (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)

{
	FileInfoPtr							localFileInfoPtr;
	
	UInt32								index,
											numberImageFiles;
	
	
	if (fileInfoPtr != NULL)
		{
		numberImageFiles = 1;
		if (windowInfoPtr != NULL)
			numberImageFiles = windowInfoPtr->numberImageFiles;
		
		for (index=0; index<numberImageFiles; index++)
			{	
			localFileInfoPtr = &fileInfoPtr[index];
					
					// Unlock the hfa structure handle.
					
			CheckAndUnlockHandle (localFileInfoPtr->channelToHdfDataSetHandle);
			
			localFileInfoPtr->channelToHdfDataSetPtr = NULL;
				
			}	// end "for (index=0; index<numberImageFiles; index++)"
			
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "UnlockHDF_FileHandles"         



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UnlockHierarchalFileHandles
//
//	Software purpose:	This routine unlocks the hierarchal structure handle and sets
//							the pointers to those structures in the input file info
//							stucture(s) to NULL.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 02/07/2007	

void UnlockHierarchalFileHandles (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)

{
	FileInfoPtr							localFileInfoPtr;
	
	UInt32								index,
											numberImageFiles;
	
	
	if (fileInfoPtr != NULL)
		{
		numberImageFiles = 1;
		if (windowInfoPtr != NULL)
			numberImageFiles = windowInfoPtr->numberImageFiles;
		
		for (index=0; index<numberImageFiles; index++)
			{	
			localFileInfoPtr = &fileInfoPtr[index];
					
					// Unlock the hfa structure handle.
					
			CheckAndUnlockHandle (localFileInfoPtr->hfaHandle);
			
			localFileInfoPtr->hfaPtr = NULL;
			localFileInfoPtr->blockFormatPtr = NULL;
				
			}	// end "for (index=0; index<numberImageFiles; index++)"
			
		}	// end "if (fileInfoPtr != NULL)"
	
}	// end "UnlockHierarchalFileHandles"         

