// WStatisticsImageDialog.h

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

	enum { IDD = IDD_ProcessorCreateImageStat};

	int				m_classCode;
	int				m_areaCode;
	//BOOL				m_channelSelection;
	BOOL				m_featureTransformationFlag;
	SInt16			m_maximumNumberChannels;
	BOOL				m_classSelection;
	int				m_perClassCode;
	int				m_perFieldCode;
	int				m_overallMinMaxCode;
	int				m_individualMinMaxCode;
	int				m_userMinMaxCode;
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

	afx_msg void OnSelendokClassCombo();
	afx_msg void OnSelendokChannelCombo();
	afx_msg void OnClickUserSettingRadio();
	afx_msg void OnClickIndividualRadio();
	afx_msg void OnClickOverallRadio();
	afx_msg void OnCbnSelchangeChannelcombo();
	afx_msg void OnFeatureTransformation();

	DECLARE_MESSAGE_MAP()

	
//public:
	//afx_msg void OnCbnSelchangeClasscombo2();
};


#endif // !defined __WSTATIMGDLG_H__
