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
//	File:						WTools.h
//	Implementation:		WTools.h
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMTool and CMSelectTool classes
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once

class CMImageView;

enum ToolType
{
	selection,
	line,
	rect,
	poly
};


class CMTool
{
			// Constructors
	public:
		CMTool (
				ToolType 							toolType);

		static CMTool* FindTool (
				ToolType 							toolType);

		virtual void OnChar (
				CMImageView* 						pView,
				UINT									nChar);
	
		virtual void OnLButtonDblClk (
				CMImageView* 						pView,
				UInt16 								nFlags);

		virtual void OnLButtonDown (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const CPoint& 						point);
	
		virtual void OnLButtonUp (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const CPoint& 						point,
				Boolean								newSelectedWindowFlag);
								
		virtual Boolean OnMouseMove (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const CPoint& 						point);
	
	
		ToolType 							mToolType;
	
		static CPoint 						c_down,
			 									c_last,
			 									c_lastMousePoint;
	
		static CPtrList 					c_tools;
	
		static ToolType 					c_toolType;
	
				// Indicates whether the select type is rectangular or polygon.
		static SInt16						s_selectType;
	
		static Boolean						c_nonClientAreaFlag;
	
};	// end class CMTool


enum SelectMode
	{
	none,
	netSelect,
	move,
	size
	};


class CMSelectTool : public CMTool
{
	public:
		CMSelectTool (void);
			
		virtual void OnLButtonDown (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const CPoint& 						point);
									
		virtual void OnLButtonUp (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const CPoint& 						point,
				Boolean								newSelectedWindowFlag);
									
		virtual Boolean OnMouseMove (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const CPoint& 						point);

		virtual void OnLButtonDblClk (
				CMImageView* 						pView,
				UInt16 								nFlags);

		virtual void OnChar (
				CMImageView* 						pView,
				UINT									nChar);
	

		static LCToWindowUnitsVariables	s_lcToWindowUnitsVariables;
	
		static Rect							s_viewRect;

		static Boolean						s_initPolygonFlag;
	
	
	private:
		void AddPolygonPoint (
				CMImageView* 						imageViewCPtr,
				LongPoint							newLineColPoint);

		Boolean InitPolygon ();
	
		
		static LOGPEN 						s_logpen;
		                        	              
		static LongPoint					s_lastLineColumnPoint,
												s_startPoint;
		
		static Rect							s_limitRectangle;

		static SelectMode 				s_selectMode;
		
		static DisplaySpecsPtr			s_displaySpecsPtr;

		static HPFieldPointsPtr			s_selectionPointsPtr;
		
		static MapProjectionInfoPtr	s_mapProjectionInfoPtr;

		static SelectionInfoPtr			s_selectionInfoPtr;
	
		static double						s_hConversionFactor,
												s_vConversionFactor;

		static SInt32						s_prevColumnEnd,
												s_prevColumnStart,
												s_prevLineEnd,
												s_prevLineStart;

		static UInt32						s_bytesNeeded,
												s_bytesNeededIncrement,
												s_memoryLimitNumber;
	
		static UInt16						s_startChannel;
	
};	// end class CMSelectTool
