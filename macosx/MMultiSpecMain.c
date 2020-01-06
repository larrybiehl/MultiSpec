//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
// File:						MMultiSpec.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.1
//
//	Revision date:			03/12/2018
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	This program is a image analysis program patterned
//								after LARSYS that runs on an IBM VM system.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Functions in file:	void 					main
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

//#include 	"SMultiSpec.h"
#define _DARWIN_UNLIMITED_STREAMS
#include <stdio.h>

#include "SDeclareGlobals.h"

SInt32					gNumberOfOpenFilesHardLimit = 32000;



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		main
//
//	Software purpose:	The purpose of this routine is to control the 
//							MultiSpec system from initialization thru stopping.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			Operating system
//
//	Coded By:			Larry L. Biehl			Date: 03/26/1988
//	Revised By:			Larry L. Biehl			Date: 03/12/2018

int main (void)
 
{
			// Declare local variables		
	
	rlimit								resourceLimits;
	
	#ifndef __XCODE__		
		SInt32								freeBytesPossible,
												longestContBlock;
	#endif	// ndef __XCODE__

											
	
	#if !TARGET_API_MAC_CARBON
				// Allocate more master pointers.  Do this here so that they			
				// are at the bottom of the heap before 'InitializeMultiSpec' is 		
				// loaded into memory.																
				// First determine how much memory is allocated for MultiSpec and		
				// then determine the number of masters to allocate. 		
				// The number of master pointers will be based on the amount of		
				// memory available (i.e. the potential size of the text in the		
				// output text window.																
				// The shift instruction is used to divide by around 131,000.			
				// Dividing is not done here so that no problems occur if there		
				// is no coprocessor.																
				
		longestContBlock = (SInt32)GetApplLimit () - (SInt32)ApplicationZone ();

				// Verify that there is enough memory to continue.
				
		if (longestContBlock < 270000)
																							ExitToShell ();
																				
		longestContBlock = (longestContBlock >> 17);
		longestContBlock += 11;
		longestContBlock = MIN (longestContBlock, 50);
				
		
		SInt32 count;
		for (count=0; count<longestContBlock; count++)					
			MoreMasters ();
	#endif	// !TARGET_API_MAC_CARBON
	
			// File resource limits
			// First set to a high value so that errors are not listed without
			// really knowing what the limit is.
			// Put at very beginning of main routine. Otherwise the change in limits
			// does not really take affect.
	
	if (getrlimit (RLIMIT_NOFILE, &resourceLimits) == 0)
		{
		gNumberOfOpenFilesLimit = (SInt32)resourceLimits.rlim_cur;
		gNumberOfOpenFilesHardLimit = (SInt32)MIN (resourceLimits.rlim_max, SInt32_MAX);
		
		if (resourceLimits.rlim_cur < 4096)
			{					
			resourceLimits.rlim_cur = 4096;
			resourceLimits.rlim_max = MAX (8192, resourceLimits.rlim_max);
			
			if (setrlimit (RLIMIT_NOFILE, &resourceLimits) == 0)
				gNumberOfOpenFilesLimit = (SInt32)resourceLimits.rlim_cur;
			
			}	// end "if (resourceLimits.rlim_cur < 4096)"
			
		}	// end "if (getrlimit (RLIMIT_NOFILE, &resourceLimits) == 0)"
	
			// Indicate a few less than the limit. Going to the limit has
			// caused MultiSpec to crash.
			
	gNumberOfOpenFilesLimit -= 20;
		
			// Now force a block of memory to be allocated which will be used by
			// the New operator.
			
	UInt32* a;		
	a = new (UInt32);
	delete a;
	
			// Set possible application heap size to maximum							
	
	#if !TARGET_API_MAC_CARBON
		SetApplLimit ((Ptr)((UInt32)GetApplLimit () - 5000));
		MaxApplZone ();
	#endif	// !TARGET_API_MAC_CARBON
	
			// Get some memory to force 'InitializeMultiSpec' to be loaded			
			// higher in memory.  This temporary memory will then be released		
			// after 'InitializeMultiSpec' is entered to be used for 				
			// non-relocatable memory needs.	
	
	#if TARGET_CPU_68K
		gTempMatrixPtr = (double*)NewPtr (4000L);
		if (gTempMatrixPtr == NULL)
																							ExitToShell ();
	#endif	// TARGET_CPU_68K
		
	#ifndef __XCODE__																				
				// Get some memory to force 'InitializeMultiSpec' to not be loaded	
				// at the top of memory.															
		
		gSpareCodeMemoryH = NULL;
		PurgeSpace (&freeBytesPossible, &longestContBlock);
		longestContBlock -= 8000;
		if (longestContBlock > 1000)
			{
			gSpareCodeMemoryH = NewHandle (longestContBlock);
			HLockHi (gSpareCodeMemoryH);
		
			}	// end "if (longestContBlock > 1000)"
	#endif		// ndef __XCODE__
	
			// Initialize MultiSpec.															
			
	InitializeMultiSpec ();
	/*		
			// The following is for testing.
			
	rlimit						resourceLimits;
	getrlimit (RLIMIT_NOFILE, &resourceLimits);
	sprintf ((char*)&gTextString, 
				" rlim_cur = %d%s gNumberOfOpenFilesLimit = %d%s",
				(int)resourceLimits.rlim_cur,
				gEndOfLine, 
				//resourceLimits.rlim_max,
				//gEndOfLine,
				gNumberOfOpenFilesLimit,
				gEndOfLine);
				
	OutputString (NULL, 
						(char*)&gTextString, 
						0, 
						gOutputForce1Code,
						true);
	*/
			// Get information about files that are to be opened on start up.		
	
	#ifndef __XCODE__		
		if (!gDoneFlag)
			{
					// Purge memory of unneeded resources and compact the memory for	
					// the maximum free memory possible.										
					
			PurgeSpace (&freeBytesPossible, &longestContBlock); 
			PurgeMem (freeBytesPossible);
			longestContBlock = CompactMem (freeBytesPossible);
			
			}	// end "if (!gDoneFlag)" 
	#endif	// ndef __XCODE__

	InitializeDateVersionStrings ();
	
	gAlgebraicTransformationFunction.denominatorString[0] = 0;
	gAlgebraicTransformationFunction.numeratorString[0] = 0;
	gAlgebraicTransformationFunction.transformFactor = 1;
	gAlgebraicTransformationFunction.transformOffset = 0;
	gAlgebraicTransformationFunction.instrumentCode = 0;
	gAlgebraicTransformationFunction.numberChannels = 0;
		
	if (!gDoneFlag)
		DoMultiSpec ();
		
			// Exit MultiSpec 																	
	
	ExitToShell ();
	
}	// end of main 
