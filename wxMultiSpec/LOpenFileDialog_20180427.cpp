// LOpenFileDialog
//
// Modified by Larry Biehl on 08/02/2017
//
#include "LMultiSpec.h"
#include "LOpenFileDialog.h"
//#include "SExternalGlobals.h"

#include "wx/dialog.h"
#include "wx/process.h"

//extern void gdk_window_raise(GdkWindow* window);

IMPLEMENT_CLASS(MyExtraPanel, wxPanel)

MyExtraPanel::MyExtraPanel(wxWindow *parent) : wxPanel(parent)
{

	m_imageType = 0;
   m_linkOption = 0;
   m_linkToActiveImageFlag = false;
   m_showLinkPopupMenuFlag = TRUE;
   m_initialLinkSelectedFilesFlag = false;
   //m_numberImageFiles = 0;
   m_selectedFileCount = 0;
   
   CreateControls();
   OnInitDialog();
   
}	// end "MyExtraPanel"


MyExtraPanel::~MyExtraPanel()
{

}	// end "~MyExtraPanel"


void MyExtraPanel::OnInitDialog(void)
{
	if (gGetFileStatus == -1)
		{
		m_staticText2->Hide();
		m_staticText3->Hide();
		m_staticText4->Hide();
		//m_staticText5->Hide();
		m_list->Hide();
		m_link->Hide();
		}

	if (gGetFileStatus == 0)
		{
      Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
      WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																		windowInfoHandle);
			
      if (windowInfoPtr == NULL ||
            windowInfoPtr->imageType != kMultispectralImageType ||
            windowInfoPtr->projectBaseImageFlag ||
            windowInfoPtr->bandInterleave == kBIS)
			{
			m_showLinkPopupMenuFlag = FALSE;
			}

				// Set link comboBox to "Do not link"
      if (!m_showLinkPopupMenuFlag)
			{
			m_link->SetSelection(0);
			}
        
		m_staticText3->Hide();
		//m_staticText5->Hide();
       
		} // end "if (gGetFileStatus == 0)"
    
  
	if(m_showLinkPopupMenuFlag)
		{
		m_link->Append(wxT("Do not link"));
		m_link->Append(wxT("Link selected file(s) to active image window"));
		m_link->Append(wxT("Link selected file(s) to new image window"));
		//m_staticText5->Hide();
		}
	else
		{
		m_link->Append(wxT("Do not link"));
		m_link->Append(wxT("Link selected file(s) to new image window"));
		}
    
	if (gMultipleImageFileCode == 2)
		{
		m_linkOption = 1;
		m_linkOptionSelectionDataCode = 2;

		SetImageLinkToTrue();
		                                        
		if (gGetFileStatus == 2)
			m_list->Hide();
			
		}		// end "if (gMultipleImageFileCode == 2)"

	if (gGetFileStatus == 2)
		{
		m_link->SetSelection (m_linkOptionSelectionDataCode-1);
		m_link->Disable();
		}
    
	if (gGetFileStatus == 1)
		gGetFileStatus = 3;

     
	if (gGetFileStatus <= 0)
		gGetFileStatus = 1;
    
			// The event call of comboBox

	m_list->Connect(14, wxEVT_COMMAND_COMBOBOX_SELECTED,
                wxCommandEventHandler(MyExtraPanel::OnSelendokImageType), NULL, this);
    
	m_link->Connect(16, wxEVT_COMMAND_COMBOBOX_SELECTED,
                    wxCommandEventHandler(MyExtraPanel::OnSelendokLinkOption), NULL, this);
    
	uploadbutton->Connect(wxID_ANY, wxEVT_BUTTON,
                wxCommandEventHandler(MyExtraPanel::OnUploadFile), NULL, this);
   
}	// end "OnInitDialog"


