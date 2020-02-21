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
//	File:						WClassifyCEMDialog.h
//	Implementation:		WClassifyCEMDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMCEMClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMCEMClassifyDialog : public CMDialog
{
	// Construction
	public:
		CMCEMClassifyDialog (	// standard constructor
				CWnd* 								pParent = NULL);

		Boolean DoDialog (
				CEMParametersPtr					cemParametersPtr,
				UInt16*								classifyProcedureEnteredCodePtr);

		// Dialog Data
		//{{AFX_DATA (CMCEMClassifyDialog)
		enum { IDD = IDD_CEMParameters };
	
		int									m_correlationMatrixCode;
	
		BOOL									m_testFieldsFlag,
												m_trainingFieldsFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMCEMClassifyDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnUseSelectedArea ();
	
		afx_msg void OnUseClasses ();
	
		afx_msg void OnSelendokClassCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void BackgroundCorrelationSettings (
 				Boolean								areaCode);
 				
	
		CEMParametersPtr					m_cemParametersPtr;
	
		SInt16								*m_correlationMatrixClassPtr,
												*m_localClassAreaPtr;
	
		UInt32								m_localNumberClassAreas;
	
		Boolean								m_initializedFlag;
	
};	// end class CMCEMClassifyDialog
