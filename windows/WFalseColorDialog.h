//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WFalseColorDialog.h
//	Implementation:		WFalseColorDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMFalseColorDlg class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMFalseColorDlg : public CMDialog
{
	// Construction
	public:
		CMFalseColorDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (void);
		

		// Dialog Data
		//{{AFX_DATA (CMFalseColorDlg)
		enum { IDD = IDD_FalseColor };
	
		long									m_blueChannel,
												m_greenChannel,
												m_redChannel;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMFalseColorDlg)
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		FileInfoPtr							m_fileInfoPtr;
		LayerInfoPtr						m_layerInfoPtr;
	
		SInt32								m_maxChannelFeatureNum;
	
		SInt16								m_blueChannelIndex,
												m_greenChannelIndex,
												m_redChannelIndex;
	
		Boolean								m_initializedFlag;
	
};	// end class CMFalseColorDlg
