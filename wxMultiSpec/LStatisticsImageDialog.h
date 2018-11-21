//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsImageDialog.h
//	Implementation:		LStatisticsImageDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMStatImageDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/04/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LSTATIMGDLG_H__
	#define	__LSTATIMGDLG_H__     

	#include "SMultiSpec.h"
	#include "LDialog.h"

	//typedef bool BOOL;

	class CMStatImageDialog : public CMDialog 
		{
		public:
			CMStatImageDialog(wxWindow* pParent = NULL, wxWindowID 
								id = IDS_ProcessorCreateImageStat , 
								const wxString& title = wxT("Set Create Statistics Images Specifications")); // standard constructor     
			~CMStatImageDialog(); // standard desctructor 

			void CreateControls();
			Boolean DoDialog();

			bool TransferDataFromWindow();
			bool TransferDataToWindow();

			enum {IDD = IDS_ProcessorCreateImageStat};

			SInt16			m_classCode;
			SInt16			m_areaCode;
			SInt16         m_channelSelection;
			Boolean			m_featureTransformationFlag;
			SInt16         m_maximumNumberChannels;
			SInt16         m_classSelection;
			SInt16			m_perClassCode;
			SInt16			m_perFieldCode;
			SInt16         m_overallMinMaxCode;
			SInt16         m_individualMinMaxCode;
			SInt16			m_userMinMaxCode;
			SInt16         m_MinMaxCode;
			double			m_userMinimum;
			double			m_userMaximum;

			SInt16*			m_featurePtr;
			SInt16*        m_channelsPtr;
			SInt16*			m_classPtr;

			Boolean        m_channelsAllAvailableFlag;
			  
			  // Implementation
		protected:
			void OnInitDialog(wxInitDialogEvent& event);
			void OnSelendokClassCombo(wxCommandEvent& event);
			void OnSelendokChannelCombo(wxCommandEvent& event);
			void OnClickUserSettingRadio(wxCommandEvent& event);
			void OnClickIndividualRadio(wxCommandEvent& event);
			void OnClickOverallRadio(wxCommandEvent& event);

			void CheckStatColumnEnd(wxCommandEvent& event);
			void CheckStatColumnInterval(wxCommandEvent& event);
			void CheckStatColumnStart(wxCommandEvent& event);
			void CheckStatLineEnd(wxCommandEvent& event);
			void CheckStatLineInterval(wxCommandEvent& event);
			void CheckStatLineStart(wxCommandEvent& event);
			void OnSelendokChannelComboDropDown(wxCommandEvent& event);
			DECLARE_EVENT_TABLE()

			wxStaticText*   m_staticText312;
			wxStaticText*   m_staticText313;
			wxStaticText*   m_staticText315;
			wxStaticText*   m_staticText316;
			wxStaticText*   m_staticText317;
			wxStaticText*   m_staticText318;
			wxStaticText*   m_staticText177;
			//wxStaticText*   m_staticText60;
			//wxStaticText*   m_staticText62;
			wxStaticText*   m_staticText319;
			wxStaticText*   m_staticText320;
			wxStaticText*   m_staticText283;

			wxTextCtrl*     m_textCtrl153;
			wxTextCtrl*     m_textCtrl154;
			wxTextCtrl*     m_textCtrl155;
			//wxTextCtrl*     m_linestartctrl;
			//wxTextCtrl*     m_lineendctrl;
			//wxTextCtrl*     m_lineintctrl;
			//wxTextCtrl*     m_colstartctrl;
			//wxTextCtrl*     m_colendctrl;
			//wxTextCtrl*     m_colintctrl;

			wxCheckBox*     m_checkBox74;

			wxComboBox*     m_comboBox56;
			wxComboBox*     m_comboBox57;
			wxComboBox*     m_comboBox58;

			wxRadioButton*  m_radioBtn36;
			wxRadioButton*  m_radioBtn37;
			wxRadioButton*  m_radioBtn38;
			wxRadioButton*  m_radioBtn39;
			wxRadioButton*  m_radioBtn40;
			wxRadioButton*  m_radioBtn41;
			wxRadioButton*  m_radioBtn42;

			wxBitmapButton* m_bpButton4;
			wxBitmapButton* m_bpButton5;

			wxButton*       m_button81;
			wxButton*       m_button82;

			wxBoxSizer*     bSizer292;
		};
#endif