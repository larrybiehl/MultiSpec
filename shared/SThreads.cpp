//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SThreads.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The routines in this file handle the creation and use of
//								threads for reading data in the Mac version. It was never 
//								implemented. More work is needed for this. And what is here
//								now is very out of date.
//
//	Functions in file:	
//
//	Include files:			"MultiSpecHeaders"
//								"SMultiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif

#if defined multispec_win
	#include "SMultiSpec.h"
#endif

//#include "SExtGlob.h"



#define	kNoCreationOptions			0		// Just use the standard default
														// creation options.

#define	kDefaultThreadStackSize		0		// Zero tells the Thread Manager
														// to use the default value.


   
#if defined multispec_mac
struct ExtendedParamBlk 
	{ 
			// PB must be first so that the file system can get the data.
	
	ParamBlockRec 		pb; 
	ThreadTaskRef 		theAppTask; 
	ThreadID 			wakeUpThread;
	 
	}; 
	
typedef struct ExtendedParamBlk ExtendedParamBlk;
typedef ExtendedParamBlk *ExtendedParamBlkPtr;


		// Routine prototypes.
		
//OSErr 			CreateFileIOThreadPool (void);
							 
//#if GENERATINGCFM
#if TARGET_RT_MAC_CFM || TARGET_RT_MAC_MACHO
	pascal void	FileIOCompletionRoutine (ParmBlkPtr pbPtr);
#else
	pascal void	FileIOCompletionRoutine (ParmBlkPtr pbPtr : __A0);
#endif
 
pascal void	FileIOThread (void);
							 
pascal void	WakeUpThread (
				ThreadID								threadToWake);
							
							
                  
		// Completion routines are called with register A0 pointing to 
		// the parameter block.
		
//#if GENERATINGCFM
#if TARGET_RT_MAC_CFM ||TARGET_RT_MAC_MACHO
	inline ExtendedParamBlk*	GetParmBlkPtr (ParmBlkPtr pbPtr)
#else
	inline ExtendedParamBlk*	GetParmBlkPtr (ParmBlkPtr pbPtr : __A0) : __A0
#endif
{
	return ((ExtendedParamBlk*)pbPtr);
}        
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CreateFileIOThreadPool
//
//	Software purpose:	This is a routine in the main thread that creates a
//							pool of threads for file I/O. A check on 6/4/96 indicated
//							that at most 1300 bytes were used by each of the 2 I/O threads.
//							I decided to allow 3072 bytes total for each thread which
//							should be plenty for stack, registers, etc.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 06/11/1996

OSErr CreateFileIOThreadPool (void) 

{  
	#if defined multispec_mac
		return (CreateThreadPool (kCooperativeThread,
											2, 
											0x1000));
											//kDefaultThreadStackSize));
	#endif	// defined multispec_mac

	#if defined multispec_mac_swift
		return (TRUE);
	#endif	// defined multispec_mac_swift

	#if defined multispec_win | defined multispec_lin
		return (TRUE);
	#endif	// defined multispec_win | defined multispec_lin
	
}	// end "CreateFileIOThreadPool"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EndFileIOThread
//
//	Software purpose:	This is a routine in the main thread that ends a thread 
//							that makes I/O calls.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

void EndFileIOThread (
				FileIOInstructionsPtr			fileIOInstructionsPtr) 

{  
	#if defined multispec_mac
		OSErr 			theError;
	
	
		if (fileIOInstructionsPtr->asyncIOThread != 0)
			{
			theError = DisposeThread (fileIOInstructionsPtr->asyncIOThread, nil, TRUE);
			
			fileIOInstructionsPtr->asyncIOThread = 0; 
												
			//if (theError) 
			//	DebugStr ("\p Could not dispose I/O thread"); 
				
			}	// end "if (fileIOInstructionsPtr->asyncIOThread != 0)"
		
		if (fileIOInstructionsPtr->wakeUpThread != 0)
			{
			theError = DisposeThread (fileIOInstructionsPtr->wakeUpThread, nil, TRUE);
			
			fileIOInstructionsPtr->wakeUpThread = 0; 
												
			//if (theError) 
			//	DebugStr ("\p Could not dispose wakeup thread"); 
				
			}	// end "if (fileIOInstructionsPtr->asyncIOThread != 0)"
			
				// Set some FileIOInstruction structure parameters back to
				// initial settings.
				
		InitializeFileIOInstructions (fileIOInstructionsPtr);
	#endif	// defined multispec_mac

	fileIOInstructionsPtr->fileStreamPtr = NULL;
	
}	// end "EndFileIOThread"


   
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FileIOCompletionRoutine
//
//	Software purpose:	This is the completion routine for asynchronous file
//							I/O. The only task that it does is to start up the
//							wakeup thread.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 05/31/1996

