//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WAbout.h
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Language:				C
//
//	System:					Windows Operating Systems
//
//	Brief description:	Header file for the CAboutDlg class
//
//	Written By:				Larry L. Biehl			Date: 05/23/1995
//	Revised By:				Larry L. Biehl			Date: 12/31/2017
//	
//------------------------------------------------------------------------------------

#if !defined __MABOUT_H__
	#define __MABOUT_H__ 
	
		// oul: added the condition of _AMD64_ to be compatible with x64
		// _AMD64_ is the predefined macro for x64 machines according to MSDN
#if defined _X86_ || defined _AMD64_     
	  
#include "WDialog.h"
#include "afxwin.h"



class CAboutDlg : public CMDialog
	{
	public:
						CAboutDlg (CWnd* pParent = NULL);
		
		BOOL 			Create (CWnd* pParent);
		
		Boolean 		DoDialog (void);
	
			// Dialog Data
		//{{AFX_DATA(CAboutDlg)
		enum { IDD = IDD_ABOUTBOX };
		//}}AFX_DATA
			
		// Implementation
	protected:
		virtual void	DoDataExchange (CDataExchange* pDX);    // DDX/DDV support
		
		Boolean			m_fromStartUpFlag; 
		Boolean			m_initializedFlag;
		Boolean			m_FullVersionSettingFlag;
		
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
			
	DECLARE_MESSAGE_MAP ()
	
	public:
		afx_msg void OnStnClickedStatic11();
		afx_msg void OnBnClickedCheck1();
		afx_msg void OnBnClickedSimplerversion();
		BOOL m_simplerVersionFlag;
	
	};                   

#endif	// defined _X86_ || defined _AMD64_

#endif	// !defined __MABOUT_H__