//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						WClassPairWeightsDialog.h
//	Implementation:		WClassPairWeightsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClassPairWeightDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMClassPairWeightDlg : public CMDialog
{
	// Construction
	public:
		CMClassPairWeightDlg (	// standard constructor
				CWnd* 								pParent = NULL);

		SInt16 DoDialog (
				UInt16								numberOfClassesToUse,
				SInt16**	 							weightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr);

		// Dialog Data
		//{{AFX_DATA (CMClassPairWeightDlg)
		enum { IDD = IDD_ClassPairWeights };
	
		long									m_localDefaultClassPairWeight,
												m_newClassPairWeight;
		//}}AFX_DATA

	// Implementation
	protected:
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL (CMClassPairWeightDlg)
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG (CMClassPairWeightDlg)
		afx_msg void OnAddButton ();
	
		afx_msg void OnChangeDefaultWeight ();
	
		afx_msg void OnChangeNewWeight ();
	
		afx_msg void OnHelpButton ();
	
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
	
		afx_msg void OnRemoveButton ();
	
		afx_msg void OnSelchangeClassList ();
	
		afx_msg void OnSelchangeClassPairWeightList ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()


		CFont									m_weightsListFont;

		SInt16**	 							m_weightsListPtrPtr;
	
		SInt16*				 				m_defaultClassPairWeightPtr;

		SInt16								m_classPairWeightsSelection,
												m_selectedClassCell,
												m_selectedWeightGroupCell;

		UInt16								m_numberOfClassesToUse;
	
		Boolean								m_initializedFlag;

};	// end "CMClassPairWeightDlg"
