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
//	File:						WToolBar.h
//	Implementation:		WToolBar.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMToolBar class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 03/27/2017
//
//------------------------------------------------------------------------------------

#pragma once


class CMToolBar : public CToolBar
{                              
	public:
		CMToolBar ();
	
		virtual ~CMToolBar ();

		UINT ConvertToID (
				CPoint 								point);
	
		void IsZoomPressed (
				CPoint 								point);


	protected:  // control bar embedded members
		void OnShowOverlay (
				UINT									menuID);

		//{{AFX_MSG (CToolBarCtrl)
		afx_msg void OnLButtonDblClk (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnLButtonDown (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnLButtonUp (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnRButtonDown (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnRButtonUp (
				UINT 									nFlags,
				CPoint 								point);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CMenu*								m_overlayMenu;
	
		SInt16								m_iButtonCapture;
	
		Boolean								m_rightButtonFlag,
												m_optionOverlayFlag;

};	// end class CMToolBar
