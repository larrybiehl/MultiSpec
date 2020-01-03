//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WStatisticsDoc.h
//	Implementation:		WStatisticsDoc.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatisticsDoc class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WStatisticsFrame.h"


class CMStatisticsDoc : public CDocument
{
	DECLARE_DYNCREATE (CMStatisticsDoc)

	protected:
		CMStatisticsDoc ();			// protected constructor used by dynamic creation
	
		virtual ~CMStatisticsDoc ();
	
		virtual BOOL OnNewDocument ();
	
		//{{AFX_MSG (CMStatisticsDoc)
			// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CMStatisticsFrame*				m_statisticsFrameCPtr;
	
	
	public:
		CMStatisticsFrame* GetStatisticsFrame (void);
	
		void SetStatisticsFrame (
				CMStatisticsFrame*				statisticsFrameCPtr);
	
};	// end class CMStatisticsDoc