//#if GENERATINGCFM
#if TARGET_RT_MAC_CFM || TARGET_RT_MAC_MACHO
	pascal void FileIOCompletionRoutine (ParmBlkPtr pbPtr)
#else
	pascal void FileIOCompletionRoutine (ParmBlkPtr pbPtr : __A0)
#endif
				 
{ 
			// Note that I have been unable to get the pbPtr set up correctly.
			// Therefore I am using a global.
			 
	ExtendedParamBlkPtr 				myAsyncPBPtr;
	ThreadTaskRef 						theAppTaskRef; 
	ThreadID 							theThreadID;  
	ThreadState 						theThreadState; 
	OSErr 								theError;
	
	//ThreadID 							theThreadID2;
	//ThreadTaskRef 					theAppTaskRef2; 
	
	
			// Get the parameter block
			
	myAsyncPBPtr = GetParmBlkPtr (pbPtr);
	
			// Get the data. 
			
	theAppTaskRef = myAsyncPBPtr->theAppTask; 
	theThreadID = myAsyncPBPtr->wakeUpThread;
	/*
	theAppTaskRef2 = fileIOInstructionsPtr->theAppRef; 
	theThreadID2 = fileIOInstructionsPtr->wakeUpThread;
	
	if (theAppTaskRef1 != theAppTaskRef2)
		DebugStr ("\pTaskRef1 not equal to TaskRef2");
	
	if (theAppTaskRef1 != theAppTaskRef2)
		DebugStr ("\ptheThreadID1 not equal to theThreadID2");
	*/
			// See if the thread is stopped yet - just to be sure. 
			
	theError = GetThreadStateGivenTaskRef (theAppTaskRef, theThreadID, &theThreadState);
	
			// If we can get the thread state, go for it!
			
	if (theError == noErr)  
		{	
				// Should be sleeping, mark it for wake up!
				
		if (theThreadState == kStoppedThreadState) 
			{ 
			theError = SetThreadReadyGivenTaskRef (theAppTaskRef, theThreadID);

			::WakeUpProcess (&gProcessSerialNumber);
				
			}	// end "else theThreadState == kStoppedThreadState"
				
		else	// It's awake, the last read must have been immediate. Do
					// not need to wake the wake-up thread.
			{
			theError = 2;

			}	// end "else theThreadState != kStoppedThreadState"
			
		}	// end "if (theError == noErr)"
		
	if (theError != noErr)
		gFileIOInstructions[0].errCode = theError;
				
}	// end "FileIOCompletionRoutine" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void FileIOThread
//
//	Software purpose: This routine contains the code for the thread that makes 
//							the I/O call. It makes an asynchronous I/O call with a 
//							completion routine that will wake up the wake-up thread, 
//							which then wakes up FileIOThread.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 05/31/1996

pascal void FileIOThread (void) 