bool MyExtraPanel::CreateControls(void)
{
			// The upload button
	uploadbutton = new wxButton(this, ID_UploadButton, wxT("Upload File"),
							wxDefaultPosition, wxDefaultSize, 0);

			// The imageType comboBox
	m_list = new wxComboBox( this, 14, wxT("default"), wxDefaultPosition, 
									  wxDefaultSize, 0, NULL, 0 );
			// To add the image type
	m_list->Append(wxT("default"));
	m_list->Append(wxT("Multispectral type"));
	m_list->Append(wxT("Thematic type"));
	  
			// The image link comboBox
	m_link = new wxComboBox( this, 16, wxT("Do not link"), wxDefaultPosition, 
									wxDefaultSize, 0, NULL, 0 );


			// The static text
	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Open Image as:"));
	m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("Multispectral type"));
	m_staticText4 = new wxStaticText(this, wxID_ANY, wxT("Select Image(s)"));

	//m_staticText5 = new wxStaticText(this, 15, wxT("  "));
			// The layout of extra panel
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);

	hbox1->Add(m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxLEFT,8);
	hbox1->Add(m_staticText3, 0, wxALIGN_CENTER_VERTICAL | wxLEFT,8);
	hbox1->Add(m_list, 0, wxALIGN_CENTER_VERTICAL | wxLEFT,8);

	//hbox1->Add (0, 0, 1, wxEXPAND);
	hbox1->Add (200, 1, wxSizerFlags(0)); 
	//hbox1->Add (uploadbutton, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL);
	hbox1->Add (uploadbutton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Right()); 

	//vbox->Add (hbox1, 0, wxLEFT,5);
	vbox->Add (hbox1, wxSizerFlags(0).Expand().Border(wxLEFT,5)); 
	vbox->Add(-1, 10);

	wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
	hbox2->Add(m_link, 0, wxLEFT,8);
	vbox->Add(hbox2, 0, wxLEFT,5);
	vbox->Add(-1, 2);

	wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);
	hbox3->Add (m_staticText4, 0, wxLEFT|wxTOP,8); 
	//vbox->Add (hbox3, 0, wxLEFT,5);
	vbox->Add (hbox3, wxSizerFlags(0).Border(wxLEFT|wxBOTTOM, 5)); 

	//wxBoxSizer *hbox4 = new wxBoxSizer(wxHORIZONTAL);
	//hbox4->Add(m_staticText5, 0, wxLEFT,8);
	//vbox->Add(hbox4, 0, wxLEFT,5);

			// To show the layout of extra panel
	this->SetSizerAndFit(vbox);
	vbox->SetSizeHints(this);

	return TRUE;
}


void MyExtraPanel::OnUploadFile(wxCommandEvent& event)
{
    
	wxExecuteEnv workingenv;
	wxString datadir = wxGetHomeDir();
	datadir.Append("/data");
	workingenv.cwd = datadir;

	FileUploadProcess * const process = new FileUploadProcess((CMainFrame*)this, wxT("importfile"));
	long returnCode = wxExecute(wxT("importfile"), wxEXEC_ASYNC, process, &workingenv); // wxEXEC_SYNC

	if (returnCode)		// note use = 0 if wxEXEC_SYNC is used.
		wxMessageBox (wxString::Format(_("Select your account name to the left and then the 'data' directory in box above to find your newly uploaded file after the upload process has completed.")),
						wxTheApp->GetAppDisplayName(),
						wxOK | wxCENTRE | wxSTAY_ON_TOP);
	
	else		// returnCode == 0
		delete process;
	
}		// end "OnUploadFile"


void MyExtraPanel::OnSelendokLinkOption(wxCommandEvent& event)
{
	m_linkOptionSelectionDataCode = m_link->GetSelection()+1;
    
	if (m_linkOptionSelectionDataCode == 1)
		SetImageLinkToFalse();
	else if (m_linkOptionSelectionDataCode == 2)
		SetImageLinkToTrue();
	else if (m_linkOptionSelectionDataCode == 3)
		SetImageLinkToTrue();
    
}


void MyExtraPanel::OnSelendokImageType(wxCommandEvent& event)
{
	m_imageType = m_list->GetSelection();
	m_userSetImageType = m_imageType;
}


void MyExtraPanel::SetImageLinkToTrue(void)
{
	//wxStaticText *linkText = (wxStaticText*) FindWindow (15);
    
	if (m_showLinkPopupMenuFlag == 0 && m_linkOptionSelectionDataCode == 2)
		{
		m_linkOptionSelectionDataCode = 3;
		}
    
	if (m_linkOptionSelectionDataCode == 2)
		{
		gMultipleImageFileCode = 2;
        
		Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																		windowInfoHandle);
		m_numberImageFiles = windowInfoPtr->numberImageFiles+1;
        
		//linkText->SetLabel(wxString::Format(_T("Select File %3d to link"), m_numberImageFiles));
		m_staticText4->SetLabel(wxString::Format(_T("Select File %3d to link"), m_numberImageFiles));
		if (m_numberImageFiles > 2 && gGetFileStatus == 2)
			{
			m_link->SetSelection((m_linkOptionSelectionDataCode-1));
			m_link->Disable();
         }
        
		//linkText->Show();
		//m_staticText4->Hide();

		}
		
	else
		{	// m_linkOptionSelectionDataCode == 3
		gMultipleImageFileCode = 3;
		//linkText->Hide();
		//m_staticText4->Show();
		m_staticText4->SetLabel(wxString::Format(_T("Select Image(s)")));
		
		}
		
			// Only allow multispectral type
			
	m_staticText2->Show();
	m_staticText3->Show();
	m_list->Hide();
  
	m_initialLinkSelectedFilesFlag = TRUE;
	m_linkToActiveImageFlag = TRUE;

	m_imageType = 1;

}	// end "SetImageLinkToTrue"


void MyExtraPanel::SetImageLinkToFalse(void)
{
   
   gMultipleImageFileCode = 0;

   if (gGetFileImageType == 0)
      m_imageType = 0;

   else if (gGetFileImageType == kMultispectralImageType)
      m_imageType = 1;

   else // gGetFileImageType == kThematicImageType)
      m_imageType = 2;

   m_imageType = m_userSetImageType;
   
   gGetFileStatus = 0;
   
   m_staticText2->Show();
   m_staticText3->Hide();
   //m_staticText5->Hide();
   //m_staticText4->Show();
	m_staticText4->SetLabel(wxString::Format(_T("Select Image(s)")));
  
	m_list->Show();
   m_list->Enable(true);
   m_list->SetSelection(m_imageType);

} // end "SetImageLinkToFalse"


