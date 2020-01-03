//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WGraphDoc.h
//	Implementation:		WGraphDoc.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMGraphDoc class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
                          
#include "WGraphFrame.h"
   

class CMGraphDoc : public CDocument
{
	DECLARE_DYNCREATE(CMGraphDoc)
	protected:
		CMGraphDoc ();			// protected constructor used by dynamic creation
	
		virtual ~CMGraphDoc ();
	
		virtual BOOL OnNewDocument ();

		// Generated message map functions
		//{{AFX_MSG (CMGraphDoc)
			// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CMGraphFrame*						m_graphFrameCPtr;
		CMGraphView*						m_graphViewCPtr;
	
	public:
		CMGraphFrame* GetGraphFrameCPtr () { return m_graphFrameCPtr; }
	
		CMGraphView* GetGraphViewCPtr (void);
	
		void SetGraphFrameCPtr (
				CMGraphFrame*						graphFrameCPtr);
	
		void SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr);
	
};	// end class CMGraphDoc