{ 
	ThreadID								wakeupThreadID, 
											myThreadID; 
	//ThreadTaskRef					theAppRef;
	ExtendedParamBlk					myAsyncPB; 
	OSErr									theError;
	
	
			// Get the ID of IOExampleThread. 
			
	theError = GetCurrentThread (&myThreadID);
	
			// Get the wake-up thread. 
	
	wakeupThreadID = gFileIOInstructions[0].wakeUpThread;
	
	if (theError != noErr)
		{
		gFileIOInstructions[0].doneFlag = TRUE;
		gFileIOInstructions[0].bufferReadyFlag = TRUE;
		gFileIOInstructions[0].errCode = theError;
		
		}	// end "if (theError != noErr)"
		
			// Prepare the I/O parameters that will not change.
			
	myAsyncPB.pb.ioParam.ioCompletion = gFileIOCompletionPtr;
	myAsyncPB.pb.ioParam.ioRefNum = gFileIOInstructions[0].fileStreamPtr->refNum;
	myAsyncPB.pb.ioParam.ioPosMode = fsFromStart;
	myAsyncPB.theAppTask = gFileIOInstructions[0].theAppRef;
	myAsyncPB.wakeUpThread = wakeupThreadID; 
		
	while (!gFileIOInstructions[0].doneFlag)
		{
				// Get the stack space available for this thread.
		
		//UInt32 freeStackSpace;		
		//theError = ThreadCurrentStackSpace (myThreadID, &freeStackSpace);
	
				// HereÕs where you prepare for the I/O call Ñ a simple asynchronous read 
				// command.
				// We also set up the extended data needed by the completion routine Ñ the 
				// thread ID of the wake-up thread and the applicationÕs task reference.
	 
		myAsyncPB.pb.ioParam.ioResult = 0; 		// Initialize the result.
		
		myAsyncPB.pb.ioParam.ioBuffer = 
			(char*)gFileIOInstructions[0].inputBufferPtrs[
															gFileIOInstructions[0].bufferUsedForIO];
			
		myAsyncPB.pb.ioParam.ioReqCount = gFileIOInstructions[0].count; 
		myAsyncPB.pb.ioParam.ioPosOffset = gFileIOInstructions[0].posOff;
		
				// IOExampleThread makes the I/O call and then calls SetThreadState to put 
				// itself to sleep. The first two parameters to SetThreadState indicate 
				// the thread to set, which is FileIOThread, and the state to set it 
				// to Ñ stopped. The kNoThreadID parameter indicates that any ready thread 
				// can run next.
				
		PBReadAsync ((ParmBlkPtr)&myAsyncPB);
		
				// Only suspend this thread if the IO request is queued or executing.
				// If the disk driver only handles synchronous requests, then PBReadAsync
				// will not return until the read has been done.
				
		//if (myAsyncPB.pb.ioParam.ioResult == 1)
		theError = SetThreadState (kCurrentThreadID, kStoppedThreadState, kNoThreadID);
	 
		if (theError == noErr)
			{ 
					// Next the completion routine fires off and tells the Thread Manager 
					// to ready the wake-up thread. The wake-up thread eventually runs and 
					// tells the Thread Manager to ready the I/O thread. Then the I/O 
					// thread awakes and continues running as if nothing happened, 
					// continuing with the rest of the code.
				
			if (gFileIOInstructions[0].errCode == noErr)
				gFileIOInstructions[0].errCode = myAsyncPB.pb.ioParam.ioResult;
			
			}	// end "if (theError == noErr)"
			
		else	// theError != noErr
			{
			if (gFileIOInstructions[0].errCode == noErr) 
				gFileIOInstructions[0].errCode = theError;
			
			}	// end "else theError != noErr"
			
		gFileIOInstructions[0].bufferReadyFlag = TRUE;
		
		if (gFileIOInstructions[0].errCode == noErr)
			{ 
			gFileIOInstructions[0].errCode = SetThreadState (
							kCurrentThreadID, kStoppedThreadState, kNoThreadID);
							//kCurrentThreadID, kStoppedThreadState, kApplicationThreadID);
							
			}	// end "if (gFileIOInstructions[0].errCode == noErr)"
			
		if (gFileIOInstructions[0].errCode != noErr)
			gFileIOInstructions[0].doneFlag = TRUE;
		
		}	// end "while (!gFileIOInstructions[0].doneFlag)"
		
			// Put the wakeup thread back into the IO thread pool.
				
	if (gFileIOInstructions[0].wakeUpThread != 0)
		{
		gFileIOInstructions[0].wakeUpThread = 0;
		theError = DisposeThread (wakeupThreadID, nil, TRUE);
		
		}	// end "if (gFileIOInstructions[0].wakeUpThread != 0)"
	
			//	This thread will be disposed of now by the Thread Manager automatically.
			
	gFileIOInstructions[0].asyncIOThread = 0;
	theError = DisposeThread (myThreadID, nil, TRUE);
	
}	// end "FileIOThread"  
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLineFileIOThread
//
//	Software purpose:	The purpose of this routine is to handle the file IO
//							thread in the GetLine routine.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1999
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

SInt16 GetLineFileIOThread (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr 						fileInfoPtr,
				UInt32								lineNumber, 
				SInt16								channelNumber, 
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32*								numberSamplesPtr, 
				HUCharPtr*							fileIOBufferPtrPtr) 

