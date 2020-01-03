//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
