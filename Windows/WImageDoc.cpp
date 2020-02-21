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
//	File:						WImageDoc.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/05/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMImageDoc class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SImageWindow_class.h"

#include "WDrawObjects.h"
#include "WImageDoc.h"
#include "WImageFrame.h"
#include "WImageView.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CMImageDoc, CDocument);

BEGIN_MESSAGE_MAP (CMImageDoc, CDocument)
	//{{AFX_MSG_MAP (CMImageDoc)
	ON_COMMAND (ID_FILE_SAVE, OnFileSave)
	ON_COMMAND (ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND (ID_VIEW_COORDINATES_BAR, OnViewCoordinatesBar)
	ON_COMMAND (ID_WINDOW_SHOW_COORDINATE_VIEW, OnWindowShowCoordinateView)

	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI (ID_VIEW_COORDINATES_BAR, OnUpdateViewCoordinatesBar)
	ON_UPDATE_COMMAND_UI (ID_WINDOW_SHOW_COORDINATE_VIEW, OnUpdateWindowShowCoordinateView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMImageDoc::CMImageDoc ()

{
			// default document size is 800 x 900 screen pixels
		
	m_sizeDoc = CSize (100,100);
	
	m_imageWindowCPtr = NULL;
	m_imageFrameCPtr = NULL;
	m_selectionAreaCPtr = NULL;
	m_initializedFlag = FALSE;
		
	m_displayCoordinatesFlag = FALSE;
	
			// Create selection object
	
	TRY
		{		
		m_selectionAreaCPtr = new CMOutlineArea ();
		
		}
		
	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		}
	
	END_CATCH_ALL
				
	if (m_selectionAreaCPtr != NULL)
		{ 
		Add (m_selectionAreaCPtr);
		m_initializedFlag = TRUE;
		
		}	// end "if (m_selectionAreaCPtr != NULL)"
		
}	// end "CMImageDoc"



CMImageDoc::~CMImageDoc ()

{  
			// delete draw objects
			      
	POSITION pos = m_objects.GetHeadPosition ();
	while (pos != NULL)
		delete m_objects.GetNext (pos);
	
}	// end "~CMImageDoc"



void CMImageDoc::Add (
				CMDrawObject* 						pObject)

{
	m_objects.AddTail (pObject);
	pObject->m_document = this;
	
}	// end "Add"


#ifdef _DEBUG
	void CMImageDoc::AssertValid () const

	{
		CDocument::AssertValid ();
	
	}
#endif    //_DEBUG


void CMImageDoc::Draw (
				CDC* 									pDC,
				CMImageView* 						pImageView)
				
{
	POSITION pos = m_objects.GetHeadPosition ();
	while (pos != NULL)
		{
		CMDrawObject* object = (CMDrawObject*)m_objects.GetNext (pos);
		object->Draw (pDC, pImageView);
			
		}	// end "while (pos != NULL)"
		
}	// end "Draw"


#ifdef _DEBUG
	void CMImageDoc::Dump (
					CDumpContext& 						dc) const
	{
		CDocument::Dump (dc);
	
	}
#endif    //_DEBUG



void CMImageDoc::InitDocument ()

{
		// default document size is 800 x 900 screen pixels
		
	m_sizeDoc = CSize (100, 100);

}	// end "InitDocument"



void CMImageDoc::OnCloseDocument ()

{  
	Handle								windowInfoHandle;
	
	Boolean								closeFlag = TRUE;
	
	
	if (gProcessorCode != -1 && m_imageWindowCPtr != NULL)
		{
		windowInfoHandle = m_imageWindowCPtr->GetWindowInfoHandle ();
		                          
		closeFlag = !SaveIfWindowChanged (windowInfoHandle, FALSE); 
		
		}	// end "if (gProcessorCode != -1 && ..."
		                
	if (closeFlag || gProcessorCode == -1)
		CDocument::OnCloseDocument ();
	
}	// end "OnCloseDocument"



void CMImageDoc::OnFileSave ()

{    
	gProcessorCode = kSaveProcessor;
	WriteThematicInfo (NULL, 0);
	gProcessorCode = 0;
	
}	// end "OnFileSave"



void CMImageDoc::OnFileSaveAs ()

{                                                                    
	SaveImageWindowAs ();
	
}	// end "OnFileSaveAs"



BOOL CMImageDoc::OnNewDocument ()

{
	//if (!CDocument::OnNewDocument ())
	//	return FALSE;

			// Add reinitialization code here
			// (SDI documents will reuse this document)

	return FALSE;
	
}	// end "OnNewDocument"



BOOL CMImageDoc::OnOpenDocument (
				LPCTSTR								pszPathName)
		
{    
	SInt16 								errCode = noErr;
	
	Boolean 								returnFlag = FALSE;
	
	
	
			// Verify that the document class was intialized properly.
			
	if (!m_initializedFlag)
																						return (FALSE);
																			
			// Verify that the image view class was intialized properly.
			
	POSITION pos = GetFirstViewPosition ();
	CMImageView* imageViewCPtr = (CMImageView*)GetNextView (pos);
	           
	if (pos != NULL && !imageViewCPtr->IsKindOf (RUNTIME_CLASS (CMImageView)))
		imageViewCPtr = (CMImageView*)GetNextView (pos);
	
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



void CMImageDoc::OnUpdateFileSave (
				CCmdUI* 								pCmdUI)

{                                                                                                                                                                         
	pCmdUI->Enable (UpdateFileSave (pCmdUI));
	
}	// end "OnUpdateFileSave"

  

void CMImageDoc::OnUpdateFileSaveAs (
				CCmdUI* 								pCmdUI)

{                                                                                 
	Boolean 								enableFlag = FALSE;
	
	                                                                                                                                                               
	Handle windowInfoHandle = GetActiveImageWindowInfoHandle (); 
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																						kLock);
	
   enableFlag = UpdateFileImageSaveAs (pCmdUI, windowInfoPtr);
		
	CheckAndUnlockHandle (windowInfoHandle); 
		
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateFileSaveAs"



void CMImageDoc::OnUpdateViewCoordinatesBar (
				CCmdUI* 								pCmdUI)
				
{                                                        
	pCmdUI->Enable (TRUE);
	pCmdUI->SetCheck (m_displayCoordinatesFlag);
	
}	// end "OnUpdateViewCoordinatesBar"



void CMImageDoc::OnUpdateWindowShowCoordinateView (
				CCmdUI* 								pCmdUI)

{                             
	pCmdUI->Enable (UpdateWindowCoordinateView (pCmdUI));
	
}	// end "OnUpdateWindowShowCoordinateView"



void CMImageDoc::OnViewCoordinatesBar (void)

{ 	 
	Boolean								displayCoordinatesFlag;


			// Toggle the coordinates bar

	displayCoordinatesFlag = !m_displayCoordinatesFlag;

	ShowCoordinateView (displayCoordinatesFlag);
	
}	// end "OnViewCoordinatesBar"



void CMImageDoc::OnWindowShowCoordinateView ()

{
	Boolean								displayCoordinatesFlag;
	
	
			// Toggle the coordinates bar

	displayCoordinatesFlag = !m_displayCoordinatesFlag; 

	ShowCoordinateView (displayCoordinatesFlag);
	
}	// end "OnWindowShowCoordinateView"



void CMImageDoc::Remove (
				CMDrawObject* 						pObject)
				
{
			// Find and remove from document
			
	POSITION pos = m_objects.Find (pObject);
	if (pos != NULL)
		m_objects.RemoveAt (pos);
		
			// set document modified flag
	
	//SetModifiedFlag ();

			// call remove for each view so that the view can remove from mSelection
			
	pos = GetFirstViewPosition ();
	while (pos != NULL)
		((CMImageView*)GetNextView (pos))->Remove (pObject);
		
}	// end "Remove"



void CMImageDoc::SetDocSize (
				SInt16								height,
				SInt16								width)
				
{                                   
	m_sizeDoc = CSize (width, height);
	 
}	// end "SetDocSize" 



void CMImageDoc::SetImageFrameCPtr (
				CMImageFrame*						imageFrameCPtr)
				
{                                   
	m_imageFrameCPtr = imageFrameCPtr;
	 
}	// end "SetImageFrameCPtr" 



void CMImageDoc::SetImageWindowCPtr (
				CMImageWindow*						imageWindowCPtr)
				
{                                   
	if (this != NULL)
		m_imageWindowCPtr = imageWindowCPtr;
	 
}	// end "SetImageWindowCPtr"



void CMImageDoc::ShowCoordinateView (
				Boolean								displayCoordinatesFlag)

{
	m_displayCoordinatesFlag = displayCoordinatesFlag;

			// Allow for m_displayCoordinatesFlag to be set before the
			// image view is set up.
	
	if (m_imageWindowCPtr->mImageViewCPtr != NULL)
		{
				// Toggle the coordinates bar
		
		if (m_displayCoordinatesFlag)
			m_imageFrameCPtr->UpdateSelectionCoordinates ();
			
		m_imageFrameCPtr->OnViewCoordinatesBar (m_displayCoordinatesFlag);

		}	// end "if (m_imageWindowCPtr->mImageViewCPtr != NULL)"
	
}	// end "ShowCoordinateView"
