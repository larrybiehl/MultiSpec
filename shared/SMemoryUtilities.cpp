//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							c Purdue Research Foundation
//									All rights reserved.
//
//	File:						SMemoryUtilities.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/19/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec.
//
//	Functions in file:	void 						CheckAndDisposePtr
//								void 						CheckAndUnlockHandle
//								Boolean 					CheckHandleSize
//								Boolean 					CheckMemoryForColorPicker
//								Boolean 					CheckPointerSize
//								void 						CheckSizeAndUnlockHandle
//								Boolean 					GetIOBufferPointers
//								SignedByte 				MHGetState
//								void 						MHSetState
//								Handle 					MNewHandle
//								Ptr 						MNewPointer
//								Ptr 						MoveHiAndLock
//								void						ReleaseSpareMemoryForWarningMessage
//								void						UnlockAndDispose
//
/*
	int numberChars = sprintf ((char*)gTextString3,
			" SMemoryUtilities: (xDataMin, xDataMax): %s",
			gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"  

#if defined multispec_lin
	#include "SMultiSpec.h"
	
			// Temporary definitions
	#include <stdlib.h>
	#include "wx/filefn.h"
	#define memsize sizeof (void*)
	typedef Byte onebyte;
#endif

#if defined multispec_mac
	#include <sys/sysctl.h>
	#include <mach/host_info.h>
	#include <mach/mach_host.h>
	#include <mach/task_info.h>
	#include <mach/task.h>
#endif	// defined multispec_mac    

#if defined multispec_win
	#include "CImageWindow.h"
#endif	// defined multispec_win

//#include "SExtGlob.h" 

#if defined multispec_lin
			// Prototypes for linux routines
	void freememory (void* ptr);
	void *memorycopy (void *output, void *input);
	void *memallocate (UInt32 bytesneeded);
	void *memreallocate (UInt32 bytesneeded, void *handle);
#endif



Boolean			CheckIfBytesRequestedAreWithinLimit (
						SInt64								bytesRequested);

void				OSXMemoryMessage (
						SInt64								bytesRequested);
						
						
#if defined multispec_mac_swift
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void BlockMoveData
//
//	Software purpose:	The purpose of this routine is to dispose of the
//							memory for the input pointer if it is not NULL.  The
//							pointer will be set to NULL.
//
//	Parameters in:		Pointer to be disposed of.
//
//	Parameters out:	None
//
// Value Returned:	NULL pointer value			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017	

void BlockMoveData (
				const void*							inStringPtr,
				void*									outStringPtr,
				UInt32								numberBytes)
{
	if (inStringPtr != NULL && outStringPtr != NULL) 
		memmove (outStringPtr, inStringPtr, numberBytes);

}	// end "BlockMoveData"
#endif



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckAndDisposePtr
//
//	Software purpose:	The purpose of this routine is to dispose of the
//							memory for the input pointer if it is not NULL.  The
//							pointer will be set to NULL.
//
//	Parameters in:		Pointer to be disposed of.
//
//	Parameters out:	None
//
// Value Returned:	NULL pointer value			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/18/1991
//	Revised By:			Larry L. Biehl			Date: 08/11/1995	

Ptr CheckAndDisposePtr (
				Ptr									pointer)

{		                 
	if (pointer != NULL)
		{ 
		#if defined multispec_mac
			DisposePtr (pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return (NULL); 
	
}	// end "CheckAndDisposePtr"



float* CheckAndDisposePtr (
				float*								pointer)

{		                   
	if (pointer != NULL) 
		{
		#if defined multispec_mac
			DisposePtr ((Ptr)pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return ((float*)NULL);
	
}	// end "CheckAndDisposePtr" 
 	


double* CheckAndDisposePtr (
				double*								pointer)

{		                    
	if (pointer != NULL)
		{ 
		#if defined multispec_mac
			DisposePtr ((Ptr)pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return ((double*)NULL);
	
}	// end "CheckAndDisposePtr" 



UInt8* CheckAndDisposePtr (
				UInt8*								pointer)

{		                   
	if (pointer != NULL)
		{ 
		#if defined multispec_mac
			DisposePtr ((Ptr)pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return ((UInt8*)NULL);
	
}	// end "CheckAndDisposePtr" 



SInt16* CheckAndDisposePtr (
				SInt16*								pointer)

{		                    
	if (pointer != NULL)
		{ 
		#if defined multispec_mac
			DisposePtr ((Ptr)pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return ((SInt16*)NULL);
	
}	// end "CheckAndDisposePtr" 



UInt16* CheckAndDisposePtr (
				UInt16*								pointer)

{		                    
	if (pointer != NULL)
		{ 
		#if defined multispec_mac
			DisposePtr ((Ptr)pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return ((UInt16*)NULL);
	
}	// end "CheckAndDisposePtr" 
 	

SInt32* CheckAndDisposePtr (
				SInt32*								pointer)

{		                   
	if (pointer != NULL)
		{ 
		#if defined multispec_mac
			DisposePtr ((Ptr)pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return ((SInt32*)NULL);
	
}	// end "CheckAndDisposePtr" 
 	

UInt32* CheckAndDisposePtr (
				UInt32*								pointer)

{		                   
	if (pointer != NULL)
		{ 
		#if defined multispec_mac
			DisposePtr ((Ptr)pointer);
		#endif	// defined multispec_mac
		
		#if defined multispec_win  
			GlobalFreePtr (pointer);
		#endif	// defined multispec_win 
		
		#if defined multispec_lin
			//delete [] pointer;
			freememory ((void *)pointer);
		#endif // defined multispec_lin

		}	// end "if (pointer != NULL)"
		
	return ((UInt32*)NULL);
	
}	// end "CheckAndDisposePtr"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckAndUnlockHandle
//
//	Software purpose:	The purpose of this routine is to unlock the handle if
//							the handle is not NULL.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			"several routines"
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1991
//	Revised By:			Larry L. Biehl			Date: 07/07/1998	

void CheckAndUnlockHandle (
				Handle								handle)

{      
			// If handle is not NULL unlock it.
			
	if (handle != NULL)
		{
		#if defined multispec_mac 
			HUnlock (handle); 
		#endif	// defined multispec_mac 
		      
		#if defined multispec_win  
					// Loop until lock count is 0.
					  
			Boolean returnFlag = TRUE;
			while (returnFlag)
				returnFlag = (Boolean)GlobalUnlock (handle);
				                          
			//int flags = GlobalFlags (handle);
			//int lockCount = (flags & GMEM_LOCKCOUNT);
		#endif	// defined multispec_win
		
		#if defined multispec_lin
			// Do nothing
		#endif

		}	// end "if (handle != NULL)"
	
}	// end "CheckAndUnlockHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckHandleSize
//
//	Software purpose:	The purpose of this routine is to check the size
//							of the handle against the bytes needed.  More 
//							memory is obtained for the handle if needed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	FALSE if memory is too full to satisfy the request.
//							TRUE if request is okay.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/16/1988
//	Revised By:			Larry L. Biehl			Date: 06/17/2015	

Ptr CheckHandleSize (
				Handle*								inputHandlePtr, 
				Boolean*								continueFlagPtr, 
				Boolean*								changedFlagPtr, 
				SInt64								bytesNeeded)

{
	Ptr									outputPtr;
	Handle								handle;
	SInt32								sizeHandle;
	Boolean								returnFlag;
	
	
	returnFlag = FALSE;
	*changedFlagPtr = FALSE;
	outputPtr = NULL;
	
	if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))
		{						
		handle = *inputHandlePtr;
		#ifndef multispec_lin	
			if (handle != NULL)
				{
						// Get size of handle														
						
				sizeHandle = MGetHandleSize (handle);
				returnFlag = (sizeHandle >= 0);
			
				if ((sizeHandle < (SInt32)bytesNeeded) && returnFlag)
					MSetHandleSize (&handle, bytesNeeded);
				
				#if defined multispec_mac 	
					if ((sizeHandle != (SInt32)bytesNeeded) && returnFlag)  
						*changedFlagPtr = TRUE;
				#endif	// defined multispec_mac  
						
				#if defined multispec_win  	
					if ((sizeHandle < (SInt32)bytesNeeded) && returnFlag)  
						*changedFlagPtr = TRUE;
				#endif	// defined multispec_mac  
					
				}	// end "if (handle != NULL)" 
			
			else	// handle == NULL 
				{	
				handle = MNewHandle (bytesNeeded); 
				*changedFlagPtr = TRUE;
				
						// Check if memory is full.													
						
				returnFlag = (handle != NULL);
				
				}	// end "else handle == NULL" 
					
			*inputHandlePtr = handle;
			
					// Get handle pointer if memory not too full and handle is not NULL 	
			
			if (returnFlag && handle != NULL)
				outputPtr = GetHandlePointer (handle, kLock, kNoMoveHi);
		#endif // if not define multispec_lin
			
		#if defined multispec_lin
			Handle	newHandle;
			
			if (handle != NULL)
				{
				if (getsize (handle) < bytesNeeded)
					{
					newHandle = memreallocate (bytesNeeded, handle);
					*changedFlagPtr = TRUE;
					
					}	// end "if (getsize (handle) < bytesNeeded)"
					
				else	// getsize (handle) >= bytesNeeded
					{
							// Already have enough memory so do nothing
					
					//outputPtr = (Ptr)memallocate (bytesNeeded, handle);
					newHandle = handle;
					
					}	// end "else getsize (handle) >= bytesNeeded"
				
				}	// end "if (handle != NULL)"
				
			else //handle == NULL
				{
				newHandle = MNewHandle (bytesNeeded);
				*changedFlagPtr = TRUE;
				//returnFlag = (newHandle != NULL);
				//outputPtr = (Ptr)handle;
				
				}	// end "else handle == NULL"
				
			returnFlag = (newHandle != NULL);
			*inputHandlePtr = newHandle;
			outputPtr = (Ptr)newHandle;
		#endif	// defined multispec_lin
    
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))"
	
	*continueFlagPtr = returnFlag;
		
	return (outputPtr);

}	// end "CheckHandleSize" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckIfBytesRequestedAreWithinLimit
//
//	Software purpose:	This routine checks to see if the requested amount
//							of memory is within the limit that can be requested.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	True if okay; False if not okay
// 
// Called By:			
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 08/25/2010
//	Revised By:			Larry L. Biehl			Date: 09/14/2018			

Boolean CheckIfBytesRequestedAreWithinLimit (
				SInt64								bytesRequested)
													
{
	Boolean								memoryRequestFlag = TRUE;
	
	#if defined multispec_mac || defined multispec_lin
		if (bytesRequested > SInt32_MAX)
	#endif
	#if defined multispec_win
		if (bytesRequested > SInt32_MAX)
	#endif
		{
		memoryRequestFlag = FALSE;
		
		DisplayAlert (kErrorAlertID, 
						  kStopAlert, 
						  kAlertStrID, 
						  IDS_Alert124,
						  0, 
						  NULL);
	
		}	// end "if (bytesRequested > SInt32_MAX)"
	
	return (memoryRequestFlag);


}	// end "CheckIfBytesRequestedAreWithinLimit"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckIfMemoryAvailable
//
//	Software purpose:	This routine checks to see if the requested amount
//							of memory is available.  It does this by requesting
//							a handle of that amount.  Therefore a warning is
//							displayed if the requested memory is not available.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateGraphicsWindow
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 02/12/1993
//	Revised By:			Larry L. Biehl			Date: 08/15/2013			

Boolean CheckIfMemoryAvailable (
				UInt32								memoryRequest)

{
	Handle								testHandle;
	
			// Make certain that there is a minimum amount of memory left.
					
   #if defined multispec_lin
		Boolean								success;
		
		testHandle = malloc (memoryRequest+memsize);
		if (testHandle != NULL)
			success = TRUE;
		else
			success = FALSE;

		free (testHandle);
		return success;
		
   #else
		testHandle = MNewHandle (memoryRequest);
		
		if (testHandle != NULL)
			UnlockAndDispose (testHandle);
		
		return (testHandle != NULL);
   #endif
	
}	// end "CheckIfMemoryAvailable"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckMemoryForColorPicker
//
//	Software purpose:	The purpose of this routine is to check if there
//							is enough memory for the color picker routine.  	
//							It crashes at times before a message is even displayed.  
//							30000 is a guess for the minimum.  I am guessing
//							that the need for lots of memory is because of
//							creation of an inverse color table.  Also get
//							the prompt string for color picker.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:	TRUE.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/08/1991
//	Revised By:			Larry L. Biehl			Date: 09/06/2017
//
Boolean CheckMemoryForColorPicker (
				Str255*								stringPtr, 
				SInt16								stringNumber)
	
{
	#if defined multispec_mac
		Boolean								returnFlag;
		
				
				// Make certain that there is enough memory left for loading	
				// the color picker.	It crashes at times before a message is	
				// even displayed.  30000 is a guess.									
		
		returnFlag = TRUE;
		
		if (CheckIfMemoryAvailable (30000))
			GetIndString (*stringPtr, kDialogStrID, stringNumber);
			
		if (gTextString[0] == 0 || gMemoryError != noErr)
			{
			returnFlag = FALSE;
			
					// Try to bring the memory allocated to the spare memory 	
					// handles up to the minimums.										
		
			gMemoryError = noErr;
			GetSpareMemory ();
			
			}	// end "if (gMemoryError != noErr)"
			
		gTextString[gTextString[0]+1] = 0;
			
		return (returnFlag);
	#endif	// defined multispec_mac

	#if defined multispec_mac_swift
		return (TRUE);
	#endif	// defined multispec_mac_swift

	#if defined multispec_win
		return (TRUE);
	#endif	// defined multispec_win

	#if defined multispec_lin
		 return (TRUE);
	#endif
	
}	// end "CheckMemoryForColorPicker" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckPointerSize
//
//	Software purpose:	The purpose of this routine is to check the size
//							of the pointer against the bytes needed.  More 
//							memory is obtained for the pointer if needed.   The
//							memory full flag is set if the desired memory can
//							not be obtained.
//
//	Parameters in:		Pointer to buffer.
//							Number of bytes needed for buffer.
//
//	Parameters out:	Pointer to buffer with at least requested bytes of memory.
//							Flag indicating whether the buffer size was changed.
//
// Value Returned:	TRUE if memory is okay
//							FALSE if memory is full.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1989
//	Revised By:			Larry L. Biehl			Date: 08/25/2010	

Boolean CheckPointerSize (
				Ptr*									inputPtr, 
				SInt64								bytesNeeded, 
				Boolean*								changedFlagPtr)

{
	Ptr									pointer;
	SInt32								sizePointer;
	Boolean								returnFlag;
	
	
	*changedFlagPtr = FALSE;
	returnFlag = FALSE;
	pointer = *inputPtr;
	
	if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))
		{
      #ifndef multispec_lin
			if (pointer != NULL)
				{
						// Get size of pointer.														
						
				sizePointer = MGetPointerSize (pointer);
				returnFlag = (sizePointer >= 0);
			
				if ((sizePointer < (SInt32)bytesNeeded) && returnFlag)
					returnFlag = MSetPointerSize (&pointer, bytesNeeded);
					
				if ((sizePointer != (SInt32)bytesNeeded) && returnFlag)  
					*changedFlagPtr = TRUE;
					
				}	// end "if (pointer != NULL)" 
			
			else	// pointer == NULL
				{	
				pointer = MNewPointer (bytesNeeded);
				if (pointer != NULL)
					{
					*changedFlagPtr = TRUE;
					returnFlag = TRUE;
					
					}	// end "if (pointer != NULL)"
				
				}	// end "else pointer == NULL"
      #else
			if (pointer != NULL)
				{
				if (getsize (pointer) < bytesNeeded)
					{
					pointer = (Ptr)memreallocate (bytesNeeded, pointer);
					if (pointer != NULL)
						*changedFlagPtr = TRUE;
					
					}	// end "if (getsize (pointer) < bytesNeeded)"
				
				returnFlag = (pointer != NULL);
				
				}	// end "if (pointer != NULL)"
		
			else	// handle == NULL
				{
				pointer = MNewPointer (bytesNeeded);
				*changedFlagPtr = TRUE;
				returnFlag = (pointer != NULL);
				
				}	// end "else handle == NULL"
      #endif
		
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))"
		
	*inputPtr = pointer; 
		
	return (returnFlag); 

}	// end "CheckPointerSize" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckSizeAndUnlockHandle
//
//	Software purpose:	The purpose of this routine is to check if the
//							input handle is not NULL and the size of handle is less than
//							8 megabytes or more than 17 megabytes.  
//							If so then the handle will be unlocked.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			"several routines"
//
//	Coded By:			Larry L. Biehl			Date: 01/19/1993
//	Revised By:			Larry L. Biehl			Date: 01/22/1993	

void CheckSizeAndUnlockHandle (
				Handle								handle)

{
	#if defined multispec_mac
		SInt32								handleSize;
		
		
				// If handle is not NULL and the size of the handle is less than		
				// gMaxMoveableBlockSize or more than 17 megabytes then unlock it.	
				
				// Do not unlock the handle if the number of bytes in it is				
				// more than 9 megabytes and less than 17 megabytes.  There is a 		
				// Mac OS bug which causes the system to crash if this amount of 		
				// memory is moved. Problem found during 1/1993.										
				
		if (handle != NULL)
			{
			handleSize = GetHandleSize (handle);
			if (handleSize < gMaxMoveableBlockSize || handleSize > 17000000)
				HUnlock (handle);
				
			}	// end "if (handle != NULL)"
	#endif	// defined multispec_mac
					
	#if defined multispec_win
				// For windows I will assume that there is no problem in unlocking any
				// size of memory										
				
		CheckAndUnlockHandle (handle);
	#endif	// defined multispec_win 
		
	#if defined multispec_lin
		 // Do nothing
	#endif

}	// end "CheckSizeAndUnlockHandle"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyHandleToHandle
//
//	Software purpose:	The purpose of this routine is to copy the information
//							(memory) represented by the input handle to that
//							represented by the output handle. The memory allocated
//							for the input and output handles must be the same.
//
//							Note that as written, this will only work for handle
//							sizes less than 64000 bytes for the windows version.
//							So far that is more than enough for the routines that it
//							is currently called by.
//
//	Parameters in:		Input handle
//							Output handle
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			"several routines"
//
//	Coded By:			Larry L. Biehl			Date: 01/29/1996
//	Revised By:			Larry L. Biehl			Date: 01/29/1996 
        		        									
SInt16 CopyHandleToHandle (
				Handle								inputHandle,
				Handle								outputHandle)
				
{                            
	SInt16								returnCode = 1;
	
   #ifndef multispec_lin
		if (inputHandle != NULL && outputHandle != NULL)
			{             
			SInt32			sizeInputHandle,
								sizeOutputHandle;
			
			sizeInputHandle = ::MGetHandleSize (inputHandle);
			sizeOutputHandle = ::MGetHandleSize (outputHandle); 
			
			if (sizeInputHandle == sizeOutputHandle)
				{                       
				if (sizeInputHandle > 0)
					{
					Ptr inputPointer = GetHandlePointer (inputHandle);
					Ptr outputPointer = GetHandlePointer (outputHandle);
					
					memcpy (outputPointer, inputPointer, (size_t)sizeInputHandle);
					
					returnCode = noErr;
					
					}	// end "if (sizeInputHandle > 0)"
				
				}	// end "if (sizeInputHandle == sizeOutputHandle)"
			
			}	// end "if (inputHandle != NULL && outputHandle != NULL)"
	#endif	// ifndef multispec_lin
	
	#if defined multispec_lin
		if (inputHandle != NULL && outputHandle != NULL)
			{
         if (getsize (inputHandle) == getsize (outputHandle))
				{
            if (getsize (inputHandle) > 0)
					{
					memorycopy (outputHandle, inputHandle);
					returnCode = noErr;

					}	// end "if (sizeInputHandle > 0)"

				}	// end "if (sizeInputHandle == sizeOutputHandle)"

			}	// end "if (inputHandle != NULL && outputHandle != NULL)"
	#endif	// defined multispec_lin
	
	return (returnCode);

}	// end "CopyHandleToHandle"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyHandleToPointer
//
//	Software purpose:	The purpose of this routine is to copy the information
//							(memory) represented by the input handle to that
//							represented by the output pointer. The memory for the pointer
//							will be allocated to be the same as that for the handle.
//
//	Parameters in:		Input handle
//							Output pointer
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			LoadDialogLocalVectors in SDlgUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Larry L. Biehl			Date: 02/15/2000 
        		        									
SInt16 CopyHandleToPointer (
				Handle								inputHandle,
				Ptr*									ptrPtr)
				
{  
	Ptr 									inputPointer; 
	                     
	SInt32								sizeInputHandle;
							    
	SInt16								returnCode = 1;
	
	Boolean								changedFlag,
											continueFlag;
	
	
   #ifndef multispec_lin
		sizeInputHandle = ::MGetHandleSize (inputHandle);
		
		if (sizeInputHandle > 0)
			{
			continueFlag = CheckPointerSize (ptrPtr, 
															sizeInputHandle, 
															&changedFlag);
			if (continueFlag)
				{ 
				inputPointer = GetHandlePointer (inputHandle);
					
				memcpy (*ptrPtr, inputPointer, (size_t)sizeInputHandle);
					
				returnCode = noErr;
				
				}	// end "if (continueFlag)"
			
			}	// end "if (sizeInputHandle > 0)"
    #endif	// ifndef multispec_lin
	
    #if defined multispec_lin
		void *outputHandle = (void *)(*ptrPtr);
		if (getsize (inputHandle) > 0)
			{
			continueFlag = CheckPointerSize (ptrPtr, getsize (inputHandle), &changedFlag);
			if (inputHandle != NULL && continueFlag)
				{
				memorycopy (outputHandle, inputHandle);
				*ptrPtr = (Ptr)outputHandle;
				returnCode = noErr;
			
				}	// end "if (inputHandle != NULL && outputHandle != NULL)"
			
			}	// end "if (getsize (inputHandle) > 0)"
	#endif	// defined multispec_lin
	
	return (returnCode);

}	// end "CopyHandleToPointer"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyPointerToHandle
//
//	Software purpose:	The purpose of this routine is to copy the information
//							(memory) represented by the input pointer to that
//							represented by the output handle. The memory for the handle
//							will be allocated to be the same as that for the pointer.
//
//	Parameters in:		Input handle
//							Output pointer
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Larry L. Biehl			Date: 07/10/2009 
        		        									
SInt16 CopyPointerToHandle (
				Ptr									inputPointer,
				Handle*								outputHandlePtr)
				
{  
	Ptr 									outputPointer; 
	                     
	SInt32								sizeInputPointer;
							    
	SInt16								returnCode = 1;
	
	Boolean								continueFlag;
	
   #ifndef multispec_lin
		sizeInputPointer = ::MGetPointerSize (inputPointer);
	#else
		sizeInputPointer = getsize ((void *)inputPointer);
   #endif
   
	if (sizeInputPointer > 0)
		{
		continueFlag = TRUE;
		 
		if (*outputHandlePtr == NULL)
			{
			*outputHandlePtr = MNewHandle (sizeInputPointer);
			
			if (*outputHandlePtr == NULL)
				continueFlag = FALSE; 
				
			}	// end "if (*outputHandlePtr == NULL)"     
		
		else	// *outputHandlePtr != NULL
			{
			#ifndef multispec_lin
				continueFlag = ::MSetHandleSize (outputHandlePtr, sizeInputPointer);
			#else
				continueFlag = MSetHandleSize (outputHandlePtr,sizeInputPointer);
			#endif
			}
		       
		if (continueFlag)
			{ 
			outputPointer = GetHandlePointer (*outputHandlePtr);
			
         #ifndef multispec_lin
				memcpy (outputPointer, inputPointer, (size_t)sizeInputPointer);
         #else
				memorycopy (outputPointer, inputPointer);
         #endif
			returnCode = noErr;
			
			}	// end "if (continueFlag)"
		
		}	// end "if (sizeInputHandle > 0)"
		
	return (returnCode);

}	// end "CopyPointerToHandle"                        



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DisposeIOBufferPointers
//
//	Software purpose:	This routine closes the general file io instruction structure
//							disposes of the io buffer memory.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/20/1999
//	Revised By:			Larry L. Biehl			Date: 10/20/1999	

void DisposeIOBufferPointers (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				HUCharPtr*			  				ioBufferPtrPtr, 
				HUCharPtr*   						ioBuffer2PtrPtr) 

{    
	CloseUpGeneralFileIOInstructions (fileIOInstructionsPtr);
		
			// Dispose of the IO buffer.												

	*ioBufferPtrPtr = (HUCharPtr)CheckAndDisposePtr ((Ptr)*ioBufferPtrPtr);
	*ioBuffer2PtrPtr = NULL; 

}	// end "DisposeIOBufferPointers"                



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetCountVectorTableMemory
//
//	Software purpose:	This routine obtains the memory needed for the classification
//							confusion matrix table for the request fields.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/24/1997
//	Revised By:			Larry L. Biehl			Date: 08/13/2010	

Handle GetCountVectorTableMemory (
				ClassifierVar*						clsfyVariablePtr,
				Boolean								trainingFieldFlag,
				Boolean								testFieldFlag) 

{  
	HSInt64Ptr							columnWidthVectorStartPtr;
											
	Handle								countVectorHandle;      
	UInt32								maximumNumberOfFields;
	
	     
			// First determine the maximum number of fields that will be 		
			// classified for training, test and/or area.							
			// Assume that all training and test fields are being classified. 
			// This will give a worst case number of fields.  Memory should 	
			// be no problem for this.														
			
	maximumNumberOfFields = 1;
	if (trainingFieldFlag)
		maximumNumberOfFields = gProjectInfoPtr->numberStatTrainFields;
				
	if (testFieldFlag) 
		maximumNumberOfFields = 
				MAX (maximumNumberOfFields, (UInt32)gProjectInfoPtr->numberStatTestFields);
	
			// Save the start of the storage for totals.  							
		
	clsfyVariablePtr->totalCountIndexStart =
							maximumNumberOfFields * clsfyVariablePtr->tableWidth;
	
			// Add space for totals and Kappa Statistics classified totals to 
			// be computed.												
			
	maximumNumberOfFields++;
	
			// Add space for subtotals and Kappa Statistics reference totals to 
			// be computed.												
			
	maximumNumberOfFields++;
	
			// Add space for the table column widths to be used to list the tables.												
			
	maximumNumberOfFields++;
	
			// Add space for correct totals vector to be used to compute the reliability
			// total line.												
			
	maximumNumberOfFields++;
	
			// Now add space for the confusion matrix with class totals. This will be
			// used for computing Kappa statistic. Allow for a background class and a
			// threshold class.
			// Also all for case when thematic image has more classes than the project
			// file.
		
	if (trainingFieldFlag || testFieldFlag) 	
		maximumNumberOfFields += MAX ((UInt32)gProjectInfoPtr->numberStatisticsClasses + 2,
												clsfyVariablePtr->tableWidth);
	
			// Get handle to some storage.												
			
	countVectorHandle = MNewHandleClear ((SInt64)maximumNumberOfFields *
													clsfyVariablePtr->tableWidth * sizeof (SInt64));
	
	clsfyVariablePtr->countVectorPtr = (HSInt64Ptr)GetHandlePointer (
													countVectorHandle, kLock, kMoveHi);
	
	columnWidthVectorStartPtr = (HSInt64Ptr)&clsfyVariablePtr->countVectorPtr[
															clsfyVariablePtr->totalCountIndexStart];
	clsfyVariablePtr->columnWidthVectorPtr = 
					(HUInt32Ptr)&columnWidthVectorStartPtr[2*clsfyVariablePtr->tableWidth];
		
	return (countVectorHandle);

}	// end "GetCountVectorTableMemory"                      



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr GetHandlePointer
//
//	Software purpose:	This routine returns the input dereferenced handle. It is a
//							helper routine for an overload with lockFlag and moveHiFlag
//							defaulting to false
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/31/2017
//	Revised By:			Larry L. Biehl			Date: 08/31/2017

Ptr GetHandlePointer (
				Handle								handle)

{
	return (GetHandlePointer (handle, kNoLock, kNoMoveHi));

}	// end "GetHandlePointer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr GetHandlePointer
//
//	Software purpose:	This routine returns the input dereferenced handle. It is a
//							helper routine for an overload with moveHiFlag defaulting
//							to false
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/31/2017
//	Revised By:			Larry L. Biehl			Date: 08/31/2017

Ptr GetHandlePointer (
				Handle								handle,
				Boolean								lockFlag)

{
	return (GetHandlePointer (handle, lockFlag, kNoMoveHi));

}	// end "GetHandlePointer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr GetHandlePointer
//
//	Software purpose:	This routine returns the input dereferenced handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/14/1995
//	Revised By:			Larry L. Biehl			Date: 08/31/2017		

Ptr GetHandlePointer (
				Handle								handle,
				Boolean								lockFlag,
				Boolean								moveHiFlag) 

{       
	Ptr									outputPtr = NULL;
	
	
	if (handle != NULL)
		{
		#if defined multispec_mac
					// Do not move the memory represented by the input handle if the
					// number of bytes in it is more than 9 megabytes and less than 	
					// 17 megabytes.  There is a Mac OS bug which causes the system 	
					// to crash if this amount of memory is moved. Problem found 		
					// during 1/1993.																
					
			if (moveHiFlag)
				{
				SignedByte handleStatus = ::HGetState (handle);
				
				if (handleStatus >= 0)
					{
							// Handle is unlocked so go ahead and move.
							
					SInt32 handleSize = ::GetHandleSize (handle);
					if (handleSize < gMaxMoveableBlockSize || handleSize > 17000000)
						MoveHHi (handle);
						
					}	// end "if (handleStatus >= 0)"
					
				}	// end "if (moveHiFlag)"
				
			if (lockFlag)
				HLock (handle);
			
			outputPtr = (Ptr)*handle;
		#endif	// defined multispec_mac
			
		#if defined multispec_win
			outputPtr = (Ptr)::GlobalLock (handle);

					// If handle is not to be locked then decrimant the
					// lock count by one.
			
			if (!lockFlag)
				::GlobalUnlock (handle);
		#endif	// defined multispec_win
				
		#if defined multispec_lin
			outputPtr = (Ptr)handle;
		#endif
		}	// end "if (handle != NULL)"
	
	return (outputPtr);

}	// end "GetHandlePointer"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr GetHandleStatusAndPointer
//
//	Software purpose:	This routine get the status of the handle, locks the handle
//							and then dereferences the handle (a pointer). The pointer and
//							handle status are returned. This is a helper for an overload
//							with moveHiFlag set to false (or kNoMoveHi).
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/31/2017
//	Revised By:			Larry L. Biehl			Date: 08/31/2017

Ptr GetHandleStatusAndPointer (
				Handle								handle,
				SignedByte*							handleStatusPtr)

{
	return (GetHandleStatusAndPointer (handle, handleStatusPtr, kNoMoveHi));
		
}	// end "GetHandleStatusAndPointer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr GetHandleStatusAndPointer
//
//	Software purpose:	This routine get the status of the handle,
//							locks the handle and then dereferences the
//							handle (a pointer). The pointer and handle
//							status are returned.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1995
//	Revised By:			Larry L. Biehl			Date: 06/04/1996		

Ptr GetHandleStatusAndPointer (
				Handle								handle,
				SignedByte*							handleStatusPtr,
				Boolean								moveHiFlag)

{                     
	Ptr									outputPtr;
	
	
	outputPtr = NULL;
	
	if (handle != NULL)
		{
		if (handleStatusPtr != NULL)
			*handleStatusPtr = MHGetState (handle);
			
		outputPtr = GetHandlePointer (handle, kLock, moveHiFlag);
		
		}	// end "if (handle != NULL)"
		
	return (outputPtr);          
		
}	// end "GetHandleStatusAndPointer"  




//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetIOBufferPointers
//
//	Software purpose:	The purpose of this routine is to allocate memory
//							for an IO buffer which will be used to read data
//							from an image file into memory.  The routine
//							will return a pointer to the memory.
//							If there is not enough memory the pointer returned
//							will be NULL.  Also the routine returned value is
//							TRUE for pointer not equal to NULL and FALSE
//							if memory is full.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None	
// 
// Called By:			InitializeClusterMemory in cluster.c
//							ListDataControl in listData.c
//							PrincipalComponentAnalysis in principalComponent.c
//							ChangeImageFileFormat in reformat.c
//							UpdateStatsControl in statCompute.c
//							HistogramStatsControl in statHistogram.c
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1989
//	Revised By:			Larry L. Biehl			Date: 02/01/2006	

Boolean GetIOBufferPointers (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				LayerInfoPtr						imageLayerInfoPtr,
				FileInfoPtr							imageFileInfoPtr,
				HUCharPtr*			  				ioBufferPtrPtr, 
				HUCharPtr*   						ioBuffer2PtrPtr,
				UInt32	 							columnStart, 
				UInt32 								columnEnd, 
				UInt32 								columnInterval, 
				UInt16								numberChannels, 
				UInt16*								channelListPtr, 
				Boolean								packDataFlag, 
				Boolean								forceBISflag, 
				UInt16								forceOutputByteCode,
				Boolean								allowForThreadedIOFlag,
				FileIOInstructionsPtr*			outputFileIOInstructionsPtrPtr)

{                                
	FileInfoPtr							localFileInfoPtr;
	HUCharPtr							bufferPtr,
											tiledBufferPtr;
	
	UInt32								bytesNeeded,
											channel,
											fileInfoIndex,
											fileNumberChannels,
											index,
											inputBufferLength, 
											inputBytesNeeded,
											nextChannel,
											nextFileInfoIndex,
											numberImageFiles,
											numberTileBytes,
											outputBytesNeeded;
	
	SInt16								bandInterleave,
											endChannel,
											maxNumberBytes,
											startChannel;
								
	Boolean								bytesDifferFlag,
											continueFlag,
											overlapInputAndOutputBufferFlag; 
											

	
			// Check input variables.	
			
	if (imageFileInfoPtr == NULL)
																							return (FALSE);
	
	if (imageWindowInfoPtr != NULL && 
			imageWindowInfoPtr->numberImageFiles > 1 &&
				imageLayerInfoPtr == NULL)
																							return (FALSE);
																					
			// Initialize local variables.
			
	tiledBufferPtr = NULL;
	numberTileBytes = 0;
				
			// Make certain that we have the file information for the selected channels
			// is correct. That information will be loaded into the window information 
			// structure.													
		
	GetFileInformationForChannelList (imageWindowInfoPtr, 
													imageLayerInfoPtr, 
													imageFileInfoPtr,
													(SInt16*)channelListPtr, 
													numberChannels);
							
			// Initialize some parameters that depend upon whether imageWindowInfoPtr
			// is NULL or not.
	
	numberImageFiles = 1;
	bandInterleave = imageFileInfoPtr->bandInterleave;
	maxNumberBytes = imageFileInfoPtr->numberBytes;
	bytesDifferFlag = FALSE;
	if (imageWindowInfoPtr != NULL)
		{
		numberImageFiles = imageWindowInfoPtr->numberImageFiles;
		maxNumberBytes = imageWindowInfoPtr->localMaxNumberBytes;
		bytesDifferFlag = imageWindowInfoPtr->localBytesDifferFlag;
		bandInterleave = imageWindowInfoPtr->bandInterleave;
		
		}	// end "if (imageWindowInfoPtr != NULL)"

			// Get handle to block of memory to use as file IO buffer.				
			// First get the bytes needed to read the data from an image file to	
			// memory.																				 
	
	if (numberImageFiles == 1)
		{
		inputBytesNeeded = imageFileInfoPtr->numberColumns;
		inputBytesNeeded *= imageFileInfoPtr->numberBytes;
		
		if (imageFileInfoPtr->bandInterleave == kBIL)
			{
			if (channelListPtr == NULL)
				inputBytesNeeded *= numberChannels;
				
			else	// channelListPtr != NULL
				{
						// Allow for possible BIL special case when more channels
						// are read in than actually used.
						
				startChannel = MIN (channelListPtr[numberChannels-1], channelListPtr[0]);
				endChannel = MAX (channelListPtr[numberChannels-1], channelListPtr[0]);
								
				inputBytesNeeded *= (endChannel - startChannel + 1);
							
				}	// end "else channelListPtr != NULL"
			
			}	// end "if (imageFileInfoPtr->bandInterleave == kBIL)"
		
		else if (imageFileInfoPtr->bandInterleave == kBIS)
			inputBytesNeeded *= imageFileInfoPtr->numberChannels;
			
		else	// imageFileInfoPtr->bandInterleave != kBIL || ... != kBIS
			inputBytesNeeded *= numberChannels;
		
		}	// end "if (numberImageFiles == 1)" 
	
	else	// numberImageFiles != 1 
		{
		inputBytesNeeded = 0;
		
		channel = 1;
		if (channelListPtr != NULL)
			channel = channelListPtr[0] + 1;
			
		fileInfoIndex = imageLayerInfoPtr[channel].fileInfoIndex;
		nextFileInfoIndex = fileInfoIndex;
		
		fileNumberChannels = 1;	
		for (index=1; index<=numberChannels; index++)
			{		
			channel = index;								
			if (channelListPtr != NULL)
				channel = channelListPtr[index-1] + 1;
			
			if (index < numberChannels)
				{
				nextChannel = index + 1;				
				if (channelListPtr != NULL)
					nextChannel = channelListPtr[index] + 1;
				
				nextFileInfoIndex = imageLayerInfoPtr[nextChannel].fileInfoIndex;
					
				}	// end "if (index < numberChannels)"
				
			if (fileInfoIndex != nextFileInfoIndex || index == numberChannels)
				{
				localFileInfoPtr = &imageFileInfoPtr[fileInfoIndex];
				
				bytesNeeded = localFileInfoPtr->numberColumns;
				bytesNeeded *= localFileInfoPtr->numberBytes;
			
						// Adjust number of bytes needed by the number of channels.		
						
				if (localFileInfoPtr->bandInterleave == kBIS)
					bytesNeeded *= localFileInfoPtr->numberChannels;
								
				else	// localFileInfoPtr->bandInterleave != kBIS 
					bytesNeeded *= fileNumberChannels;
					
				inputBytesNeeded = MAX (inputBytesNeeded, bytesNeeded);
			
				fileNumberChannels = 0;
				fileInfoIndex = nextFileInfoIndex;
				
				}	// end "if (fileInfoIndex != ..." 
				
			fileNumberChannels++;
				
			}	// end "for (index=1; index<=numberChannels; index++)" 
		
		}	// end "else numberImageFiles != 1"
		
			// Get the number of bytes needed for tile file IO.
			// Force it to be a multiple of 8 bytes. 
			// For now tile buffer bytes will only be set up for the first buffer set.
		
	numberTileBytes = GetSetTiledIOBufferBytes (imageLayerInfoPtr,
																imageFileInfoPtr, 
																channelListPtr, 
																numberChannels,
																NULL);
		
			// Force input bytes to be a multiple of 8 bytes.						
			
	inputBytesNeeded = ((inputBytesNeeded + 7)/8) * 8;
		
			// Now determine the maximum number of bytes needed to pack the 		
			// data into. 	Allow for 2, 4 or 8 bytes.												
	
	UInt32 numberOutBytes = 1;
	if (forceOutputByteCode == kDoNotForceBytes)
		numberOutBytes = maxNumberBytes;
	else if (forceOutputByteCode == kForce2Bytes)
		numberOutBytes = 2;
	else if (forceOutputByteCode == kForce4Bytes || 
															forceOutputByteCode == kForceReal4Bytes)
		numberOutBytes = 4;
	else if (forceOutputByteCode == kForceReal8Bytes)
		numberOutBytes = 8;
	outputBytesNeeded = (columnEnd - columnStart + columnInterval)/
											columnInterval * numberOutBytes * numberChannels;
		
			// Force output bytes to be a multiple of 8 bytes.							
			
	outputBytesNeeded = ((outputBytesNeeded + 7)/8) * 8;			
				
			// Now determine if 'outputBytesNeeded' can overlap 						
			// 'inputBytesNeeded'																
	
	overlapInputAndOutputBufferFlag = TRUE;		
	if (forceOutputByteCode != kDoNotForceBytes && 
			(maxNumberBytes != numberOutBytes || bytesDifferFlag))
		overlapInputAndOutputBufferFlag = FALSE;
											
	else if (bandInterleave != kBIS && forceBISflag)
		overlapInputAndOutputBufferFlag = FALSE;
		
			// Do not allow the buffers to overlap if more than one disk file 	
			// is linked together to represent the image.								
			
	if (numberImageFiles != 1 && ioBufferPtrPtr != ioBuffer2PtrPtr && packDataFlag)
		overlapInputAndOutputBufferFlag = FALSE;
		
			// Get the total bytes that are needed.	
			// Allow memory for 2 input buffers if threaded IO may be used.									
			
	if (overlapInputAndOutputBufferFlag)
		{
		bytesNeeded = MAX (inputBytesNeeded, outputBytesNeeded);
			
		inputBufferLength = bytesNeeded;
		
		if (allowForThreadedIOFlag)
			bytesNeeded *= 2;
		
		}	// end "overlapInputAndOutputBufferFlag"
		
	else	// !overlapInputAndOutputBufferFlag
		{ 
		bytesNeeded = inputBytesNeeded + outputBytesNeeded;
			
		inputBufferLength = inputBytesNeeded;
		
		if (allowForThreadedIOFlag)
			{
			bytesNeeded += inputBytesNeeded;
			
					// Indicate the total input bytes allowed for in case it
					// is needed below to compute an offset for the output
					// buffer.
					
			inputBytesNeeded *= 2;
			
			}	// end "if (allowForThreadedIOFlag)"
		
		}	// end "else !overlapInputAndOutputBufferFlag"
		
	if (allowForThreadedIOFlag)
		numberTileBytes *= 2;
	
	bufferPtr = (HUCharPtr)MNewPointer (bytesNeeded + numberTileBytes);
	*ioBufferPtrPtr = *ioBuffer2PtrPtr = bufferPtr;
	
	if (!overlapInputAndOutputBufferFlag && 
				bufferPtr != NULL &&  
						ioBufferPtrPtr != ioBuffer2PtrPtr)
		*ioBuffer2PtrPtr = &bufferPtr[inputBytesNeeded];
		
	if (bufferPtr != NULL && numberTileBytes > 0)
		tiledBufferPtr = &bufferPtr[bytesNeeded];
	
	continueFlag = (bufferPtr != NULL);
		
			// Set up the general parameters in the File IO Instructions structure.

	if (continueFlag)
		SetUpGeneralFileIOInstructions (fileIOInstructionsPtr,
													imageWindowInfoPtr,
													imageLayerInfoPtr,	
													imageFileInfoPtr,
													numberChannels,
													channelListPtr,
													*ioBufferPtrPtr,
													*ioBuffer2PtrPtr,
													tiledBufferPtr,
													inputBufferLength,
													packDataFlag,
													forceBISflag,	
													forceOutputByteCode,
													outputFileIOInstructionsPtrPtr);
														
	else	// !continueFlag
		*outputFileIOInstructionsPtrPtr = NULL;
	
	return (continueFlag);
	
}	// end "GetIOBufferPointers"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetSetTiledIOBufferBytes
//
//	Software purpose:	This routine sets the buffer pointers in the HierarchalFileFormat
//							structures in the input fileInfo structure.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	number of bytes required for the block or tiled io buffer.
// 
// Called By:			GetClassifyDataBuffers in SClassfy.cpp
//							DisplayColorThematicImage in SDisThem.cpp
//							SetUpGeneralFileIOInstructions in SFileIO.cpp
//							GetIOBufferPointers in SMemUtil.cpp
//							ShowGraphWindowSelection in SSelGraf.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1999
//	Revised By:			Larry L. Biehl			Date: 09/06/2017	

UInt32 GetSetTiledIOBufferBytes (
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr, 
				UInt16*								channelListPtr,
				UInt16								numberChannels, 
				HUCharPtr							tiledBufferPtr)

{
	FileInfoPtr							localFileInfoPtr;
	
	HierarchalFileFormatPtr			hfaPtr,
											localHfaPtr;
	
	SInt32								fileInfoIndex,
											nextFileInfoIndex;
	
	UInt32								channelNumber,
											fileChannelNumber,
											index,
											numberTiledIOBufferBytes;
											
	Boolean								onlyOneHierarchalSetFlag;
											
	
	numberTiledIOBufferBytes = 0;
	fileChannelNumber = 0;
	fileInfoIndex = -1;
	hfaPtr = NULL;
	localFileInfoPtr = NULL;
	
	for (index=1; index<=numberChannels; index++)
		{	
		channelNumber = index;								
		if (channelListPtr != NULL)
			channelNumber = channelListPtr[index-1] + 1;
			
		nextFileInfoIndex = 0;
		fileChannelNumber++;
		if (layerInfoPtr != NULL)
			{
			nextFileInfoIndex = layerInfoPtr[channelNumber].fileInfoIndex;
			fileChannelNumber = layerInfoPtr[channelNumber].fileChannelNumber;
			
			}	// end "if (layerInfoPtr != NULL)"
			
		if (fileInfoIndex != nextFileInfoIndex)
			{
			fileInfoIndex = nextFileInfoIndex;
			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
				
					// Get pointer to the hfa structure.
					
			hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (
													localFileInfoPtr->hfaHandle);
			
			onlyOneHierarchalSetFlag = FALSE;
			
			}	// end "if (if (fileInfoIndex != nextFileInfoIndex)"
		
		if (hfaPtr != NULL && !onlyOneHierarchalSetFlag)
			{
			if (localFileInfoPtr->bandInterleave == kBIBlock ||
												localFileInfoPtr->bandInterleave == kBIS)
				{
				fileChannelNumber = 1;
				onlyOneHierarchalSetFlag = TRUE;
				
				}	// end "if (localFileInfoPtr->bandInterleave == kBIBlock)"
		
			localHfaPtr = &hfaPtr[fileChannelNumber-1];
			
			if (tiledBufferPtr != NULL)
				localHfaPtr->tiledBufferPtr = &tiledBufferPtr[numberTiledIOBufferBytes];
													
					// Add to the total the number of bytes needed to store a set of 
					// tile blocks for this channel.
			
			numberTiledIOBufferBytes += localHfaPtr->blockOffset *  
																	localHfaPtr->numberBlockWidths;
	
					// Force number bytes to be a multiple of 8 bytes.							
					
			numberTiledIOBufferBytes = ((numberTiledIOBufferBytes + 7)/8) * 8;			
			
			}	// end "if (hfaPtr != NULL && !onlyOneHierarchalSetFlag)"
			
		}	// end "for (index=1; index<=numberChannels; index++)" 
		
	return (numberTiledIOBufferBytes);
	
}	// end "GetSetTiledIOBufferBytes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetSpareMemory
//
//	Software purpose:	This routine checks that the reserved spare 
//							code memory is at 'gSpareCodeSize' bytes and 
//							reserve spare warning memory is at 'gSpareWarnSize'
//							bytes.  If it is not, then it tries to increase 
//							the spare memory back to the specified amounts or 
//							as close as it can.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 02/08/1989
//	Revised By:			Larry L. Biehl			Date: 08/16/1995	

void GetSpareMemory (void)

{
	UInt32								longestContBlock,
											spareSize;
											
	SInt16								returnCode;
	
	
	
			// Set variable indicating that this memory request is for updating	
			// the spare memory handles, in case the MyGrowZone routine is 		
			// called.																				
			
	gMemoryTypeNeeded = 2;
	gGetMemoryTries = 0;
	returnCode = noErr;
		
			// Check spare memory allocated for memory warning message.				
				
	if (gSpareWarnMemoryH)
		{		
		spareSize = (UInt32)MGetHandleSize (gSpareWarnMemoryH);
		if (spareSize < gSpareWarnSize)
			returnCode = MSetHandleSize (&gSpareWarnMemoryH, gSpareWarnSize); 
			
		}	// end "if (gSpareWarnMemoryH != NULL)" 
	
			// Check spare memory allocated for code resources.						
			
	if (gSpareCodeMemoryH != NULL && returnCode == noErr)
		{		
		spareSize = (UInt32)MGetHandleSize (gSpareCodeMemoryH);
		if (spareSize < gSpareCodeSize)	
			returnCode = MSetHandleSize (&gSpareCodeMemoryH, gSpareCodeSize);
			
		}	// end "if (gSpareCodeMemoryH != NULL && ...)" 
			
	gMemoryTypeNeeded = 0;
	
	if (returnCode == noErr)
		{
				// Display an alert if the amount of memory left is less than 		
				// the spare warning size.														
		                          
		MGetFreeMemory (&longestContBlock);

		if (gGetMemoryTries == 0 && longestContBlock < gSpareWarnSize)
			returnCode = DisplayAlert (
										kMemoryWarningAlertID, kCautionAlert, 0, 0, 0, NULL);
			
		}	// end "if (returnCode == noErr)" 
	
			// Indicate that there have been no tries on this set of spare			
			// memory.																				
			
	gGetMemoryTries = 0;
		
}	// end "GetSpareMemory"     


	      
#if defined multispec_lin || defined multispec_win
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MemoryMessage
//
//	Software purpose:	The purpose of this routine is to display a message box
//							indicating the amount of additional memory that is needed
//							to complete the memory request. The 'sort of' equivalent
//							in the Macintosh version is 'MyGrowZone'.
//
//	Parameters in:		Number of bytes
//
//	Parameters out:	None.
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/17/1995
//	Revised By:			Larry L. Biehl			Date: 03/01/2017	

void MemoryMessage (
				UInt32								numberBytes,
				UInt16								messageCode)

{
	#if defined multispec_win 
		Str63									numberString;

		double								doubleBytesNeeded;
		
		UInt32 								bytesNeeded;
		
		SInt16								numberChars;
		
		Boolean 								returnFlag;
										 
		USES_CONVERSION;
		

				// Get amount of contiguous memory available 
						
		UInt32 longestContBlock = (UInt32)::GlobalCompact (0);
				
		switch (messageCode)
			{
			case kMemoryMessage:
						// Sometimes the longest contigous block is larger than the bytes 
						// needed. I do not know why an error was given allocating the 
						// memory. For now the number of bytes requested will be show to 
						// the user for these cases.
						
				if (longestContBlock < numberBytes)
					bytesNeeded = numberBytes - longestContBlock;
					
				else	// longestContBlock >= numberBytes
					bytesNeeded = numberBytes;
			
				doubleBytesNeeded = bytesNeeded;
					
				numberString[0] = sprintf ((char*)&numberString[1], 
													"%.0f", 
													doubleBytesNeeded);
				
				numberChars = numberString[0];
				numberChars = InsertCommasInNumberString ((char*)&numberString[1], 
																		numberChars, 
																		-1,
																		numberChars);
				numberString[0] = (UInt8)numberChars;
				
				sprintf ((CharPtr)gTextString,
							"%s more bytes of memory are needed. You can trying closing some image windows or other applications that are not needed.",
							(char*)&numberString[1]);

				gMemoryError = 1;
				break;
				
			case kObjectMessage:
				sprintf ((CharPtr)gTextString,
							"Could not create object (s) needed for operation. Probably more memory is needed.");   	
				break;  
						
			}	// end "switch (messageCode)"
					
				// Release spare memory to be sure that there is enough to display
				// the message box.     			
		
		ReleaseSpareMemoryForWarningMessage ();

		SInt16 itemHit = AfxMessageBox (
								(LPCTSTR)A2T ((LPCSTR)gTextString), MB_ICONSTOP | MB_OK, 0);
		
				// Get the spare memory back again if we can.
				
		returnFlag = MSetHandleSize (&gSpareWarnMemoryH, gSpareWarnSize);                
	#endif	// defined multispec_win
	
	#if defined multispec_lin
		UInt32 bytesNeeded;
		
		Boolean returnFlag;
		

		switch (messageCode) 
			{
			case kMemoryMessage:
					// Sometimes the longest contigous block is larger than the bytes needed.
					// I do not know why an error was given allocating the memory. For now
					// the number of bytes requested will be show to the user for these
					// cases.

				bytesNeeded = numberBytes;

				sprintf ((CharPtr)gTextString,
							"%lu more bytes of memory are needed. You can trying closing some image windows or other applications that are not needed.",
							bytesNeeded);
					break;

			case kObjectMessage:
				sprintf ((CharPtr)gTextString,
							"Could not create object(s) needed for operation. Probably more memory is needed.");
				break;

			}	// end "switch (messageCode)"

				// Release spare memory to be sure that there is enough to display
				// the message box.

		ReleaseSpareMemoryForWarningMessage ();

		DisplayAlert (0,0,0,0,0,gTextString);
		
				// Get the spare memory back again if we can.

		returnFlag = MSetHandleSize (&gSpareWarnMemoryH, gSpareWarnSize);
	#endif	// defined multispec_lin
		
}	// end "MemoryMessage"
#endif	// defined multispec_lin || defined multispec_win



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 MGetFreeMemory
//
//	Software purpose:	The purpose of this routine is to return the amount of
//							free memory that is available in the system and the largest
//							free contiguous block of memory.
//
//	Parameters in:		Handle
//							Number of bytes
//
//	Parameters out:	None.
//
//	Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/24/1995
//	Revised By:			Larry L. Biehl			Date: 06/25/2018

UInt32 MGetFreeMemory (
				UInt32*								lContBlockPtr)

{         
	#if defined multispec_mac
		mach_msg_type_number_t 			count = HOST_VM_INFO_COUNT;
		vm_statistics_data_t 			vmstat;
		//SInt64								physicalMemoryAvailable;
		Ptr									pointer;
	
		int 									i,
												mib[6];
		
		SInt64								freeMemory = 0;
	
		SInt16								errCode = noErr;
	
		
				// Information is from:
				// https://stackoverflow.com/questions/6094444/how-can-i-programmatically-check-free-system-memory-on-mac-like-the-activity-mon
				//
				// If the sysctl approach is not available, the approach being used is to
				// turn the memory flag off and just make requests
				// 100 megabytes at a time less until an amount is found that works. I 
				// will start checking at 510 MB of RAM for now.
		
		mib[0] = CTL_HW;
		mib[1] = HW_PAGESIZE;

		int pagesize;
		size_t length;
		length = sizeof (pagesize);
		errCode = sysctl (mib, 2, &pagesize, &length, NULL, 0);

		if (errCode == noErr)
			{
			if (host_statistics (mach_host_self (),
										HOST_VM_INFO,
										(host_info_t)&vmstat,
										&count) != KERN_SUCCESS)
				errCode = -1;
				
			}	// end "if (errCode == noErr)"
	
		if (errCode == noErr)
			{
			freeMemory = pagesize * (vmstat.free_count + vmstat.inactive_count);
			
					// Limit it to max of 32 bit unsigned integer untill software
					// is 64-bit
			
			if (freeMemory > UInt32_MAX)
				freeMemory = UINT32_MAX;
			
			}	// end "if (errCode == noErr)"
	
		else	// errCode != noErr
			{
			gDisplayOSXMemoryMessageFlag = FALSE;
	
			pointer = NULL;
			freeMemory = 550000010;
			for (i=11; i>0; i--)
				{
				pointer = ::NewPtr ((UInt32)freeMemory);
				if (pointer != NULL)
					break;
				
				freeMemory -= 50000000;
				
				}	// end "for (i=11; i>0; i--)"
			
			if (pointer != NULL)
				DisposePtr (pointer);
			
			gDisplayOSXMemoryMessageFlag = TRUE;
			
			}	// end "else errCode != noErr"
			
		*lContBlockPtr = (UInt32)freeMemory;

		return ((UInt32)freeMemory);
	#endif	// defined multispec_mac
		
	#if defined multispec_mac_swift
		return (0);
	#endif	// defined multispec_mac
				
	#if defined multispec_win  
		UInt32					freeMemory;                                                        
		
		MEMORYSTATUS			memoryStatus;

		GlobalMemoryStatus (&memoryStatus);
		*lContBlockPtr = (UInt32)memoryStatus.dwAvailPageFile;
		*lContBlockPtr = MIN (*lContBlockPtr, (UInt32)(1.5*memoryStatus.dwAvailPhys));
		freeMemory = (UInt32)memoryStatus.dwAvailPageFile;
			
		return (freeMemory);
	#endif	// defined multispec_win 
			
	#if defined multispec_lin
				// For now just returning same values returned as Mac
				
		UInt32 freeMemory;
		wxString cpath = wxGetCwd ();
		//wxGetDiskSpace (&cpath, NULL, &freememory);
		freeMemory = 256000000;
		*lContBlockPtr = 256000000;
		
		return (freeMemory);
	#endif	// defined multispec_lin

}	// end "MGetFreeMemory" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 MGetHandleSize
//
//	Software purpose:	The purpose of this routine is to return the state
//							of the input handle.
//
//	Parameters in:		Handle
//							Number of bytes
//
//	Parameters out:	None.
//
//	Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1995
//	Revised By:			Larry L. Biehl			Date: 04/20/1995	

SInt32 MGetHandleSize (
				Handle								handle)

{      
	SInt32								sizeHandle = -1;
		
	if (handle != NULL)
		{
		#if defined multispec_mac 
			sizeHandle = ::GetHandleSize (handle);
			if (::MemError () != noErr) 
				sizeHandle = -1;
		#endif	// defined multispec_mac
			
		#if defined multispec_win   
			sizeHandle = (SInt32)::GlobalSize (handle);
		#endif	// defined multispec_win
		
      #if defined multispec_lin
			sizeHandle = getsize (handle);
      #endif 
					
		}	// end "if (handle != NULL)"
		
	return (sizeHandle);
		
}	// end "MGetHandleSize" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 MGetPointerSize
//
//	Software purpose:	The purpose of this routine is to obtain the size
//							of the input pointer.
//
//	Parameters in:		Handle
//							Number of bytes
//
//	Parameters out:	None.
//
//	Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1995
//	Revised By:			Larry L. Biehl			Date: 03/27/2017	

SInt32 MGetPointerSize (
				Ptr									pointer)

{     
	SInt32								sizePointer = -1;
		
	if (pointer != NULL)
		{
		#if defined multispec_mac 
			sizePointer = ::GetPtrSize (pointer);
			if (::MemError () != noErr) 
				sizePointer = -1;
		#endif	// defined multispec_mac
		      
		#if defined multispec_win 
			Handle	handle;

			handle = GlobalHandle ((LPSTR)pointer);
			
			sizePointer = (SInt32)::GlobalSize (handle);
		#endif	// defined multispec_win 
				
      #if defined multispec_lin
			sizePointer = getsize ((void*)pointer);
      #endif
		
		}	// end "if (pointer != NULL)"
		
	return (sizePointer);  
		
}	// end "MGetPointerSize"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SignedByte MHGetState
//
//	Software purpose:	The purpose of this routine is to return the state
//							of the input handle.
//
//	Parameters in:		Handle
//							Number of bytes
//
//	Parameters out:	None.
//
//	Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/07/1995
//	Revised By:			Larry L. Biehl			Date: 09/06/2017
// 
// For linux since no memory locking is involved, always returning 1.
SignedByte MHGetState (
				Handle								handle)

{         
	#if defined multispec_mac
		SignedByte							handleStatus = 0;
		
		
		if (handle != NULL)
			handleStatus = ::HGetState (handle);
		
		return (handleStatus);
	#endif	// defined multispec_mac
		
	#if defined multispec_mac_swift
		return 1;
	#endif	// defined multispec_mac_swift
	      
	#if defined multispec_win
		SignedByte		returnValue;


		returnValue = 1;

		if (handle != NULL)
			{          
			if ((GlobalFlags (handle) & GMEM_LOCKCOUNT) > 0)
				returnValue = -1; 
				
			}	// end "if (handle != NULL)"
				
		return (returnValue);
	#endif	// defined multispec_win
		
	#if defined multispec_lin
		return 1;
	#endif

}	// end "MHGetState"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MHLock
//
//	Software purpose:	The purpose of this routine is to unlock the handle if
//							the handle is not NULL.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			"several routines"
//
//	Coded By:			Larry L. Biehl			Date: 10/10/1995
//	Revised By:			Larry L. Biehl			Date: 10/10/1995	

void MHLock (
				Handle								handle)

{      
			// If handle is not NULL lock it.
			
	if (handle != NULL)
		{
		#if defined multispec_mac 
			::HLock (handle); 
		#endif	// defined multispec_mac 
		      
		#if defined multispec_win 
			::GlobalLock (handle); 
		#endif	// defined multispec_win
		
      #if defined multispec_lin
			// Do nothing
      #endif
		
		}	// end "if (handle != NULL)"
	
}	// end "MHLock"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle MHSetState
//
//	Software purpose:	The purpose of this routine is to set the state of
//							of the input handle to the input setting - either
//							locked or unlocked.
//
//	Parameters in:		Handle
//							Number of bytes
//
//	Parameters out:	None.
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/07/1995
//	Revised By:			Larry L. Biehl			Date: 07/07/1998	

void MHSetState (
				Handle								handle,
				SignedByte							handleStatus)


{             
	#if defined multispec_mac 
		if (handle != NULL)
			::HSetState (handle, handleStatus); 
	#endif	// defined multispec_mac
				
	#if defined multispec_win  
		if (handleStatus >= 0)
			CheckAndUnlockHandle (handle);
	#endif	// defined multispec_win
			
	#if defined multispec_lin
				// Do nothing
	#endif	// defined multispec_lin

}	// end "MHSetState"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle MNewHandle
//
//	Software purpose:	The purpose of this routine is to obtain a new handle
//							for a block of memory for the input number of bytes.
//
//	Parameters in:		Number of bytes
//
//	Parameters out:	None.
//
// Value Returned:	Handle			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/07/1995
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

Handle MNewHandle (
				SInt64								numberBytes)

{        
	Handle 								handle = NULL;
	
	
	if (CheckIfBytesRequestedAreWithinLimit (numberBytes))
		{
		#if defined multispec_mac 
			handle = ::NewHandle ((long)numberBytes);
			
			OSXMemoryMessage (numberBytes);
			
			gMemoryAlertDisplayFlag = FALSE;
		#endif	// defined multispec_mac 
					
		#if defined multispec_win    
			handle = GlobalAlloc (GHND, (SIZE_T)numberBytes); 
			
			if (handle == NULL)   
				MemoryMessage ((UInt32)numberBytes, kMemoryMessage);
		#endif	// defined multispec_win
		
      #if defined multispec_lin
         handle = memallocate (numberBytes);
         if (handle == NULL)   
				MemoryMessage ((UInt32)numberBytes, kMemoryMessage);
      #endif
		
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (numberBytes))"
			
	return (handle);
		
}	// end "MNewHandle"      



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle MNewHandleClear
//
//	Software purpose:	The purpose of this routine is to obtain a new handle
//							for a block of memory for the input number of bytes.
//							The new block will be set to all 0's.
//
//	Parameters in:		Number of bytes
//
//	Parameters out:	None.
//
// Value Returned:	Handle	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/13/1995
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

Handle MNewHandleClear (
				SInt64								numberBytes)

{    
	Handle 								handle = NULL;
	
	
	if (CheckIfBytesRequestedAreWithinLimit (numberBytes))
		{	     
		#if defined multispec_mac 
			handle = ::NewHandleClear ((long)numberBytes);
			
			OSXMemoryMessage (numberBytes);
		#endif	// defined multispec_mac 
					
		#if defined multispec_win    
			handle = GlobalAlloc (GHND, (SIZE_T)numberBytes);
			
			if (handle != NULL)
				{
				UInt32 count;
				
				HPtr hptr = (HPtr)GetHandlePointer (handle);
				
				for (count=0; count<numberBytes; count++)
					hptr[count] = 0;
					
				}	// end "if (handle != NULL)"  
			
			else	// handle == NULL
				{                   
				MemoryMessage ((UInt32)numberBytes, kMemoryMessage);
				
				}	// end "else handle == NULL" 
		#endif	// defined multispec_win
			
      #if defined multispec_lin
			handle = memallocate (numberBytes);
         memset (handle,0,getsize (handle));
      #endif
		
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (numberBytes))"
			
	return (handle); 
		
}	// end "MNewHandleClear"     



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr MNewPointer
//
//	Software purpose:	The purpose of this routine is to obtain a new pointer
//							for a block of memory for the input number of bytes.
//
//	Parameters in:		Number of bytes
//
//	Parameters out:	None.
//
// Value Returned:	Handle	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/07/1995
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

Ptr MNewPointer (
				SInt64								numberBytes)

{     
	Ptr 									ptr = NULL;
	
	
	if (CheckIfBytesRequestedAreWithinLimit (numberBytes))
		{	     			
		#if defined multispec_mac 
			ptr = ::NewPtr ((long)numberBytes);
			
			OSXMemoryMessage (numberBytes);
				
			gMemoryAlertDisplayFlag = FALSE;
		#endif	// defined multispec_mac 
					
		#if defined multispec_win
			ptr = (Ptr)GlobalAllocPtr (GHND, (SIZE_T)numberBytes); 
			
			if (ptr == NULL)      
				MemoryMessage ((UInt32)numberBytes, kMemoryMessage);
		#endif	// defined multispec_win 
		
      #if defined multispec_lin
         ptr = (Ptr)memallocate (numberBytes);
         if (ptr == NULL)      
				MemoryMessage ((UInt32)numberBytes, kMemoryMessage);
      #endif   
         
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (numberBytes))"
			
	return (ptr);
		
}	// end "MNewPointer"      



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Ptr MNewPointerClear
//
//	Software purpose:	The purpose of this routine is to obtain a new pointer
//							for a block of memory for the input number of bytes.
//							The new block will be set to all 0's.
//
//	Parameters in:		Number of bytes
//
//	Parameters out:	None.
//
// Value Returned:	Handle	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/10/1995
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

Ptr MNewPointerClear (
				SInt64								numberBytes)

{     
	Ptr 									ptr = NULL;
	
	if (CheckIfBytesRequestedAreWithinLimit (numberBytes))
		{	     						
		#if defined multispec_mac 			
			ptr = ::NewPtrClear ((long)numberBytes);
			
			OSXMemoryMessage (numberBytes);
		#endif	// defined multispec_mac 
								
		#if defined multispec_win   				  
			ptr = (Ptr)GlobalAllocPtr (GHND, (SIZE_T)numberBytes);
			
			if (ptr != NULL)
				{
				UInt32 count;
				
				HPtr hptr = (HPtr)ptr;
				for (count=0; count<numberBytes; count++)
					hptr[count] = 0;
					
				}	// end "if (ptr != NULL)"  
			
			else	// ptr == NULL
				MemoryMessage ((UInt32)numberBytes, kMemoryMessage);
		#endif	// defined multispec_win

      #if defined multispec_lin
			Handle	handle;
			
         handle = memallocate (numberBytes);
         if (handle == NULL)      
				MemoryMessage ((UInt32)numberBytes, kMemoryMessage);
				
         else
				{
            memset (handle,0,getsize (handle));
            ptr = (Ptr)handle;
				
				}
      #endif
			
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (numberBytes))"
		
	return (ptr); 
		
}	// end "MNewPointerClear"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean MSetHandleSize
//
//	Software purpose:	The purpose of this routine is to return the state
//							of the input handle.
//
//	Parameters in:		Handle
//							Number of bytes
//
//	Parameters out:	None.
//
//	Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1995
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

Boolean MSetHandleSize (
				Handle*								handlePtr,
				SInt64								bytesNeeded)

{       
	Boolean								returnFlag = FALSE;
	
	
	if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))
		{	 
		#if defined multispec_mac 
			if (*handlePtr != NULL)
				{
				::HUnlock (*handlePtr);
				::SetHandleSize (*handlePtr, (long)bytesNeeded);
						
						// Check if memory is full. 												
						
				if (::LMGetMemErr () == noErr)
					returnFlag = TRUE;
							
				OSXMemoryMessage (bytesNeeded);
					
				gMemoryAlertDisplayFlag = FALSE;
					
				}	// end "if (*handlePtr != NULL)"
		#endif	// defined multispec_mac
			
		#if defined multispec_win   
			if (*handlePtr != NULL)
				{
				Handle newHandle = ::GlobalReAlloc (
												*handlePtr, (SIZE_T)bytesNeeded, GMEM_MOVEABLE);
				
						// Check if memory is full. 												
						
				if (newHandle != NULL)
					{
					returnFlag = TRUE;
					*handlePtr = newHandle;
					
					}	// end "if (newHandle != NULL)"
					
				else	// newHandle == NULL
					MemoryMessage ((UInt32)bytesNeeded, kMemoryMessage);
					
				}	// end "if (*handlePtr != NULL)"
		#endif	// defined multispec_win 
			
      #if defined multispec_lin
			if (*handlePtr != NULL) 
				{
				Handle newHandle = memreallocate (bytesNeeded, *handlePtr);
			
				if (newHandle != NULL) 
					{
					returnFlag = TRUE;
					*handlePtr = newHandle;
					
					}	// end "if (newHandle != NULL)"
        
				else	// newHandle == NULL
					MemoryMessage ((UInt32)bytesNeeded, kMemoryMessage);
					
				}	// end "if (*handlePtr != NULL)"
      #endif
   
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))"
		
	return (returnFlag);
		
}	// end "MSetHandleSize"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean MSetPointerSize
//
//	Software purpose:	The purpose of this routine is to return the state
//							of the input handle.
//
//	Parameters in:		Handle
//							Number of bytes
//
//	Parameters out:	None.
//
// Value Returned:	TRUE if memory is okay
//							FALSE if memory is full			
// 
// Called By:			FeatureExtraction in SFeatExt.cpp
//							CheckPointerSize in SMemUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1995
//	Revised By:			Larry L. Biehl			Date: 03/12/2018
										
Boolean MSetPointerSize (
				Ptr*									pointerPtr,
				SInt64								bytesNeeded)      

{   
	Boolean								returnFlag = FALSE;
	
	
	if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))
		{	
		#if defined multispec_mac 
			if (*pointerPtr != NULL)
				{
				::SetPtrSize (*pointerPtr, (long)bytesNeeded);
						
						// Check if memory is full. 												
				
				if (::LMGetMemErr () == noErr)
					returnFlag = TRUE;
							
				OSXMemoryMessage (bytesNeeded);
					
				gMemoryAlertDisplayFlag = FALSE;
					
				}	// end "if (*handlePtr != NULL)"
		#endif	// defined multispec_mac
					
		#if defined multispec_win   
			if (*pointerPtr != NULL)
				{          
				Handle	handle;   
																	
				handle = GlobalHandle ((LPSTR)*pointerPtr);
				
				Ptr newPointer = (Ptr)::GlobalLock (
								::GlobalReAlloc (handle, (SIZE_T)bytesNeeded, GMEM_MOVEABLE));
				
						// Check if memory is full. 												
						
				if (newPointer != NULL)
					{
					returnFlag = TRUE;
					*pointerPtr = newPointer;
					
					}	// end "if (newPointer != NULL)"
					
				else	// newPointer == NULL
					MemoryMessage ((UInt32)bytesNeeded, kMemoryMessage);
					
				}	// end "if (*pointerPtr != NULL)"
		#endif	// defined multispec_win 

      #if defined multispec_lin
         if (*pointerPtr != NULL)
				{
				Ptr newHandle = (Ptr)memreallocate (bytesNeeded, *pointerPtr);
            if (newHandle != NULL)
					{
					returnFlag = TRUE;
					*pointerPtr = newHandle;
					
					}	// end "newHandle != NULL"
					
            else	// newHandle == NULL
               MemoryMessage ((UInt32)bytesNeeded, kMemoryMessage);
 
				}	// if (*pointerPtr != NULL)
		#endif
         			
		}	// end "if (CheckIfBytesRequestedAreWithinLimit (bytesNeeded))"
		
	return (returnFlag);
		
}	// end "MSetPointerSize"  



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OSXMemoryMessage
//
//	Software purpose:	The purpose of this routine is to display an alert indicating 
//                   that there is not enough memory available for the operation.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/09/2009
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

void OSXMemoryMessage (
				SInt64								bytesRequested)

{ 
	Str255								alertString;
	Str63									numberString;
	
	double								doubleBytesNeeded;
	
	SInt16								memoryError,
											numberChars;
	
	
	memoryError = ::MemError ();
	
	if (memoryError != noErr && gDisplayOSXMemoryMessageFlag)
		{
		gMemoryError = 1;
		
		doubleBytesNeeded = bytesRequested;
			
		numberString[0] = sprintf ((char*)&numberString[1], "%.0f", doubleBytesNeeded);
		
		numberChars = numberString[0];
		numberChars = InsertCommasInNumberString (
									(char*)&numberString[1], 
									numberChars, 
									-1,
									numberChars);
		numberString[0] = numberChars;
			
		alertString[0] = sprintf ((CharPtr)&alertString[1],
						"A request for %s bytes of memory could not be allocated. You can trying closing some image windows or other applications that are not needed.",
						(char*)&numberString[1]);

		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							0, 
							0,
							0,
							alertString);
		
		}	// end "if (memoryError != noErr && gDisplayOSXMemoryMessageFlag)"
		
}	// end "OSXMemoryMessage"
#endif		// defined multispec_mac



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseSpareMemoryForWarningMessage
//
//	Software purpose:	The purpose of this routine is to release the spare
//							memory in case it it is needed to display an Alert
//							message or the standard file dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/12/1993
//	Revised By:			Larry L. Biehl			Date: 02/12/1993	

void ReleaseSpareMemoryForWarningMessage (void)


{    
	UInt32								sizeSpare = 0;
	
	#if defined multispec_mac 
		if (gSpareWarnMemoryH)
			sizeSpare = ::GetHandleSize (gSpareWarnMemoryH);
			
		if (sizeSpare > 0)
			::SetHandleSize (gSpareWarnMemoryH, (Size)0);  
	#endif	// defined multispec_mac    

	#if defined multispec_win 
		if (gSpareWarnMemoryH != NULL)
			sizeSpare = (UInt32)::GlobalSize (gSpareWarnMemoryH);
			
		if (sizeSpare > 0)
			::GlobalReAlloc (gSpareWarnMemoryH, 0L, GMEM_MOVEABLE); 
	#endif	// defined multispec_win 
		
	#if defined multispec_lin
				// See what needs to be done for linux
	#endif

}	// end "ReleaseSpareMemoryForWarningMessage" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UnlockAndDispose
//
//	Software purpose:	This routine unlocks and disposes of the input
//							handle if it is not NULL
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			Mac OS Memory Manager
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1988
//	Revised By:			Larry L. Biehl			Date: 10/16/1995			

Handle UnlockAndDispose (
				Handle								handle)

{
	if (handle != NULL)
		{
		#if defined multispec_mac 
			HUnlock (handle);
			DisposeHandle (handle);
		#endif	// defined multispec_mac 
			
		#if defined multispec_win 
			Boolean		returnFlag = TRUE; 
			           
					// Make sure that the global lock count is zero 
					// This may not be needed as I can never get a lockcount greater
					// than 0. However it is included because of the way that I read
					// the documentation on lockcount, globalunlock and globallock.
					                     
			while (returnFlag)
				returnFlag = (Boolean)GlobalUnlock (handle); 
			/*
			SInt16 maxLoopCount = 255;
			while (maxLoopCount > 0)
				{                           
				if (!GlobalUnlock (handle))
					break;
					
				maxLoopCount--;
				
				}	// end "while (maxLoopCount > 0)"
			*/
			handle = GlobalFree (handle);
		#endif	// defined multispec_win 
		
      #if defined multispec_lin
			freememory (handle);
      #endif
		
		}	// end "if (handle != NULL)"
			
	return (NULL);               
		
}	// end "UnlockAndDispose"    

