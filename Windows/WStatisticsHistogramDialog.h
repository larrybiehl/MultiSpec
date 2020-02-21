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
//	File:						WStatisticsHistogramDialog.h
//	Implementation:		WStatisticsHistogramDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatHistogramSpecsDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMStatHistogramSpecsDlg : public CMDialog
{
	// Construction
	public:
		CMStatHistogramSpecsDlg (	// standard constructor
				CWnd* 								pParent = NULL);

		~CMStatHistogramSpecsDlg (void);
	
		Boolean DoDialog (
				StatHistogramSpecsPtr			statHistogramSpecsPtr,
				FileInfoPtr							fileInfoPtr);

	// Dialog Data
		//{{AFX_DATA (CMStatHistogramSpecsDlg)
		enum { IDD = IDD_StatHistogram };
	
		int									m_channelsClassesCode,
												m_histogramClassFieldCode,
												m_listPlotCode,
												m_matrixColumnCode;
	
		long									m_columnInterval,
												m_lineInterval;
	
		BOOL									m_blankValuesFlag,
												m_featureTransformationFlag,
												m_includeEmptyBinsFlag,
												m_overlayDensityFunctionFlag;
		//}}AFX_DATA


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMStatHistogramSpecsDlg)
		afx_msg void OnClassesRadio ();
	
		afx_msg void OnFeatureTransformation ();
	
		afx_msg void OnFieldsRadio ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnListRadio ();
	
		afx_msg void OnPlotRadio ();
	
		afx_msg void OnSelendokChannelCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		StatHistogramSpecsPtr 			m_statHistogramSpecsPtr;
		FileInfoPtr							m_fileInfoPtr;

		SInt16								m_savedMatrixColumnCode;

		Boolean								m_featureTransformAllowedFlag,
												m_overlayDFAllowedFlag;

};	// end class CMStatHistogramSpecsDlg
