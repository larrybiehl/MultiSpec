//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsView.h
//	Implementation:		LStatisticsView.cpp
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
//
#ifndef __LSTATVEW_H__
	#define __LSTATVEW_H__   

#include "LStatisticsDoc.h"

typedef bool BOOL;
typedef long unsigned int UINT;


class CMStatisticsView : public wxView
	{
   DECLARE_DYNAMIC_CLASS (CMStatisticsView);

	public:
		CMStatisticsView (); // protected constructor used by dynamic creation
		~CMStatisticsView ();
		virtual void OnInitialUpdate (); // first time after construct

		bool TransferDataToWindow();
		bool TransferDataFromWindow();
		CMStatisticsFrame *m_frame;

		enum { IDD = IDD_StatisticsWindow };

		int m_classList;

		BOOL m_polygonFlag;

		virtual  void OnDraw(CDC* pDC); // overridden to draw this view

		CMStatisticsDoc* GetDocument(void);

	protected:
		bool OnClose (bool deleteWindow = true);
		void virtual OnActivateView (
				bool				bActivate,
				wxView*			pActivateView,
				wxView*			pDeactiveView);
	
		bool OnCreate (wxDocument *doc, long WXUNUSED(flags));

		Boolean m_initializedFlag;
	
	private:
		 DECLARE_EVENT_TABLE ()
	
	};
#endif	// !defined __LSTATVEW_H__
