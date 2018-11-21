//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LAppendFileDialog.cpp : class implementation file
//	Class Definition:		LAppendFileDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMAppendFileDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                     
#include "LMultiSpec.h" 
#include "LAppendFileDialog.h"



IMPLEMENT_CLASS (CMAppendFileDialog, CMDialog)

BEGIN_EVENT_TABLE (CMAppendFileDialog, CMDialog)
	EVT_RADIOBUTTON (IDC_LineChannel, CMAppendFileDialog::OnLineChannel)
	EVT_RADIOBUTTON (IDC_LastLineChannel, CMAppendFileDialog::OnLastLineChannel)
	EVT_TEXT (IDC_LineChannelNumber, CMAppendFileDialog::OnTextChange)
END_EVENT_TABLE ()				
							
extern void AppendFileDialogOK (
				FileInfoPtr							appendFileInfoPtr,
				FileInfoPtr							newFileInfoPtr,
				SInt16								lineColumnCode,
				Boolean								lineFlag,
				Boolean								modifyFlag,
				SInt32								newAfterLineChannel,
				SInt64*								bytesToSkipPtr);


CMAppendFileDialog::CMAppendFileDialog (CWnd* pParent /*=NULL*/)
	: CMDialog (CMAppendFileDialog::IDD, pParent)
{
	m_appendFileName = "";
	m_lineChannel = 0;
	m_lineChannelNumber = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
		
	if (m_initializedFlag)                                                              
		{
		TRY
			{																			
					//	Get space for the c string										 
			
			m_appendFileName.GetBufferSetLength (256);
				
			}
			
		CATCH_ALL (e)
			{
			m_initializedFlag = FALSE;
			
			}
		END_CATCH_ALL
		
		}	// end "if (m_initializedFlag)" 
		
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText31 = new wxStaticText (this, 
													IDC_AppendFileName, 
													wxT("Append to:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText31->Wrap (-1);
	bSizer42->Add (m_staticText31, 0, wxALL, 5);
	
	m_staticText32 = new wxStaticText (
						this, wxID_ANY, wxT("After"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText32->Wrap (-1);
	bSizer42->Add (m_staticText32, 0, wxALL, 5);
	
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn3 = new wxRadioButton (this, 
												IDC_LineChannel, 
												wxT("Line:"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												wxRB_GROUP);
	bSizer43->Add (m_radioBtn3, 0, wxALL, 5);
	
	m_textCtrl14 = new wxTextCtrl (
		this, IDC_LineChannelNumber, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer43->Add (m_textCtrl14, 0, wxALL, 5);
	
	bSizer42->Add (bSizer43, 1, wxEXPAND, 5);
	
	m_radioBtn4 = new wxRadioButton (this, 
												IDC_LastLineChannel, 
												wxT("Last Line"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
	bSizer42->Add (m_radioBtn4, 0, wxALL, 5);
	/*
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button15 = new wxButton (
					this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer44->Add (m_button15, 0, wxALL, 5);
	
	m_button16 = new wxButton (
							this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer44->Add (m_button16, 0, wxALL, 5);
	
	bSizer42->Add (bSizer44, 1, wxEXPAND, 5);
	*/
	CreateStandardButtons (bSizer42);
	
	this->SetSizer (bSizer42);
	this->Layout ();
	bSizer42->Fit (this);
	OnInitDialog ();
	
}	// end "CMAppendFileDialog"



//------------------------------------------------------------------------------------
//								 Copyright (2013-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 DoDialog
//
//	Software purpose:	The purpose of this routine is to present the Append File
//							specification dialog box to the user and copy the
//							revised info to the append file specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in AppendFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/30/1997
//	Revised By:			Larry L. Biehl			Date: 11/21/2006	

SInt16 CMAppendFileDialog::DoDialog (
				FileInfoPtr							appendFileInfoPtr, 
				FileInfoPtr							newFileInfoPtr, 
				Boolean								modifyFlag,
				SInt64*								bytesToSkipPtr,
				Boolean								lineFlag,
				SInt32								newAfterLineChannel)

{                                               
	SInt16		returnCode,	returnValue = -1;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																								return (-1);
																			
	m_modifyFlag = modifyFlag;
	m_lineFlag = lineFlag;
	
	m_lineChannelNumber = newAfterLineChannel;
	m_appendFileInfoPtr = appendFileInfoPtr; 
	m_bytesToSkipPtr = bytesToSkipPtr;
	m_maximumValue = newAfterLineChannel;
										
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{ 						                    
		returnValue = 0;
		
		AppendFileDialogOK (appendFileInfoPtr, 
									newFileInfoPtr,
									m_lineChannel, 
									lineFlag,
									modifyFlag, 
									m_lineChannelNumber,
									bytesToSkipPtr);
					
		}	// end "if (returnCode == IDOK)"
		
	return (returnValue);
		
}	// end "DoDialog"



bool CMAppendFileDialog::OnInitDialog ()
{  
	SInt16	numberCharacters;
	
			// Append or modify file name.
	
	char* bufferPtr = m_appendFileName.GetWriteBuf (256);													
				
	if (m_modifyFlag)
		{
		numberCharacters = sprintf (bufferPtr, "Modify ");
		//m_appendFileName = wxT("Modify");
		
		}	
			
	else	// !modifyFlag   
		{                                 
		numberCharacters = sprintf (bufferPtr, "Append to ");		
		//m_appendFileName = wxT("Append to:");
		
		}
			
	StringPtr appendFileNamePtr = GetFileNameCPointer (m_appendFileInfoPtr);
		
	sprintf (&bufferPtr[numberCharacters], "'%s'", appendFileNamePtr);
	m_appendFileName.UngetWriteBuf ();
		
			// After specific line/channel or last line/Channel
			// The last line/channel is the default.								
				                                                   
	m_lineChannel = 1;                                       
		
	if (m_lineFlag)
		{                                           	
		((wxRadioButton*)FindWindow (IDC_LineChannel))->SetLabel (wxT("Line:"));
		((wxRadioButton*)FindWindow (IDC_LastLineChannel))->SetLabel (wxT("Last Line:"));
		
		}	// end "if (m_lineFlag)"
			
	else	// !m_lineFlag
		{                                               	
		((wxRadioButton*)FindWindow (IDC_LineChannel))->SetLabel (wxT("Channel:"));
		((wxRadioButton*)FindWindow (IDC_LastLineChannel))->SetLabel (wxT("Last Channel:"));
		}	// end "else !m_lineFlag"  
		
			// Line or channel number.											
				                                   
	MHideDialogItem (this, IDC_LineChannelNumber);               
	                 
	return true; 
	
}	// end "OnInitDialog"



void CMAppendFileDialog::OnLastLineChannel (
				wxCommandEvent&					event)

{                                                           
	MHideDialogItem (this, IDC_LineChannelNumber);     
	
}	// end "OnLastLineChannel"



void CMAppendFileDialog::OnLineChannel (
				wxCommandEvent&					event)

{                                                          
	MShowDialogItem (this, IDC_LineChannelNumber);
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_LineChannelNumber, 0, SInt16_MAX);     
	
}	// end "OnLineChannel"



void CMAppendFileDialog::OnTextChange (
				wxCommandEvent&					event)

{
	long									value;
	
	
	wxTextCtrl* first = (wxTextCtrl*)FindWindow (IDC_lineChannelNumber);
	wxString val = first->GetValue ();
	if (!val.ToLong (&value))
		wxMessageBox (wxT("Line Channel number is out of bounds"),
							wxT("Error"),
							wxOK|wxICON_ERROR|wxSTAY_ON_TOP,
							this);

}	// end "OnTextChange"



bool CMAppendFileDialog::TransferDataFromWindow ()

{
	wxRadioButton* rbutton = (wxRadioButton*)FindWindow (IDC_LineChannel);
	wxTextCtrl* first = (wxTextCtrl*)FindWindow (IDC_LineChannelNumber);
	m_lineChannelNumber = wxAtoi (first->GetValue ());
	
			// Test to see whether data is in the range
			
	if (m_lineChannelNumber > m_maximumValue || m_lineChannelNumber < 0)
		{ 
				// Out of range value
				
		wxMessageBox (wxT("Line Channel number is out of bounds"), 
							wxT("Error"), 
							wxOK|wxICON_ERROR|wxSTAY_ON_TOP,
							this);
		m_lineChannelNumber = 0;
		
		}	// end "if (m_lineChannelNumber > m_maximumValue || ..."
	
	if (rbutton->GetValue ())
		m_lineChannel = 0;

	else
		m_lineChannel = 1;

	return true;
	
}	// end "TransferDataFromWindow"



bool CMAppendFileDialog::TransferDataToWindow ()

{
	wxTextCtrl* first = (wxTextCtrl*)FindWindow (IDC_lineChannelNumber);
	(*first) << m_lineChannelNumber;
	return true;
	
}	// end "TransferDataToWindow"

