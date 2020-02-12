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
//	File:						WListDataDialog.h
//	Implementation:		WListDataDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMListDataDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 07/03/2018
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMListDataDialog : public CMDialog
{
	// Construction
	public:
		CMListDataDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMListDataDialog ();		// standard desctructor
	
		SInt16 DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMListDataDialog)
		enum { IDD = IDD_ListData };
	
		int									m_listDataFormatCode;
	
		long									m_numberDecimalPlaces;
	
		BOOL									m_areaFlag,
												m_classFlag,
												m_diskFileFlag,
												m_graphDataFlag,
												m_includeClassFieldFlag,
												m_includeLatLongFlag,
												m_includeLineColumnFlag,
												m_localGraphDataFlag,
												m_testFlag,
												m_textWindowFlag,
												m_trainingFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		void CheckClassItems (
				Boolean								listClassDataFlag);
	
		void CheckOKButton (void);
	
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMListDataDialog)
		afx_msg void OnArea ();
	
		afx_msg void OnCbnSelendokListchannelsformatcombo ();
	
		afx_msg void OnClasses ();
	
		afx_msg void OnEnChangeEdit1 ();
	
		afx_msg void OnGraphData ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()


		Boolean 								m_initializedFlag,
												m_latLongPossibleFlag;
	
};	// end class CMListDataDialog
