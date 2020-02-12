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
//	File:						xTools.h
//	Implementation:		xTools.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/24/2019
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file is the definition for the CMTool class
//
//------------------------------------------------------------------------------------

#pragma once

#include "wx/list.h"
#include "wx/wx.h"
#include "SDefines.h"
class CMImageView;
class CPtrList;

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
		CMTool (ToolType toolType);
		
		~CMTool ();

		Boolean OnCharHook ();

				// Overridables

		virtual void OnLButtonDown (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const wxPoint 						point);

		virtual void OnLButtonDblClk (
				CMImageView* 						pView,
				UInt16 								nFlags);

		virtual void OnLButtonUp (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const wxPoint 						point,
				Boolean 								newSelectedWindowFlag);

		virtual Boolean OnMouseMove (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const wxPoint 						point,
				Boolean								updateViewRectFlag);

		static CMTool* FindTool (
				ToolType 							toolType);
	

		static CPtrList 					c_tools;
	
		ToolType 							mToolType;
		static ToolType 					c_toolType;
	
		static wxPoint 					c_down,
							 					c_last;

				// Indicates whether the select type is rectangular or polygon.
		static SInt16 						s_selectType;
	
		static bool 						c_flag_addtolist;
	
		static Boolean 					c_nonClientAreaFlag;

};	// end "class CMTool"


WX_DECLARE_LIST (CMTool, CPtrList);

enum SelectMode
	{
	none,
	netSelect,
	move,
	size
	
	};

class CMSelectTool : public CMTool
{
		 // Constructors
	public:
		 CMSelectTool (void);

		 virtual void OnLButtonDown (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const wxPoint 						point);

		 virtual void OnLButtonUp (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const wxPoint 						point,
				Boolean 								newSelectedWindowFlag);

		 virtual Boolean OnMouseMove (
				CMImageView* 						pView,
				UInt16 								nFlags,
				const wxPoint 						point,
				Boolean								updateViewRectFlag);

		 virtual void OnLButtonDblClk (
				CMImageView* 						pView,
				UInt16 								nFlags);

		 static LCToWindowUnitsVariables 	s_lcToWindowUnitsVariables;

		 static Rect 								s_viewRect;

		 static Boolean 							s_initPolygonFlag;
	
	
	private:
		 void AddPolygonPoint (
				CMImageView* 						imageViewCPtr,
				LongPoint 							newLineColPoint);

		 Boolean InitPolygon ();

	
		 static LongPoint 						s_lastLineColumnPoint,
		 												s_startPoint;

		 static Rect 								s_limitRectangle;
	
		 static DisplaySpecsPtr 				s_displaySpecsPtr;
		 static HPFieldPointsPtr 				s_selectionPointsPtr;
		 static MapProjectionInfoPtr 			s_mapProjectionInfoPtr;
		 static SelectionInfoPtr 				s_selectionInfoPtr;

		 static SelectMode 						s_selectMode;

		 static double 							s_hConversionFactor,
		 												s_vConversionFactor;

		 static SInt32 							s_prevColumnEnd,
		 												s_prevColumnStart,
		 												s_prevLineEnd,
		 												s_prevLineStart;

		 static UInt32 							s_bytesNeeded,
		 												s_bytesNeededIncrement,
		 												s_memoryLimitNumber;

		 static UInt16 							s_startChannel;
	
}; // end "class CMSelectTool"
