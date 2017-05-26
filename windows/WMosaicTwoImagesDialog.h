// WMosaicTwoImagesDialog.h : header file
//
#pragma once

#include "WDialog.h"


// CMMosaicTwoImagesDialog dialog

class CMMosaicTwoImagesDialog : public CMDialog
{
public:
	CMMosaicTwoImagesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMMosaicTwoImagesDialog();

	Boolean DoDialog(
				FileInfoPtr							outFileInfoPtr,
				FileInfoPtr							fileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);

// Dialog Data
	enum {IDD = IDD_Mosaic};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMMosaicTwoImagesDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CheckRightImageLines(
			UInt16							itemSelected);
	void CheckBottomImageColumns(
			UInt16							itemSelected);

	FileInfoPtr				m_fileInfoPtr;
	FileInfoPtr				m_outputFileInfoPtr;
	ReformatOptionsPtr	m_reformatOptionsPtr;

	SInt16					m_headerOptionsSelection;
	
	Boolean					m_initializedFlag;

	Handle					m_rightBottomWindowInfoHandle;

public:
	DialogSelectArea		m_rightBottomDialogSelectArea;

	long						m_backgroundValue;
	long						m_maxDataValue;

	int						m_headerListSelection;
	int						m_mosaicDirectionCode;
	int						m_rightFileNameSelection;
	int						m_bottomFileNameSelection;

	BOOL						m_ignoreBackgroundValueFlag;
	BOOL						m_writeChannelDescriptionsFlag;

	afx_msg void			OnCbnSelchangeMosaicdirection();
	afx_msg void			OnCbnSelchangeHeaderformatlist();
	afx_msg void			OnCbnSelchangeRightimagefilelist();
	afx_msg void			OnCbnSelchangeBottomimagefilelist();
	afx_msg void			OnEnChangeLeftTopLineStart();
	afx_msg void			OnEnChangeLeftTopLineEnd();
	afx_msg void			OnEnChangeLeftTopColumnStart();
	afx_msg void			OnEnChangeLeftTopColumnEnd();
	afx_msg void			OnEnChangeRightLineStart2();
	afx_msg void			OnEnChangeBottomColumnStart3();
	afx_msg void			OnBnClickedIgnoreBackgroundValue();
};
