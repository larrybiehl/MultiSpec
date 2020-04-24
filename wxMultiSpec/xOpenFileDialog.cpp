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
//	File:						xOpenFileDialog.cpp : class implementation file
//	Class Definition:		xOpenFileDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/04/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMOpenFileDialog and MyExtraPanel classes.
//
//------------------------------------------------------------------------------------

#include "xMultiSpec.h"
#include "xOpenFileDialog.h"

#include "wx/dialog.h"
#include "wx/process.h"



IMPLEMENT_CLASS (MyExtraPanel, wxPanel)

MyExtraPanel::MyExtraPanel (wxWindow *parent) : wxPanel(parent)

{
   m_link = NULL;
   m_list = NULL;
	m_imageType = 0;
   m_linkOption = 0;
   m_userSetImageType = 0;
   m_linkToActiveImageFlag = false;
   m_showLinkPopupMenuFlag = TRUE;
   m_initialLinkSelectedFilesFlag = false;
   m_selectedFileCount = 0;
   
   CreateControls();
   OnInitDialog();

}	// end "MyExtraPanel"



MyExtraPanel::~MyExtraPanel ()

{

}	// end "~MyExtraPanel"



void MyExtraPanel::OnInitDialog (void)

{
	if (gGetFileStatus == -1)
		{
		m_staticText2->Hide ();
		m_staticText3->Hide ();
		m_list->Hide ();
		m_link->Hide ();
		
		}	// end "if (gGetFileStatus == -1)"

	if (gGetFileStatus == 0)
		{
      Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();
      WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																					windowInfoHandle);
			
      if (windowInfoPtr == NULL ||
            windowInfoPtr->imageType != kMultispectralImageType ||
            windowInfoPtr->projectBaseImageFlag ||
            windowInfoPtr->bandInterleave == kBIS)
			{
			m_showLinkPopupMenuFlag = FALSE;
			
			}	// end " if (windowInfoPtr == NULL || ..."

				// Set link comboBox to "Do not link"
		
      if (!m_showLinkPopupMenuFlag)
			m_link->SetSelection (0);
        
		m_staticText3->Hide ();
      
		}	// end "if (gGetFileStatus == 0)"
  
	if (m_showLinkPopupMenuFlag)
		{
		//m_link->Append (wxT("Do not link")); // Already loaded
		m_link->Append (wxT("Link selected file(s) to new image window"));
		m_link->Append (wxT("Link selected file(s) to active image window"));
		
		}	// end "	if (m_showLinkPopupMenuFlag)"
	
	else	// !m_showLinkPopupMenuFlag
		//m_link->Append (wxT("Do not link")); // Already loaded
		m_link->Append (wxT("Link selected file(s) to new image window"));
    
	if (gMultipleImageFileCode == 2)
		{
		m_linkOption = 2;
		m_linkOptionSelectionDataCode = 3;

		SetImageLinkToTrue();
		                                        
		if (gGetFileStatus == 2)
			m_list->Hide ();
			
		}	// end "if (gMultipleImageFileCode == 2)"

	if (gGetFileStatus == 2)
		{
		m_link->SetSelection (m_linkOptionSelectionDataCode-1);
		m_link->Disable ();
		
		}	// end "if (gGetFileStatus == 2)"
    
	if (gGetFileStatus == 1)
		gGetFileStatus = 3;

	if (gGetFileStatus <= 0)
		gGetFileStatus = 1;
    
			// The event call of comboBox
			// Note that the Connects (and Binds) do not work. The setting is obtain
			// after the user selects OK.
	
	#if defined multispec_wxlin
		uploadbutton->Connect (wxID_ANY,
										wxEVT_BUTTON,
                					wxCommandEventHandler (MyExtraPanel::OnUploadFile),
                					NULL,
                					this);
	#endif
	
}	// end "OnInitDialog"



bool MyExtraPanel::CreateControls (void)

