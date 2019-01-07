// Revised by Larry Biehl on 01/06/2019

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

#include "LTextDoc.h"
#include "LTextView.h"
#include "SMultiSpec.h"
IMPLEMENT_DYNAMIC_CLASS(CMTextDoc, wxDocument)


bool CMTextDoc::Close ()

{
			// The application is closing. First determine whether any project needs to be
			// saved and give the user a chance to cancel the shutdown.
		
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
			//printf("before saving text window\n");
			
			switch (wxMessageBox (
								wxString::Format(_("Do you want to save changes to %s?"),
								GetUserReadableName ()),
							wxTheApp->GetAppDisplayName (),
							wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE | wxSTAY_ON_TOP ))
				{
				case wxNO:
					Modify (false);
					break;

				case wxYES:
					return Save();

				case wxCANCEL:
					return false;
						 
				}	// end "switch (wxMessageBox (... "
				
			}	// end "if (IsModified() && gOutputViewCPtr->..."
			
		}	// end "if (CloseTheProject ())"
		
	else	// !CloseTheProject ()
		return false;
		
   return true;
	
}	// end "Close"


bool CMTextDoc::IsModified(void) const
{
  CMTextView *view = (CMTextView *)GetFirstView();
  if (view)
  {
    return (wxDocument::IsModified() || view->m_textsw->IsModified());
  }
  else
    return wxDocument::IsModified();
}


void CMTextDoc::Modify(bool mod)
{
  CMTextView *view = (CMTextView *)GetFirstView();

  wxDocument::Modify(mod);

  if (!mod && view && view->m_textsw)
    view->m_textsw->DiscardEdits();
}


bool CMTextDoc::OnOpenDocument(const wxString& filename)
{
    CMTextView *view = (CMTextView *)GetFirstView();
    if (!view->m_textsw->LoadFile(filename))
        return false;

    SetFilename(filename, true);
    Modify(false);
    UpdateAllViews();
    return true;
}


		// Since text windows have their own method for saving to/loading from files,
		// we override OnSave/OpenDocument instead of Save/LoadObject

bool CMTextDoc::OnSaveDocument (const wxString& filename)

{
	CMTextView *view = (CMTextView*)GetFirstView ();
	 
			// writing this to the output text window never worked.
	
	SInt16 numberChars = sprintf ((char*)gTextString3,
												" default output file name: %s%s",
												(const char*)filename.mb_str (wxConvUTF8),
												gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
	
	if (!view->m_textsw->SaveFile (filename))
		return false;
	
	Modify (false);
	return true;
	
}	// end "OnSaveDocument"


/*
bool CMTextDoc::SaveAs ()

{
	wxBell();
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return false;
    wxString filter = docTemplate->GetFileFilter() ;

    wxString defaultDir = docTemplate->GetDirectory();
    if ( defaultDir.empty() )
    {
        defaultDir = wxPathOnly(GetFilename());
        if ( defaultDir.empty() )
            defaultDir = GetDocumentManager()->GetLastDirectory();
    }

    wxString fileName = wxFileSelector(_("Save As"),
            defaultDir,
            wxFileNameFromPath(GetFilename()),
            docTemplate->GetDefaultExtension(),
            filter,
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxSTAY_ON_TOP,
            GetDocumentWindow());

    if (fileName.empty())
        return false; // cancelled by user

    // Files that were not saved correctly are not added to the FileHistory.
    if (!OnSaveDocument(fileName))
        return false;

    SetTitle(wxFileNameFromPath(fileName));
    SetFilename(fileName, true);    // will call OnChangeFileName automatically

    // A file that doesn't use the default extension of its document template
    // cannot be opened via the FileHistory, so we do not add it.
    if (docTemplate->FileMatchesTemplate(fileName))
    {
        GetDocumentManager()->AddFileToHistory(fileName);
    }
    //else: the user will probably not be able to open the file again, so we
    //      could warn about the wrong file-extension here

    return true;
}
*/
