//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2009)
//							© Purdue Research Foundation
//									All rights reserved.
//
//					The following was put here for easy access to a template
//  				DebugStr ("\pSJPEG2000.cpp");
//
//	File:						SJPEG2000.cpp
//
//	Functions in file:	
//
//	Brief description:	This file contains routines which are used to access
//								various disk files.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			01/07/2009
//
//------------------------------------------------------------------------------------

#include "SMulSpec.h"
	
#if defined multispec_mac
		
#endif	// defined multispec_mac    

#if defined multispec_win
   	 
#endif	// defined multispec_win

#include <jasper/jasper.h>
#include	"jp2_cod.h"

extern void jp2_box_destroy(jp2_box_t *box);
extern jp2_box_t *jp2_box_get(jas_stream_t *in);

#include "SExtGlob.h"

#ifndef EQUAL
#if defined(WIN32) || defined(WIN32CE)
#  define EQUALN(a,b,n)           (strnicmp(a,b,n)==0)
#  define EQUAL(a,b)              (stricmp(a,b)==0)
#else
#  define EQUALN(a,b,n)           (strncasecmp(a,b,n)==0)
#  define EQUAL(a,b)              (strcasecmp(a,b)==0)
#endif
#endif


extern SInt16 				GetHDFFilePathCPointer (
									FileInfoPtr		 					fileInfoPtr,
									char*									filePathStringPtr,
									SInt16								fileStringLength,
									char**								filePathCPtrPtr);


SInt16						LoadJPEG2000Information (
									FileInfoPtr 						fileInfoPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadJPEG2000Header
//
//	Software purpose:	This routine verifies that the input file buffer is from a 
//							JPEG2000 formatted file and then reads the file specifications 
//							for the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/2009
//	Revised By:			Larry L. Biehl			Date: 01/07/2009

SInt16 ReadJPEG2000Header (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{	
	SInt16								returnCode = 1;
	
	unsigned char						charKeyCode[12];
		
	CMFileStream*						fileStreamPtr;		
	SInt32								jpeg2000Return = 2;
		
	SInt16								errCode,
											fileType = 0;
	
			
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);					
		
	if (fileInfoPtr != NULL && fileStreamPtr != NULL)		
		{
				// Check if JPEG2000 formatted image file.
				
		charKeyCode[0] = 0x00;
		charKeyCode[1] = 0x00;
		charKeyCode[2] = 0x00;
		charKeyCode[3] = 0x0C;
		charKeyCode[4] = 0x6A;
		charKeyCode[5] = 0x50;
		charKeyCode[6] = 0x20;
		charKeyCode[7] = 0x20;
		charKeyCode[8] = 0x0D;
		charKeyCode[9] = 0x0A;
		charKeyCode[10] = 0x87;
		charKeyCode[11] = 0x0A;
		
		if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 12) == 0)
			fileType = kJPEG2000Type;
		
		}		// end "if (fileInfoPtr != NULL && fileStreamPtr != NULL)"
		
	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			
					// For now we will not check whether the file is thematic
					// or multispectral
					
			if (formatOnlyCode == kMultiFiles)
				fileInfoPtr->thematicType = FALSE;
			
			else if (formatOnlyCode == kThematicFiles)
				fileInfoPtr->thematicType = TRUE;
				
																			return (noErr);
																								
			}		// end "if (formatOnlyCode != kLoadHeader)"
		
		returnCode = 0; 
		
				// Make sure that the default volume is correct.
				
		errCode = SetVolume (fileStreamPtr, kErrorMessages);
		
		jpeg2000Return = LoadJPEG2000Information (fileInfoPtr);		// jp2_box_t *box;

		if (jpeg2000Return != noErr)
			{
					// Display an alert indicating that MultiSpec could not read or had
					// problems reading this HDF file.
					
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								(SInt16)(IDS_Alert95 + jpeg2000Return - 1),
								0, 
								NULL);
								
			returnCode = 1;
			
			}		// end "if (hdfReturn != noErr)"
			
		if (returnCode == noErr)
			{
			fileInfoPtr->format = fileType;
			
			if (gGetFileImageType == 0)
				{
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else		// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}		// end "if (gGetFileImageType == 0)"
				
			}		// end "if (returnCode > 0)" 
		
		}		// end "if (fileTpye != 0)"  
		
	return (returnCode);
	
}		// end "ReadJPEG2000Header" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadJPEG2000Information
//
//	Software purpose:	This routine reads the file specifications from
//							a HDF formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/07/2009
//	Revised By:			Larry L. Biehl			Date: 01/07/2009

