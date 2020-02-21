//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
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

