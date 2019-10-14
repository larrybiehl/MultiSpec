//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsDoc.h
//	Implementation:		LStatisticsDoc.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDisplaySpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------
//
#if !defined __LSTATDOC_H__
	#define __LSTATDOC_H__

#include "LStatisticsFrame.h"
#include "wx/docview.h"

typedef bool BOOL;


class CMStatisticsDoc : public wxDocument
	{
	DECLARE_DYNAMIC_CLASS (CMStatisticsDoc)

	public:
		CMStatisticsDoc (); // protected constructor used by dynamic creation
		~CMStatisticsDoc ();
	
		CMStatisticsFrame* GetStatisticsFrame (void);
		void SetStatisticsFrame (CMStatisticsFrame* statisticsFrameCPtr);
		virtual bool Close ();

		// Implementation
	protected:
		BOOL OnNewDocument ();

				// Generated message map functions

		DECLARE_EVENT_TABLE ()

		CMStatisticsFrame* m_statisticsFrameCPtr;
	
	};
#endif	// !defined __LSTATDOC_H__
