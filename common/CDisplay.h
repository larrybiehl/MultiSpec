//===========================================================================
//	CDisplay.h				c1994 Metrowerks Inc. All rights reserved.
//===========================================================================
//	CDisplay.cp	<- double-click + Command-D to see class implementation
               
#if !defined __MDISPLAY_H__
	#define	__MDISPLAY_H__ 

   #include "SMulSpec.h"                        
	#include "CPalette.h"
	#include "CProcess.h"

class	CMDisplay : public CMProcessor
//	#if !defined __MWERKS__ 
//		: public CCmdTarget
//	#endif	// !defined __MWERKS__ 
	{
	public:
										CMDisplay();		// constructor
		virtual 						~CMDisplay();		// destructor stub 
		
//		static void					ActivatePalette (               
//											CMImageView					*imageViewCPtr); 
		
		DisplaySpecsPtr 			GetDisplaySpecsPtr (void);
		
		void							GetOffscreenDimensions (
											UInt16						*heightPtr,
											UInt16						*widthPtr);
		
		double 						GetMagnification (void);
		
		double 						GetMaxMagnification (void);
		
		static double				GetMaxZoomValue(
											DisplaySpecsPtr				displaySpecsPtr);

		void							ReleaseDisplaySupportMemory(void);
		
		void							SetDisplaySpecsPtr (
											DisplaySpecsPtr				displaySpecsPtr);
		
		void	 						SetMagnification (
											double							magnification);
	
				// Handle to display specifications for image.
		Handle						mDisplaySpecsHandle;
		
		Boolean 						m_initializedFlag;
	
	protected:                                        
	
//		static void					SetDisplayImageWindowSizeVariables (void);
									
		void							SetImageWTitle (
											WindowPtr					theWindow,  
											MFileInfo						*fileInfoPtr);
										
		Boolean 						SetUpColorImageMemory (
											Rect							*sourceRectPtr,
											UInt32						*pixRowBytesPtr);
		
		static DisplaySpecsPtr	sDisplaySpecsPtr;
		
	};  
	
#endif	// !defined __MDISPLAY_H__