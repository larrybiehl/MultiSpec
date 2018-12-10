//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CMProcessor.cpp
//	Class Definition:		CMProcessor.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/09/2018
//
//	Language:				C++
//
//	System:					Windows Operating Systems
//
//	Brief description:	This file contains routines for the CMProcessor class
//
//	Functions in file:						CMProcessor             
//													~CMProcessor
//													 
//
//	Include files:			"SMultiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "CImageWindow.h"
#include "CProcessor.h"

#include "WImageView.h"
#include "WImageDoc.h"

#if defined multispec_lin
	#include "LImageDoc.h"
	#include "LImageView.h"
#endif


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMProcessor
//
//	Software purpose:	Constructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMProcessor::CMProcessor ()
{

}	// end "CMProcessor"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		~CMProcessor
//
//	Software purpose:	Destructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMProcessor::~CMProcessor ()
{

}	// end "~CMProcessor"


#if defined multispec_win 
//------------------------------------------------------------------------------------
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
//	Coded By:			Larry L. Biehl			Date: 07/18/1995
//	Revised By:			Larry L. Biehl			Date: 07/18/1995	

CMImageDoc* CMProcessor::GetActiveImageDocCPtr (void)

{
	if (gActiveImageViewCPtr != NULL)
		return ((CMImageDoc*)gActiveImageViewCPtr->GetDocument ());
		
	else	// gActiveImageWindowCPtr == NULL
		return (NULL);
	
}	// end "GetActiveImageDocCPtr"  



//------------------------------------------------------------------------------------
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
//	Coded By:			Larry L. Biehl			Date: 07/18/1995
//	Revised By:			Larry L. Biehl			Date: 07/18/1995	

CMOutlineArea* CMProcessor::GetActiveSelectionAreaCPtr (void)

{  
	CMOutlineArea*		outlineAreaCPtr = NULL;
	
	if (gActiveImageViewCPtr != NULL)
		{
		CMImageDoc* imageDocCPtr = (CMImageDoc*)gActiveImageViewCPtr->GetDocument ();
								
		outlineAreaCPtr = imageDocCPtr->GetSelectionAreaCPtr();
								
		}	// end "if (gActiveImageViewCPtr != NULL)"
		
	return (outlineAreaCPtr);
	
}	// end "GetActiveSelectionAreaCPtr"



//------------------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMOutlineArea* GetSelectionAreaCPtr
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
//	Coded By:			Larry L. Biehl			Date: 04/03/1998
//	Revised By:			Larry L. Biehl			Date: 04/03/1998

CMOutlineArea* CMProcessor::GetSelectionAreaCPtr (
				CMImageView*						imageViewCPtr)

{  
	CMOutlineArea*		outlineAreaCPtr = NULL;
	
	
	if (imageViewCPtr != NULL)
		{
		CMImageDoc* imageDocCPtr = (CMImageDoc*)imageViewCPtr->GetDocument();
		
		if (imageDocCPtr != NULL)						
			outlineAreaCPtr = imageDocCPtr->GetSelectionAreaCPtr ();
								
		}		// end "if (gActiveImageViewCPtr != NULL)"
		
	return (outlineAreaCPtr);
	
}		// end "GetSelectionAreaCPtr"

 

CMOutlineArea* CMProcessor::GetSelectionAreaCPtr (
				WindowInfoPtr						imageWindowInfoPtr)

{                                                  
	CMOutlineArea*					outlineAreaCPtr = NULL; 
	
	
	if (imageWindowInfoPtr != NULL)
		{   	                            
		CMImageWindow* imageWindowCPtr = imageWindowInfoPtr->cImageWindowPtr;
	      	
		if (imageWindowCPtr != NULL)
			{
			CMImageDoc* imageDocCPtr = (CMImageDoc*)imageWindowCPtr->GetImageDocCPtr ();
				
			if (imageDocCPtr != NULL) 
				outlineAreaCPtr = imageDocCPtr->GetSelectionAreaCPtr ();
				
			}		// end "if (imageWindowCPtr != NULL)"
			
		}		// end "if (imageWindowInfoPtr != NULL)"
		
	return (outlineAreaCPtr);
	
}		// end "GetSelectionAreaCPtr"
#endif	// defined multispec_win 
