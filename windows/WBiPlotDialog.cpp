// WBiPlotDialog.cpp : implementation file
//
// Revised by Larry Biehl on 08/21/2017

#include	"SMultiSpec.h"
#include	"WBiPlotDialog.h"
#include	"SExternalGlobals.h"


extern void				BiPlotDataDialogInitialize(
	DialogPtr							dialogPtr,
	BiPlotDataSpecsPtr				biPlotDataSpecsPtr,
	DialogSelectArea*					dialogSelectAreaPtr,
	WindowInfoPtr						imageWindowInfoPtr,
	WindowPtr							activeImageWindow,
	UInt16*								localClassPtr,
	unsigned char*						localSymbolsPtr,
	SInt16*								newXAxisFeaturePtr,
	SInt16*								newYAxisFeaturePtr,
	Boolean*								featureTransformationFlagPtr,
	Boolean*								featureTransformAllowedFlagPtr,
	Boolean*								checkFeatureTransformFlagPtr,
	SInt32*								maxChannelFeatureNumPtr,
	SInt16*								plotDataCodePtr,
	SInt16*								displayPixelCodePtr,
	SInt16*								outlineClassCodePtr,
	Boolean*								thresholdFlagPtr,
	double*								saveThresholdPercentPtr,
	SInt16*								classSelectionPtr,
	UInt32*								localNumberClassesPtr,
	SInt16*								symbolSelectionPtr,
	SInt16*								weightsSelectionPtr,
	Boolean*								createNewGraphicsWindowFlagPtr);

extern void				BiPlotDataDialogOK(
	DialogPtr							dialogPtr,
	BiPlotDataSpecsPtr				biPlotDataSpecsPtr,
	DialogSelectArea*					dialogSelectAreaPtr,
	SInt16								newXAxisFeature,
	SInt16								newYAxisFeature,
	Boolean								featureTransformationFlag,
	Boolean								createNewGraphicsWindowFlag,
	SInt16								plotDataCode,
	SInt16								displayPixelCode,
	SInt16								outlineClassCode,
	double								saveThresholdPercent,
	Boolean								thresholdFlag,
	SInt16								classSelection,
	UInt16								localNumberClasses,
	UInt16*								localClassPtr,
	SInt16								symbolSelection,
	unsigned char*						localSymbolsPtr,
	SInt16								weightsSelection,
	float*								classWeightsPtr);

extern  Boolean		BiPlotDataDialogCheckFeatureTransform(
	DialogPtr							dialogPtr,
	WindowInfoPtr						imageWindowInfoPtr,
	TransformationSpecs*				transformationMatrixPtr,
	Boolean								featureTransformAllowedFlag,
	Boolean*								featureTransformationFlagPtr,
	SInt32*								maxChannelFeatureNumPtr);

extern void				BiPlotDataDialogHideShowClassItems(
	DialogPtr							dialogPtr,
	SInt16								plotDataCode,
	SInt16								outlineClassCode);

extern Boolean			CheckIfStatisticsChannel(
	SInt16								channelNumber,
	Boolean								featureTransformationFlag);

extern SInt16			UpdateDialogClassWeightsInfo(
	SInt16								currentWeightsSelection,
	Boolean								useEnhancedStatisticsFlag,
	SInt16*								classWeightSetCodePtr,
	Boolean								initializeClassWeightSetFlag);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CMBiPlotDialog dialog

//IMPLEMENT_DYNAMIC(CMBiPlotDialog, CDialog)

CMBiPlotDialog::CMBiPlotDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMBiPlotDialog::IDD, pParent)
{
	m_classWeightsPtr = NULL;
	m_checkChannelStatisticsFlag = FALSE;
	m_checkClassesPopUpFlag = FALSE;
	m_createNewGraphicsWindowFlag = FALSE;

	m_checkFeatureTransformFlag = FALSE;
	m_featureTransformAllowedFlag = FALSE;
	m_featureTransformationFlag = FALSE;

	m_thresholdPercent = 0;
	m_saveThresholdPercent = 0.;
	m_maxChannelFeatureNum = 1;

	m_displayPixelCode = 0;
	m_outlineClassCode = 0;
	m_plotDataCode = 0;
	m_newXAxisFeature = 1;
	m_newYAxisFeature = 1;
	m_initializedFlag = CMDialog::m_initializedFlag;

	m_trainingAreaFlag = FALSE;
	m_imageAreaFlag = FALSE;
	m_testFlag = FALSE;

	if (gBiPlotDataSpecsPtr->projectFlag)
		m_initializedFlag = GetDialogLocalVectors(NULL,
			NULL,
			&m_classListPtr,
			NULL,
			&m_classWeightsPtr,
			NULL,
			NULL,
			NULL);
		
}

