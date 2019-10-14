//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsListDialog.h
//	Implementation:		LStatisticsListDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMListStatsDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/21/2017
//
//------------------------------------------------------------------------------------
//
#include "SMultiSpec.h"

#include "LDialog.h"
#include "LImageView.h"
#include "wx/wx.h"

typedef long unsigned int UINT;

class CMListStatsDialog : public CMDialog
	{
		 // Construction
	public:
				// standard constructor
		CMListStatsDialog (
		 		wxWindow* 			pParent = NULL,
		 		wxWindowID 			id = wxID_ANY,
		 		const wxString& 	title = wxT("List Stats"));
		Boolean DoDialog (
				SInt16 statsWindowMode);

		void CheckListFieldClassSettings(void);
		bool TransferDataToWindow();
		bool TransferDataFromWindow();

			// Dialog Data

		enum { IDD = IDD_ListStats };
		Boolean m_listClassFlag;
		Boolean m_listFieldFlag;
		Boolean m_listMeansStdDevFlag;
		Boolean m_listCovarianceFlag;
		Boolean m_listCorrelationFlag;
		Boolean m_featureTransformationFlag;
		UINT m_listMeanStdPrecision;
		UINT m_listCovCorPrecision;
		wxString m_meanPrecisionString;
		wxString m_covPrecisionString;

		 		// Implementation
	protected:

		 		// Generated message map functions
	
		void OnInitDialog(wxInitDialogEvent& event);
		void OnClasses(wxCommandEvent& event);
		void OnFields(wxCommandEvent& event);

		void CreateControls ();

		DECLARE_EVENT_TABLE ()

		SInt16 m_statsWindowMode;

		wxStaticText* m_staticText258;
		wxStaticText* m_staticText259;
		wxStaticText* m_staticText264;
		wxStaticText* m_staticText265;
		wxStaticText* m_staticText266;
		wxCheckBox* m_checkBox44;
		wxCheckBox* m_checkBox45;
		wxCheckBox* m_checkBox46;
		wxCheckBox* m_checkBox47;
		wxCheckBox* m_checkBox48;
		wxTextCtrl* m_textCtrl125;
		wxTextCtrl* m_textCtrl126;
		wxCheckBox* m_checkBox49;
		wxButton* m_button20;
		wxButton* m_button21;
	
	};
