//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xTitleBar.h
//	Implementation:		xTitleBar.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMTitleBar class
//
//	Written By:				Larry L. Biehl			Date: 03/19/2019
//	Revised By:				Larry L. Biehl			Date: 03/20/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "wx/wx.h"
#include "wx/statline.h"

class CMTitleBar : public wxPanel
{
	DECLARE_DYNAMIC_CLASS (CMTitleBar)
	
	public:
		CMTitleBar ();
		
		CMTitleBar (
				wxWindow*							parent);
	
		virtual ~CMTitleBar ();

		void InitialUpdate ();
	
	
		CMImageFrame						*m_frame;
	
		CMImageView							*m_view;


		 // Implementation
	private:
		void OnPaint (
				wxPaintEvent& 						event);
		DECLARE_EVENT_TABLE ()

};	// end "class CMTitleBar"