CMBiPlotDialog::~CMBiPlotDialog()
{
	ReleaseDialogLocalVectors(NULL,
		NULL,
		m_classListPtr,
		NULL,
		m_classWeightsPtr,
		NULL,
		NULL,
		NULL);
}

void CMBiPlotDialog::DoDataExchange(CDataExchange* pDX)
{
	CMDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_LineStart, m_LineStart);
	DDX_Text(pDX, IDC_LineEnd, m_LineEnd);
	DDX_Text(pDX, IDC_LineInterval, m_LineInterval);
	DDX_Text(pDX, IDC_ColumnStart, m_ColumnStart);
	DDX_Text(pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDX_Text(pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDX_Text(pDX, IDC_Horizontal_channel, m_newXAxisFeature);
	DDV_MinMaxUInt(pDX, m_newXAxisFeature, 1, m_maxChannelFeatureNum);
	DDX_Text(pDX, IDC_Vertical_channel, m_newYAxisFeature);
	DDV_MinMaxUInt(pDX, m_newYAxisFeature, 1, m_maxChannelFeatureNum);
	DDX_Text2(pDX, IDC_ThresholdLevel, m_thresholdPercent);


	if (m_displayPixelCode & kDisplayPixels)
		m_displayPixelCode = 1;
	else
		m_displayPixelCode = 0;

	if (m_outlineClassCode & kOutlineClasses)
		m_outlineClassCode = 1;
	else
		m_outlineClassCode = 0;

	mb_displayPixelCode = m_displayPixelCode;
	mb_outlineClassCode = m_outlineClassCode;

	DDX_Check(pDX, IDC_ThresholdPixelCheck, m_thresholdFlag);
	DDX_Check(pDX, IDC_CreateNewGraphWindow, m_createNewGraphWindowFlag);
	DDX_Check(pDX, IDC_FeatureTransformation, m_featureTransformationFlag);
	DDX_Check(pDX, IDC_Training, m_trainingAreaFlag);

	DDX_Check(pDX, IDC_TestAreas, m_testFlag); 
	DDX_Check(pDX, IDC_Area, m_imageAreaFlag);
	DDX_Check(pDX, IDC_DisplayPixels, mb_displayPixelCode);
	DDX_Check(pDX, IDC_OutlineClassAsEllipse, mb_outlineClassCode);

	DDX_CBIndex(pDX, IDC_ClassCombo, m_classSelection);
	DDX_CBIndex(pDX, IDC_WeightsCombo, m_classWeightsSelection);

	m_displayPixelCode = (Boolean)mb_displayPixelCode;
	m_outlineClassCode = (Boolean)mb_outlineClassCode;

}

BEGIN_MESSAGE_MAP(CMBiPlotDialog, CMDialog)
	ON_EN_CHANGE(IDC_Vertical_channel, OnChangeVerticalAxis)
	ON_EN_CHANGE(IDC_Horizontal_channel, OnChangeHorizontalAxis)
	ON_EN_CHANGE(IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE(IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE(IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE(IDC_LineStart, CheckLineStart)
	ON_BN_CLICKED(IDC_FeatureTransformation, OnFeatureTranformation)
	ON_BN_CLICKED(IDC_TestAreas, OnTestArea)
	ON_BN_CLICKED(IDC_Training, OnTrainArea)
	ON_BN_CLICKED(IDC_Area, OnImageArea)
	ON_BN_CLICKED(IDC_ThresholdPixelCheck, OnThresholdPixelFlag)
	ON_BN_CLICKED(IDC_CreateNewGraphWindow, OnCreateNewWindowFlag)
	ON_BN_CLICKED(IDC_DisplayPixels, OnDisplayPixelAsSymbol)
	ON_BN_CLICKED(IDC_OutlineClassAsEllipse, OnOutlineClassAsEllipse)
	ON_CBN_SELENDOK(IDC_ClassPairWeightsCombo, OnSelendokClassWeightsCombo)
	ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)
	ON_BN_CLICKED(IDEntireImage, ToEntireImage)
	ON_BN_CLICKED(IDSelectedImage, ToSelectedImage)

	ON_EN_CHANGE(IDC_Horizontal_channel, &CMBiPlotDialog::OnEnChangeHorizontalchannel)
END_MESSAGE_MAP()

BOOL CMBiPlotDialog::DoDialog()
{
	INT_PTR		returnCode;

	BOOL		continueFlag = FALSE;

	
	// Make sure intialization has been completed.							                         
	if (!m_initializedFlag)
		return (continueFlag);
	
	returnCode = DoModal();

	if (returnCode == IDOK)
	{
		continueFlag = TRUE;

		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;

		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		BiPlotDataDialogOK (this,
									gBiPlotDataSpecsPtr,
									&m_dialogSelectArea,
									m_newXAxisFeature,
									m_newYAxisFeature,
									m_featureTransformationFlag,
									m_createNewGraphicsWindowFlag,
									m_plotDataCode,
									m_displayPixelCode,
									m_outlineClassCode,
									m_thresholdPercent,
									m_thresholdFlag,
									m_classSelection,
									(UInt16)m_localNumberClasses,
									m_classListPtr,
									m_symbolSelection,
									m_localSymbolsPtr,
									m_classWeightsSelection + 1,
									m_classWeightsPtr);
		}

	return (continueFlag);

}

BOOL CMBiPlotDialog::OnInitDialog()
{
	SInt32				maxChannelFeatureNum;

	SInt16				classSelection,
							//controlValue,
							//fieldTypesPresentCode,
							newXAxisFeature,
							newYAxisFeature,
							symbolSelection = 0,
							weightsSelection = 0;

	CMDialog::OnInitDialog();

			// Make sure that we have the bitmaps for the entire image buttons.

	VERIFY(toEntireButton.AutoLoad(IDEntireImage, this));
	VERIFY(toSelectedButton.AutoLoad(IDSelectedImage, this));
	/*
	SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);
	*/
	BiPlotDataDialogInitialize (this,
											gBiPlotDataSpecsPtr,
											&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											m_classListPtr,
											m_localSymbolsPtr,
											&newXAxisFeature,
											&newYAxisFeature,
											(Boolean*)&m_featureTransformationFlag,
											(Boolean*)&m_featureTransformAllowedFlag,
											(Boolean*)&m_checkFeatureTransformFlag,
											&maxChannelFeatureNum,
											&m_plotDataCode,
											&m_displayPixelCode,
											&m_outlineClassCode,
											(Boolean*)&m_thresholdFlag,
											&m_saveThresholdPercent,
											&classSelection,
											&m_localNumberClasses,
											&symbolSelection,
											&weightsSelection,
											(Boolean*)&m_createNewGraphicsWindowFlag);

	m_newXAxisFeature = newXAxisFeature;
	m_newYAxisFeature = newYAxisFeature;
	m_maxChannelFeatureNum = maxChannelFeatureNum;

	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval;

	m_thresholdPercent = m_saveThresholdPercent;
	
			// Classes to use.

	m_classSelection = classSelection;

			// Class weights.
			// Adjust for 0 base index.

	m_classWeightsSelection = weightsSelection - 1;

			// Symbols to use.
			// User defined symbols are not available yet.

	m_symbolSelection = symbolSelection;

			// Set check boxes for "pixels to plot".	      
	if (m_plotDataCode & kTrainingType)
		m_trainingAreaFlag = TRUE;

	if (m_plotDataCode & kTestingType)
		m_testFlag = TRUE;

	if (m_plotDataCode & kAreaType)
		m_imageAreaFlag = TRUE;

			// Hide some of the area selection boxes if needed.						

	if (!(m_plotDataCode & kAreaType))
		HideSomeAreaSelectionItems();

	if (UpdateData(FALSE))
		PositionDialogWindow();

	SelectDialogItemText (this, IDC_Horizontal_channel, 0, SInt16_MAX);

	return FALSE; 
}


// CMBiPlotDialog message handlers

void CMBiPlotDialog::OnChangeVerticalAxis()
{
	m_checkChannelStatisticsFlag = TRUE;
}

void CMBiPlotDialog::OnChangeHorizontalAxis()
{
	m_checkChannelStatisticsFlag = TRUE;
}

void CMBiPlotDialog::OnFeatureTranformation()
{
	
	DDX_Check(m_dialogFromPtr, IDC_FeatureTransformation, m_featureTransformationFlag);

	if (m_featureTransformationFlag)
	{
		MHideDialogItem(this, IDC_ChannelsPrompt);
		MShowDialogItem(this, IDD_FeatureSelection);

	}
	else
	{
		MShowDialogItem(this, IDC_ChannelsPrompt);
		MHideDialogItem(this, IDD_FeatureSelection);
	}

}
void CMBiPlotDialog::OnTestArea()
{

	DDX_Check(m_dialogFromPtr, IDC_TestAreas, m_testFlag);

	if (m_testFlag == 0)
	{
		m_plotDataCode -= kTestingType;
	}
	else
	{
		m_plotDataCode += kTestingType;
	}

	BiPlotDataDialogHideShowClassItems(this, m_plotDataCode, m_outlineClassCode);

	//CheckOKButton();

}
void CMBiPlotDialog::OnTrainArea()
{

	DDX_Check(m_dialogFromPtr, IDC_Training, m_trainingAreaFlag);

	if (m_trainingAreaFlag == 0)
	{
		m_plotDataCode -= kTrainingType;
	}
	else
	{
		m_plotDataCode += kTrainingType;
	}

	BiPlotDataDialogHideShowClassItems(this, m_plotDataCode, m_outlineClassCode);

	//CheckOKButton();

}		// end "OnTrainAr

void CMBiPlotDialog::OnImageArea()
{
	UInt16 selectItem;

	DDX_Check(m_dialogFromPtr, IDC_Area, m_imageAreaFlag);

	if (m_imageAreaFlag)
	{
		ShowSomeAreaSelectionItems();
		selectItem = IDC_LineStart;

	}		// end "if (m_areaFlag)"

	else // !m_areaFlag
	{
		HideSomeAreaSelectionItems();
		selectItem = IDC_LineInterval;

	}		// end "else !m_areaFlag"

	SelectDialogItemText(this, selectItem, 0, SInt16_MAX);

	//CheckOKButton();

	if (m_imageAreaFlag == 0)
	{
		m_plotDataCode -= kAreaType;
	}
	else
	{
		m_plotDataCode += kAreaType;
	}

}		// end "OnImageArea"

void CMBiPlotDialog::OnThresholdPixelFlag()
{

	DDX_Check(m_dialogFromPtr, IDC_ThresholdPixelCheck, m_thresholdFlag);

}

void CMBiPlotDialog::OnCreateNewWindowFlag()
{
	DDX_Check(m_dialogFromPtr, IDC_CreateNewGraphWindow, m_createNewGraphicsWindowFlag);

}

void CMBiPlotDialog::OnDisplayPixelAsSymbol()
{
	BOOL displayPixel;
	DDX_Check(m_dialogFromPtr, IDC_DisplayPixels, displayPixel);
	if (displayPixel == 0)
		m_displayPixelCode &= (kPixelsAsSymbols + kPixelsAsColor);
	else
		m_displayPixelCode |= kDisplayPixels;

}

void CMBiPlotDialog::OnOutlineClassAsEllipse()
{
	BOOL outlineClass;
	DDX_Check(m_dialogFromPtr, IDC_OutlineClassAsEllipse, outlineClass);
	if (outlineClass == 0)
	{
		m_outlineClassCode &= kClassesAsEllipses + kClassesAsBorders;
	}
	else
	{
		m_outlineClassCode |= kOutlineClasses;
	}

	BiPlotDataDialogHideShowClassItems(this, m_plotDataCode, m_outlineClassCode);

}		// end "OnOutlineClassAsEllipse"

void CMBiPlotDialog::OnSelendokClassWeightsCombo()
{
	HandleClassWeightsMenu((SInt16)gBiPlotDataSpecsPtr->numberClasses,
		(SInt16*)gBiPlotDataSpecsPtr->classPtr,
		m_classWeightsPtr,
		gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
		IDC_WeightsCombo,
		&m_classWeightsSelection);

}		// end "OnSelendokClassPairWeightsCombo"void CMPrincipalCompDialog::OnSelendokClassCombo()

void CMBiPlotDialog::OnSelendokClassCombo()
{
	HandleClassesMenu(&m_localNumberClasses,
		(SInt16*)m_classListPtr,
		1,
		(SInt16)gProjectInfoPtr->numberStatisticsClasses,
		IDC_ClassCombo,
		&m_classSelection);

}		// end "OnSelendokClassCombo" 



void CMBiPlotDialog::OnEnChangeHorizontalchannel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CMDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
