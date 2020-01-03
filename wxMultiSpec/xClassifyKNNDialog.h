//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                           Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2020)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//	File:               	xClassifyKNNDialog.h
//	Implementation:      xClassifyKNNDialog.cpp
//
//	Authors:					Tsung Tai Yeh
//								Larry L. Biehl
//
//	Language:            C++
//
//	System:              Linux Operating System
//
//	Brief description:   Header file for the CMKNNClassifyDialog class
//
//	Written By:         	Tsung Tai Yeh         Date: 04/??/2019
// Revised By:          Larry L. Biehl        Date: 08/09/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMKNNClassifyDialog : public CMDialog
	{
	public:
		CMKNNClassifyDialog (
				wxWindow*							parent = NULL,
				wxWindowID           			id = IDD_KNNParameters,
				const wxString&					title =
														wxT("Specify KNN Classifier Parameters"));
		
		Boolean DoDialog (
				SInt16*								nearestNeighborKValuePtr);
		
			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxString 							m_kValueString;
		
		wxBoxSizer							*bSizer166;
		
		wxStaticText						*m_staticText183,
												*m_staticText184;
		
		wxTextCtrl							*m_kValueControl;
	
		SInt16								m_nearestNeighborKValue;
	
		bool									m_initializedFlag;
		
};	// end "class CMKNNClassifyDialog"
