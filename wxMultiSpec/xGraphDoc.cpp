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
//	File:						xGraphDoc.cpp : class implementation file
//	Class Definition:		xGraphDoc.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/21/2020
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMGraphDoc class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xGraphDoc:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xGraphDoc.h"


IMPLEMENT_DYNAMIC_CLASS(CMGraphDoc, wxDocument)
   
CMGraphDoc::CMGraphDoc ()

{
	m_graphViewCPtr = NULL;
	m_graphFrameCPtr = NULL;
	
}	// end "CMGraphDoc"



CMGraphDoc::~CMGraphDoc()

{

} 	// end "~CMGraphDoc"



bool CMGraphDoc::OnNewDocument ()

{
	if (!wxDocument::OnNewDocument ())
																							return FALSE;

	SetTitle (wxT("Graph Window"));

	return TRUE;
	
}	// end "OnNewDocument"



CMGraphView* CMGraphDoc::GetGraphViewCPtr (void)

{  
	CMGraphView*						graphViewCPtr = NULL;
	

	graphViewCPtr = m_graphViewCPtr;
		
	return (graphViewCPtr);
	
}	// end "GetGraphViewCPtr"



void CMGraphDoc::SetGraphFrameCPtr (
				CMGraphFrame*						graphFrameCPtr)
				
{                                   
	m_graphFrameCPtr = graphFrameCPtr; 
	 
}	// end "SetGraphFrameCPtr"



void CMGraphDoc::SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr)
				
{
	m_graphViewCPtr = graphViewCPtr;

		
	if (m_graphFrameCPtr != NULL)
		m_graphFrameCPtr->SetGraphViewCPtr (m_graphViewCPtr);

}	// end "SetGraphViewCPtr" 

