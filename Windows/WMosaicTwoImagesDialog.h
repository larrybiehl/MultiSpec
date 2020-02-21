//	 									MultiSpec
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
