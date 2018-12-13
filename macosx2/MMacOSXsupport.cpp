//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
// File:						MMacOSXsupport.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/22/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The routines in this file support OSX for MultiSpec.
//
//
//	Functions in file:	
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

//#include "SMultiSpec.h"
//#include "SExtGlob.h"
								


		// Prototypes for routines in this file that are only called by		
		// other routines in this file.



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OSErr CreateFrameworkBundleFromName
//
//	Software purpose:	
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/2001
//	Revised By:			Larry L. Biehl			Date: 10/12/2001		

OSErr CreateFrameworkBundleFromName (
				CFStringRef 						theFrameworkName, 
				CFBundleRef*						theBundle)
				
{
	OSErr 								theErr;
	FSRef 								theRef;
	CFURLRef 							theFrameworkURL;
	CFURLRef 							theBundleURL;
	
	
			// Find the folder containing all the frameworks
			
	theErr = FSFindFolder (kOnAppropriateDisk, kFrameworksFolderType, false, &theRef);
	
	if (theErr == noErr)
		{
				// Turn the framework folder FSRef into a CFURL
				
		theFrameworkURL = CFURLCreateFromFSRef (kCFAllocatorSystemDefault, &theRef);
		
		if (theFrameworkURL != NULL)
			{
					// Create a CFURL pointing to the desired framework
					
			theBundleURL = CFURLCreateCopyAppendingPathComponent (
																				kCFAllocatorSystemDefault,
																				theFrameworkURL, 
																				theFrameworkName, 
																				false);
			
			CFRelease (theFrameworkURL);
			
			if (theBundleURL != NULL)
				{
						// Turn the CFURL into a bundle reference
						
				*theBundle = CFBundleCreate (kCFAllocatorSystemDefault, theBundleURL);
				
				CFRelease (theBundleURL);
				
				}	// end "if (theBundleURL != NULL)"
				
			}	// end "if (theFrameworkURL != NULL)"
			
		}	// end "if (theErr == noErr)"
	
	return theErr;
	
}	// end "CreateFrameworkBundleFromName"