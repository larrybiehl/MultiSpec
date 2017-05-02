// WBiPlotDialog.cpp : implementation file
//


#include	"SMulSpec.h"
//#include "MULTSPEC.h"
#include	"WBiPlotDialog.h"
//#include	"WDialog.h"
#include	"SExtGlob.h"


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
	m_maxChannelFeatureNum = 0;

	m_displayPixelCode = 0;
	m_outlineClassCode = 0;
	m_plotDataCode = 0;
	m_newXAxisFeature = 0;
	m_newYAxisFeature = 0;
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
	DDX_Text(pDX, IDC_Vertical_channel, m_newYAxisFeature);
	DDX_Text(pDX, IDC_ThresholdLevel, m_thresholdPercent);

	mb_trainingAreaFlag = m_trainingAreaFlag;
	mb_testFlag = m_testFlag;
	mb_imageAreaFlag = m_imageAreaFlag;
	mb_displayPixelCode = m_displayPixelCode;
	mb_outlineClassCode = m_outlineClassCode;
	

	DDX_Check(pDX, IDC_ThresholdPixelCheck, mb_thresholdFlag);
	DDX_Check(pDX, IDC_CreateNewGraphWindow, mb_createNewGraphWindowFlag);
	DDX_Check(pDX, IDC_FeatureTransformation, mb_featureTransformationFlag);
	DDX_Check(pDX, IDC_Training, mb_trainingAreaFlag);

	DDX_Check(pDX, IDC_TestAreas, mb_testFlag); 
	DDX_Check(pDX, IDC_Area, mb_imageAreaFlag);
	DDX_Check(pDX, IDC_DisplayPixels, mb_displayPixelCode);
	DDX_Check(pDX, IDC_OutlineClassAsEllipse, mb_outlineClassCode);

	DDX_CBIndex(pDX, IDC_ClassCombo, m_classSelection);
	DDX_CBIndex(pDX, IDC_WeightsCombo, m_classWeightsSelection);

	m_thresholdFlag = (Boolean)mb_thresholdFlag;
	m_createNewGraphWindowFlag = (Boolean)mb_createNewGraphWindowFlag;
	m_featureTransformationFlag = (Boolean)mb_featureTransformationFlag;
	m_thresholdFlag = (Boolean)mb_thresholdFlag;
	m_trainingAreaFlag = (Boolean)mb_trainingAreaFlag;
	m_testFlag = (Boolean)mb_testFlag;
	m_imageAreaFlag = (Boolean)mb_imageAreaFlag;
	m_displayPixelCode = (Boolean)mb_displayPixelCode;
	m_outlineClassCode = (Boolean)mb_outlineClassCode;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////0------+



BEGIN_MESSAGE_MAP(CMBiPlotDialog, CMDialog)
	ON_EN_CHANGE(IDC_Vertical_channel, OnChangeVerticalAxis)
	ON_EN_CHANGE(IDC_Vertical_channel, OnChangeHorizontalAxis)
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

END_MESSAGE_MAP()