//To create addition panel in the file dialog window
static wxWindow* createMyExtraPanel(wxWindow *parent)
{
	return new MyExtraPanel(parent);
}


CMOpenFileDialog::CMOpenFileDialog (
				wxWindow* pParent)
{
   m_parent = pParent;

}	// end "CMOpenFileDialog"


bool CMOpenFileDialog::DoDialog(int stringIndex, long style)
{
	wxString  promptString;
   bool continueFlag = FALSE;
   SInt16 returnCode;
	
   //gGetFileS = 0;
   //SInt16 returnCode;	
	
	//style |= wxSTAY_ON_TOP;	// Does not help.
	
			// wxDIALOG_NO_PARENT 0x0020 in wx/dialog.h conflicts with wxFD_MULTIPLE.
			// Problem routine is GetParentForModalDialog in dlgcmn.cpp
	
	if (stringIndex == IDS_SelectProject)
		promptString =  _T("Select Project");
   else if (stringIndex == IDS_FileIO95)
      promptString =  _T("Select Training Mask Image");
   else if (stringIndex == IDS_FileIO96)
      promptString =  _T("Select Test Mask Image");
   else if (stringIndex == IDS_SelectENVI)
      promptString =  _T("Select ENVI ROI File");
	else
		promptString =  _T("Select Image");
   
   wxFileDialog dialog
           (
           m_parent,
           promptString,
           wxEmptyString,
           wxEmptyString,
           wxEmptyString,
           style,
           wxDefaultPosition,
           wxSize(600,450));
   //        wxFD_MULTIPLE | wxFD_OPEN);
   
   m_wxFileDialog = &dialog;
    
   dialog.Reparent (m_parent);
	
   dialog.SetMaxSize(wxSize(1000,500));
   dialog.SetMinSize(wxSize(600,300));
   wxString wildcard(&gTextString[1], wxConvUTF8);
   dialog.SetWildcard(wildcard);
   
			//To create additional panel in the file dialog window
			
	dialog.SetExtraControlCreator(&createMyExtraPanel);
   //dialog.CenterOnParent();
	//dialog.Raise();
	//gdk_window_raise ((GdkWindow*)dialog);
   /*
   wxString datadir;
   #ifdef NetBeansProject
        datadir = wxGetHomeDir();
   #else
        //dialog.SetDirectory("/apps/multispec/current/data");
        datadir = wxString::FromUTF8("/data/tools/multispec");
        //datadir = wxString::FromUTF8((char*)getenv ("DATA_PATH"));
   #endif
	*/
   dialog.SetDirectory (gDefaultDataDirectory);
	
   returnCode = dialog.ShowModal();
      
   if (returnCode == wxID_OK) 
		{
      wxWindow * const extra = dialog.GetExtraControl();

      if (wxDynamicCast(extra, MyExtraPanel) != NULL) 
			{
         if (static_cast<MyExtraPanel*> (extra)->getimagetype() == 0)
            gGetFileImageType = 0;

         else if (static_cast<MyExtraPanel*> (extra)->getimagetype() == 1)
            gGetFileImageType = kMultispectralImageType;

         else if (static_cast<MyExtraPanel*> (extra)->getimagetype() == 2)
            gGetFileImageType = kThematicImageType;

         else // (m_imageType > 2) 
            gGetFileImageType = 0;
         /*
			#if 0  
         if(static_cast<MyExtraPanel*> (extra)->getlinktoactiveimagevalue() == 0)
            gMultipleImageFileCode = 0; 
         else          
         if(GetFileStatus > 1)
             gGetFileStatus = 2;if (static_cast<MyExtraPanel*> (extra)->getlinktoactiveimagevalue() == 1)
            gMultipleImageFileCode = 2;
         else if (static_cast<MyExtraPanel*> (extra)->getlinktoactiveimagevalue() == 2)
            gMultipleImageFileCode = 3;
			#endif
			*/   
			if (style &= wxFD_MULTIPLE)
            dialog.GetPaths(m_paths);
         else 
            m_path = dialog.GetPath();
 
         continueFlag = TRUE;
			}
		
		}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"

/*
// This is a temporary function for use in places where 
// DoModal is being called in windows code
Boolean CMOpenFileDialog::DoModal()
{
   DoDialog(0,0);
   
   return true;
}
*/
/*
void CMOpenFileDialog::OnInitDialog(wxInitDialogEvent& event)
{
    m_wxFileDialog->OnInitDialog(event);
    m_wxFileDialog->CenterOnParent();
    
}
*/

wxArrayString CMOpenFileDialog::GetPaths()
{
   return m_paths;
}


// Leaving this dialog for now
// Need to implement the following equivalent code:
// OnInitDialog()wxArrayString


