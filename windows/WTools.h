//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
