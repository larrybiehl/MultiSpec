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
//	File:						xImageDoc.cpp : class implementation file
//	Class Definition:		xImageDoc.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rachman Maud
//
//	Revision date:			11/25/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMImageDoc class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" xImageDoc:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SImageWindow_class.h"

#include "xDrawObjects.h"
#include "xImageDoc.h"
#include "xImageFrame.h"
#include "xImageView.h"



IMPLEMENT_DYNAMIC_CLASS (CMImageDoc, wxDocument)


CMImageDoc::CMImageDoc ()

{
			// Default document size is 150 x 150 screen pixels

	m_sizeDoc = wxSize (150, 150);

	m_imageWindowCPtr = NULL;
	m_imageFrameCPtr = NULL;
	m_imageViewCPtr = NULL;
	m_selectionAreaCPtr = NULL;
	m_initializedFlag = FALSE;

	m_displayCoordinatesFlag = FALSE;
	m_updateflag = false;

			// Create selection object

	try
		{
		m_selectionAreaCPtr = new CMOutlineArea ();

		}

	catch (int e)
		{
		MemoryMessage (0, kObjectMessage);
		
		}

	if (m_selectionAreaCPtr != NULL)
		{
		m_selectionAreaCPtr->m_document = this;
		Add (m_selectionAreaCPtr);
		m_initializedFlag = TRUE;

		}	// end "if (m_selectionAreaCPtr != NULL)"

}	// end "CMImageDoc"



CMImageDoc::~CMImageDoc ()

{

}	// end "~CMImageDoc"


void CMImageDoc::Add (
				CMDrawObject* 						pObject)

{
    
    m_objects.Append (pObject);
    pObject->m_document = this;

}	// end "Add"



bool CMImageDoc::Close ()

{
	Handle 								windowInfoHandle;
	bool 									closeFlag = true;

	
	if (gProcessorCode != -1)
		{
		windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);

		closeFlag = !SaveIfWindowChanged (windowInfoHandle, FALSE);
		
				// Indicate that any desired changes were already saved.
				
		Modify (false);

		}	// end "if (gProcessorCode != -1)"

   return closeFlag;
	
}	// end "Close"



void CMImageDoc::DrawObjects (
				CDC* 									pDC,
				CMImageView* 						pImageView)

{
	wxNode* pos = m_objects.GetFirst ();
	while (pos)
    	{
		CMDrawObject* object = (CMDrawObject*)pos->GetData ();
		object->Draw (pDC, pImageView);
		pos = pos->GetNext ();
		
    	}	// end "while (pos != NULL)"

}	// end "DrawObjects"


Boolean CMImageDoc::GetCoordinateFlag ()

{
    return m_displayCoordinatesFlag;
	
}	// end "GetCoordinateFlag"



void CMImageDoc::InitDocument ()

{
			// Default document size is 150 x 150 screen pixels

	m_sizeDoc = CSize (150, 150);

}	// end "InitDocument"



bool CMImageDoc::IsModified (void) const

{
   return m_updateflag;
	
}	// end "IsModified"



void CMImageDoc::Modify (
				bool 									mod)

{
	m_updateflag = mod;
   wxDocument::Modify (mod);
	
}	// end "Modify"



bool CMImageDoc::OnNewDocument ()

{
	return FALSE;

}	// end "OnNewDocument"



bool CMImageDoc::OnOpenDocument (
				const wxString&					pszPathName)

{
	SInt16 								errCode = noErr;
	Boolean 								returnFlag = FALSE;
	
	
			// Verify that the document class was intialized properly.

	if (!m_initializedFlag)
																						return (FALSE);

			// Verify that the image view class was intialized properly.

	CMImageView* imageViewCPtr = (CMImageView*)this->GetFirstView ();
	if (!imageViewCPtr->m_initializedFlag)
		return (FALSE);

	m_imageWindowCPtr = imageViewCPtr->GetImageWindowCPtr ();

	if (m_imageWindowCPtr != NULL && m_imageWindowCPtr->m_initializedFlag)
		{
				// Save image document class pointer in the image window class

		m_imageWindowCPtr->mImageDocCPtr = this;

				// Save the image view class pointer in the windows info structure

		m_imageWindowCPtr->SetImageWindowCPtrInWindowInfo (m_imageWindowCPtr);

		errCode = m_imageWindowCPtr->OpenImageWindow ();
		returnFlag = (errCode == noErr);

		}	// end "if (imageWindowPtr != NULL)"

	return returnFlag;

}	// end "OnOpenDocument"



Boolean CMImageDoc::OnViewCoordinatesBar ()

{
    m_displayCoordinatesFlag = !m_displayCoordinatesFlag;

    return m_displayCoordinatesFlag;

}	// end "OnViewCoordinatesBar"



void CMImageDoc::Remove (
				CMDrawObject* 						pObject)

{
			// Find and remove from document

	wxNode* pos;
	pos = m_objects.Find ((wxObject*)pObject);
	if (pos != NULL)
		m_objects.DeleteNode (pos);

}	// end "Remove"


bool CMImageDoc::SaveAs ()

{
   SaveImageWindowAs ();
	
   return (true);
	
}	// end "SaveAs"



void CMImageDoc::SetDocSize (
				SInt16 								height,
				SInt16 								width)

{
    m_sizeDoc = CSize (width, height);

}	// end "SetDocSize"



void CMImageDoc::SetImageFrameCPtr (
				CMImageFrame* 						imageFrameCPtr)

{
    m_imageFrameCPtr = imageFrameCPtr;

}	// end "SetImageFrameCPtr"



void CMImageDoc::SetImageWindowCPtr (
				CMImageWindow* 					imageWindowCPtr)

{
	m_imageWindowCPtr = imageWindowCPtr;

}	// end "SetImageWindowCPtr"



bool CMImageDoc::SetImageWindowCPtrV (
				CMImageView*						view)

{
	SInt16 								errCode = noErr;
	Boolean 								returnFlag = FALSE;


			// Verify that the document class was intialized properly.

	if (!m_initializedFlag)
																						return (FALSE);

			// Verify that the image view class was intialized properly.

	if (!view->m_initializedFlag)
																						return (FALSE);

	m_imageViewCPtr = view;
	m_imageWindowCPtr = view->GetImageWindowCPtr ();

	if (m_imageWindowCPtr != NULL && m_imageWindowCPtr->m_initializedFlag) 
		{
				// Save image document class pointer in the image window class

		m_imageWindowCPtr->mImageDocCPtr = this;

				// Save the image view class pointer in the windows info structure

		m_imageWindowCPtr->SetImageWindowCPtrInWindowInfo (m_imageWindowCPtr);

		errCode = m_imageWindowCPtr->OpenImageWindow ();
		returnFlag = (errCode == noErr);

		}	// end "if (imageWindowPtr != NULL)"

    return returnFlag;
	 
}	// end "SetImageWindowCPtrV"
