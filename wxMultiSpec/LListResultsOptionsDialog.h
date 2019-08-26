//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LListResultsOptionsDialog.h
//	Implementation:		LListResultsOptionsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMListResultsOptionsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------
//
#if !defined __LLIOPDLG_H__
#define	__LLIOPDLG_H__             

#include "LDialog.h"

typedef bool BOOL;


class CMListResultsOptionsDlg : public CMDialog
	{
		 // Construction
	public:
				 // standard constructor
		CMListResultsOptionsDlg (
		 		wxWindow* 			pParent = NULL,
		 		wxWindowID 			id = IDD_ClassWeights,
		 		const wxString& 	title = wxT("Classification Results List Options"));

		void DoDialog (
					SInt16* 		listResultsTrainingCodePtr,
					SInt16* 		listResultsTestCodePtr);

				// Dialog Data

		enum { IDD = IDD_ResultsListOptions };
		BOOL m_trainAreasUsedFlag;
		BOOL m_trainAreaSummariesFlag;
		BOOL m_trainAreaPerformanceFlag;
		BOOL m_trainClassPerformanceFlag;
		BOOL m_testAreasUsedFlag;
		BOOL m_testAreaSummariesFlag;
		BOOL m_testAreaPerformanceFlag;
		BOOL m_testClassPerformanceFlag;

				// Implementation
	protected:
		bool TransferDataToWindow();
		bool TransferDataFromWindow();
		void CreateControls();
	
		void OnInitDialog(wxInitDialogEvent& event);

		DECLARE_EVENT_TABLE ();
	
		SInt16 			m_listResultsTestCode,
							m_listResultsTrainingCode;

		wxBoxSizer* bSizer142;
		wxStaticText* m_staticText169;
		wxStaticText* m_staticText170;
		wxCheckBox* m_checkBox17;
		wxCheckBox* m_checkBox18;
		wxCheckBox* m_checkBox19;
		wxCheckBox* m_checkBox20;
		wxCheckBox* m_checkBox21;
		wxCheckBox* m_checkBox22;
		wxCheckBox* m_checkBox23;
		wxCheckBox* m_checkBox24;
		wxButton* m_button31;
		wxButton* m_button32;
	};

#endif // !defined __LLIOPDLG_H__
