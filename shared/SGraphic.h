//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
// File:						SGraphic.h
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	Include file for NuTools Graphics routines definitions
//
//	Written By:				Larry L. Biehl			Date: 07/31/1992
//	Revised By:				Tsung Tai Yeh			Date: 08/06/2015
//	Revised By:				Larry L. Biehl			Date: 06/29/2018
//
//------------------------------------------------------------------------------------

#if !defined NU_TOOLS_NUMERICAL
#define	NU_TOOLS_NUMERICAL

#define	NU_LINE_PLOT		1
#define	NU_SCATTER_PLOT	2
#define	NU_BAR_PLOT			4
#define	NU_HISTOGRAM_PLOT	8

#define	NU_LINEAR			0
#define	NU_XLOG				1
#define	NU_YLOG				2
#define	NU_GRID				4

#define GR_LISTDATA                  1
#define GR_BIPLOT                    2
#define GR_HISTOGRAM                 3

#ifndef	kTWO_PI
	#define 	kTWO_PI 						(6.28318530717958647692)
#endif

#ifndef	kDegreesToRadians
	#define 	kDegreesToRadians			(0.0174532925199432958)
#endif

#ifdef	_MC68881_
	#define	NU_MAX_DIGITS		18
#endif 
#ifndef	_MC68881_     
	#define	NU_MAX_DIGITS		15
#endif

#define	NU_MAXREAL_SIZE			( NU_MAX_DIGITS + 10 )
#define	NU_MAXREALTERM_SIZE		( NU_MAXREAL_SIZE + 20 )

#define NU_NO_ERROR		  			0	// Normal successful completion

// ======================================================================
//
//		Error Message Display Routine
//
// ======================================================================

#define	ERROR_DIALOG	20000
#define	ERROR_TYPE		'ERRS'
#define	ABORT_ITEM		1
#define	CONTINUE_ITEM	2
#define	DEFAULT_ITEM	99


// General math errors
#define NU_DIVBY0						110		// Division by zero

// Memory allocation error
#define	NU_ALLOC_ERR				3010

// Graphics errors
#define	NU_MUSTBEPOS				5010		// Values must be positive	
#define	NU_NOTSAMESIZEG			5020		// Structures must be same size

// Vector
#define	NU_NOTSAMESIZEV			2710		// Structures must be same size
#define 	NU_OUTOFRANGEV				2720		// Argument is out of range

class	CMImageView;
class	CMGraphView;

#if defined multispec_lin
	class LCMGraphView;
#endif

//#ifdef _MC68881_
//	typedef long double	DOUBLE;
//#else
//	typedef	double	DOUBLE;
//#endif
                            
#if defined multispec_mac
		// This is a holder variable for a Windows structure.
	typedef SInt32				CDC; 
#endif	// defined multispec_mac

typedef	double	GRAPHDATA;

typedef	double	elem_type;

typedef struct 
	{
	SInt32		numberPoints;
	SInt32		size;
	Handle		baseHandle;
	GRAPHDATA*	basePtr;
	
	} vector;

