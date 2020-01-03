//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WImageSplitterWnd.h
//	Implementation:		WImageSplitterWnd.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMImagDoc class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once  

#ifndef __AFXEXT_H__
	#include <afxext.h>
#endif         
                     
class CMImageSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC (CMImageSplitterWnd)
	
	public:
		CMImageSplitterWnd () { }
	
		void set_cxBorder (int cxBorder) { m_cxBorder = cxBorder; }
	
		void set_cxSplitter (int cxSplitter) { m_cxSplitter = cxSplitter; }
	
		void set_cxSplitterGap (int cxSplitterGap) { m_cxSplitterGap = cxSplitterGap; }

	protected:
		// Generated message map functions
		//{{AFX_MSG (CMImageFrame)
		afx_msg LRESULT OnNcHitTest (
				CPoint 								point);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

};	// end class CMImageSplitterWnd