{ 
	
	UInt32								count,
											posOff;
	
	SInt16								errCode = noErr;
			
	Boolean								endHalfByte;
	
	
	if (!fileIOInstructionsPtr->bufferReadyFlag)
		{
		if (!fileIOInstructionsPtr->fileIOThreadStartedFlag)
			{	
			posOff = (UInt32)GetFilePositionOffset (fileIOInstructionsPtr,
																	fileInfoPtr,
																	lineNumber, 
																	channelNumber, 
																	columnStart, 
																	columnEnd,
																	numberSamplesPtr,
																	&count,
																	&endHalfByte);
													
			fileIOInstructionsPtr->posOff = posOff;
			fileIOInstructionsPtr->count = count;
			fileIOInstructionsPtr->errCode = noErr;
		
					// Get things started. We need to read the first line.
					
			errCode = StartFileIOThread (fileIOInstructionsPtr);
			
			}	// end "if (!fileIOInstructionsPtr->fileIOThreadStartedFlag)"
			
				// Now wait until the requested line has been read into
				// the buffer.
			
		if (errCode == noErr)
			{
			while (!fileIOInstructionsPtr->bufferReadyFlag)
				{
				errCode = YieldToThread (fileIOInstructionsPtr->wakeUpThread);
				
				if (errCode != noErr)
					{
					fileIOInstructionsPtr->bufferReadyFlag = TRUE;
					fileIOInstructionsPtr->errCode = errCode;
					
					}	// end "if (errCode != noErr)"
				
				}	// end "while (!fileIOInstructionsPtr->bufferReadyFlag)"
			
			}	// end "if (errCode == noErr)"
		
				// Set the buffer that is being used for the line of data just
				// read.
					
		*fileIOBufferPtrPtr = fileIOInstructionsPtr->inputBufferPtrs[
															fileIOInstructionsPtr->bufferUsedForIO];
			
		}	// end "if (!fileIOInstructionsPtr->bufferReadyFlag)"
	
			// Get the error code from the last buffer read if no error already.
				
	if (errCode == noErr)
		errCode = fileIOInstructionsPtr->errCode;
		
	if (errCode == noErr)
		{
				// Now start up the read for the next line to be read.
				
		UInt32 nextLineNumber = lineNumber + fileIOInstructionsPtr->lineInterval;
		
		if (nextLineNumber <= (UInt32)fileIOInstructionsPtr->lineEnd)
			{
			posOff = (UInt32)GetFilePositionOffset (fileIOInstructionsPtr,
																	fileInfoPtr,
																	nextLineNumber, 
																	channelNumber, 
																	columnStart, 
																	columnEnd,
																	numberSamplesPtr,
																	&count,
																	&endHalfByte);
													
			fileIOInstructionsPtr->posOff = posOff;
			fileIOInstructionsPtr->count = count;
			fileIOInstructionsPtr->bufferReadyFlag = FALSE;
			fileIOInstructionsPtr->bufferUsedForIO = 
													1 - fileIOInstructionsPtr->bufferUsedForIO;
			
			errCode = SetThreadState (fileIOInstructionsPtr->asyncIOThread, 
												kReadyThreadState, 
												fileIOInstructionsPtr->asyncIOThread);  
												//kNoThreadID); 
			
			if (errCode == noErr)									
				errCode = YieldToThread (fileIOInstructionsPtr->asyncIOThread);
													
			}	// end "if (nextLineNumber <= fileIOInstructionsPtr->lineEnd)"
			
		else	// nextLineNumber > fileIOInstructionsPtr->lineEnd
			{
					// Indicate that the threaded IO series is done.
					
			fileIOInstructionsPtr->doneFlag = TRUE;
		
			}	// end "else nextLineNumber > fileIOInstructionsPtr->lineEnd"
			
		}	// end "if (errCode == noErr)"
		
	return (errCode);
	
}	// end "GetLineFileIOThread"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 StartFileIOThread
//
//	Software purpose:	The purpose of this routine is to start the IO
//							thread the first time.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

SInt16 StartFileIOThread (
				FileIOInstructionsPtr			fileIOInstructionsPtr) 

