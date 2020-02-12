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
//	File:						xStatisticsView.cpp : class implementation file
//	Class Definition:		xStatisticsView.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/11/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMStatisticsView class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xImageFrame.h"
#include "xMainFrame.h"
#include "xMultiSpec.h" 
#include "xStatisticsView.h"
#include "xStatisticsDoc.h"
#include "xStatisticsFrame.h"
#include "xTools.h"
	
#include "wx/wx.h"
#include "wx/docview.h"

typedef wxString CString;



IMPLEMENT_DYNAMIC_CLASS (CMStatisticsView, wxView)

BEGIN_EVENT_TABLE (CMStatisticsView, wxView)
END_EVENT_TABLE ()



CMStatisticsView::CMStatisticsView ()

{
   m_classList = 0;
   m_initializedFlag = FALSE;
	m_initializedFlag = TRUE;

}	// end "CMStatisticsView"



CMStatisticsView::~CMStatisticsView ()

{
   gProjectWindow = NULL;

}	// end "~CMStatisticsView"



CMStatisticsDoc* CMStatisticsView::GetDocument ()

{
   return (CMStatisticsDoc*)m_viewDocument;
	
}	// end "GetDocument"



// 		This function is called when we call wxView::Activate ()
// 		Inherited from wxView
void CMStatisticsView::OnActivateView (
				bool									bActivate,
				wxView*								pActivateView,
				wxView*								pDeactiveView)

{
	if (m_frame != NULL)
		{
		if (bActivate)
			wxASSERT(pActivateView == (wxView*)this);
		
				// If the window is being activated, outside of a processing operating,
				// make sure the global active image information is up to date.
		
		if (gProcessorCode == 0 && bActivate)
			{
			gActiveWindowType = kProjectWindowType;
			gTheActiveWindow = (WindowPtr)gProjectWindow;
			
         if (gProjectInfoPtr != NULL)
            m_frame->ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
			
			}	// end "if (gProcessorCode == 0 && bActivate)"
		
		}	// end "if (m_frame != NULL)"

}	// end "OnActivateView"



bool CMStatisticsView::OnClose (
				bool 									deleteWindow)

{
	if (!((CMultiSpecApp*)wxTheApp)->IsActive ())
		{
				// Treat this as just a mouse down to activate the application and window.
		
																						return false;
		
		}	// end "if (!((CMultiSpecApp*)wxTheApp)->IsActive ())"
	
	if (!GetDocument()->Close ())
																						return false;
	
	if ((CMStatisticsView*)gTheActiveWindow == this)
		gTheActiveWindow = NULL;
	
	if (gActiveWindowType == kProjectWindowType)
		gActiveWindowType = -1;
	
	gProjectWindow = NULL;
	Activate (false);
	SetFrame (NULL);
	return true;
	
}	// end "OnClose"



bool CMStatisticsView::OnCreate (
				wxDocument*							doc,
				long 									WXUNUSED (flags))

