//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xRecodeThematicImageDialog.h
//	Implementation:		xRecodeThematicImageDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMRecodeThematicDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/08/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMRecodeThematicDialog : public CMDialog
{
			// Construction
	public:
		CMRecodeThematicDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
															wxT("Set Thematic Recode Parameters"));

		~CMRecodeThematicDialog (); // standard desctructor

		Boolean DoDialog (
				RecodeThematicImagePtr  		recodeThematicImagePtr);
	
				// Dialog Data
		enum { IDD = IDD_ThematicRecode };


			// Implementation
	protected:
		virtual void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnSelendokThresholdFileCombo (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		RecodeThematicImagePtr  		m_recodeThematicImagePtr;

		wxChoice								*m_operationCtrl,
												*m_thresholdFileCtrl;

		wxStaticText						*m_staticText339,
												*m_staticText340,
												*m_staticText341,
												*m_staticText342,
												*m_staticText343,
												*m_staticText344,
												*m_staticText345;

		wxTextCtrl							*m_textCtrl173,
												*m_textCtrl175;
	
		Handle                  		m_thresholdFileInfoHandle;
	
		int									m_compareParameterCode,
												m_thresholdImageSelection;
	
		SInt32								m_recodedValue,
												m_thresholdValue;
	
		Boolean                			m_initializedFlag;
	
};	// end "class CMRecodeThematicDialog"