SInt16 LoadJPEG2000Information (
				FileInfoPtr 						fileInfoPtr)

{	
	char									filePathString[512];
	
	char*									filePathPtr;
	char*									pszFormatNamePtr = NULL;
	CMFileStream*						fileStreamPtr; 
	
	int									*paiDepth = NULL, 
											*pabSignedness = NULL;
		
	jas_stream_t						*jasperStreamPtr;
											
	jp2_box_t							*box;
											
	int									iBand,
											iFormat;
											
	UInt32								numberBytes;
	
	SInt16								returnCode;
	
				
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
   
   returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
   												filePathString, 
   												510,
   												&filePathPtr);
	if (returnCode != noErr)	
																							return (returnCode);	
					
				// Load header information about the file.
				// The following code was adapted from the GDAL routine JPEG2000Dataset::Open.

	returnCode = 2;
	jas_init();
	 
	if (!(jasperStreamPtr = jas_stream_fopen (filePathPtr, "rb" )))
		{
		jas_image_clearfmts ();
																							return (returnCode);
		
		}		// end "if (!(sS = jas_stream_fopen (..."

	iFormat = jas_image_getfmt (jasperStreamPtr);
	if (!(pszFormatNamePtr = jas_image_fmttostr (iFormat)))
		{
		jas_stream_close (jasperStreamPtr);
		jas_image_clearfmts ();
																							return (returnCode);
    
		}		// end "if (!(pszFormatName = jas_image_fmttostr (iFormat)))"
		
	if ( strlen (pszFormatNamePtr) < 3 ||
		  (!EQUALN (pszFormatNamePtr, "jp2", 3) &&
         !EQUALN (pszFormatNamePtr, "jpc", 3) &&
         !EQUALN (pszFormatNamePtr, "pgx", 3)) )
		{
//		CPLDebug( "JPEG2000", "JasPer reports file is format type `%s'.", 
//                  pszFormatName );
		jas_stream_close (jasperStreamPtr);
		jas_image_clearfmts ();
																							return (returnCode);
		
		}		// end "if (strlen( pszFormatName ) < 3 ||"

			// Loop through the boxes to get the file description information.
				
		box = 0;
		while ( (box = jp2_box_get (jasperStreamPtr)) )
