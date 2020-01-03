//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xStatisticsView.h
//	Implementation:		xStatisticsView.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMStatisticsView class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xStatisticsDoc.h"


class CMStatisticsView : public wxView
{
   DECLARE_DYNAMIC_CLASS (CMStatisticsView);

	public:
		CMStatisticsView ();
	
		~CMStatisticsView ();

		CMStatisticsDoc* GetDocument (void);
	
		virtual void OnInitialUpdate (); // first time after construct
	

		enum { IDD = IDD_StatisticsWindow };
	
		CMStatisticsFrame*				m_frame;

		int 									m_classList;

		bool 									m_polygonFlag;

	protected:
		void virtual OnActivateView (
				bool									bActivate,
				wxView*								pActivateView,
				wxView*								pDeactiveView);
	
		bool OnClose (
				bool 									deleteWindow = true);
	
		bool OnCreate (
				wxDocument*							doc,
				long 									WXUNUSED (flags));

		virtual void OnDraw (CDC* pDC); // overridden to draw this view

		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();

		Boolean 								m_initializedFlag;
	
	private:
		 DECLARE_EVENT_TABLE ()
	
};	// end "class CMStatisticsView"
