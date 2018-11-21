//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LAppendFileDialog.h : CMAppendFileDialog Definition
//	Implementation:		LAppendFileDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMAppendFileDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 01/10/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LAPDFDLG_H__
	#define	__LAPDFDLG_H__             
	     
	#include "LDialog.h"
	#include <wx/string.h>
	#include <wx/stattext.h>
	#include <wx/gdicmn.h>
	#include <wx/font.h>
	#include <wx/colour.h>
	#include <wx/settings.h>
	#include <wx/radiobut.h>
	#include <wx/textctrl.h>
	#include <wx/sizer.h>
	#include <wx/button.h>
	#include <wx/dialog.h>

		// CMAppendFileDialog dialog

	class CMAppendFileDialog : public CMDialog
		{
		DECLARE_CLASS (CMAppendFileDialog)
		DECLARE_EVENT_TABLE ()
		
				// Construction
		public:
			CMAppendFileDialog (wxWindow* parent = NULL, 
										wxWindowID id = IDD_AppendFile, 
										const wxString& title = wxT("Append File Specifications"), 
										const wxPoint& pos = wxDefaultPosition, 
										const wxSize& size = wxDefaultSize, 
										long style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
										
			SInt16 DoDialog (
							FileInfoPtr							appendFileInfoPtr, 
							FileInfoPtr							newFileInfoPtr, 
							Boolean								modifyFlag,
							SInt64*								bytesToSkipPtr,
							Boolean								lineFlag,
							SInt32								newAfterLineChannel);

					// Dialog Data
					
			wxString								m_appendFileName;
			int									m_lineChannel;
			long									m_lineChannelNumber;

				// Implementation
		protected:
					// Generated message map functions
			bool OnInitDialog ();
			void OnLineChannel (
							wxCommandEvent&					event);
							
			void OnLastLineChannel (
							wxCommandEvent&					event);
							
			void OnTextChange (
							wxCommandEvent&					event);
							
			bool TransferDataFromWindow ();
			
			FileInfoPtr							m_appendFileInfoPtr; 
									
			SInt64*								m_bytesToSkipPtr;
																
			SInt32								m_maximumValue;
			 
			Boolean								m_initializedFlag, 
													m_lineFlag, 
													m_modifyFlag;
									
			wxStaticText*						m_staticText31;
			wxStaticText*						m_staticText32;
			wxRadioButton*						m_radioBtn3;
			wxTextCtrl*							m_textCtrl14;
			wxRadioButton*						m_radioBtn4;
			wxButton*							m_button15;
			wxButton*							m_button16;
			
		};  
#endif // !defined __LAPDFDLG_H__
