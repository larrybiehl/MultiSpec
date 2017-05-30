// WBiPlotDialog.h : header file
//         
// Revised by Larry Biehl on 05/22/2017
//
#if !defined __WBIPLOTDLG_H__
#define	__WBIPLOTDLG_H__  

#include "WDialog.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CMBiPlotDialog dialog

class CMBiPlotDialog : public CMDialog
{
	//DECLARE_DYNAMIC(CMBiPlotDialog)

public:
	CMBiPlotDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMBiPlotDialog();

	BOOL DoDialog(void);

// Dialog Data
	enum { IDD = IDD_BiPlot };

	float*		m_classWeightsPtr;

	double		m_thresholdPercent,
					m_saveThresholdPercent;

	int			m_classWeightsSelection;

	UINT			m_maxChannelFeatureNum,
					m_newXAxisFeature,
					m_newYAxisFeature;

	SInt16		m_displayPixelCode,
					m_entireIconItem,
					m_outlineClassCode,
					m_plotDataCode;

	BOOL			m_checkChannelStatisticsFlag,
					m_checkClassesPopUpFlag,
					m_checkFeatureTransformFlag,
					m_createNewGraphWindowFlag,
					m_featureTransformAllowedFlag,
					m_featureTransformationFlag,
					m_createNewGraphicsWindowFlag,
					m_thresholdFlag,
					m_trainingAreaFlag,
					m_testFlag,
					m_imageAreaFlag;

	BOOL			mb_displayPixelCode,
					mb_outlineClassCode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	Boolean		m_initializedFlag;
	

	afx_msg void OnChangeVerticalAxis();
	afx_msg void OnChangeHorizontalAxis();
	afx_msg void OnFeatureTranformation();
	afx_msg void OnCreateNewWindowFlag();
	afx_msg void OnTestArea();
	afx_msg void OnTrainArea();
	afx_msg void OnImageArea();
	afx_msg void OnThresholdPixelFlag();
	afx_msg void OnDisplayPixelAsSymbol();
	afx_msg void OnOutlineClassAsEllipse();
	afx_msg void OnSelendokClassWeightsCombo();
	afx_msg void OnSelendokClassCombo();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeHorizontalchannel();
};

#endif // !defined __WBIPLOTDLG_H__
