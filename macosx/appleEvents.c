//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						appleEvents.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			03/25/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Functions in file:	OSErr 				DisposeSomeDescs
//								OSErr 				DoOpenOrPrintDocument
//								pascal OSErr 		DummyAEHandler
//								OSErr 				GetMissingParameters
//								pascal OSErr 		HandleAEOpenApplication
//								pascal OSErr 		HandleAEOpenDocument
//								pascal OSErr 		HandleAEPrintDocument
//								pascal OSErr		HandleAEQuitApplication
//								OSErr 				SendAEOpenOrPrintDocument
//								OSErr 				SendAEQuitApplication
//
//	Brief description:	The routines in this file handle Apple Events.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"SMulSpec.h"

//#include "SMulSpec.h"
//#include "SExtGlob.h"

extern OSErr					OpenOrPrintSelectedFiles (
										AEDescList*							docListPtr, 
										Boolean								printFlag);
											
extern pascal OSErr 			HandleAEOpenApplication (
										const AppleEvent*					appleEventPtr, 
										AEDescList*							replyPtr, 
										SInt32	 							handlerRefcon);
											
extern pascal OSErr 			HandleAEOpenDocument (
										const AppleEvent*					appleEventPtr, 
										AEDescList*							replyPtr, 
										SInt32								handlerRefcon);
											
extern pascal OSErr 			HandleAEPrintDocument (	
										const AppleEvent*					appleEventPtr, 
										AEDescList*							replyEventPtr, 
										SInt32	 							handlerRefcon);
											
extern pascal OSErr			HandleAEQuitApplication (
										const AppleEvent*					appleEventPtr, 
										AEDescList*							replyPtr, 
										SInt32				 				handlerRefcon);


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.											

OSErr 							DisposeSomeDescs(	AEDesc* desc1Ptr, AEDesc* desc2Ptr, 
															AEDesc* desc3Ptr, AEDesc* desc4Ptr, 
															AEDesc* desc5Ptr);
															
OSErr 							DoOpenOrPrintDocument (AppleEvent* appleEventPtr, 
															Boolean printFlag);

OSErr 							GetMissingParameters (AppleEvent* theAppleEventPtr);
											
void 								InitSomeDescs (AEDesc* desc1Ptr, AEDesc* desc2Ptr, 
														AEDesc* desc3Ptr, AEDesc* desc4Ptr, 
														AEDesc* desc5Ptr);


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal OSErr DummyAEHandler
//
//	Software purpose:	This routine disposes of a set of descriptors.  The 
//							inputs are pointers to the descriptors. If one or more 
//							of the pointers is NULL, then the routine ignores all the 
//							inputs after that one (they should be NULL, also).  So, 
//							for example, if desc3Ptr is NULL,  only the descriptors 
//							pointed to by desc1Ptr and desc2Ptr are disposed of.  
//							This routine came from Apple's Developer CD Series Disk 
//							(March 1993).
//
//	Parameters in:		Pointers to up to 5 descriptors.
//
//	Parameters out:	None
//
// Value Returned:	The error code (noErr if none).  If any of the 
//							AEDisposeDesc calls returns an error, DisposeSomeDesc 
//							will return the first such error generated; but it will 
//							continue to (try to) dispose of all the input descriptors.
//					
//	NOTES:	**WARNING** this routine can be used to dispose of both
//				local AEDesc's (local to the caller) and/or AEDesc's the caller
//				is creating & returning for some higher routine to use (such desc's
//				generally show up as VAR inputs in the calling routine).  You ONLY 
//				dispose of these VAR input desc's to clean up when your routine 
//				generates an error.  Thus many routines that call DisposeSomeDescs 
//				will contain two calls to it, one to be used when the call succeeds 
//				(to get rid of any local desc's that may have been created) and one 
//				to be used when the call fails (to get rid of both local and return 
//				desc's that may have been created).  If you intialize all desc's to 
//				the null desc at the start of your routine (using InitSomeDescs), 
//				you won't have to keep track of which ones you have already created 
//				when you hit an error.  But you do have to distinguish between local 
//				desc's and return desc's.	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/03/93
//	Revised By:			Larry L. Biehl			Date: 09/03/93	

OSErr DisposeSomeDescs(
				AEDesc*								desc1Ptr, 
				AEDesc*								desc2Ptr, 
				AEDesc*								desc3Ptr, 
				AEDesc*								desc4Ptr, 
				AEDesc*								desc5Ptr)
									
