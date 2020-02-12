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
//	File:						WStatisticsView.h
//	Implementation:		WStatisticsView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatisticsForm class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once

#ifndef __AFXEXT_H__
	#include <afxext.h>
#endif         

#include "WStatisticsDoc.h"
#include "WListBox.h"


class CMStatisticsForm : public CFormView
{
		DECLARE_DYNCREATE (CMStatisticsForm)
	protected:
		CMStatisticsForm ();			// protected constructor used by dynamic creation
	
		virtual ~CMStatisticsForm ();
	
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
				
		virtual void OnInitialUpdate ();	// first time after construct
	
		// Generated message map functions
		//{{AFX_MSG (CMStatisticsForm)
		afx_msg void OnAddToList ();
	
		afx_msg void OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnClass ();
	
		afx_msg void OnDblclkListBox ();
	
		afx_msg void OnDestroy ();
	
		afx_msg void OnDropdownStatsTypeCombo ();
	
		afx_msg void OnEditName ();
	
		afx_msg void OnField ();
	
		afx_msg void OnHistogram ();
	
		afx_msg void OnList ();
	
		afx_msg void OnPolygon ();
	
		afx_msg void OnProject ();
	
		afx_msg void OnSelchangeListBox ();
	
		afx_msg void OnSelect ();
	
		afx_msg void OnSelendokClassList ();
	
		afx_msg void OnSelendokHistogramStatsCombo ();
	
		afx_msg void OnSelendokListStatsCombo ();
	
		afx_msg void OnSelendokStatsCombo ();
		
		afx_msg void OnUpdate ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CDataExchange						*m_dialogFromPtr,
												*m_dialogToPtr;
	
		Boolean								m_initializedFlag,
												m_optionKeyFlag;
	
	// Form Data
	public:
		CMStatisticsDoc* GetDocument (void);
	
		virtual void OnDraw (
				CDC* 									pDC);  // overridden to draw this view
	
		void UpdateHistogramStatsCombo (void);
	
		void UpdateListStatsCombo (void);
	
		void UpdateStatsTypeCombo (
				SInt16								statsWindowMode);
	

		//{{AFX_DATA (CMStatisticsForm)
		enum { IDD = IDD_StatisticsWindow };
	
		CMListBox							m_listBox;
	
		int									m_classList,
												m_histogramStatsCode,
												m_listStatsCode,
												m_statsTypeCode;
	
		BOOL     							m_polygonFlag;
		//}}AFX_DATA
	
};	// end class CMStatisticsForm
