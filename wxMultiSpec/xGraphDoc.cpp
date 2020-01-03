//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xGraphDoc.cpp : class implementation file
//	Class Definition:		xGraphDoc.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2018
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

	SetTitle ("Graph Window");

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

