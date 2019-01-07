//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LTools.h
//	Implementation:		LTools.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/19/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file is the definition for the CMTool class
//
//------------------------------------------------------------------------------------

#ifndef __MTOOLS_H__
#define __MTOOLS_H__

#include "wx/list.h"
#include "wx/wx.h"
//#include "wx/kbdstate.h"
#include "SDefines.h"
//	#include "drawobj.h"
typedef wxPoint CPoint;
typedef wxRect CRect;
//typedef unsigned int UINT;
class CMImageView;
class CPtrList;

enum ToolType {
    selection,
    line,
    rect,
    poly
};
//WX_DECLARE_LIST(CMTool,CPtrList);

class CMTool {
    // Constructors
public:
    CMTool(ToolType toolType);

    // Overridables

    virtual void OnCharHook ();

    virtual void OnLButtonDown(
            CMImageView* pView,
            UInt16 nFlags,
            const CPoint point);

    virtual void OnLButtonDblClk(
            CMImageView* pView,
            UInt16 nFlags);

    virtual void OnLButtonUp(
            CMImageView* pView,
            UInt16 nFlags,
            const CPoint point,
            Boolean newSelectedWindowFlag);

    virtual Boolean OnMouseMove(
            CMImageView* pView,
            UInt16 nFlags,
            const CPoint point);
	/*
	virtual void OnChar(
            CMImageView* pView,
            unsigned int nChar);
	*/
    /*UINT SetTimer(
        HWND hWnd,	// handle of window for timer messages
        UINT nIDEvent,	// timer identifier
        UINT uElapse,	// time-out value
        TIMERPROC lpTimerFunc 	// address of timer procedure
       );	virtual void		SetTimer;
     */ // Attributes

    ToolType mToolType;

    static CMTool* FindTool(ToolType toolType);
    static CPtrList c_tools;
    static CPoint c_down;
    static CPoint c_last;
    static CPoint c_lastMousePoint;
    //	static UInt16 		c_nDownFlags;
    static ToolType c_toolType;
    static Boolean c_nonClientAreaFlag;

    // Indicates whether the select type is rectangular or polygon.

    static SInt16 s_selectType;
    static bool c_flag_addtolist;

};

WX_DECLARE_LIST(CMTool,CPtrList);

enum SelectMode {
    none,
    netSelect,
    move,
    size
};

class CMSelectTool : public CMTool {
    // Constructors

public:

    CMSelectTool(void);

    virtual void OnLButtonDown(
            CMImageView* pView,
            UInt16 nFlags,
            const CPoint point);

    virtual void OnLButtonUp(
            CMImageView* pView,
            UInt16 nFlags,
            const CPoint point,
            Boolean newSelectedWindowFlag);

    virtual Boolean OnMouseMove(
            CMImageView* pView,
            UInt16 nFlags,
            const CPoint point);

    virtual void OnLButtonDblClk(
            CMImageView* pView,
            UInt16 nFlags);
	/*
    virtual void OnChar(
            CMImageView* pView,
            unsigned int nChar);
	*/
    static Rect s_viewRect;

    static Boolean s_initPolygonFlag;

    static LCToWindowUnitsVariables s_lcToWindowUnitsVariables;
private:
    /*		UInt16						GetSelectionRectangleLimits (
                                                                                            Boolean					firstTimeFlag,
                                                                                            UInt16					startChannel,
                                                                                            LongPoint				startPt);
     */
    void AddPolygonPoint(
            CMImageView* imageViewCPtr,
            //											Point						newOffscreenPoint,
            LongPoint newLineColPoint);


    Boolean InitPolygon();

    //wxKeyboardState kbdstate;

    //static LOGPEN s_logpen;

    static LongPoint s_lastLineColumnPoint;
    static LongPoint s_startPoint;

    static Rect s_limitRectangle;

    static SelectMode s_selectMode;

    static double s_hConversionFactor,
    s_vConversionFactor;

    static DisplaySpecsPtr s_displaySpecsPtr;

    static HPFieldPointsPtr s_selectionPointsPtr;

    static MapProjectionInfoPtr s_mapProjectionInfoPtr;

    static SelectionInfoPtr s_selectionInfoPtr;

    static SInt32 s_prevLineStart;
    static SInt32 s_prevColumnStart;
    static SInt32 s_prevLineEnd;
    static SInt32 s_prevColumnEnd;

    static UInt32 s_memoryLimitNumber,
    s_bytesNeededIncrement,
    s_bytesNeeded;

    static UInt16 s_startChannel;

}; // end "class CMSelectTool"

////////////////////////////////////////////////////////////////////////////

#endif // __MTOOLS_H__
