//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WListBox.h
//	Implementation:		WListBox.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMLegendList class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once


class CMListBox : public CListBox
{
	// Construction
	public:
		CMListBox ();
		
		virtual ~CMListBox ();

	protected:
		// Generated message map functions
		//{{AFX_MSG (CMListBox)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
};	// end CMListBox
