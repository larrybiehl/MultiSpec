//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WAppendFileDialog.h
//	Implementation:		WAppendFileDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMAppendFileDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/31/2017
//	
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"

class CMAppendFileDialog : public CMDialog
{
	public:
		CMAppendFileDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		SInt16 DoDialog (
				FileInfoPtr							appendFileInfoPtr,
				FileInfoPtr							newFileInfoPtr,
				Boolean								modifyFlag,
				SInt64*								bytesToSkipPtr,
				Boolean								lineFlag,
				SInt32								newAfterLineChannel);

		// Dialog Data
		//{{AFX_DATA (CMAppendFileDialog)
		enum { IDD = IDD_AppendFile };
	
		CString								m_appendFileName;
	
		int									m_lineChannel;
		
		long									m_lineChannelNumber;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMAppendFileDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnLineChannel ();
	
		afx_msg void OnLastLineChannel ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP () 
		
		FileInfoPtr							m_appendFileInfoPtr;
								
		SInt64*								m_bytesToSkipPtr;
															  
		SInt32								m_maximumValue;
		 
		Boolean								m_initializedFlag,
												m_lineFlag,
												m_modifyFlag;
		
}; // end class CMAppendFileDialog