{
	Boolean								continueFlag;

	OSErr									returnError,
											tempError;
					
					
	returnError = noErr;
	tempError = noErr;
	
	continueFlag = (desc1Ptr != NULL);
	if ( continueFlag )
  		returnError = AEDisposeDesc(desc1Ptr);
  
	if (continueFlag)
		{
		continueFlag = (desc2Ptr != NULL);
		if ( continueFlag )
			tempError = AEDisposeDesc(desc2Ptr);
		if (returnError == noErr)
			returnError = tempError;
			
		}		// end "if (continueFlag)" 
  
	if (continueFlag)
		{
		continueFlag = (desc3Ptr != NULL);
		if ( continueFlag )
			tempError = AEDisposeDesc(desc3Ptr);
		if (returnError == noErr)
			returnError = tempError;
			
		}		// end "if (continueFlag)" 
  
	if (continueFlag)
		{
		continueFlag = (desc4Ptr != NULL);
		if ( continueFlag )
			tempError = AEDisposeDesc(desc4Ptr);
		if (returnError == noErr)
			returnError = tempError;
			
		}		// end "if (continueFlag)" 
  
	if (continueFlag)
		{
		continueFlag = (desc5Ptr != NULL);
		if ( continueFlag )
			tempError = AEDisposeDesc(desc5Ptr);
		if (returnError == noErr)
			returnError = tempError;
			
		}		// end "if (continueFlag)" 
  
	return (returnError);
	
}		// end "DisposeSomeDescs" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal OSErr DummyAEHandler
//
//	Software purpose:	This is a dummy handler that receives Apple events which 
//							are not handled.  This routine came from Apple's Developer
//							CD Series Disk (March 1993).  It is mainly provided as 
//							a stub in order to support all the events in the
//							required suite.  This is the handler for the	Open Document 
//							and Print Document events (required suite).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			InitializeAEHandlers in intialize.c
//
//	Coded By:			Larry L. Biehl			Date: 08/23/93
//	Revised By:			Larry L. Biehl			Date: 08/23/93	

pascal OSErr DummyAEHandler(
				AppleEvent* 						theAppleEventPtr, 
				AppleEvent* 						replyPtr, 
				SInt32								refCon)

{
	return (errAEEventNotHandled);
	
}		// end "DummyAEHandler" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSErr GetMissingParameters
//
//	Software purpose:	This routine check to see if there exists any additional 
//							parameters in the Apple Event.  If so, an error has 
//							occurred and we terminate the application.  This routine 
//							came from Apple's Developer CD Series Disk (March 1993).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			InitializeAEHandlers in intialize.c
//
//	Coded By:			Larry L. Biehl			Date: 08/23/93
//	Revised By:			Larry L. Biehl			Date: 08/23/93	

OSErr GetMissingParameters (
				AppleEvent*							theAppleEventPtr)

{
	DescType								theType;
	Size									actualSize;
	OSErr									err;
	
	
	err = AEGetAttributePtr(theAppleEventPtr, keyMissedKeywordAttr, typeWildCard,
								&theType, nil, 0, &actualSize);
	if (err == errAEDescNotFound)
		return(noErr);
	else
		return(errAEEventNotHandled);
		
}		// end "GetMissingParameters" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal OSErr HandleAEOpenApplication
//
//	Software purpose:	This routine handles the Quit Application Apple Event.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/23/93
//	Revised By:			Larry L. Biehl			Date: 08/23/93	

pascal OSErr HandleAEOpenApplication (
				const AppleEvent*					appleEventPtr, 
				AEDescList*							replyPtr, 
				SInt32								handlerRefcon)

{
	OSErr									err;
	
	
	err = GetMissingParameters ((AppleEvent*)appleEventPtr);
	
	return (err);
	
}		// end "HandleAEOpenApplication" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal OSErr HandleAEOpenDocument
//
//	Software purpose:	This routine handles the Open Document Apple Event.
//							The routine gets a list of files to be openned and
//							opens them up.   This routine came from Apple's 
//							Developer CD Series Disk (March 1993).
//
//  NOTES:	the official definition of the OpenDocs event says that
//  			it takes a list of alias records.  I ask for them as FSSpec's
//				instead; it's more convenient for me, and the AE Manager will
//				do the coercion automatically
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/01/93
//	Revised By:			Larry L. Biehl			Date: 09/08/93	

