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
//	File:						xTextDoc.cpp : class implementation file
//	Class Definition:		xTextDoc.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			09/01/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMTextDoc class.
//
//------------------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
	#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif

#include "SMultiSpec.h"

#include "xMultiSpec.h"
#include "xTextDoc.h"
#include "xTextView.h"


IMPLEMENT_DYNAMIC_CLASS(CMTextDoc, wxDocument)

bool CMTextDoc::Close (void)

{
			// The application is closing. First determine whether any project needs to
			// be saved and give the user a chance to cancel the shutdown.
		
	if (CloseTheProject ())
		{
		CMTextView *view = (CMTextView*)GetFirstView ();
		if (IsModified () && view->m_textsw->GetNumberOfLines () > 2)
			{
			/*
			if (LoadSpecifiedStringNumberStringP (kAlertStrID,
																IDS_Alert54,
																(char*)gTextString,
																(char*)gTextString2,
																TRUE,
																(char*)&gTextString3[1]))
				itemHit = DisplayAlert (kSaveAlertID, kStopAlert, 0, 0, 0, gTextSTring);
				
			if (itemHit == 1)
				return Save();
				
			if (itemHit == 2)
				closeFlag = false;
			*/
			if (((CMultiSpecApp*)wxTheApp)->m_systemTerminationFlag)
				switch (wxMessageBox (
							wxString::Format(_("Do you want to save changes to %s?"),
							GetUserReadableName ()),
							wxTheApp->GetAppDisplayName (),
							wxYES_NO | wxICON_QUESTION | wxCENTRE | wxSTAY_ON_TOP))
					{
					case wxNO:
						Modify (false);
						break;

					case wxYES:
						m_applicationExitFlag = Save ();
						return true;
							 
					}	// end "switch (wxMessageBox (... "
				
			else
				switch (wxMessageBox (
								wxString::Format(_("Do you want to save changes to %s?"),
								GetUserReadableName ()),
								wxTheApp->GetAppDisplayName (),
								wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE | wxSTAY_ON_TOP))
					{
					case wxNO:
						Modify (false);
						break;

					case wxYES:
						m_applicationExitFlag = Save ();
						return m_applicationExitFlag;

					case wxCANCEL:
						return false;
							 
					}	// end "switch (wxMessageBox (... "
				
			}	// end "if (IsModified() && gOutputViewCPtr->..."
			
		}	// end "if (CloseTheProject ())"
		
	else	// !CloseTheProject ()
		return false;
	
	m_applicationExitFlag = TRUE;
	
   return true;
	
}	// end "Close"



bool CMTextDoc::IsModified (void) const

{
	CMTextView *view = (CMTextView*)GetFirstView();
	if (view != NULL)
     	return (wxDocument::IsModified() || view->m_textsw->IsModified());
	
  else	// view == NULL
		return wxDocument::IsModified();
	
}	// end "IsModified"



void CMTextDoc::Modify (
				bool 									mod)

{
	CMTextView* view = (CMTextView*)GetFirstView ();

	wxDocument::Modify (mod);

	if (!mod && view && view->m_textsw)
		view->m_textsw->DiscardEdits();
	
}	// end "Modify"



bool CMTextDoc::OnOpenDocument (
				const wxString& 					filename)

{
    CMTextView* view = (CMTextView*)GetFirstView ();
    if (!view->m_textsw->LoadFile (filename))
        return false;

    SetFilename (filename, true);
    Modify (false);
    UpdateAllViews ();
    return true;
	
}	// end "OnOpenDocument"


		// Since text windows have their own method for saving to/loading from files,
		// we override OnSave/OpenDocument instead of Save/LoadObject

bool CMTextDoc::OnSaveDocument (
				const wxString& 					filename)

{
	CMTextView *view = (CMTextView*)GetFirstView ();
	
	if (!view->m_textsw->SaveFile (filename))
		return false;
	
	Modify (false);
	return true;
	
}	// end "OnSaveDocument"


/*
bool CMTextDoc::SaveAs ()

{
	wxBell();
	wxDocTemplate *docTemplate = GetDocumentTemplate ();
	if (!docTemplate)
		return false;
	wxString filter = docTemplate->GetFileFilter ();

	wxString defaultDir = docTemplate->GetDirectory ();
	if (defaultDir.empty ())
		{
		defaultDir = wxPathOnly(GetFilename ());
		if (defaultDir.empty ())
			defaultDir = GetDocumentManager()->GetLastDirectory ();
 
		}	// end "if (defaultDir.empty ())"

	wxString fileName = wxFileSelector (
											_("Save As"),
											defaultDir,
											wxFileNameFromPath(GetFilename()),
											docTemplate->GetDefaultExtension(),
											filter,
											wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxSTAY_ON_TOP,
											GetDocumentWindow());

	if (fileName.empty())
			// Cancelled by user
																						return false;

			// Files that were not saved correctly are not added to the FileHistory.
 
	if (!OnSaveDocument(fileName))
		return false;

	SetTitle (wxFileNameFromPath (fileName));
	SetFilename (fileName, true);    // will call OnChangeFileName automatically

			// A file that doesn't use the default extension of its document template
			// cannot be opened via the FileHistory, so we do not add it.
 
	if (docTemplate->FileMatchesTemplate(fileName))
		{
		GetDocumentManager()->AddFileToHistory(fileName);
		}
 
			//else: the user will probably not be able to open the file again, so we
			//      could warn about the wrong file-extension here

	return true;
 
}	// end "SaveAs"
*/
