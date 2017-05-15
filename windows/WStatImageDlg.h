#if !defined __WSTATIMGDLG_H__
#define	__WSTATIMGDLG_H__     

#include "WDialog.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMStatImageDialog : public CMDialog {

public:
	CMStatImageDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMStatImageDialog();

	BOOL DoDialog();

	enum { IDD = IDS_ProcessorCreateImageStat};

	BOOL				m_classCode;
	SInt16			m_areaCode;
	BOOL				m_channelSelection;
	BOOL				m_featureTransformationFlag;
	SInt16			m_maximumNumberChannels;
	BOOL				m_classSelection;
	BOOL				m_perClassCode;
	BOOL				m_perFieldCode;
	BOOL				m_overallMinMaxCode;
	BOOL				m_individualMinMaxCode;
	BOOL				m_userMinMaxCode;
	SInt16			m_MinMaxCode;
	double			m_userMinimum;
	double			m_userMaximum;

	SInt16*			m_featurePtr;
	SInt16*			m_channelsPtr;
	SInt16*			m_classPtr;

	Boolean			m_channelsAllAvailableFlag;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	Boolean		m_initializedFlag;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedUsersettingradio();
};


#endif // !defined __WSTATIMGDLG_H__
