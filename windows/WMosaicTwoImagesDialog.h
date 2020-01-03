//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WMosaicTwoImagesDialog.h
//	Implementation:		WMosaicTwoImagesDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMMosaicTwoImagesDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMMosaicTwoImagesDialog : public CMDialog
{
	public:
		CMMosaicTwoImagesDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		virtual ~CMMosaicTwoImagesDialog ();

		Boolean DoDialog (
				FileInfoPtr							outFileInfoPtr,
				FileInfoPtr							fileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);


		// Dialog Data
		enum {IDD = IDD_Mosaic};
	
		DialogSelectArea					m_rightBottomDialogSelectArea;

		int									m_bottomFileNameSelection,
												m_headerListSelection,
												m_mosaicDirectionCode,
												m_rightFileNameSelection;
	
		long									m_backgroundValue,
												m_maxDataValue;

		BOOL									m_ignoreBackgroundValueFlag,
												m_writeChannelDescriptionsFlag;

	protected:
		void CheckBottomImageColumns (
				UInt16								itemSelected);

		void CheckRightImageLines (
				UInt16								itemSelected);

		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
	
		// Generated message map functions
		//{{AFX_MSG (CMMosaicTwoImagesDialog)
		afx_msg void OnBnClickedIgnoreBackgroundValue ();
	
		afx_msg void OnCbnSelchangeBottomimagefilelist ();
	
		afx_msg void OnCbnSelchangeHeaderformatlist ();
	
		afx_msg void OnCbnSelchangeMosaicdirection ();
	
		afx_msg void OnCbnSelchangeRightimagefilelist ();
	
		afx_msg void OnEnChangeBottomColumnStart3 ();
	
		afx_msg void OnEnChangeLeftTopLineEnd ();
	
		afx_msg void OnEnChangeLeftTopLineStart ();
	
		afx_msg void OnEnChangeLeftTopColumnEnd ();
	
		afx_msg void OnEnChangeLeftTopColumnStart ();
	
		afx_msg void OnEnChangeRightLineStart2 ();
	
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		FileInfoPtr							m_fileInfoPtr,
												m_outputFileInfoPtr;
	
		ReformatOptionsPtr				m_reformatOptionsPtr;

		Handle								m_rightBottomWindowInfoHandle;

		SInt16								m_headerOptionsSelection;
	
		Boolean								m_initializedFlag;
	
};	// end class CMMosaicTwoImagesDialog
