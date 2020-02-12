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
