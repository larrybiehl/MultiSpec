//===========================================================================
//	MHistgrm.h				c1994 Metrowerks Inc. All rights reserved.
//===========================================================================
//	MHistgrm.cpp	<- double-click + Command-D to see class implementation
 
#if !defined __MHISTGRM_H__  
	#define __MHISTGRM_H__
	
	#include "CProcess.h"


		// This structure defines the histogram for a channel in the image	
/*		
typedef struct HistogramSummary
	{
	double 					stdDeviation;	// Standard deviation of data 		
													// values for channel					
	
	double 					averageValue;	// Average value for channel			
	
			// Geometric mean of all channels.											
	float						geometricMean;
													
	SInt32					badValues;		// Number of bad values for channel	
	
	UInt16 					minValue;		// Minimum value for channel.			
	
	UInt16				 	maxValue;		// Maximum value for channel.			
	
	UInt16					medianValue;	// Median value for channel.			
	
	UInt16				 	minNonSatValue; // Minimum nonsaturated value for 	
													 //	channel.								
	
	UInt16				 	maxNonSatValue; // Maximum nonsaturated value for 	
													 // channel.								
	
	Boolean					availableFlag;	// Indicates that information is		
													// available for the channel.			
	
	} HistogramSummary, *HistogramSummaryPtr;
	
	
		//	This structure defines the histogram block data to be used in 			
		//	computing the histogram statistics												

typedef struct HistogramSpecs
	{
			// Geometric mean of all channels.												
	float						overallGeometricMean;
			
			// Handle for the histogram array information.								
	Handle					histogramArrayH;
	
			// List of channels to be histogrammed.										
	Handle					channelsHandle;
	
			// Buffer to use to read the statistics information from disk.			
	CharPtr					statBufferPtr;
	
			// Pointer for the histogram array information.								
	HUInt32Ptr				histogramArrayPtr;
	
			// Number of pixels used for histogram statistics.							
	UInt32					totalPixels;
	
			// First line to use to compute histogram statistics. 					
	UInt32					lineStart;
	
			// Last line to use to compute histogram statistics. 						
	UInt32					lineEnd;
	
			// Line interval to use to compute histogram statistics.					
	UInt32					lineInterval;
	
			// First column to use to compute histogram statistics. 					
	UInt32					columnStart;
	
			// Last column to use to compute histogram statistics. 					
	UInt32					columnEnd;
	
			// Column interval to use to compute histogram statistics.				
	UInt32					columnInterval;
	
			// Number of bad lines in the image.											
	SInt32					numBadLines;
	
			// Overall min and max values used when listing histogram values.		
	SInt32					overallMaxValue;
	SInt32					overallMinValue;
	
			// Indicates whether all (=0) or a subset (=1) of the channels are	
			// to be histogrammed.																
	SInt16					channelSet;
	
			//	Variable indicating method desired to determine the histogram		
			// statistics.  "Maximum range" = 1. "Compute" = 2. "Read from file	
			// = 3.																					
	SInt16					method;
	
			//	Number of channels histogramed.												
	SInt16					numberChannels;
	
			//	Number of channels histogramed.												
	SInt16					numberStatBufferChannels;
	
			//	Number of channels histogramed.												
	SInt16					numberSummaryChannels;
	
			// Variable to indicate where classification output is to be placed.	
			//		=0, no output; =1, to output window; =2, to disk file; 			
			//		=3, both output window and disk file.									
	SInt16					outputStorageType;
	
			// Flag indicating whether all channels can be processed at once		
			// when getting the histogram information.									
	Boolean					allChannelsAtOnceFlag;
	
			// Flag indicating if histogram is to be computed.							
	Boolean					computeFlag;
	
			// Flag indicating (if true) that the histogram specifications have	
			// not been loaded the first time.												
	Boolean					firstTime;
	
			// Flag indicating whether histogram information has changed since	
			// the last image display.															
	Boolean					histogramChangedSinceDisplayFlag;
	
			// Flag indicating if empty bins are to be listed. 						
	Boolean					includeEmptyBinsFlag;
	
			// Flag indicating if this structure needs to be initialized. 			
	Boolean					initializeStructureFlag;
	
			// Flag indicating if the histogram is to be listed in a line 			
			// format. 																				
	Boolean					lineFormatHistFlag;
	
			// Flag indicating if histogram is to be listed. 							
	Boolean					listHistFlag;
	
			// Flag indicating if histogram summary is to be listed. 				
	Boolean					listHistSummaryFlag;
	
			// Flag indicating that the histogram is loaded into the structure.	
	Boolean					loadedFlag;
	
			// Flag indicating if only min, max, mean and standard deviation is	
			// to be computed. 																	
	Boolean					minMaxMeanSDOnlyFlag;
			
	}	HistogramSpecs, *HistogramSpecsPtr;
*/	

class	CMHistogram : public CMProcessor
//	#if defined _X86_ 
//		: public CCmdTarget
//	#endif	// defined _X86_ 
	{
	public:
									CMHistogram();		// constructor
		virtual 					~CMHistogram();		// destructor stub
		
//		void 						CloseUpHistogramArrayReadParameters (void);
		
//		Boolean					Dialog (void);
		
//		Boolean					HistogramControl (
//										SInt16				computeCode);

/*		Boolean 					GetClippedMinMaxValues ( 
										SInt16	 				percentTailsClipped,
										SInt16 					channel, 
										UInt32				 	numberDataValues, 
										UInt32* 					minValuePtr, 
										UInt32* 					maxValuePtr,
										Boolean 					histLoadedFlag);
*/										
//		HUInt32Ptr				GetHistogramArrayPtr (void);
		
//		Boolean					GetHistogramChangedSinceDisplayFlag (void);
		
//		Boolean					GetHistogramLoadedFlag (void);
										
		HistogramSpecsPtr		GetHistogramSpecsPtr (void);
		
//		Boolean					GetHistogramVectorForChannel (  
//										SInt16					*channelListPtr, 
//										SInt16					numberChannels);
		
		void						ReleaseHistogramSpecsPtr (void);
		
		void						ReleaseHistogramSupportMemory(void);
		
//		void						SetHistogramChangedSinceDisplayFlag (
//										Boolean					setting);
										
		Boolean					SetUpHistogramSpecsPtr (void);
										
//		Boolean 					SetUpToReadHistogramArray (void);
		
		Boolean					m_initializedFlag;
		
				// Handle to histogram specifications for image.
		Handle					mHistogramSpecsHandle;
		
				// Handle to histogram summary for image.
		Handle					mHistogramSummaryHandle;
	
	protected:
		
		static HistogramSpecsPtr		sHistogramSpecsPtr;
		
		static HistogramSummaryPtr		sHistogramSummaryPtr;
		
	};
	
#endif	// !defined __MHISTGRM_H__ 
