//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