/** Notes For Linux version
 * C++ routines new and delete are type specific
 * So using malloc () in this program for now
 * No way to check size of memory allocated
 * To store size, use the first chunk of memory to store size
 * The following are Linux specific functions
 * TODO: Recheck code keeping in mind Ptr is a pointer so Ptr * needs to be
 * changed to *ptr before use
 */
#if defined multispec_lin
int getsize (
				void*									ptr)
				
{
	if (ptr != NULL)
		return *(UInt32*)((onebyte*)(ptr) - memsize);
		
	else
		return -1;
		
}	// end "getsize"



void freememory (
				void*									ptr)
				
{
	if (ptr != NULL)
		{
		free ((void*)((onebyte*)ptr-memsize));
		
		}
		
}	// end "freememory"



void* memallocate (
				UInt32								bytesneeded)
				
{
	void									*ptr;
	void									*ptr4;
	UInt32								*ptr2;
	UInt32								*ptr3;
	
	
			// Make total size with bytes needed and size count to be no less than 256 bytes
	
	bytesneeded = MAX (256-memsize, bytesneeded);
	
	ptr = malloc (bytesneeded + memsize);
	
	if (ptr != NULL)
		{
		memset (ptr, 0, bytesneeded+memsize);
		ptr2 = (UInt32*)ptr;
		*ptr2 = bytesneeded;
		ptr3 = (UInt32*)((onebyte*)ptr2 + memsize);
		ptr4 = (void*)ptr3;
		
		}	// end "if (ptr != NULL)"
		
	else
		ptr4 = NULL;

	return ptr4;
	
}	// end "memallocate"



void *memreallocate (
				UInt32								bytesneeded, 
				void*									handle)
				
{
	void									*ptr,
											*ptr4;
											
	UInt32								*ptr2,
											*ptr3;
											
	
			// Make total size with bytes needed and size count to be no less than 256 bytes
	
	bytesneeded = MAX (256-memsize, bytesneeded);
	
	ptr = realloc ((void*)((onebyte*)handle-memsize), bytesneeded+memsize);
	
	if (ptr != NULL)
		{
		ptr2 = (UInt32*)ptr;
		*ptr2 = bytesneeded;
		ptr3 = (UInt32*)((onebyte*)ptr2 + memsize);
		ptr4 = (void*)ptr3;
		
		}	// end "if (ptr != NULL)"
		
	else	// ptr == NULL
		ptr4 = NULL;

	return ptr4;
	 
}	// end "memreallocate"



void *memorycopy (
				void*									output, 
				void*									input)
				
{
	int input_memsize = getsize (input); 
	memcpy ((void*)((onebyte*)output-memsize), 
				(void*)((onebyte*)input-memsize), 
				(size_t)(input_memsize+memsize));
				
}
#endif
