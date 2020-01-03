//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xFieldsToThematicDialog.h
//	Implementation:		xFieldsToThematicDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFieldsToThematicDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/07/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "xDialog.h"


class CMFieldsToThematicDialog : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMFieldsToThematicDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_FeatureExtraction,
				const wxString& 					title =
															wxT("Project Areas to Thematic Image"));
		
		Boolean DoDialog (
				ReformatOptionsPtr				reformatOptionsPtr);

	
		enum {IDD = IDD_AreasToThematicImage};

			// Implementation
	protected:
	
				// Generated message map functions

		void OnInitDialog (
				wxInitDialogEvent& 				event);    
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
		ReformatOptionsPtr				m_reformatOptionsPtr;
			  
		wxChoice								*m_fileFormatCtrl;
	
		wxCheckBox							*m_checkBox66,
												*m_checkBox67;
	
		wxStaticText						*m_staticText282,
												*m_staticText283,
												*m_staticText284,
												*m_staticText285;
	
		int									m_outputFormatCode;

		bool									m_testingAreasFlag,
												m_trainingAreasFlag;
	
		Boolean								m_initializedFlag;
				 
};	// end "class CMFieldsToThematicDialog"
