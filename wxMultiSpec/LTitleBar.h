//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CMTitleBar.h
//	Implementation:		CMTitleBar.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: 03/19/2019
//	Revised By:				Larry L. Biehl			Date: 03/20/2019
//	
//------------------------------------------------------------------------------------

#if !defined __LTITLEBAR_H__
	#define __LTITLEBAR_H__

	#include "wx/wx.h"
	#include "wx/statline.h"

	class CMTitleBar : public wxPanel
		{
		DECLARE_DYNAMIC_CLASS (CMTitleBar)
		public:
			CMTitleBar ();
			
			CMTitleBar (
					wxWindow*					parent);
		
			virtual ~CMTitleBar ();

			void InitialUpdate ();

			 // Implementation
		private:
			void OnPaint (wxPaintEvent& event);
		DECLARE_EVENT_TABLE ()

		public:
			CMImageFrame*		m_frame;
			CMImageView*		m_view;

		};

#endif	// !defined __LTITLEBAR_H__