//		jp2_box_get (jasperStreamPtr);
//		if (box = 0)
			{
			switch (box->type)
				{
				case JP2_BOX_IHDR:
					fileInfoPtr->numberChannels = box->data.ihdr.numcmpts;
					fileInfoPtr->numberColumns = box->data.ihdr.width;
					fileInfoPtr->numberLines = box->data.ihdr.height;
//					CPLDebug( "JPEG2000",
//                          "IHDR box found. Dump: "
//                          "width=%d, height=%d, numcmpts=%d, bpp=%d",
//                          box->data.ihdr.width, box->data.ihdr.height,
//                          box->data.ihdr.numcmpts, (box->data.ihdr.bpc & 0x7F) + 1 );
					if (box->data.ihdr.bpc)
						{
						numberBytes = fileInfoPtr->numberChannels * sizeof(int);
						paiDepth = (int*)MNewPointer (numberBytes);
						pabSignedness = (int*)MNewPointer (numberBytes);
						if (paiDepth != NULL && pabSignedness != NULL)
							{
							for (iBand=0; iBand<fileInfoPtr->numberChannels; iBand++)
								{
								paiDepth[iBand] = (box->data.ihdr.bpc & 0x7F) + 1;
								pabSignedness[iBand] = box->data.ihdr.bpc >> 7;
//								CPLDebug( "JPEG2000",
//                                  "Component %d: bpp=%d, signedness=%d",
//                                  iBand, paiDepth[iBand], pabSignedness[iBand] );

								if (paiDepth[iBand] != paiDepth[0])
									returnCode = 3;
									
								if (pabSignedness[iBand] != pabSignedness[0])
									returnCode = 4;
							
								}		// end "for (iBand=0; iBand<..."

							}		// end "if (paiDepth != NULL && pabSignedness != NULL)"
						
						if (returnCode != 3 && returnCode != 4)
							{
//							fileInfoPtr->numberBytes = numberBytes;
//							fileInfoPtr->signedDataFlag = signedDataFlag;
//							fileInfoPtr->dataTypeCode = dataTypeCode;
//							fileInfoPtr->dataCompressionCode = dataCompressionCode;

							}		// end "if (returnCode != 3 && returnCode != 4)"

						paiDepth = (int*)CheckAndDisposePtr ((Ptr)paiDepth);
						pabSignedness = (int*)CheckAndDisposePtr ((Ptr)pabSignedness);
							
						}		// end "if (box->data.ihdr.bpc)"
		  
                break;

/*				case JP2_BOX_BPCC:
					CPLDebug( "JPEG2000", "BPCC box found. Dump:" );
					if (!paiDepth && !pabSignedness)
						{
						paiDepth = (int *)
                        CPLMalloc( box->data.bpcc.numcmpts * sizeof(int) );
						pabSignedness = (int *)
                        CPLMalloc( box->data.bpcc.numcmpts * sizeof(int) );
						for (iBand = 0; iBand < (int)box->data.bpcc.numcmpts; iBand++)
							{
							paiDepth[iBand] = box->data.bpcc.bpcs[iBand] && 0x7F;
							pabSignedness[iBand] = box->data.bpcc.bpcs[iBand] >> 7;
							CPLDebug ("JPEG2000",
                                  "Component %d: bpp=%d, signedness=%d",
                                  iBand, paiDepth[iBand], pabSignedness[iBand]);
							
							}		// end "for (iBand = 0; iBand < (int)box->data.bpcc.numcmpts; iBand++)"
                
						}		// end "if (!paiDepth && !pabSignedness)"
					break;

				case JP2_BOX_PCLR:
					CPLDebug( "JPEG2000",
                          "PCLR box found. Dump: number of LUT entries=%d, "
                          "number of resulting channels=%d",
                          box->data.pclr.numlutents, box->data.pclr.numchans );
					poDS->nBands = box->data.pclr.numchans;
					if (paiDepth)
						CPLFree( paiDepth );
					if (pabSignedness)
						CPLFree( pabSignedness );
					paiDepth = (int *)
                        CPLMalloc( box->data.pclr.numchans * sizeof(int) );
					pabSignedness = (int *)
                        CPLMalloc( box->data.pclr.numchans * sizeof(int) );
					for (iBand = 0; iBand < (int)box->data.pclr.numchans; iBand++)
						{
						paiDepth[iBand] = box->data.pclr.bpc[iBand] && 0x7F;
						pabSignedness[iBand] = box->data.pclr.bpc[iBand] >> 7;
						CPLDebug ("JPEG2000",
                              "Component %d: bpp=%d, signedness=%d",
                              iBand, paiDepth[iBand], pabSignedness[iBand] );
						
						}		// end "for (iBand = 0; iBand < (int)box->data.pclr.numchans; iBand++)"
                
					break;
*/					
				}		// end "switch (box->type)"
					
			jp2_box_destroy (box);
			box = 0;
				
			}		// end "while ( (box = jp2_box_get(poDS->psStream)) )"
		
	return (returnCode);
	
}		// end "LoadJPEG2000Information" 
/*
jp2_box_t *jp2_box_get(jas_stream_t *in)
{
	jp2_box_t *box;
	jp2_boxinfo_t *boxinfo;
	jas_stream_t *tmpstream;
	uint_fast32_t len;
	uint_fast64_t extlen;
	bool dataflag;

	box = 0;
	tmpstream = 0;
	
	return 0;
	
}
*/