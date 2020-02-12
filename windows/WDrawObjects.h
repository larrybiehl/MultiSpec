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
//	File:						WDrawObjects.h
//	Implementation:		WDrawObjects.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMDrawObject and CMOutlineArea classes.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

class CMImageView;
class CMImageDoc;


//----------------------------------------------------------------------------
// CMDrawObject - base class for all 'drawable objects'

class CMDrawObject : public CObject
{
	public:
		CMDrawObject (
				const CRect& 						position);
	                          
		virtual ~CMDrawObject ();
				
		virtual void Draw (
				CDC* 									pDC,
				CMImageView*						imageViewCPtr);
		
		void Invalidate (void);
											
		virtual void OnOpen (
				CMImageView* 						pView);
		                                        		
		virtual void Remove ();
		
		#ifdef _DEBUG
			void AssertValid ();
		#endif       
	
	
		CMImageDoc* 						m_document;
		
		Boolean								m_drawFlag;
		
	protected:
		CMDrawObject ();
	
	
		static Rect							s_viewRect;
	
		CRect									m_position;
	
		LOGBRUSH 							m_logbrush;
		                       
		LOGPEN 								m_logpen;

};	// end class CMDrawObject


//----------------------------------------------------------------------------
// CMOutlineArea - class for outlining areas

class CMOutlineArea : public CMDrawObject
{
	public:                                 
		CMOutlineArea ();
										
		CMOutlineArea (
				const CRect& 						position);
											
		virtual ~CMOutlineArea ();
	
		void Clear (
				CMImageView*						imageViewCPtr);
				
		virtual void Draw (
				CDC* 									pDC,
				CMImageView*						imageViewCPtr);
	
		void Invalidate (
				CMImageView*						imageViewCPtr);

		#ifdef _DEBUG
			void AssertValid ();
		#endif
	
};	// end class CMOutlineArea
