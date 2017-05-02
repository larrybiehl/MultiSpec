//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2011)
//							c Purdue Research Foundation
//									All rights reserved.
//
//	File:						WResources.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			12/15/2011
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Functions in file:	
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec for
//								accessing the resources in MultiSpec.

#include "SMulSpec.h"
 

#include	"SExtGlob.h"
		
		

extern char*		LoadFileInResource (
							int									name,
							int									type,
							int*									size);

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		LoadFileInResource
//
//	Software purpose:	This routine loads the specified file resource
//
//	Parameters in:				
//
//	Parameters out:				
//
//Value Returned:		None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/15/2011
//	Revised By:			Larry L. Biehl			Date: 12/15/2011		

char* LoadFileInResource (
				int									name,
				int									type,
				int*									sizePtr)
{
	char*									dataPtr = NULL;

	HMODULE								handle;
	HRSRC									rc = NULL;
	HGLOBAL								rcData = NULL;


	*sizePtr = 0;
	handle = ::GetModuleHandle (NULL);

	if (handle != NULL)
		rc = ::FindResource (handle, 
									MAKEINTRESOURCE (IDR_EPSG_GCS),
									"EPSG_CSV");

	if (rc != NULL)
		{
		rcData = ::LoadResource (handle, rc);
		*sizePtr = ::SizeofResource (handle, rc);

		}		// end "if (rc != NULL)"

	if (rcData != NULL)
		dataPtr = (char*)(::LockResource (rcData));

	return (dataPtr);

}		// end "LoadFileInResource"
*/