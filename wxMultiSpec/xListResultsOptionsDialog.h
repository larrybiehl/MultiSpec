//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xListResultsOptionsDialog.h
//	Implementation:		xListResultsOptionsDialog.cpp
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

#pragma once

#include "xDialog.h"


class CMListResultsOptionsDlg : public CMDialog
{
		 // Construction
	public:
				 // standard constructor
		CMListResultsOptionsDlg (
		 		wxWindow* 							pParent = NULL,
		 		wxWindowID 							id = IDD_ClassWeights,
		 		const wxString& 					title =
													wxT("Classification Results List Options"));

		void DoDialog (
				SInt16* 								listResultsTrainingCodePtr,
				SInt16* 								listResultsTestCodePtr);


				// Dialog Data

		enum { IDD = IDD_ResultsListOptions };
	
		bool 									m_testAreaPerformanceFlag,
												m_testAreaSummariesFlag,
												m_testAreasUsedFlag,
												m_testClassPerformanceFlag,
												m_trainAreaPerformanceFlag,
												m_trainAreaSummariesFlag,
												m_trainAreasUsedFlag,
												m_trainClassPerformanceFlag;

				// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ();
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxBoxSizer							*bSizer142;
		
		wxCheckBox							*m_checkBox17,
												*m_checkBox18,
												*m_checkBox19,
												*m_checkBox20,
												*m_checkBox21,
												*m_checkBox22,
												*m_checkBox23,
												*m_checkBox24;
	
		wxStaticText						*m_staticText169,
												*m_staticText170;
	
		SInt16 								m_listResultsTestCode,
												m_listResultsTrainingCode;
	
};	// end "class CMListResultsOptionsDlg"
