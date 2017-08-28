//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2016)
//							c Purdue Research Foundation
//									All rights reserved.
//
//	File:						MProcess.cpp
//
// Class Declaration		MCProcessProcess.h		<- double-click + Command-D to open
//
//	Functions in file:	
//
//	Brief description:	The purpose of the routines in this file is to
//								display multispectral type images
//
//	Language:				C++
//
//	System:					Macintosh Operating System
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		2.8
//
//	Revision date:			01/20/2016


#include "SMulSpec.h"
#include "CProcess.h"
#include "CImagWin.h"

#if defined multispec_win
	#include "CImagVew.h"
	#include "WImagDoc.h"
#endif

#if defined multispec_lin
	#include "LImageDoc.h"
	#include "LImageView.h"
#endif

#include	"SExtGlob.h"


// === Static Member Variables ===
                                                     					

// ---------------------------------------------------------------------------
//		CMProcessor
// ---------------------------------------------------------------------------
//	Constructor

CMProcessor::CMProcessor()
{

}		// end "CMProcessor"


// ---------------------------------------------------------------------------
//		~CMProcessor
// ---------------------------------------------------------------------------
//	Destructor
//

CMProcessor::~CMProcessor()
{

}		// end "~CMProcessor"




#if defined multispec_win 
//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GetActiveImageDocCPtr
//
//	Software purpose:	The purpose of this routine is to return the document class
//							pointer for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/18/95
//	Revised By:			Larry L. Biehl			Date: 07/18/95	

CMImageDoc* 
CMProcessor::GetActiveImageDocCPtr (void)

{
	if (gActiveImageViewCPtr != NULL)
		return ( (CMImageDoc*)gActiveImageViewCPtr->GetDocument() );
		
	else	// gActiveImageWindowCPtr == NULL
		return (NULL);
	
}		// end "GetActiveImageDocCPtr"  



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GetActiveSelectionAreaCPtr
//
//	Software purpose:	The purpose of this routine is to return the selection area
//							class pointer for the active image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	Selection area class pointer		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/18/95
//	Revised By:			Larry L. Biehl			Date: 07/18/95	

CMOutlineArea* 
CMProcessor::GetActiveSelectionAreaCPtr (void)

{  
	CMOutlineArea*		outlineAreaCPtr = NULL;
	
	if (gActiveImageViewCPtr != NULL)
		{
		CMImageDoc* imageDocCPtr = 
									(CMImageDoc*)gActiveImageViewCPtr->GetDocument();
								
		outlineAreaCPtr = imageDocCPtr->GetSelectionAreaCPtr();
								
		}		// end "if (gActiveImageViewCPtr != NULL)"
		
	return (outlineAreaCPtr);
	
}		// end "GetActiveSelectionAreaCPtr"



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GetSelectionAreaCPtr
//
//	Software purpose:	The purpose of this routine is to return the selection area
//							class pointer for the input image view class pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	Selection area class pointer		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/03/98
//	Revised By:			Larry L. Biehl			Date: 04/03/98

CMOutlineArea* 
CMProcessor::GetSelectionAreaCPtr (
				CMImageView*						imageViewCPtr)

{  
	CMOutlineArea*		outlineAreaCPtr = NULL;
	
	
	if (imageViewCPtr != NULL)
		{
		CMImageDoc* imageDocCPtr = (CMImageDoc*)imageViewCPtr->GetDocument();
		
		if (imageDocCPtr != NULL)						
			outlineAreaCPtr = imageDocCPtr->GetSelectionAreaCPtr();
								
		}		// end "if (gActiveImageViewCPtr != NULL)"
		
	return (outlineAreaCPtr);
	
}		// end "GetSelectionAreaCPtr"

 

CMOutlineArea* 
CMProcessor::GetSelectionAreaCPtr (
				WindowInfoPtr						imageWindowInfoPtr)

{                                                  
	CMOutlineArea*					outlineAreaCPtr = NULL; 
	
	
	if (imageWindowInfoPtr != NULL)
		{   	                            
		CMImageWindow* imageWindowCPtr = imageWindowInfoPtr->cImageWindowPtr;
	      	
		if (imageWindowCPtr != NULL)
			{
			CMImageDoc* imageDocCPtr = (CMImageDoc*)imageWindowCPtr->GetImageDocCPtr();
				
			if (imageDocCPtr != NULL) 
				outlineAreaCPtr = imageDocCPtr->GetSelectionAreaCPtr();
				
			}		// end "if (imageWindowCPtr != NULL)"
			
		}		// end "if (imageWindowInfoPtr != NULL)"
		
	return (outlineAreaCPtr);
	
}		// end "GetSelectionAreaCPtr"

#endif	// defined _X86_ 