pascal OSErr HandleAEOpenDocument (
				const AppleEvent*					appleEventPtr, 
				AEDescList*							replyPtr, 
				SInt32								handlerRefcon)

{	

	SInt16								returnCode;
		
		
	returnCode = DoOpenOrPrintDocument ((AppleEvent*)appleEventPtr, FALSE);
	
	return (returnCode);
	
}		// end "HandleAEOpenDocument" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSErr DoOpenOrPrintDocument
//
//	Software purpose:	This routine handles the Open Document Apple Event.
//							The routine gets a list of files to be openned and
//							opens them up.   This routine came from Apple's 
//							Developer CD Series Disk (March 1993).
//
//  NOTES:	the official definition of the OpenDocs event says that
//  			it takes a list of alias records.  I ask for them as FSSpec's
//				instead; it's more convenient for me, and the AE Manager will
//				do the coercion automatically
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/09/93
//	Revised By:			Larry L. Biehl			Date: 09/10/93	

OSErr DoOpenOrPrintDocument (
				AppleEvent*							appleEventPtr, 
				Boolean								printFlag)

{	
	AEDescList							docList;
//	AEKeyword							keyword;
//	DescType								returnedType;
//	Size									actSize;
	
//	SInt32								itemCount;

//	short int							index,
//											projectIndex,
	SInt16								returnCode,
											tempCode;
							
					
  	docList = gNullDesc;
  
  			// Get the direct object, which is a list of things to be 			
  			// opened.																			
  		
	returnCode = AEGetParamDesc(
				appleEventPtr, keyDirectObject, typeAEList, (AEDesc*)&docList);
  
  			// Check for missing required parameters.									
  	
	if (returnCode == noErr)
		returnCode = GetMissingParameters(appleEventPtr);
		
	if (returnCode == noErr)
		returnCode = OpenOrPrintSelectedFiles (&docList, printFlag);
/*	
			// Count the items in the list.												
			
	if (returnCode == noErr)
		returnCode = AECountItems(&docList, &itemCount);
	
			// For each item, open the associated document, if possible.		
			
	index = 1;
	projectIndex = itemCount + 1;
	if (returnCode == noErr && itemCount > 0)
		while (index <= itemCount)
			{
					// Get the file.															
				
	  		returnCode = AEGetNthPtr(	&docList,
	  									index,
	  									typeFSS,
	  									&keyword,
	  									&returnedType,
	  									(Ptr)&gFileFSSpec,
	  									sizeof(FSSpec),
	  									&actSize);
	  									
	  		if (returnCode == noErr)
	  			returnCode = FSpGetFInfo (&gFileFSSpec, &gFinderInfo);
	  			
	  		if (returnCode == noErr)
	  			{
				gLocalAppFile.vRefNum = gFileFSSpec.vRefNum;
				gLocalAppFile.parID = gFileFSSpec.parID;
				gLocalAppFile.fileName[0] = 0;
				gLocalAppFile.type = gFinderInfo.fdType;
				gLocalAppFile.refNum = 0;
				gLocalAppFile.fSSpecFlag = TRUE;
				gLocalAppFile.fNamePtr = (Str63*)&gFileFSSpec.name;
				
				if (index == projectIndex)
					{
					ForceProjectSegmentsToBeLoaded ();
			
					if (gMemoryTypeNeeded == 0)
						{
						OpenProjectFile (&gLocalAppFile);
		
						if (gProjectInfoPtr != NULL && gProjectWindow == NULL)
							CreateProjectWindow ();

						}		// end "if (gMemoryTypeNeeded == 0)"
						
					index = projectIndex = itemCount + 2;
					
					}		// end "if (index == projectIndex)" 
				
				else		// index != projectIndex 	
					returnCode = OpenMultiSpecDocument (&gLocalAppFile);
					
				if (returnCode > 0 && projectIndex == itemCount+1)
					projectIndex = index;
					
				if (gMemoryError != 0)
					projectIndex = index = itemCount + 1;
					
				if (gMemoryError == 0 && returnCode == 0 && printFlag)
					{
					DoPageSetup (gTheActiveWindow);										
							
					DoPrint (TRUE, gTheActiveWindow);
					
					}		// end "if (gMemoryError == 0 && ..." 
					
				returnCode = 0;
					
				}		// end "if (returnCode == noErr)" 
			
			index++;
			if (index > itemCount && !printFlag)
				index = projectIndex;
			
			}		// end "while (index <= itemCount)" 
			
	if (returnCode == noErr && itemCount == 0)
		{
		if (printFlag)
			DoPrint (TRUE, gTheActiveWindow);
			
		else		// !printFlag 
			returnCode = OpenImageFile (NULL);
			
		}		// end "if (returnCode == noErr && itemCount == 0)" 
*/
	tempCode = AEDisposeDesc(&docList);
	
	return (returnCode);
	
}		// end "DoOpenOrPrintDocument" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal OSErr HandleAEPrintDocument
//
//	Software purpose:	This routine handles the Print Document Apple Event.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/09/93
//	Revised By:			Larry L. Biehl			Date: 09/09/93	

