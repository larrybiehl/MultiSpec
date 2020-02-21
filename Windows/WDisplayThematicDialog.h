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
//	File:						WDisplayThematicDialog.h
//	Implementation:		WDisplayThematicDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMDisplayThematicDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 11/07/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMDisplayThematicDlg : public CMDialog
{
	// Construction
	public:
		CMDisplayThematicDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMDisplayThematicDlg ();		// standard desctructor
	
		Boolean DoDialog (
				DisplaySpecsPtr					displaySpecsPtr);

		// Dialog Data
		//{{AFX_DATA (CMDisplayThematicDlg)
		enum { IDD = IDD_DisplayThematic };
	
		CString								m_fileName,
												m_numberClasses,
												m_numberGroups;
	
		double								m_magnification;
	
		int									m_classGroupSelection,
												m_classSelection,
												m_paletteSelection;
	
		BOOL									m_includeVectorOverlaysFlag,
												m_showLegendFlag,
												m_useThresholdFileFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		virtual BOOL OnInitDialog ();

		// Generated message map functions
		//{{AFX_MSG (CMDisplayThematicDlg)
		afx_msg void OnBackgroundColor ();
	
		afx_msg void OnBnClickedCheck1 ();
	
		afx_msg void OnDropdownPaletteCombo ();
	
		afx_msg void OnPaint ();
	
		afx_msg void OnSelendokClassesGroupsCombo ();
	
		afx_msg void OnSelendokPaletteCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		RGBColor								m_oldRGB;
	
		DisplaySpecsPtr					m_displaySpecsPtr;
	
		SInt16								*m_localClassGroupsPtr,
												*m_localClassToGroupPtr;
	
		Handle								m_localClassGroupsHandle;
	
		UInt32								m_localDisplayClasses,
												m_localDisplayGroups;
	
		SInt16								m_localAllSubsetClass,
												m_localAllSubsetGroup,
												m_localNumberGroups;
	
		Boolean								m_initializedFlag,
												m_localPaletteUpToDateFlag,
												m_paletteChangedFlag;
	
};	// end class CMDisplayThematicDlg
