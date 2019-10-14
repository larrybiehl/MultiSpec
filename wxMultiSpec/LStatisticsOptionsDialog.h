//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsOptionsDialog.h
//	Implementation:		LStatisticsOptionsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMStatOptionsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 07/07/2017
//
//------------------------------------------------------------------------------------
//
#if !defined __LSTATISTICSOPTIONSDLG_H__
	#define	__LSTATISTICSOPTIONSDLG_H__  

#include "LDialog.h"  
 
typedef wxString CString;


class CMStatOptionsDlg : public CMDialog
	{
		 // Construction
	public:
				// standard constructor
		CMStatOptionsDlg (
				wxWindow* 			pParent = NULL,
				wxWindowID 			id = IDD_ClassifyDialog,
				const wxString& 	title = wxT("Set Project Statistics Options"));

		SInt16 DoDialog (
				SInt16* 				statCodePtr,
				Boolean* 			keepClassStatsFlagPtr,
				double* 				variancePtr,
				double* 				minLogDeterminantPtr,
				Boolean* 			useCommonCovarianceInLOOCFlagPtr);

				// Dialog Data

		enum { IDD = IDD_StatisticsOptionsDialog };
	
		bool 					m_classStatsOnlyFlag,
								m_setZeroVarianceFlag;
		int m_statCode;
		double m_minLogDetValue;
		double m_varianceValue;
		bool m_useCommonCovarianceInLOOCFlag;

		 // Implementation
	protected:
		bool TransferDataFromWindow();
		bool TransferDataToWindow();
		void CreateControls();
		void OnInitDialog(wxInitDialogEvent& event);
		void OnMeanStd(wxCommandEvent& event);
		void OnSetZeroVariance(wxCommandEvent& event);
		void OnMeanStdCov(wxCommandEvent& event);

		DECLARE_EVENT_TABLE ()

		void SetZeroVariance (void);

		wxBoxSizer* bSizer181;
		wxStaticText* m_staticText193;
		wxStaticText* m_staticText194;
		wxStaticText* m_staticText195;
		wxRadioButton* m_radioBtn17;
		wxRadioButton* m_radioBtn18;
		wxCheckBox* m_checkBox33;
		wxCheckBox* m_checkBox34;
		wxCheckBox* m_checkBox35;
		wxTextCtrl* m_textCtrl92;
		wxTextCtrl* m_textCtrl93;
		wxButton* m_button42;
		wxButton* m_button43;

		wxString m_varienceString;
		wxString m_minLogString;
	
	};
#endif // !defined __LSTATISTICSOPTIONSDLG_H__ 
