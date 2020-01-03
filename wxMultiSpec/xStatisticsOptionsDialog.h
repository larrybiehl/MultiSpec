//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xStatisticsOptionsDialog.h
//	Implementation:		xStatisticsOptionsDialog.cpp
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

#pragma once

#include "xDialog.h"


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

		 // Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnMeanStd (
				wxCommandEvent& 					event);
	
		void OnMeanStdCov (
				wxCommandEvent& 					event);
	
		void OnSetZeroVariance (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();

		void SetZeroVariance (void);
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		wxBoxSizer									*bSizer181;
	
		wxCheckBox									*m_checkBox33,
														*m_checkBox34,
														*m_checkBox35;
	
		wxRadioButton								*m_radioBtn17,
														*m_radioBtn18;
	
		wxStaticText								*m_staticText193,
														*m_staticText194,
														*m_staticText195;
	
		wxTextCtrl									*m_textCtrl92,
														*m_textCtrl93;

		wxString 									m_minLogString,
			 											m_varienceString;
	
		double 										m_minLogDetValue,
			 											m_varianceValue;
	
		int 											m_statCode;
	
		bool 											m_classStatsOnlyFlag,
														m_setZeroVarianceFlag,
														m_useCommonCovarianceInLOOCFlag;
	
};	// end "class CMStatOptionsDlg"