pascal OSErr HandleAEPrintDocument (
				const AppleEvent*					appleEventPtr, 
				AEDescList*							replyEventPtr, 
				SInt32								handlerRefcon)

{
	OSErr									returnCode;
							
				
	returnCode = noErr;
	
	gMemoryTypeNeeded = 1;
	ForcePrintSegmentLoad ();
	
	if (gMemoryTypeNeeded == 0)	
		returnCode = DoOpenOrPrintDocument ((AppleEvent*)appleEventPtr, TRUE);
	
	gMemoryTypeNeeded = 0;
	
	return (returnCode);

}		// end "HandleAEPrintDocument" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal OSErr HandleAEQuitApplication
//
//	Software purpose:	This routine handles the Quit Application Apple Event.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/23/93
//	Revised By:			Larry L. Biehl			Date: 08/23/93	

pascal OSErr HandleAEQuitApplication (
				const AppleEvent*					appleEventPtr, 
				AEDescList*							replyEventPtr, 
				SInt32								handlerRefcon)
				
{
	OSErr									returnError;
	
	
	if ( !(returnError = GetMissingParameters ((AppleEvent*)appleEventPtr)) )
		{
		ExitMultiSpec (FALSE);
	
		if ( !gDoneFlag )
			returnError = 1;
			
		}		// end "if ( !(returnError = GetMissingParams(appleEventPtr)) )" 
		
	return (returnError);

}		// end "HandleAEQuitApplication" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitSomeDescs
//
//	Software purpose:	This routine sets the input descriptors to the null 
//							descriptor.  The inputs are pointers to the descriptors.  
//							If one or more of the pointers is NULL, then the routine 
//							ignores all the inputs after that one (they should be NULL,
//							also).  So, for example, if desc3Ptr is NIL, only the 
//							descriptors pointed to by desc1Ptr and desc2Ptr are
//  						initialized.
//
//	Parameters in:		Apple Event descriptors (up to 5).
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/03/93
//	Revised By:			Larry L. Biehl			Date: 09/03/93	

void InitSomeDescs (
				AEDesc*								desc1Ptr,
				AEDesc*								desc2Ptr, 
				AEDesc*								desc3Ptr, 
				AEDesc*								desc4Ptr, 
				AEDesc*								desc5Ptr)
								
{
	if (desc1Ptr == NULL)
																					return;
	*desc1Ptr = gNullDesc;
	
	if (desc2Ptr == NULL)
																					return;
	*desc2Ptr = gNullDesc;
	
	if (desc3Ptr == NULL)
																					return;
	*desc3Ptr = gNullDesc;
	
	if (desc4Ptr == NULL)
																					return;
	*desc4Ptr = gNullDesc;
	
	if (desc5Ptr == NULL)
																					return;
	*desc5Ptr = gNullDesc;
	
	return;

}		// end "InitSomeDescs" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSErr OpenSelectedFiles
//
//	Software purpose:	This routine handles the Open Document Apple Event.
//							The routine gets a list of files to be openned and
//							opens them up.   This routine came from Apple's 
//							Developer CD Series Disk (March 1993).
//
//  NOTES:	the official definition of the OpenDocs event says that
//  			it takes a list of alias records.  I ask for them as FSSpec's
//				instead; it's more convenient for me, and the AE Manager will
//				do the coercion automatically
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/23/2001
//	Revised By:			Larry L. Biehl			Date: 03/25/2017

OSErr OpenOrPrintSelectedFiles (
				AEDescList*							docListPtr, 
				Boolean								printFlag)

