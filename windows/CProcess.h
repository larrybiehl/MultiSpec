//===========================================================================
//	CProcess.h				c1994 Metrowerks Inc. All rights reserved.
//===========================================================================
//	CProcess.cpp	<- double-click + Command-D to see class implementation
               
#if !defined __MPROCESS_H__
	#define	__MPROCESS_H__                      


class	CMProcessor

	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
	#if defined _X86_ || defined _AMD64_
		: public CCmdTarget
	#endif	// defined _X86_ || _AMD64_
	{
	public:
										CMProcessor();		// constructor
		virtual 						~CMProcessor();		// destructor stub 

// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN
#if defined _X86_ || defined _AMD64_		
		static CMImageDoc*		GetActiveImageDocCPtr (void);
		
		static CMOutlineArea*	GetActiveSelectionAreaCPtr (void);
		
		static CMOutlineArea*	GetSelectionAreaCPtr (
											CMImageView*						imageViewCPtr);
											
		static CMOutlineArea*	GetSelectionAreaCPtr (
											WindowInfoPtr						imageWindowInfoPtr);
#endif	// defined _X86_ || defined _AMD64_
	
	protected:
		
	};  
	
#endif	// !defined __MPROCESS_H__