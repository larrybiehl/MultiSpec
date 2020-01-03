//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xGraphDoc.h
//	Implementation:		xGraphDoc.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMGraphDoc class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 08/23/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xGraphFrame.h"
#include "xGraphView.h"

#include "wx/docview.h"
#include "wx/cmdproc.h"


class CMGraphDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS (CMGraphDoc)
    
	public:
		CMGraphDoc ();   // protected constructor used by dynamic creation

		~CMGraphDoc ();
	
		CMGraphFrame* GetGraphFrameCPtr () { return m_graphFrameCPtr; }
	
		CMGraphView* GetGraphViewCPtr (void);

		bool OnNewDocument ();

		void SetGraphFrameCPtr (
				CMGraphFrame* 						graphFrameCPtr);
	
		void SetGraphViewCPtr (
				CMGraphView* 						graphViewCPtr);
	

		CMGraphFrame*   					m_graphFrameCPtr;
		CMGraphView*    					m_graphViewCPtr;
	
};	// end "class CMGraphDoc"
