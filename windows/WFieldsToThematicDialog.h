//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WFieldsToThematicDialog.h
//	Implementation:		WFieldsToThematicDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMFieldsToThematicDialog class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMFieldsToThematicDialog : public CMDialog
{
	// Construction
	public:
		CMFieldsToThematicDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				ReformatOptionsPtr				reformatOptionsPtr);

		// Dialog Data
		//{{AFX_DATA (CMFieldsToThematicDialog)
		enum { IDD = IDD_AreasToThematicImage };
	
		INT									m_outputFormatCode;
	
		BOOL									m_testingAreasFlag,
												m_trainingAreasFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMFieldsToThematicDialog)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		ReformatOptionsPtr				m_reformatOptionsPtr;
	
		Boolean								m_initializedFlag;
		
};	// end class CMFieldsToThematicDialog