BOOL CMBiPlotDialog::DoDialog()
{
	SInt16		returnCode;

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

		BiPlotDataDialogOK(this,
			gBiPlotDataSpecsPtr,
			&m_dialogSelectArea,
			m_newXAxisFeature,
			m_newYAxisFeature,
			m_featureTransformationFlag,
			m_createNewGraphicsWindowFlag,
			m_plotDataCode,
			m_displayPixelCode,
			m_outlineClassCode,
			m_saveThresholdPercent,
			m_thresholdFlag,
			m_classSelection,
			m_localNumberClasses,
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

	SInt16				classSelection,
						controlValue,
						fieldTypesPresentCode,
						symbolSelection,
						weightsSelection;

	CMDialog::OnInitDialog();
	
	BiPlotDataDialogInitialize(this,
		gBiPlotDataSpecsPtr,
		&m_dialogSelectArea,
		gImageWindowInfoPtr,
		gActiveImageWindow,
		m_classListPtr,
		m_localSymbolsPtr,
		&m_newXAxisFeature,
		&m_newYAxisFeature,
		&m_featureTransformationFlag,
		&m_featureTransformAllowedFlag,
		&m_checkFeatureTransformFlag,
		&m_maxChannelFeatureNum,
		&m_plotDataCode,
		&m_displayPixelCode,
		&m_outlineClassCode,
		&m_thresholdFlag,
		&m_saveThresholdPercent,
		&classSelection,
		&m_localNumberClasses,
		&symbolSelection,
		&weightsSelection,
		&m_createNewGraphicsWindowFlag);

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
	// Adjust for 0 base index.
	// User defined symbols are not available yet.

	m_symbolSelection = symbolSelection - 1;

	// Set check boxes for "pixels to plot".	      
	if (m_plotDataCode & kTrainingType)
		m_trainingAreaFlag = TRUE;

	if (m_plotDataCode & kTestingType)
		m_testFlag = TRUE;

	if (m_plotDataCode & kAreaType)
		m_imageAreaFlag = TRUE;

	// Hide some of the area selection boxes if needed.						

	if (!(m_plotDataCode & kAreaType))
	{
		HideSomeAreaSelectionItems();

	}		// end "if (!(plotDataCode & kAreaType))" 

	if (UpdateData(FALSE))
	{
		PositionDialogWindow();
		return FALSE;
	}

	return TRUE; 
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
	
	//wxCheckBox* featureTrans = (wxCheckBox*)FindWindow(IDC_FeatureTransformation);
	//m_featureTransformationFlag = featureTrans->GetValue();
	DDX_Check(m_dialogFromPtr, IDC_FeatureTransformation, mb_featureTransformationFlag);
	m_featureTransformationFlag = (Boolean)mb_featureTransformationFlag;

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
	//wxCheckBox* test = (wxCheckBox*)FindWindow(IDC_TestAreas);
	//m_testFlag = test->GetValue();

	DDX_Check(m_dialogFromPtr, IDC_TestAreas, mb_testFlag);
	m_testFlag = (Boolean)mb_testFlag;

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
	//wxCheckBox* train = (wxCheckBox*)FindWindow(IDC_Training);
	//m_trainingAreaFlag = train->GetValue();

	DDX_Check(m_dialogFromPtr, IDC_Training, mb_trainingAreaFlag);
	m_trainingAreaFlag = (Boolean)mb_trainingAreaFlag;
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

	//m_imageAreaFlag = ((wxCheckBox *)FindWindow(IDC_ImageArea))->GetValue();
	DDX_Check(m_dialogFromPtr, IDC_Area, mb_imageAreaFlag);
	m_imageAreaFlag = (Boolean)mb_imageAreaFlag;
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
	//wxCheckBox* thresholdPixel = (wxCheckBox*)FindWindow(IDC_ThresholdPixelCheck);
	//m_thresholdFlag = thresholdPixel->GetValue();
	DDX_Check(m_dialogFromPtr, IDC_ThresholdPixelCheck, mb_thresholdFlag);
	m_thresholdFlag = (Boolean)mb_thresholdFlag;

}

void CMBiPlotDialog::OnCreateNewWindowFlag()
{
	DDX_Check(m_dialogFromPtr, IDC_CreateNewGraphWindow, mb_createNewGraphicsWindowFlag);
	m_createNewGraphicsWindowFlag = (Boolean)mb_createNewGraphicsWindowFlag;
	//wxCheckBox* createNewWindow = (wxCheckBox*)FindWindow(IDC_CreateNewGraphWindow);
	//m_createNewGraphicsWindowFlag = createNewWindow->GetValue();

}

void CMBiPlotDialog::OnDisplayPixelAsSymbol()
{
	//wxCheckBox* displayPixel = (wxCheckBox*)FindWindow(IDC_DisplayPixels);
	BOOL displayPixel;
	DDX_Check(m_dialogFromPtr, IDC_DisplayPixels, displayPixel);
	if (displayPixel == 0)
		m_displayPixelCode &= (kPixelsAsSymbols + kPixelsAsColor);
	else
		m_displayPixelCode |= kDisplayPixels;

}

void CMBiPlotDialog::OnOutlineClassAsEllipse()
{
	//wxCheckBox* outlineClass = (wxCheckBox*)FindWindow(IDC_OutlineClassAsEllipse);
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