{
			// The imageType comboBox
	
	m_list = new wxChoice (this,
									14,
									wxDefaultPosition,
									wxDefaultSize);
	
			// To add the image type
	
	m_list->Append (wxT("default"));
	m_list->Append (wxT("Multispectral type"));
	m_list->Append (wxT("Thematic type"));
   m_list->SetSelection (0);

	m_list->Bind (wxEVT_COMMAND_CHOICE_SELECTED,
                	&MyExtraPanel::OnSelendokImageType,
                	this);
	  
			// The image link comboBox
	
	m_link = new wxChoice (this,
									16,
									wxDefaultPosition,
									wxSize (300, -1));
	m_link->Append (wxT("Do not link"));
   m_link->SetSelection (0);
	
	m_link->Bind (wxEVT_COMMAND_CHOICE_SELECTED,
						&MyExtraPanel::OnSelendokLinkOption,
						this);

			// The static text
	
	m_staticText2 = new wxStaticText (this,
													wxID_ANY,
													wxT("Open Image as:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	
	m_staticText3 = new wxStaticText (this,
													wxID_ANY,
													wxT("Multispectral type"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	
			// The layout of extra panel
	
	wxBoxSizer *vbox = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *hbox1 = new wxBoxSizer (wxHORIZONTAL);

	hbox1->Add (m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);
	hbox1->Add (m_staticText3, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);
	hbox1->Add (m_list, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);
	
	#if defined multispec_wxlin
				// The upload button
	
		uploadbutton = new wxButton (this,
												ID_UploadButton,
												wxT("Upload File"),
												wxDefaultPosition,
												wxDefaultSize,
												0);

		hbox1->Add (200, 1, wxSizerFlags(0));
		hbox1->Add (uploadbutton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
	#endif
	
	vbox->Add (hbox1, wxSizerFlags(0).Expand().Border(wxLEFT, 5)); 
	vbox->Add (-1, 10);

	wxBoxSizer *hbox2 = new wxBoxSizer (wxHORIZONTAL);
	hbox2->Add (m_link, 0, wxLEFT, 8);
	vbox->Add (hbox2, 0, wxLEFT, 5);
	vbox->Add (-1, 2);

			// To show the layout of extra panel
	
	SetSizerAndFit (vbox);
	vbox->SetSizeHints (this);

	return TRUE;
	
}	// end "CreateControls"



void MyExtraPanel::OnUploadFile (
				wxCommandEvent& 					event)

{
	wxExecuteEnv 		workingenv;
	wxString 			datadir = wxGetHomeDir();
	
	
	datadir.Append("/data");
	workingenv.cwd = datadir;

	FileUploadProcess * const process = new FileUploadProcess ((CMainFrame*)this,
																					wxT("importfile"));
	long returnCode = wxExecute (wxT("importfile"),
											wxEXEC_ASYNC,
											process,
											&workingenv); // wxEXEC_SYNC

	if (returnCode)		// note use = 0 if wxEXEC_SYNC is used.
		wxMessageBox (wxString::Format(_("Select your account name to the left and then the 'data' directory in box above to find your newly uploaded file after the upload process has completed.")),
							wxTheApp->GetAppDisplayName(),
							wxOK | wxCENTRE | wxSTAY_ON_TOP);
	
	else	// returnCode == 0
		delete process;
	
}	// end "OnUploadFile"



void MyExtraPanel::OnSelendokLinkOption (
				wxCommandEvent& 					event)

{
	m_linkOptionSelectionDataCode = m_link->GetSelection () + 1;
    
	if (m_linkOptionSelectionDataCode == 1)
		SetImageLinkToFalse ();
	
	else if (m_linkOptionSelectionDataCode == 2)
		SetImageLinkToTrue ();
	
	else if (m_linkOptionSelectionDataCode == 3)
		SetImageLinkToTrue ();
    
}	// end "OnSelendokLinkOption"


void MyExtraPanel::OnSelendokImageType (
				wxCommandEvent& 					event)

{
	m_imageType = m_list->GetSelection ();
	m_userSetImageType = m_imageType;
	
}	// end "OnSelendokImageType"


void MyExtraPanel::SetImageLinkToTrue (void)

{
	if (m_linkOptionSelectionDataCode == 2)
		{
				// Link to new image window
		
		gMultipleImageFileCode = 3;
		
		}	// end "if (m_linkOptionSelectionDataCode == 2)"
    
	else if (m_linkOptionSelectionDataCode == 3)
		{
				// Link to active image window
		
		gMultipleImageFileCode = 2;
        
		Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																				windowInfoHandle);
		m_numberImageFiles = windowInfoPtr->numberImageFiles+1;
        
		if (m_numberImageFiles > 2 && gGetFileStatus == 2)
			{
			m_link->SetSelection (m_linkOptionSelectionDataCode-1);
			m_link->Disable ();
			
         }	// end "if (m_numberImageFiles > 2 && gGetFileStatus == 2)"

		}	// end "else if (m_linkOptionSelectionDataCode == 3)"
		
			// Only allow multispectral type
			
	m_staticText2->Show ();
	m_staticText3->Show ();
	m_list->Hide ();
  
	m_initialLinkSelectedFilesFlag = TRUE;
	m_linkToActiveImageFlag = TRUE;

	m_imageType = 1;
	
	Layout();
	Fit ();

}	// end "SetImageLinkToTrue"



void MyExtraPanel::SetImageLinkToFalse (void)

{
   gMultipleImageFileCode = 0;
	
	m_imageType = m_list->GetSelection ();
	
	if (m_imageType == 0)
		{
				// This implies that the user has not specifically set the
				// image type parameter. Make sure it is set to the default
				// in case it was changed during an earlier multiple file
				// setting.
		
		if (gGetFileImageType == 0)
			m_imageType = 0;

		else if (gGetFileImageType == kMultispectralImageType)
			m_imageType = 1;

		else // gGetFileImageType == kThematicImageType)
			m_imageType = 2;

		m_imageType = m_userSetImageType;
		
		gGetFileStatus = 0;
		
		}	// end "if (m_imageType == 0)"
   
   m_staticText2->Show ();
   m_staticText3->Hide ();
  
	m_list->Show ();
   m_list->Enable (true);
   m_list->SetSelection (m_imageType);
	
	Layout ();
	Fit ();

}	// end "SetImageLinkToFalse"


		// To create addition panel in the file dialog window

static wxWindow* createMyExtraPanel (
				wxWindow*							parent)

{
	return new MyExtraPanel (parent);
	
}



CMOpenFileDialog::CMOpenFileDialog (
				wxWindow* 							pParent)

{
   m_parent = pParent;

}	// end "CMOpenFileDialog"



bool CMOpenFileDialog::DoDialog (
				int 									stringIndex,
				long 									style)

{
	wxString  							promptString;
	
	int									filterIndex = 1,
											link_option;
	
   SInt16 								returnCode;
	
   bool 									continueFlag = FALSE;
	

	//style |= wxSTAY_ON_TOP;	// Does not help keep dialog on top in Linux (gtk)
	
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
	else if (stringIndex == IDS_SelectImageStatistics)
      promptString =  _T("Select Image Statistics (.sta) File");
	else if (stringIndex == IDS_SelectNewBaseImage)
      promptString =  _T("Select New Base Image");
	else
		{
		if (gMultipleImageFileCode == 2)
			{
			Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																		windowInfoHandle);
			promptString =  wxString::Format (_T("Select File %d to Link"),
															windowInfoPtr->numberImageFiles + 1);
			
			}	// end "if (gMultipleImageFileCode == 2)"
		
		else	// gMultipleImageFileCode != 2
			promptString =  _T("Select Image(s)");
		
		}	// end "else gMultipleImageFileCode != 2"
	
   wxString wildcard (&gTextString[1], wxConvUTF8);
   
	wxFileDialog dialog (m_parent,
								promptString,
								wxEmptyString,
								wxEmptyString,
								wildcard,
								style,
								wxDefaultPosition,
								wxSize (600, 450));
								//wxFD_MULTIPLE | wxFD_OPEN);
   
   m_wxFileDialog = &dialog;
	
   if (gProcessorCode == kOpenImageFileProcessor)
		filterIndex = gImageFileFilterIndex;
	
   dialog.SetMaxSize (wxSize (1000, 500));
   dialog.SetMinSize (wxSize (600, 300));
   dialog.SetWildcard (wildcard);
   dialog.SetFilterIndex (filterIndex);
   
			// Create additional panel in the file dialog window
	
   if (gProcessorCode == kOpenImageFileProcessor)
		dialog.SetExtraControlCreator (&createMyExtraPanel);
	
			// Set font size for open file dialog
			// Found that this does not work for title
	
	//wxFont currentFont = dialog.GetFont ();
	//currentFont.SetPointSize (gFontSize+10);
	//dialog.SetFont (currentFont);
	//dialog.m_caption
	
   //dialog.CenterOnParent();
	//dialog.Raise();
	//gdk_window_raise ((GdkWindow*)dialog);
	
   dialog.SetDirectory (gDefaultDataDirectory);
	/*
			// Note:  This did not work
	#if defined multispec_wxlin
				// Move open file dialog box to center of main frame.
				// Only need to do this for multispec online
	
		wxPoint	newOrig;
	
		int		dialogHeight,
					dialogWidth,
					mainFrameHeight,
					mainFrameWidth;
	
		GetMainFrame()->GetClientSize (&mainFrameWidth, &mainFrameHeight);
		dialog.GetSize (&dialogWidth, &dialogHeight);
	
		newOrig.x = (mainFrameWidth - dialogWidth) / 2;
		//newOrig.x = MAX (0, newOrig.x);
		newOrig.x = 0;
		newOrig.y = (mainFrameHeight - dialogHeight) / 2;
		newOrig.y = MAX (0, newOrig.y);
	
		dialog.SetPosition (newOrig);
	
		int numberChars = sprintf ((char*)gTextString3,
									" xImageFrame:OnInitDialog (): %d, %d, %d, %d, %d, %d%s",
									mainFrameWidth,
									mainFrameHeight,
									dialogWidth,
									dialogHeight,
									newOrig.x,
									newOrig.y,
									gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
	#endif
	*/
   returnCode = dialog.ShowModal();
      
   if (returnCode == wxID_OK) 
		{
      wxWindow * const extra = dialog.GetExtraControl ();
      
      if (wxDynamicCast (extra, MyExtraPanel) != NULL)
			{
         MyExtraPanel* extra_panel = (MyExtraPanel*)extra;
			
            	// Save the filter index that was used.
			
  			if (gProcessorCode == kOpenImageFileProcessor)
   			gImageFileFilterIndex = dialog.GetFilterIndex ();
			
 			if (style &= wxFD_MULTIPLE)
            dialog.GetPaths (m_paths);
				
         else 
            m_path = dialog.GetPath ();
         
         wxArrayString filenames;
         m_wxFileDialog->GetFilenames (filenames);
			
         		// Check if message needs to be displayed to the user to verify
         		// whether multiple files should be linked.
			
         if (filenames.size() > 1)
         	{
         	link_option = extra_panel->m_link->GetSelection ();
          	if (link_option <= 0)
					{
							// More than 1 files selected, ask if want to link them
					
					if (wxYES == wxMessageBox (
											wxT("Multiple files selected. Link them together?"),
											wxT("Link Images"),
											wxYES_NO | wxCENTRE | wxSTAY_ON_TOP))
						{
						extra_panel->m_link->SetSelection (1);
						//extra_panel->m_linkOptionSelectionDataCode = 2;
						//extra_panel->SetImageLinkToTrue ();
						
						}	// end "if (wxYES == wxMessageBox (..."
					
					}	// end "if (link_option <= 0)"
				
				}	// end "if (filenames.size() > 1)"
			
					// Note that event is not used in OnSelectokLinkOption
			
			wxCommandEvent event;
         extra_panel->OnSelendokLinkOption (event);

					// Get the image type.
			
			m_imageType = extra_panel->m_list->GetSelection ();
			
			//int listOption = extra_panel->m_list->GetSelection();
			gGetFileImageType = 0;

         if (m_imageType == 1)
            gGetFileImageType = kMultispectralImageType;

         else if (m_imageType == 2)
            gGetFileImageType = kThematicImageType;
			
			continueFlag = TRUE;
			
			}	// end " if (wxDynamicCast(extra, MyExtraPanel) != NULL)"
		
		else	// wxDynamicCast(extra, MyExtraPanel) == NULL
         {
			m_path = dialog.GetPath ();
			
			wxArrayString filenames;
			m_wxFileDialog->GetFilenames (filenames);
			continueFlag = TRUE;
			
         }	// end "else wxDynamicCast(extra, MyExtraPanel) == NULL"
		
		}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"



wxArrayString CMOpenFileDialog::GetPaths ()

{
   return m_paths;
	
}	// end "GetPaths"
