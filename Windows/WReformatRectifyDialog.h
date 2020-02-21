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
//	File:						WReformatRectifyDialog.h
//	Implementation:		WReformatRectifyDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMReformatRectifyDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMReformatRectifyDlg : public CMDialog
{
	// Construction
	public:
		CMReformatRectifyDlg (	// standard constructor
				CWnd* 								pParent = NULL);
							
		Boolean DoDialog (
				FileInfoPtr							outFileInfoPtr,
				FileInfoPtr							fileInfoPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				LayerInfoPtr						imageLayerInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				double								minBackgroundValue,
				double								maxBackgroundValue);

	// Dialog Data
		//{{AFX_DATA (CMReformatRectifyDlg)
		enum { IDD = IDD_ReformatRectify };
	
		double								m_backgroundValue,
												m_columnScaleFactor,
												m_lineScaleFactor,
												m_rotationAngle;
	
		long									m_columnShift,
												m_lineShift;
	
		int									m_channelSelection,
												m_fileNamesSelection,
												m_headerListSelection,
												m_procedureCode,
												m_resampleSelection;
	
		BOOL									m_blankOutsideSelectedAreaFlag,
												m_useMapOrientationAngleFlag;
		//}}AFX_DATA


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		void CMReformatRectifyDlg::UpdateProcedureItems (
				int									selectItemNumber,
				Boolean								blankOutsideSelectedAreaFlag);

		// Generated message map functions
		//{{AFX_MSG (CMReformatRectifyDlg)
		afx_msg void OnBnClickedReprojectToRadio ();
	
		afx_msg void OnBnClickedTranslateScaleRotateRadio ();
	
		afx_msg void OnBnClickedUsemaporientationangle ();
	
		afx_msg void OnCbnSelendokResamplemethod ();
	
		afx_msg void OnCbnSelendokTargetcombo ();
	
		afx_msg void OnEnChangeRotationclockwise ();
	
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	

		FileInfoPtr							m_fileInfoPtr,
												m_outputFileInfoPtr;
	
		LayerInfoPtr						m_imageLayerInfoPtr;
		ReformatOptionsPtr				m_reformatOptionsPtr;
		WindowInfoPtr						m_imageWindowInfoPtr;

		double								m_mapOrientationAngle,
												m_maxBackgroundValue,
												m_minBackgroundValue;

		Handle								m_referenceWindowInfoHandle;

		SInt16								m_headerOptionsSelection,
												m_resampleMethodCode;
		
		Boolean								m_initializedFlag;

};	// end class CMReformatRectifyDlg
