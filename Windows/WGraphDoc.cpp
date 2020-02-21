//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						WGraphDoc.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMGraphDoc class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                     
#include "WGraphDoc.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CMGraphDoc, CDocument)

BEGIN_MESSAGE_MAP (CMGraphDoc, CDocument)
	//{{AFX_MSG_MAP (CMGraphDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMGraphDoc::CMGraphDoc ()

{
	m_graphViewCPtr = NULL;
	m_graphFrameCPtr = NULL;
	
}	// end "CMGraphDoc"



CMGraphDoc::~CMGraphDoc ()

{

}	// end "~CMGraphDoc"
 


CMGraphView* CMGraphDoc::GetGraphViewCPtr (void)

{  
	CMGraphView*						graphViewCPtr = NULL;
	
	
	if (this != NULL)
		graphViewCPtr = m_graphViewCPtr;
		
	return (graphViewCPtr);
	
}	// end "GetGraphViewCPtr"



BOOL CMGraphDoc::OnNewDocument ()

{
	if (!CDocument::OnNewDocument ())
																							return FALSE;

	SetTitle ((LPCTSTR)_T("Graph Window"));

	return TRUE;
	
}	// end "OnNewDocument"



void CMGraphDoc::SetGraphFrameCPtr	(
				CMGraphFrame*						graphFrameCPtr)
				
{                                   
	m_graphFrameCPtr = graphFrameCPtr; 
	 
}	// end "SetGraphFrameCPtr"

 

void CMGraphDoc::SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr)

{
	if (this != NULL)
		{
		m_graphViewCPtr = graphViewCPtr;
	
		if (m_graphFrameCPtr != NULL)
			m_graphFrameCPtr->SetGraphViewCPtr (m_graphViewCPtr);

		}	// end "if (this != NULL)"
	
}	// end "SetGraphViewCPtr"
