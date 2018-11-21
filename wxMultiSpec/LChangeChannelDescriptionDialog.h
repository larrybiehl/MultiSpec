//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LChangeChannelDescriptionDialog.h : 
//															CMChangeChannelDescriptionDlg Definition
//	Implementation:		LChangeChannelDescriptionDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMChangeChannelDescriptionDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LChangeChannelDescriptionDialog_H__
	#define __LChangeChannelDescriptionDialog_H__             

	#include "LDialog.h"


	class CMChangeChannelDescriptionDlg : public CMDialog
		{  
			 // Construction
		public:
			 CMChangeChannelDescriptionDlg (
							wxWindow*						pParent = NULL, 
							wxWindowID						id = wxID_ANY, 
							const wxString&				title = wxT("Channel Descriptions"));
			 
			 ~CMChangeChannelDescriptionDlg (); // standard desctructor  
			 
			 
			 
			 enum {IDD = IDD_ChangeChannelDescriptions};
			 
			 
			 SInt16 DoDialog (
							FileInfoPtr							fileInfoPtr, 
							ChannelDescriptionPtr			channelDescriptionPtr, 
							float*								channelValuesPtr);
			
		protected:
			void OnInitDialog (wxInitDialogEvent& event); 

			void CreateControls ();

			void OnBnClickedAddModifyChannelDescriptionRadio (wxCommandEvent& event);
			void OnBnClickedDeleteChannelDescriptionRadio (wxCommandEvent& event);
			void OnNextChannelButton (wxCommandEvent& event);
			void OnOK ();
			void OnPreviousChannelButton (wxCommandEvent& event);
			void OnSetfocusDescription (wxFocusEvent& event);
			void OnSetfocusValue (wxFocusEvent& event);
			
			bool TransferDataFromWindow ();
			bool TransferDataToWindow ();
			
			void Update ();
			 
			DECLARE_EVENT_TABLE ();
			 
			wxString							m_description;
			
			ChannelDescriptionPtr		m_channelDescriptionPtr; 
			FileInfoPtr						m_fileInfoPtr;
			float*							m_channelValuesPtr;
			
			float								m_value;
			
			SInt16							m_addModifiyDeleteCode,
												m_channelIndex,
												m_lastSelectedTextItem;
														
			Boolean							m_changeFlag,
												m_initializedFlag;

			wxButton*						m_button84;
			wxButton*						m_button85;
			wxButton*						m_button88;
			wxButton*						m_button89;
			wxStaticText*					m_staticText306;
			wxStaticText*					m_staticText324;
			wxStaticText*					m_staticText325;
			wxStaticText*					m_staticText326;
			wxStaticText*					m_staticText327;
			wxStaticText*					m_staticText328;
			wxStaticText*					m_staticText329;
			wxTextCtrl*						m_textCtrl143;
			wxTextCtrl*						m_textCtrl144;
			wxRadioButton*					m_radioBtn38;
			wxRadioButton*					m_radioBtn39;
			 
		};
#endif // !defined __LChangeChannelDescriptionDialog_H__