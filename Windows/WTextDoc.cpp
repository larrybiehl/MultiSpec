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
//	File:						WTextDoc.cpp : implementation file
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
//								CMTextDoc class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WImageView.h"
#include "WTextDoc.h"
#include "WTextView.h" 

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMTextDoc, CDocument)
	//{{AFX_MSG_MAP (CMTextDoc)
	ON_COMMAND (ID_VIEW_COORDINATES_BAR, OnViewCoordinatesBar)

	ON_UPDATE_COMMAND_UI (ID_VIEW_COORDINATES_BAR, OnUpdateViewCoordinatesBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



IMPLEMENT_SERIAL (CMTextDoc, CDocument, 0 /* schema number*/)

CMTextDoc::CMTextDoc ()

{ 

			// default document size is 700 x 800 screen pixels
		
	m_sizeDoc = CSize (600,700);
	
}	// end "CMTextDoc"



CMTextDoc::~CMTextDoc ()

{

}	// end "~CMTextDoc"



void CMTextDoc::InitDocument ()

{

			// default document size is 700 x 800 screen pixels
		
	//m_sizeDoc = CSize (700,800);

}	// end "InitDocument"



void CMTextDoc::OnCloseDocument ()

{                                                                               
	CDocument::OnCloseDocument ();
	
}	// end "OnCloseDocument"



BOOL CMTextDoc::OnNewDocument ()

{
	if (!CDocument::OnNewDocument ())
																							return FALSE;
	
	SetTitle (_T("Text Output"));
		
	return TRUE;
	
}	// end "OnNewDocument"



BOOL CMTextDoc::OnSaveDocument (
				LPCTSTR								lpszPathName)

{  
	BOOL									returnFlag;


	returnFlag = CDocument::OnSaveDocument (lpszPathName);

	if (returnFlag)
		gOutputViewCPtr->GetEditCtrl ().SetModify (FALSE);

	return (returnFlag);
	
}	// end "OnSaveDocument"



void CMTextDoc::OnViewCoordinatesBar (void)

{
			// Add your command handler code here
	
}	// end "OnViewCoordinatesBar"



void CMTextDoc::OnUpdateViewCoordinatesBar (
				CCmdUI* 								pCmdUI)
				
{                                                        
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateViewCoordinatesBar"



BOOL CMTextDoc::SaveModified (void)

{
   		// The user will be requested if they wish to save the output text window.
		
	Boolean modifiedFlag = gOutputViewCPtr->GetEditCtrl ().GetModify ();
   
   if (modifiedFlag && gOutputViewCPtr->GetTextBufferLength () > 0)
		{
		OnFileSaveAs ();
		gOutputViewCPtr->GetEditCtrl ().SetModify (FALSE);

		}	// end "if (modifiedFlag && ...->GetTextBufferLength () > 0)"
   		
	return (TRUE);
	
}	// end "SaveModified"



void CMTextDoc::Serialize (
				CArchive& 							ar)

{
	if (ar.IsStoring ())
		{
				// Add storing code here
		
		gOutputViewCPtr->Serialize (ar);
	
		}	// end "if (ar.IsStoring ())"
	
	else	// !ar.IsStoring ()
		{
				// Add loading code here
		
		}	// end "else !ar.IsStoring ()"
	
}	// end "Serialize"
