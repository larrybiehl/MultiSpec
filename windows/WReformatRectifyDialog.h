// WReformatRectifyDialog.h : header file
//
#if !defined __WREFORMATRECTIFYDLG_H__
	#define	__WREFORMATRECTIFYDLG_H__             
	     
	#include "WDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CMReformatRectifyDlg dialog

class CMReformatRectifyDlg : public CMDialog
{
// Construction
public:
	CMReformatRectifyDlg(CWnd* pParent = NULL);   // standard constructor  
						
	Boolean			DoDialog(
				FileInfoPtr							outFileInfoPtr,
				FileInfoPtr							fileInfoPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				LayerInfoPtr						imageLayerInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				double								minBackgroundValue,
				double								maxBackgroundValue); 

// Dialog Data
	//{{AFX_DATA(CMReformatRectifyDlg)
	enum { IDD = IDD_ReformatRectify };
	double	m_backgroundValue;
	long		m_lineShift;
	long		m_columnShift;
	double	m_columnScaleFactor;
	double	m_lineScaleFactor;
	double	m_rotationAngle;
	BOOL		m_blankOutsideSelectedAreaFlag;
	int		m_headerListSelection;
	int		m_channelSelection;
	BOOL		m_useMapOrientationAngleFlag;
	int		m_procedureCode;
	int		m_fileNamesSelection;
	int		m_resampleSelection;
								
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMReformatRectifyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMReformatRectifyDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CMReformatRectifyDlg::UpdateProcedureItems(
						int									selectItemNumber,
						Boolean								blankOutsideSelectedAreaFlag);

	double					m_mapOrientationAngle;

	FileInfoPtr				m_fileInfoPtr;
	FileInfoPtr				m_outputFileInfoPtr;
	LayerInfoPtr			m_imageLayerInfoPtr;
	ReformatOptionsPtr	m_reformatOptionsPtr;
	WindowInfoPtr			m_imageWindowInfoPtr;

	SInt16					m_headerOptionsSelection,
								m_resampleMethodCode;
	
	Boolean					m_initializedFlag;

	double					m_maxBackgroundValue,
								m_minBackgroundValue;

	Handle					m_referenceWindowInfoHandle;

public:
	afx_msg void OnEnChangeRotationclockwise();
	afx_msg void OnBnClickedUsemaporientationangle();
	afx_msg void OnBnClickedTranslateScaleRotateRadio();
	afx_msg void OnBnClickedReprojectToRadio();
	afx_msg void OnCbnSelendokTargetcombo();
	afx_msg void OnCbnSelendokResamplemethod();
};

#endif // !defined __WREFORMATRECTIFYDLG_H__
