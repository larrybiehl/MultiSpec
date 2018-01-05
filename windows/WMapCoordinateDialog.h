#if !defined(AFX_WMAPCDLG_H__3DCE9436_D051_11D4_8E14_00105AA88EE3__INCLUDED_)
#define AFX_WMAPCDLG_H__3DCE9436_D051_11D4_8E14_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WMapCoordinateDialog.h : header file
//
	
	#include "WDialog.h"   
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CMMapCoordinateDlg dialog

class CMMapCoordinateDlg : public CMDialog
{
// Construction
public:
	CMMapCoordinateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMMapCoordinateDlg)
	enum { IDD = IDD_MapCoordinates };
	int		m_ellipsoidSelection;
	int		m_projectionSelection;
	int		m_mapUnitsSelection;
	int		m_gridZone;
	int		m_referenceSystemSelection;
	double	m_horizontalPixelSize;
	double	m_verticalPixelSize;
	double	m_xMapCoordinate11;
	double	m_yMapCoordinate11;
	double	m_falseEasting;
	double	m_falseNorthing;
	double	m_latitudeOrigin;
	double	m_longitudeCentralMeridian;
	double	m_semiMinorAxis;
	double	m_radiusSpheroid;
	double	m_scaleFactorOfCentralMeridian;
	CString	m_gridZoneDirection;
	int		m_datumSelection;
	double	m_mapOrientationAngle;
	double	m_standardParallel1;
	double	m_standardParallel2;
	//}}AFX_DATA
	
	Boolean DoDialog(
					FileInfoPtr							fileInfoPtr,
					MapProjectionInfoPtr				mapProjectionInfoPtr);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMMapCoordinateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMMapCoordinateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokMapUnitsCombo();
	afx_msg void OnSelendokEllipsoidCombo();
	afx_msg void OnSelendokReferenceSystemCombo();
	afx_msg void OnSelendokProjectionCombo();
	afx_msg void OnSelendokDatumCombo();
	afx_msg void OnChangeZoneDirection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DoublePoint						m_upperLeftLatitudeLongitudePoint;

	UInt8								m_datumName[64],
										m_ellipsoidName[64],
										m_epsgName[64],
										m_gridZoneDirectionString[4];
//										m_statePlaneZoneName[32];

	double							m_semiMajorAxis;
		
	FileInfoPtr						m_fileInfoPtr;
	MapProjectionInfoPtr			m_mapProjectionInfoPtr;

	SInt16							m_areaUnitsCode,
										m_datumCode,
										m_ellipsoidCode,
										m_epsgCode,
										m_gridZoneWithDirection,
										m_projectionCode,
										m_referenceSystemCode,
										m_zoneProjectionCode;
	
	Boolean							m_initializedFlag,
										m_adjustUpperLeftMapPointFlag,
										m_validEPSGCodeFlag;

public:
	afx_msg void OnEnChangeZone();
	afx_msg void OnEnChangeEPSGCode();
	afx_msg void OnCbnSelchangeDatumcombo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WMAPCDLG_H__3DCE9436_D051_11D4_8E14_00105AA88EE3__INCLUDED_)
