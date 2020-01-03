//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xStatisticsDoc.h
//	Implementation:		xStatisticsDoc.cpp
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

#pragma once

#include "xStatisticsFrame.h"
#include "wx/docview.h"


class CMStatisticsDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS (CMStatisticsDoc)

	public:
		CMStatisticsDoc ();
	
		~CMStatisticsDoc ();
	
		CMStatisticsFrame* GetStatisticsFrame (void);
	
		void SetStatisticsFrame (
				CMStatisticsFrame* 				statisticsFrameCPtr);
	
		virtual bool Close ();


		// Implementation
	protected:
		bool OnNewDocument ();
		DECLARE_EVENT_TABLE ()

		CMStatisticsFrame* 				m_statisticsFrameCPtr;
	
};	// end "class CMStatisticsDoc"