typedef struct 
	{
	Str63				title;
	Str31				title2;
	Str31				xLabel;
	Str31				yLabel;
	Str15				channelDescriptionUnitString;
	
	vector			xVector;
	vector			yVector;
	Rect				clientRect;
	double			textScaling;
	double			xDataMin;
	double			xDataMax;
	double			xEllipseMin;
	double			xEllipseMax;
	double			xMin;
	double			xMax;
	
			// Reflective and Thermal Data min/max used for response vs wavelength 
			// (selection) graphs.
	double			xReflectiveDataMin;
	double			xReflectiveDataMax;
	double			xScaleMin;
	double			xScaleMax;
	double			xThermalDataMin;
	double			xThermalDataMax;
	
	double			yDataMin;
	double			yDataMax;
	double			yEllipseMin;
	double			yEllipseMax;
	double			yMin;
	double			yMax;
	double			yScaleMin;
	double			yScaleMax;
	
			// Probability threshold to use when drawing class information.
	double			classThresholdValue; 
	
			// The tic interval for the x-axis.
	double			xInt;
	
			// the tic interval for the y-axis.	
	double			yInt;
	
			// The width being used for the histogram bin
	double			histogramBinWidth;
	
			// The width of the histogram bin
	double			origHistogramBinWidth;
	
			// Pointer to the Windows and Linux DC class
	#ifndef multispec_lin
		CDC*				pDC;
	#endif
	#ifdef multispec_lin
		wxDC*          pDC;
	#endif	
	
			// Pointer to the window structure of the image window that
			// is the 'parent' of this graph window.
	CMImageView*	imageViewCPtr;
	
			// Control handle for the xAxis popup menu control
	ControlHandle		xAxisPopupControlHandle;
	
			// Pointer to the pattern or brush to be used for the background
			// of the graphs.
	Ptr				backgroundPatternPtr;
	
			// Pointer to the graph view class which uses this graph record.
	CMGraphView*	graphViewCPtr;
	
			// Handle to the class means and variances for channels used in 
			// the plot.																
	Handle			classStatisticsHandle;
	
			// Handle to the bin widths for histogram plots for each channel.																
	Handle			histogramBinWidthHandle;
	
			// Handle to list of values related to the sets of graphs that are stored.
			// For example the channel or feature numbers.	
	Handle			setListHandle;
	
			// Handle to the codes indicating which vectors are to be drawn
			// in the graph.												
	Handle			vectorDisplayHandle;
	
			// Handle to the labels for each vector.
	Handle			vectorLabelHandle;
	
			// Handle to the number of data values in each vector.
	Handle			vectorLengthsHandle;
	
			// Handle to the code for the color to be used for each vector	
			// in the graph.				
	Handle			vectorPaletteColorHandle;
	
			// Handle to the symbol to be used for each vector in the graph.
	Handle			vectorSymbolHandle;
	
			// Handle to the offsets to use access the first data value for
			// the x-axis each vector.	
	Handle			xVectorDataHandle;
	
			//
	LongRect			imageLineColRect;
	
			// Pointer to Mac window structure for this graph.
	WindowPtr		window;
	
			// Pointer to the window structure of the image window that
			// is the 'parent' of the data in the structure for the graph window.
			// This is for the non frame work version.
	WindowPtr		imageWindow;
	
			// Pointer to the window structure of the image window that the
			// selection graph was associated with. If there is no selection,
			// the data in the structure could belong to a different image window.
	WindowPtr		lastActiveImageWindow;
	
			// Increment to be used when stepping through the class stats vector.
	UInt32			classStatsIncrement;
	
			// Code indicating whether the axes are linear or log.
	SInt16			attrb;
	
			// Code indicating what wavelength data is available to plot
			//		See description code in Window Info structure for details.
	SInt16			descriptionCode;
	
			// Code indicating whether the graph is to be drawn, 
			//		0 = no, 
			//		bit 1 = draw vectors, 
			//		bit 2 = draw class outline.	
			//		bit 3 = draw class orginal statistics density vector.
			//		bit 4 = draw class LOOC statistics density vector.
			//		bit 5 = draw class enhanced statistics density vector.
	SInt16			drawGraphCode;
	
	SInt16			leftInset;
	SInt16			topInset;
	SInt16			rightInset;
	SInt16			bottomInset;
	
			// Code for the graph type - 1=line plot, 2=scatter plot.
	SInt16			flag;
	
			// Code indicating which graph codes are available in this graph.
			// The codes are the same as those listed for "drawGraphCode".
	SInt16			graphCodesAvailable;
	
			// Code indicating whether the histogram data should be put into
			// bins other than 1. 
			//		<=0 	= automatic binning depending on the size of the graph.
			//					No histogram bins should be on top of another. It may
			//					be different for each channel or feature.
			//		1 		= histogram bin should have a data value width of 1.
			//		2->n 	= histogram bin should have a data with of n counts.
	SInt16			histogramBinCode;
	
			// Number channels used in the project statistics.
	SInt16			numberStatisticsChannels;
	
			// Number of sets of vectors.
	SInt16			numberSets;
	
			// Number of vectors (lines or sets) of data which will be drawn
			// with different symbols and/or colors.
	SInt16			numberVectors;
	
			// The code for the processor which created this graph.
	SInt16			processorCode;
	
			// The code for the selection type used for the selection graphs.
			//	- kRectangleType or kPolygonType
	SInt16			selectionTypeCode;
	
			// Set of data that is currently being plotted.
	SInt16			set;
	
			// Code indicating what a set represents such as:
			//		1 - Channels 
			//		2 - Features
	SInt16			setCode;
	     
			// The size of the text used for labels and titles.
	SInt16			textSize;
	     
			// The maximum width of the text used for labels.
	SInt16			textWidth;
	
			// Code for type of x-axis to plot
			//		= 1 channel numbers
			//		= 2 wavelength band centers
			//		= 3 wavelength band widths
			//		= 4 wavelength band centers for reflective bands
			//		= 5 wavelength band widths for reflective bands
			//		= 6 wavelength band centers for thermal bands
			//		= 7 wavelength band widths for thermal bands
	SInt16			xAxisCode;
	
			// The number of significant digits to be used in x and y labels.
	UInt16			xESignificantDigits;
	UInt16			xFSignificantDigits;
	UInt16			yESignificantDigits;
	UInt16			yFSignificantDigits;
	
	SInt16			xTicLabelWidth;
	SInt16			yTicLabelWidth;
	
			// The code for the value type: 0=integer, 1=real
	SInt16			xValueTypeCode;
	SInt16			yValueTypeCode;  
	 
	bool				channelsInWavelengthOrderFlag;
	bool				graphWindowFlag;
	
	} GraphRecord, *GraphPtr;


