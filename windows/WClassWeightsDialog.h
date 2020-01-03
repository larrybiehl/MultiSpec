//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClassWeightsDialog.h
//	Implementation:		WClassWeightsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClassWeightsDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMClassWeightsDlg : public CMDialog
{
	// Construction
	public:
		CMClassWeightsDlg (	// standard constructor
				CWnd* 								pParent = NULL);

		SInt16 DoDialog (
				UInt16								numberOutputClassesToUse,
				SInt16*								classListPtr,
				float*								weightsListPtr,
				SInt16								weightsSelection,
				Boolean								useEnhancedStatFlag);

		// Dialog Data
		//{{AFX_DATA (CMClassWeightsDlg)
		enum { IDD = IDD_ClassWeights };
	
		double								m_saveNewWeight;
	
		int									m_weightUnits;
		//}}AFX_DATA

	// Implementation
	protected:
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL (CMClassWeightsDlg)
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG (CMClassWeightsDlg)
		afx_msg void OnChangeWeight ();
	
		afx_msg void OnEnterNewWeightButton ();
	
		afx_msg void OnEqualWeightButton ();
	
		afx_msg void OnHelpButton ();
	
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelchangeList ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		CFont									m_weightsListFont;
	
		double								m_weightSum;
	
		float*								m_weightsListPtr;
		SInt16*								m_classListPtr;

		SInt16								m_selectedCell,
												m_weightsSelection;

		UInt16								m_numberOutputClassesToUse;

		Boolean								m_initializedFlag,
												m_useEnhancedStatFlag;

};	// end "CMClassWeightsDlg"
