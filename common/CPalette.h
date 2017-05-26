//===========================================================================
//	CPalette.h				c1994 Metrowerks Inc. All rights reserved.
//===========================================================================
//	CPalette.cp	<- double-click + Command-D to see class implementation
               
#if !defined __MPALETTE_H__
	#define	__MPALETTE_H__ 

#ifndef multispec_lin
	
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
	#if !defined _X86_ && !defined _AMD64_	                 
		class	CMPalette
	#endif	// !defined _X86_ && !defined _AMD64_

	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
	#if defined _X86_ || defined _AMD64_           
		class	CMPalette : public CPalette
	#endif	// defined _X86_ || defined _AMD64_
	{
	public:
									CMPalette();		// constructor
		virtual 					~CMPalette();		// destructor stub
		
//		Boolean 					CreatePalette (                       
//										DisplaySpecsPtr				displaySpecsPtr, 
//										short int						windowType);

		// oul: added the condition of _AMD64_ to be compatible with x64
		// _AMD64_ is the predefined macro for x64 machines according to MSDN
		#if defined _X86_ || defined _AMD64_          
			Boolean					LoadRGBQUADStructure (
											RGBQUAD							*RGBQuadPtr,
											SInt16							wUsage);
		#endif	// defined _X86_ || defined _AMD64_
											
		SInt16					GetNumberPaletteEntries (void);
		
		void						SetNumberPaletteEntries (
										SInt16							numberEntries);
										
		void 						SetPalette (
										SInt16				paletteId);  
										
		PaletteHandle			mPaletteObject; 
	
	protected:
//		Boolean					MNewPalette (
//										SInt16							numberEntries);
										
//		void						MSetEntryColor (
//										UInt16							entry, 
//										RGBColor							*theColorPtr);
										
		void						MSetPalette (void);
										
//		Boolean					MVerifyPaletteSize (
//										SInt16							numberEntries);
		
		SInt16					mNumberEntries;
		
		static UInt16			s_palette1001[117];
		
		static UInt16			s_palette1002[396];
		
		static UInt16			s_palette1003[759];
		
		static UInt16			s_palette1004[315];
		
		static UInt16			s_palette1005[372];
		
		static UInt16			s_palette1006[768];
		
		static UInt16			s_palette1007[768];
		
		static UInt16			s_palette1008[768];
		
	};  
#endif
#if defined multispec_lin
#include "SMulSpec.h"
	
class CMPalette : public wxPalette {
public:
    CMPalette(); // constructor
#if defined multispec_lin
    CMPalette(SInt16 numberEntries);
#endif
    virtual ~CMPalette(); // destructor stub

    //	Boolean 		CreatePalette (	DisplaySpecsPtr		displaySpecsPtr,
    //										short int						windowType);
    bool Create(SInt16 numberEntries,UInt8* redpalette,UInt8* greenpalette,UInt8* bluepalette);
#if defined _X86_
    Boolean LoadRGBQUADStructure(
            RGBQUAD *RGBQuadPtr,
            SInt16 wUsage);
#endif	// defined _X86_

    SInt16 GetNumberPaletteEntries(void);

    void SetNumberPaletteEntries(SInt16 numberEntries);

    void SetPalette(SInt16 paletteId);

    PaletteHandle mPaletteObject;

protected:
    //	Boolean			MNewPalette (SInt16 numberEntries);
    //	void			MSetEntryColor (UInt16	entry,RGBColor		*theColorPtr);

    void MSetPalette(void);

    //		Boolean					MVerifyPaletteSize (
    //										SInt16							numberEntries);

    SInt16 mNumberEntries;

    static UInt16 s_palette1001[117];

    static UInt16 s_palette1002[396];

    static UInt16 s_palette1003[759];

    static UInt16 s_palette1004[315];

    static UInt16 s_palette1005[372];

    static UInt16 s_palette1006[768];

    static UInt16 s_palette1007[768];

    static UInt16 s_palette1008[768];

};
#endif
#endif	// !defined __MPALETTE_H__