typedef	struct 
	{
	SInt32		m,n;
	elem_type	**mat;
	
	} matrix;

#ifndef	MAX
	#define MAX(a, b) 	((a) > (b) ? (a) : (b))
	#define MIN(a, b) 	((a) < (b) ? (a) : (b))
#endif		// if !defined MAX


extern void AllocateV (
				vector*								v0,
				SInt32								size,
				SInt32*								error);

void DeallocateV (
				vector*								v1,
				SInt32*								error);

extern void DensityFunctionPlotV (
				GraphPtr								graphPtr,
				SInt32*								errorPtr,
				SInt16								drawGraphCode);

void DrawAxis (
				GraphPtr								graphPtr,
				SInt32*								error);

void DrawLinTicks (
				GraphPtr								graph,
				double								xint,
				double								yint,
				SInt16								size,
				SInt32*								error);

void DrawLogTicks (
				GraphPtr								graph,
				SInt16								size1,
				SInt16								size2,
				SInt32*								error);

void EllipsePlotV (
				GraphPtr								graph,
				SInt32*								error);

void ErrorMessage (
				SInt32								error,
				char*									text,
				SInt16								def,
				SInt16								icon);

extern void FindMaxBinV (
				double*								maxPtr,
				vector*								xVector,
				vector*								yVector,
				HSInt32Ptr							vectorLengthsPtr,
				HSInt32Ptr							vectorDataPtr,
				char*									vectorDisplayPtr,
				double*								histogramBinWidthPtr,
				UInt32								numberVectors,
				UInt32								set,
				GraphPtr								graph);

void FindMaxMinV (
				double*								max,
				double*								min,
				vector*								vect,
				SInt32*								error);

extern void FindMaxMinV (
				double*								maxPtr,
				double*								minPtr,
				vector*								vect,
				HSInt32Ptr							vectorLengthsPtr,
				HSInt32Ptr							vectorDataPtr,
				char*									vectorDisplayPtr,
				UInt32								numberVectors,
				UInt32								set);

extern void FindMaxV (
				double*								maxPtr,
				vector*								vect,
				HSInt32Ptr							vectorLengthsPtr,
				HSInt32Ptr							vectorDataPtr,
				char*									vectorDisplayPtr,
				UInt32								numberVectors,
				UInt32								set);

void FlaggedPlotV (
				GraphPtr								graph,
				vector*								x,
				vector*								y,
				SInt16								flag,
				SInt16								width,
				SInt32*								error);

void FormatR (
				char*									str,
				double*								num,
				SInt16								esigfigs,
				SInt16								fsigfigs,
				Boolean								noEFormatFlag,
				SInt32*								error);

extern void HistogramPlotV (
				GraphPtr								graph,
				vector*								x,
				vector*								y,
				SInt32*								error);

void InitV (
				vector*								vectorPtr,
				SInt32*								errorPtr);

void InsetGraph (
				GraphPtr								graph,
				SInt16								left,
				SInt16								top,
				SInt16								right,
				SInt16								bottom,
				SInt32*								error);

void LabelLinAxis (
				GraphPtr								graph,
				double								xint,
				double								yint,
				SInt16								size,
				SInt16								xEDecimals,
				SInt16								xFDecimals,
				SInt16								yEDecimals,
				SInt16								yFDecimals,
				SInt32*								error);

void LabelLogAxis (
				GraphPtr								graph,
				SInt16								size,
				SInt16								decimals,
				SInt32*								error);

void LinePlotV (
				GraphPtr								graph,
				vector*								x,
				vector*								y,
				SInt32*								error);

void LockAndGetVectorPointer (
				vector*								x,
				SInt32*								error);

GraphPtr NewGraph (
				GraphRecord*						graphRec,
				WindowPtr							window,
				double								xMin,
				double								xMax,
				double								yMin,
				double								yMax,
				SInt16								attrb,
				char									scroll,
				SInt32*								error);

void ScatterPlotV (
				GraphPtr								graph,
				vector*								x,
				vector*								y,
				SInt32*								error);

void SetBackgroundColor (
				CDC*									pDC,
				UInt16								level);

void SetGraphClip (
				GraphPtr								graph,
				SInt32*								error);

extern void SetGraphVectorSize (
				vector*								v0,
				SInt32								size, 
				SInt32*								error);

void SetV (
				vector*								v1,
				SInt32								element,
				GRAPHDATA							value,
				SInt32*								error);

void SetWindowClip (
				GraphPtr								graph,
				SInt32*								error);

void UnlockVectorPointer (
				vector*								x);

void UpdateMaxMinV (
				double*								max,
				double*								min,
				vector*								vect,
				SInt32*								error);

#endif	//	!defined NU_TOOLS_NUMERICAL   