{ 
	//ThreadID							newCoopID; 
	OSErr									errCode = noErr;
	
	
	#if defined multispec_mac
		if (!fileIOInstructionsPtr->fileIOThreadStartedFlag)
			{
			errCode = SetThreadState (fileIOInstructionsPtr->asyncIOThread, 
												kReadyThreadState, 
												kNoThreadID); 
									
			fileIOInstructionsPtr->fileIOThreadStartedFlag = TRUE;
			
					// Now let the thread start the IO request.
					
			if (errCode == noErr)
				errCode = YieldToThread (fileIOInstructionsPtr->asyncIOThread);
	
			}	// end "if (fileIOInstructionsPtr->asyncIOThread == 0)"
	#endif	// defined multispec_mac
	
			// Return and let the I/O thread do its thing!
		
	return (errCode); 
	
}	// end "StartFileIOThread"


                    
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetupFileIOThread
//
//	Software purpose:	The purpose of this routine is to set up a FileIO thread.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			SetUpFileIOInstructions in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 04/28/2016

SInt16 SetupFileIOThread (
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
	OSErr 			errCode = noErr;
		
		          
	fileIOInstructionsPtr->nextLineToRead = fileIOInstructionsPtr->lineStart;
	fileIOInstructionsPtr->numberLinesAtOnce = 1;
		
	fileIOInstructionsPtr->channelEnd = 0;
	fileIOInstructionsPtr->channelStart = 0;
	
	fileIOInstructionsPtr->fileStreamPtr = GetFileStreamPointer (
																fileIOInstructionsPtr->fileInfoPtr);
		
	#if defined multispec_mac  
				// Initialize thread information in File IO Instructions structure.
				
		InitializeFileIOInstructions (fileIOInstructionsPtr);
		
		if (gUseThreadedIOFlag)
			{
					// Get the task reference for this application.
						 
			errCode = GetThreadCurrentTaskRef (&fileIOInstructionsPtr->theAppRef);
			
					// Create the IO thread.
			
			if (errCode == noErr)
				errCode = NewThread (kCooperativeThread, 
											//(ThreadEntryProcPtr)FileIOThread, 
											(ThreadEntryTPP)FileIOThread,
											nil, 
											0x0c00, 
											kUsePremadeThread+kNewSuspend+kFPUNotNeeded,
											nil, 
											&fileIOInstructionsPtr->asyncIOThread);
											
					// Create the wake-up thread. 
				
			if (errCode == noErr)
				errCode = NewThread (kCooperativeThread, 
											//(ThreadEntryProcPtr)WakeUpThread,
											(ThreadEntryTPP)WakeUpThread, 
											(void*)fileIOInstructionsPtr->asyncIOThread, 
											0x0c00, 
											kUsePremadeThread+kNewSuspend+kFPUNotNeeded,
											nil, 
											&fileIOInstructionsPtr->wakeUpThread);
			
			}	// end "if (gUseThreadedIOFlag)" 
	#endif	// defined multispec_mac
	
	return (errCode);
	
}	// end "SetupFileIOThread"


           
#if defined multispec_mac 
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void WakeUpThread
//
//	Software purpose:	The wake-up thread eventually begins execution and has only 
//							one job Ñ to wake up the thread that made the I/O call.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 06/03/1996
		
pascal void WakeUpThread (
				ThreadID								threadToWake)

{ 
	OSErr									theError;
	//ThreadID							myThreadID;
	
	
	while (!gFileIOInstructions[0].doneFlag)
		{
				// Get the stack space available for this thread.
		
		//UInt32 freeStackSpace;		
		//theError = ThreadCurrentStackSpace (kCurrentThreadID, &freeStackSpace);
	
				// Wake up the specified thread.
		
		theError = SetThreadState (threadToWake, kReadyThreadState, kNoThreadID); 
		if (theError == noErr)
			{ 
					// We've done our deed, so stop and be ready for the next time.
				
			theError = SetThreadState (kCurrentThreadID, 
												kStoppedThreadState, 
												gFileIOInstructions[0].asyncIOThread);
			
			}	// end "if (theError == noErr)"
			
		if (theError != noErr)
			{
			gFileIOInstructions[0].bufferReadyFlag = TRUE;
			gFileIOInstructions[0].doneFlag = TRUE;
			gFileIOInstructions[0].errCode = theError; 

			}	// end "if (theError != noErr)"
			
		}	// end "while (!gFileIOInstructions[0].doneFlag)"
	/*		
			// We should only fall through to here when there is an error condition.
			// Therefore, dispose of this thread.
				
	theError = GetCurrentThread (&myThreadID);
	if (theError == noErr)
		{
		gFileIOInstructions[0].wakeUpThread = 0;
		theError = DisposeThread (myThreadID, nil, TRUE);
		
		}	// end "if (theError == noErr)"
	*/			
}	// end "WakeUpThread" 
#endif	// defined multispec_mac
