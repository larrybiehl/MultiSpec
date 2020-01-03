//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WHistogramDialog.h
//	Implementation:		WHistogramDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMGraphFrame class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

	     
#include "WDialog.h"

class CMHistogramSpecsDlg : public CMDialog
{
	// Construction
	public:
		CMHistogramSpecsDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMHistogramSpecsDlg (void);		// standard destructor
	
		Boolean DoDialog (
				HistogramSpecsPtr					histogramSpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr);

		// Dialog Data
		//{{AFX_DATA (CMHistogramSpecsDlg)
		enum { IDD = IDD_HistogramSpecs };
	
		CString								m_fileName,
												m_supportFileName;
	
		int									m_columnsLinesFormat,
												m_histogramMethod;
	
		BOOL									m_computeOnlyMinMaxFlag,
												m_diskFileFlag,
												m_includeEmptyBinsFlag,
												m_listHistogramValuesFlag,
												m_listHistogramSummaryFlag,
												m_textWindowFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG (CMHistogramSpecsDlg)
		afx_msg void OnComputeOnly ();
	
		afx_msg void OnDiskFile ();
	
		afx_msg void OnEmptyBins ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnListHistogram ();
	
		afx_msg void OnSelendokChannels ();
	
		afx_msg void OnSelendokColumnLineFormat ();
	
		afx_msg void OnSelendokMethod ();
	
		afx_msg void OnTextWindow ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		void CheckWriteResultsToSettings (void);
	
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void SetListAndEmptyBinsDialogBoxes (
				DialogPtr							dialogPtr,
				Boolean								minMaxetcOnlyFlag,
				Boolean								listHistogramFlag,
				Boolean								includeEmptyBinsFlag,
				Boolean								lineFormatHistFlag);
	
		void ShowAreaHistogramItems (void);
	
		Boolean UpdateAllChannelsAtOnceFlag (
				DialogPtr							dialogPtr,
				SInt16								numberChannels);

		void UpdateListAndEmptyBinsDialogBoxes (void);
	
	
		FileInfoPtr							m_fileInfoPtr;
		HistogramSpecsPtr 				m_histogramSpecsPtr;
		WindowInfoPtr						m_windowInfoPtr;
	
		SInt16								m_histogramMethodCode;
	
		Boolean								m_allChannelsAtOnceFlag,
												m_defaultStatFileChangedFlag,
												m_initializedFlag,
												m_lineFormatOnlyFlag,
												m_updateListHistogramItemsFlag;
	
};	// end class CMHistogramSpecsDlg