{	
	#if TARGET_API_MAC_CARBON		
		CFStringRef							cfStringRef;
		FSCatalogInfo						fsCatalogInfo;
		FSRef 								fileAsFSRef;
	#endif 		// TARGET_API_MAC_CARBON
	
	AEKeyword							keyword;
	DescType								returnedType;
	Size									actSize;
	
	SInt32								itemCount;

	SInt16								index,
											projectIndex,
											returnCode;
	
			// Count the items in the list.												
			
	returnCode = AECountItems(docListPtr, &itemCount);
	
			// For each item, open the associated document, if possible.		
			
	index = 1;
	projectIndex = itemCount + 1;
	if (returnCode == noErr && itemCount > 0)
		while (index <= itemCount)
			{
					// Get the file.	
						
			if (gOSXFlag)
				{
				#if TARGET_API_MAC_CARBON	
					returnCode = AEGetNthPtr (docListPtr, 
														index, 
														typeFSRef, 
														NULL, 
														NULL, 
														&fileAsFSRef, 
														sizeof(FSRef),
														NULL);
				  			
					if (returnCode == noErr)
						{
						gLocalAppFile.fSRefFlag = TRUE;
						gLocalAppFile.fSSpecFlag = FALSE;
						gLocalAppFile.fsRef = fileAsFSRef;
						
						returnCode = FSGetCatalogInfo (&gLocalAppFile.fsRef,
																	kFSCatInfoVolume,
																	&fsCatalogInfo,
																	&gLocalAppFile.uniFileName,
																	NULL,
																	&gLocalAppFile.parentFSRef);

						if (returnCode == noErr)
							{						
							gLocalAppFile.vRefNum = fsCatalogInfo.volume;
																
							cfStringRef = CFStringCreateWithCharacters (
																kCFAllocatorDefault,
																gLocalAppFile.uniFileName.unicode,
																gLocalAppFile.uniFileName.length);
																
							gLocalAppFile.fileName[0] = gLocalAppFile.uniFileName.length;
							CFStringGetCString (cfStringRef,
													(char*)&gLocalAppFile.fileName[1],
													(CFIndex)255,
													kCFStringEncodingUTF8);
							gLocalAppFile.fileName[0] = strlen ((char*)&gLocalAppFile.fileName[1]);
																
							//CFAllocatorDeallocate (kCFAllocatorDefault, (void*)cfStringRef);
							CFRelease (cfStringRef);
														
							gLocalAppFile.fNamePtr = gLocalAppFile.fileName;
							
							}		// end "if (returnCode == noErr)"
							
						}		// end "if (returnCode == noErr)"
				#endif		// TARGET_API_MAC_CARBON
				
				}
				
			else		// !gOSXFlag
				{	
				returnCode = AEGetNthPtr (docListPtr,
													index,
													typeFSS,
													&keyword,
													&returnedType,
													(Ptr)&gFileFSSpec,
													sizeof(FSSpec),
													&actSize);
											
				if (returnCode == noErr)
					returnCode = FSpGetFInfo (&gFileFSSpec, &gFinderInfo);
					
				if (returnCode == noErr)
					{
					gLocalAppFile.vRefNum = gFileFSSpec.vRefNum;
					gLocalAppFile.parID = gFileFSSpec.parID;
					gLocalAppFile.fileName[0] = 0;
					gLocalAppFile.type = gFinderInfo.fdType;
					gLocalAppFile.refNum = 0;
					gLocalAppFile.fSSpecFlag = TRUE;
					gLocalAppFile.fNamePtr = gFileFSSpec.name;
						
					}		// end "if (returnCode == noErr)"
			
				}		// end "else !gOSXFlag"
					
			if (returnCode == noErr)
				{
				if (index == projectIndex)
					{
					ForceProjectSegmentsToBeLoaded ();
			
					if (gMemoryTypeNeeded == 0)
						{
						gProcessorCode = kOpenImageFileProcessor;
						OpenProjectFile (&gLocalAppFile);
						gProcessorCode = 0;
		
						if (gProjectInfoPtr != NULL && gProjectWindow == NULL)
							CreateProjectWindow ();

						}		// end "if (gMemoryTypeNeeded == 0)"
						
					index = projectIndex = itemCount + 2;
					
					}		// end "if (index == projectIndex)" 
				
				else		// index != projectIndex 	
					returnCode = OpenMultiSpecDocument (&gLocalAppFile);
					
				if (returnCode > 0 && projectIndex == itemCount+1)
					projectIndex = index;
					
				if (gMemoryError != 0)
					projectIndex = index = itemCount + 1;
					
				if (gMemoryError == 0 && returnCode == 0 && printFlag)
					{
					DoPageSetup (gTheActiveWindow);										
							
					DoPrint (TRUE, gTheActiveWindow);
					
					}		// end "if (gMemoryError == 0 && ..." 
					
				returnCode = 0;
					
				}		// end "if (returnCode == noErr)" 
			
			index++;
			if (index > itemCount && !printFlag)
				index = projectIndex;
			
			}		// end "while (index <= itemCount)" 
			
	if (returnCode == noErr && itemCount == 0)
		{
		if (printFlag)
			DoPrint (TRUE, gTheActiveWindow);
			
		else		// !printFlag 
			returnCode = OpenImageFile (NULL);
			
		}		// end "if (returnCode == noErr && itemCount == 0)" 
		
	return (returnCode);

}		// end "OpenSelectedFiles" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSErr SendAEOpenOrPrintDocument
//
//	Software purpose:	This routine sends an Open Document Apple Event or
//							Print Document Apple Event to MultiSpec.  A NULL 
//							FSSpec is sent.  This will force the open document 
//							or print document handler to make a call to allow
//							the user to select the file.
//
//	Parameters in:		file spec for file to be opened
//
//	Parameters out:	None
//
// Value Returned:	Error number returned by Apple Event routines.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/03/93
//	Revised By:			Larry L. Biehl			Date: 09/10/93	

