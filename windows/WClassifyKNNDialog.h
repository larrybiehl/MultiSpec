//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClassifyKNNDialog.h
//	Implementation:		WClassifyKNNDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMKNNClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 07/27/2019
//
//------------------------------------------------------------------------------------

#include "WDialog.h"


class CMKNNClassifyDialog : public CMDialog
{
	public:
		CMKNNClassifyDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16*								nearestNeighborKValuePtr);

		// Dialog Data
		//{{AFX_DATA (CMKNNClassifyDialog)
		enum { IDD = IDD_ClassifyKNNParameters };
	
		int									m_nearestNeighborKValue;
		//}}AFX_DATA

	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMKNNClassifyDialog)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		Boolean								m_initializedFlag;
		
};	// end class CMKNNClassifyDialog
