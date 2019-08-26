//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                           Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2019)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//	File:               	LClassifyKNNDialog.h
//	Implementation:      LClassifyKNNDialog.cpp
//
//	Authors:					Tsung Tai Yeh
//								Larry L. Biehl
//
//	Language:            C++
//
//	System:              Linux and MacOS Operating Systems
//
//	Brief description:   Header file for the CMKNNClassifyDialog class
//
//	Written By:         	Tsung Tai Yeh         Date: 04/??/2019
// Revised By:          Larry L. Biehl        Date: 08/09/2019
//
//------------------------------------------------------------------------------------

#if !defined __LKNNLG_H__
#define   __LKNNLG_H__

#include "LDialog.h"

class CMKNNClassifyDialog : public CMDialog
{
public:
   CMKNNClassifyDialog (
   			wxWindow*                        parent = NULL,
				wxWindowID                       id = IDD_KNNParameters,
				const wxString&                  title = wxT("Specify KNN Classifier Parameters"));
   
   Boolean DoDialog (
				SInt16*								nearestNeighborKValuePtr);
   
   SInt16			m_nearestNeighborKValue;
	wxString 		m_kValueString;
   
		// Implementation
protected:
   void CreateControls ();
   void OnInitDialog (wxInitDialogEvent&               event);
   bool TransferDataFromWindow ();
   bool TransferDataToWindow ();
   
   DECLARE_EVENT_TABLE()
   
   bool                            m_initializedFlag;
   
   		// Linux GUI controls
	
   wxBoxSizer*                     bSizer166;
	
   wxStaticText*                   m_staticText183;
   wxStaticText*                   m_staticText184;
   
   wxTextCtrl*                     m_kValueControl;
   
};	// end "CMKNNClassifyDialog dialog"

#endif   // !defined __LKNNLG_H__


