//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                           Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2019)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//   File: 					LClassifyCorrelationDialog.h
//   Implementation: 	LClassifyCorrelationDialog.cpp
//
//   Authors: 				Larry L. Biehl
//
//   Language:   			C++
//
//   System: 				Linux and Mac Operating Systems
//
//   Brief description:	Header file for the CMSVMClassifyDialog class
//
//   Written By: 			Tsung Tai Yeh         	Date: 09/??/2019
//   Revised By:			Larry L. Biehl 			Date: 09/24/2019
//
//------------------------------------------------------------------------------------

#if !defined __LSVMLG_H__
#define   __LSVMLG_H__

#include "LDialog.h"

// CMCorrelationClassifyDialog dialog

class CMSVMClassifyDialog : public CMDialog
{
public:
   CMSVMClassifyDialog (
			wxWindow*						parent = NULL,
			wxWindowID						id = IDD_SVMParameters,
			const wxString&				title = wxT("Specify SVM Classifier Parameters"));
   
   Boolean DoDialog (void);
	
   int		m_svm_type;
   int		m_kernel_type;
   int		m_degree;
   double	m_gamma;
   double	m_coef0;
   double	m_cache_size;
   double	m_eps;
   double	m_cost;
   double	m_nu;
   double	m_p;
   int		m_shrinking;
   int		m_probability;
	
   // Implementation
protected:
   void CreateControls ();
   void OnInitDialog (wxInitDialogEvent& event);
   void OnSelendokSVMKernelCombo (wxCommandEvent& event);
   void OnSelendokSVMTypeCombo (wxCommandEvent& event);
   bool TransferDataFromWindow ();
   bool TransferDataToWindow ();
   
   DECLARE_EVENT_TABLE ()
   
   bool                             m_initializedFlag;
   
   		// Linux GUI controls
	
   wxBoxSizer* 							m_bVSizer151;
	
};   // end "CMSVMClassifyDialog dialog"

#endif   // !defined __LCLSFDLG_H__