OSErr SendAEOpenOrPrintDocument (
				FSSpec*								fileFSSpecPtr, 
				AEEventID							theAEEventID)

{
	AppleEvent							myAppleEvent,
											replyEvent;
						
	AEDescList							docList;
							
	OSErr									tempError,
											returnError;
	
	
	returnError = noErr;
	
	InitSomeDescs(&myAppleEvent, &replyEvent, &docList, NULL, NULL);
  
 			// Create an empty list.													
 		
	returnError = AECreateList(NIL, 0, FALSE, &docList);
  
			// Add the file spec to the list.										
	
	if (returnError == noErr && fileFSSpecPtr != NULL)	
		returnError = AEPutPtr(
					&docList, 1, typeFSS, (Ptr)fileFSSpecPtr, sizeof(FSSpec));
	  
			// Create the event.															
			
	if (returnError == noErr)
		returnError = AECreateAppleEvent(	kCoreEventClass,
													theAEEventID,
													&gSelfAddressDescription,
													kAutoGenerateReturnID,
													kAnyTransactionID,
													&myAppleEvent);
	
			// Add list to the event.													
			
	if (returnError == noErr)
		returnError = AEPutParamDesc(&myAppleEvent, keyDirectObject, &docList);

			// Send the event.															
			
	if (returnError == noErr)
		returnError = AESend(&myAppleEvent, 
									&replyEvent, 
									kAENoReply+kAEAlwaysInteract, 
									kAENormalPriority,
									kAEDefaultTimeout,
									NIL,
									NIL);
	  
	tempError = DisposeSomeDescs(&myAppleEvent, 
											&replyEvent, 
											&docList, 
											NULL, 
											NULL);
											
	return (returnError);
  	
}		// end "SendAEOpenOrPrintDocument" 
	


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				OSErr SendAEQuitApplication
//
//	Software purpose:	This routine sends a Quit Application Apple Event to
//							quit MultiSpec.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/01/93
//	Revised By:			Larry L. Biehl			Date: 09/01/93	

OSErr SendAEQuitApplication(void)

{
	AppleEvent							myAppleEvent, 
											reply;
							
	OSErr									returnError;
	
	
	returnError = noErr;
	
	if (gAppleEventsFlag)
		{
				//	Create the Apple Event.												
		
		returnError = AECreateAppleEvent(
										kCoreEventClass, 
										kAEQuitApplication, 
										&gSelfAddressDescription,
										kAutoGenerateReturnID, 
										kAnyTransactionID, 
										&myAppleEvent);
		
				// Send the Apple Event.												
				
		if (returnError == noErr)
			{
			returnError = AESend(&myAppleEvent, 
										&reply, 
										kAENoReply+kAECanInteract, 
										kAENormalPriority,
										kAEDefaultTimeout, 
										nil, 
										nil);	
										
	  				// Dispose of the Apple Event.									
	  			
	  		AEDisposeDesc(&myAppleEvent);
	  		
	  		}		// end "if (returnError == noErr)" 
	  		
	  	}		// end "gAppleEventsFlag" 
	  	
	else		// !gAppleEventsFlag 	
		ExitMultiSpec (FALSE);
  		
  	return (returnError);			
  	
}		// end "SendAEQuitApplication" 

