//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WReformatChangeDialog.h
//	Implementation:		WReformatChangeDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMChangeFormatDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 07/03/2018
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"
#include "afxwin.h"


class CMChangeFormatDlg : public CMDialog
{
	// Construction
	public:
		CMChangeFormatDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);

		// Dialog Data
		//{{AFX_DATA (CMChangeFormatDlg)
		enum { IDD = IDD_FileFormatChange };
		int									m_bandInterleaveSelection,
												m_channelSelection,
												m_dataValueListSelection,
												m_headerListSelection,
												m_outputFileSelection;
	
		BOOL									m_invertBottomToTopFlag,
												m_invertLeftToRightFlag,
												m_outputInWavelengthOrderFlag,
												m_swapBytesFlag,
												m_transformDataFlag,
												m_writeChanDescriptionFlag;
		//}}AFX_DATA


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void SetChannelDescriptionFlag (void);
	

		// Generated message map functions
		//{{AFX_MSG (CMChangeFormatDlg)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOutputInWavelengthOrder ();
	
		afx_msg void OnSelendokBandInterleave ();
	
		afx_msg void OnSelendokDataValueType ();
	
		afx_msg void OnSelendokHeader ();
	
		afx_msg void OnSelendokOutChannels ();
	
		afx_msg void OnTransformData ();
	
		afx_msg void OnWriteChanDescriptions ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		UInt8									m_inputBandInterleaveString[64],
												m_inputDataValueTypeString[64],
												m_tiffMenuNameString[16];
	
		static ReformatOptionsPtr		s_reformatOptionsPtr;

		int									m_headerOptionsSelection;
	
		SInt16								m_dataValueTypeSelection,
												m_eightBitsPerDataSelection,
												m_noTransformDataValueTypeSelection,
												m_savedDataValueTypeSelection;
	
		Boolean								m_channelDescriptionAllowedFlag,
												m_channelThematicDisplayFlag,
												m_dataValueTypeSelectionFlag,
												m_GAIAFormatAllowedFlag,
												m_initializedFlag,
												m_outputInWavelengthOrderAllowedFlag,
												m_savedChannelDescriptionFlag,
												m_sessionUserSetDataValueTypeSelectionFlag;
	
};	// end class CMChangeFormatDlg