{
	int									clientWidth,
											clientHeight,
											width = 225,
											height = 505,
											textWindowXPosition,
											textWindowYPosition;
			
	
	GetMainFrame()->GetClientSize (&clientWidth, &clientHeight);
	width = MIN (width, clientWidth);
	height = MIN (height, clientHeight);

	textWindowXPosition = clientWidth - width - 12;
	
	#ifdef NetBeansProject
		textWindowYPosition = 80;
	#else	// mygeohub
		textWindowYPosition = 80;
	#endif
   
   m_frame = new CMStatisticsFrame (
			doc,
			this,
			GetMainFrame (),
			wxID_ANY,
			wxT("Project"),
			wxPoint (textWindowXPosition, textWindowYPosition),
			wxSize (width, height),
			#if defined multispec_wxmac
				(wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
			#else
				(wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP) & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
			#endif
	
   SetFrame (m_frame);
	
			// Make non resizable
   m_frame->SetSizeHints (width, height, width, height, width, height);
   m_frame->SetClientSize (width, height);
   m_frame->SetTitle (wxT("Project"));
   m_frame->SetSize (width, height);
   m_frame->SetSizeConstraint (-1, -1, width, height);
   
   ((CMStatisticsDoc*)doc)->SetStatisticsFrame ((CMStatisticsFrame*)m_frame);
	gTheActiveWindow = (WindowPtr)this;   
   m_frame->Show (true);
	Activate (true);
   
   OnInitialUpdate ();
   
   return true;
	
}	// end "OnCreate"



void CMStatisticsView::OnDraw (
				CDC* 									pDC)

{
   CString 								tempCString;

   SInt16 								classStorage;


   if (gProjectInfoPtr == NULL)
      																							return;

   switch (gProjectInfoPtr->statsWindowMode)
		{
      case kClassListMode:
			m_frame->UpdateStatsTypeCombo (gProjectInfoPtr->statsWindowMode);
         break;

      case kFieldListMode: // "Field List" mode
         m_frame->UpdateStatsTypeCombo (gProjectInfoPtr->statsWindowMode);

      case kCoordinateListMode: // "Polygon Point List" mode
         classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];

         tempCString =
            CString ((char*) &gProjectInfoPtr->classNamesPtr[classStorage].name[1]);
			
         wxWindow* projWin = this->GetFrame ();
         wxStaticText* classname = (wxStaticText*)projWin->FindWindow (IDC_ClassName);
         classname->SetLabel (tempCString);
			
         if (gProjectInfoPtr->statsWindowMode == 4)
				{
            GetFieldNameWithType (gProjectInfoPtr->currentField, gTextString);
            tempCString = CString ((char*) &gTextString[1]);
				wxStaticText* fieldname =
											(wxStaticText*)projWin->FindWindow (IDC_FieldName);
				fieldname->SetLabel (tempCString);

				}	// end "if (gProjectInfoPtr->statsWindowMode == 4)"
         break;

   	}	// end "switch (gProjectInfoPtr->statsWindowMode)"
	
}	// end "OnDraw"



void CMStatisticsView::OnInitialUpdate (void)

{
   CMStatisticsDoc* documentPtr = GetDocument ();
   documentPtr->SetStatisticsFrame ((CMStatisticsFrame*)gProjectWindow);
   
	gProjectWindow = (CMImageView*)this;
	gStatisticsListHandle = (wxListBox*)m_frame->FindWindow (IDC_ListBox);

			// Add current list of classes to the combo box.

   CreateClassNameComboBoxList ((wxChoice*)m_frame->FindWindow (IDC_ClassList));

   MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 1);

			// This control is no longer used.

   wxChoice* classList = (wxChoice*)m_frame->FindWindow (IDC_ClassList);
   classList->SetSelection (m_classList);
	
	#if defined multispec_wxlin
		wxComboBox*	statsCtrl;
   	statsCtrl = (wxComboBox*)m_frame->FindWindow (IDC_StatsCombo);
	#endif
	#if defined multispec_wxmac
   	wxChoice* statsCtrl;
   	statsCtrl = (wxChoice*)m_frame->FindWindow (IDC_StatsCombo);
	#endif
	
   statsCtrl->SetClientData (0, (void*)kOriginalStats);
   statsCtrl->SetClientData (1, (void*)kLeaveOneOutStats);
   statsCtrl->SetClientData (2, (void*)kEnhancedStats);
   statsCtrl->SetClientData (3, (void*)kMixedStats);

   WriteProjectName ();
	
			// If a selected area exists in the project window indicate that it
			// is available for adding to the list of statistics areas.
	
	Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
	if (ImageWindowIsAvailable (windowInfoHandle))
		{
				// Get handle to the selection information for the window.

		Handle selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);
		SelectionInfoPtr selectionInfoPtr =
								(SelectionInfoPtr)GetHandlePointer (selectionInfoHandle);

		if (selectionInfoPtr != NULL)
			{
			if (selectionInfoPtr->typeFlag == kRectangleType ||
														selectionInfoPtr->typeFlag == kPolygonType)
				{
				CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
				if (pTool != NULL)
					pTool->c_flag_addtolist = TRUE;
				
				}	// end "if (selectionInfoPtr->typeFlag == kRectangleType ..."
			
			}	// end "if (selectionInfoPtr != NULL)"
	
		}	// end "if (ImageWindowIsAvailable (windowInfoHandle))"
	
}	// end "OnInitialUpdate"
