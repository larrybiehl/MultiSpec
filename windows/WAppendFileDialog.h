//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WAppendFileDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Language:				C
//
//	System:					Windows Operating Systems
//
//	Brief description:	Header file for the CMAppendFileDialog class
//
//	Written By:				Larry L. Biehl			Date: 05/23/1995
//	Revised By:				Larry L. Biehl			Date: 12/31/2017
//	
//------------------------------------------------------------------------------------

#if !defined __WAPDFDLG_H__
	#define	__WAPDFDLG_H__             
	     
#include "WDialog.h"

class CMAppendFileDialog : public CMDialog
	{
	// Construction
	public:
		CMAppendFileDialog (CWnd* pParent = NULL);	// standard constructor
		
		SInt16 DoDialog (
						FileInfoPtr					appendFileInfoPtr,
						FileInfoPtr					newFileInfoPtr, 
						Boolean						modifyFlag,
						SInt64*						bytesToSkipPtr,
						Boolean						lineFlag,
						SInt32						newAfterLineChannel);

			// Dialog Data
		//{{AFX_DATA(CMAppendFileDialog)
		enum { IDD = IDD_AppendFile };
		CString	m_appendFileName;
		int		m_lineChannel;
		long	m_lineChannelNumber;
		//}}AFX_DATA

			// Implementation
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG(CMAppendFileDialog)
		virtual BOOL OnInitDialog ();
		afx_msg void OnLineChannel ();
		afx_msg void OnLastLineChannel ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP () 
		
		FileInfoPtr			m_appendFileInfoPtr; 
								
		SInt64*				m_bytesToSkipPtr;
															  
		SInt32				m_maximumValue;
		 
		Boolean				m_initializedFlag,
								m_lineFlag,
								m_modifyFlag;
		
	};  
  
#endif // !defined __WAPDFDLG_H__
