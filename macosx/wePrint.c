//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2008)
//								� Purdue Research Foundation
//									All rights reserved.
//
//					The following was put here for easy access to a template
//		    		DebugStr ("\pprint.c");
//
//
//	File:						print.c
//
//	Functions in file:	Boolean 				AbortPrint
//								static void 		DoPageHeader
//								void					DoPageSetup
//								void 					DoPrint
//								void 					DrawPrintImage
//								void 					ForcePrintSegmentLoad
//								void 					GetPrinterPageSize
//								void 					PrintImageWindow
//								void 					PrintTextWindow
//								Boolean 				SetUpPrintedImageInfo
//								short int 			ValidatePrintRecord
//
//	Brief description:	The routines in this file handle printing of the
//								text in the output window.
//
//								NOTE.  These are from WASTE Demo Project.
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			12/29/2008

#if !use_mlte_for_text_window

#include	"SMulSpec.h"
#include "SExtGlob.h" 
#include "wePrint.h" 

//	Mac OS #includes

#ifndef __FIXMATH__
#include <FixMath.h>
#endif

#if TARGET_API_MAC_CARBON
	#ifndef __PMAPPLICATION__
	#include <PMApplication.h>
	#endif
#else
	#ifndef __PRINTING__
	#include <Printing.h>
	#endif
#endif

#ifndef __MEASURES__
#include "Measures.h"
#endif

//	dialog item numbers

enum
{
	kItemTopField			=	1,
	kItemBottomField		=	2,
	kItemLeftField			=	3,
	kItemRightField			=	4,

	kItemFirstField			=	kItemTopField,
	kItemLastField			=	kItemRightField
} ;

typedef struct PrCallbackData
{
	DialogRef				theDialog ;				//	reference to page setup or print dialog
	ModalFilterUPP			origFilterProc ;		//	original event filter
	UserItemUPP				origItemProc ;			//	original item handler function
	Fixed					margins [ 4 ] ;			//	page margins
	ResID					additionalItemsID ;		//	ID of DITL resource for extra items
	SInt16					numItems ;				//	count of original items in dialog
	UnitCode				preferredUnit ;			//	preferred unit for numeric values
	DialogItemIndex			recentEditField ;		//	most recently active edit field
} PrCallbackData ;
		
#if !TARGET_API_MAC_CARBON
	extern Boolean 				DoPrintDialog (
											THPrint 									inPrintRecord, 
											PageMarginRecHandle 					inMargins, 
											PrintDialogStyle 						inStyle);
#endif	// !TARGET_API_MAC_CARBON
										
ModalFilterUPP					GetMyStandardDialogFilter (void);

DialogItemType					GetDialogItemType (
										DialogRef 								inDialog, 
										DialogItemIndex 						inDialogItem);
										
extern OSStatus 				DoImagePageSetup (void);


//	static variables

#if !PM_USE_SESSION_APIS
	static PrCallbackData *			sPrCallbackData;
#endif	// !PM_USE_SESSION_APIS
			
			

static void ValidateMargins
	(
		const Rect *		inPaperRect,		//	paper rectangle
		const Rect *		inMaxPageRect,		//	maximum printable area
		PageMarginRec *		ioMargins			//	page margins
	)
{
	const Fixed			kMinPageWidth = ( 36 << 16 ) ;		//	half inch
	const Fixed			kMinPageHeight = ( 36 << 16 ) ;		//	half inch
	PageMarginRec		minMargins ;
	PageMarginRec		maxMargins ;

	//	find the narrowest possible margins allowed by the specified print record
	minMargins.left = ( inMaxPageRect->left - inPaperRect->left ) << 16 ;
	minMargins.right = ( inPaperRect->right - inMaxPageRect->right ) << 16 ;
	minMargins.top = ( inMaxPageRect->top - inPaperRect->top ) << 16 ;
	minMargins.bottom = ( inPaperRect->bottom - inMaxPageRect->bottom ) << 16 ;

	//	and the widest possible margins
	maxMargins.left = ( ( ( inPaperRect->right - inPaperRect->left ) << 16 ) - kMinPageWidth ) / 2 ;
	maxMargins.top = ( ( ( inPaperRect->bottom - inPaperRect->top ) << 16 ) - kMinPageHeight ) / 2 ;
	maxMargins.right = maxMargins.left ;
	maxMargins.bottom = maxMargins.top ;

	//	make sure the margins aren't too wide or too narrow
	if ( ioMargins->top < minMargins.top )
	{
		ioMargins->top = minMargins.top ;
	}
	else if ( ioMargins->top > maxMargins.top )
	{
		ioMargins->top = maxMargins.top ;
	}

	if ( ioMargins->bottom < minMargins.bottom )
	{
		ioMargins->bottom = minMargins.bottom ;
	}
	else if ( ioMargins->bottom > maxMargins.bottom )
	{
		ioMargins->bottom = maxMargins.bottom ;
	}

	if ( ioMargins->left < minMargins.left )
	{
		ioMargins->left = minMargins.left ;
	}
	else if ( ioMargins->left > maxMargins.left )
	{
		ioMargins->left = maxMargins.left ;
	}

	if ( ioMargins->right < minMargins.right )
	{
		ioMargins->right = minMargins.right ;
	}
	else if ( ioMargins->right > maxMargins.right )
	{
		ioMargins->right = maxMargins.right ;
	}
}

static void CalculatePageRect
	(
		const Rect *				inPaperRect,
		const PageMarginRec *		inPageMargins,
		Rect *						outPageRect
	)
{
	Rect marginRect ;

	//	get page margins rounded to nearest integral values

	marginRect.left = FixRound ( inPageMargins->left ) ;
	marginRect.top = FixRound ( inPageMargins->top ) ;
	marginRect.right = FixRound ( inPageMargins->right ) ;
	marginRect.bottom = FixRound ( inPageMargins->bottom ) ;

	//	calculate page rect (the destination rectangle for printing)
	//	based on the paper rect and page margins
	//	all values are expressed in integral PostScript points (1/72 inch)

	outPageRect->left = ( inPaperRect->left + marginRect.left ) ;
	outPageRect->top = ( inPaperRect->top + marginRect.top ) ;
	outPageRect->right = outPageRect->left + ( ( inPaperRect->right - inPaperRect->left ) - ( marginRect.left + marginRect.right ) ) ;
	outPageRect->bottom = outPageRect-> top + ( ( inPaperRect->bottom - inPaperRect->top ) - ( marginRect.top + marginRect.bottom ) ) ;
}

#if TARGET_API_MAC_CARBON

static void PMRectToRect ( const PMRect * inPMRect, Rect * outRect )
{
	outRect->left = inPMRect->left;
	outRect->top = inPMRect->top;
	outRect->right = inPMRect->right;
	outRect->bottom = inPMRect->bottom;
}

#if PM_USE_SESSION_APIS

#pragma mark ====== Carbon Printing with Session APIs ======

//	CARBON PRINTING WITH SESSION APIS

//	Session APIs are the recommended APIs for Mac OS X and CarbonLib 1.1
//	AFAIK, they're not available in CarbonLib 1.0.x, though, so if you
//	need CarbonLib 1.0.x compatibility, you must use the "old" Carbon APIs

OSStatus DoWePageSetup (
			WEReference								we)
			
{
	PMPrintSession	printSession = NULL;
	PMPageFormat	pageFormat = NULL;
	Handle			oldFlattenedPageFormat = NULL;
	Handle			newFlattenedPageFormat = NULL;
	Boolean			accepted = FALSE;
	OSStatus			err;


			//	Begin a print session
			
	if ((err = PMCreateSession (&printSession)) != noErr)
		goto cleanup;

			//	Get the PMPageFormat object associated with our WASTE instance, if any
			
	if ((WEGetUserInfo (kPageFormatTag, (SInt32*)&oldFlattenedPageFormat, we) == noErr) &&
		 		(oldFlattenedPageFormat != NULL) )
		{
				//	Pre-existing flattened page format data: unflatten it
				
		if ((err = PMUnflattenPageFormat (oldFlattenedPageFormat, &pageFormat)) != noErr)
			goto cleanup;
			
		}
		
	else
		{
				//	no pre-existing page format: create one
				
		if ((err = PMCreatePageFormat (&pageFormat) ) != noErr)
			goto cleanup;
			
		if ((err = PMSessionDefaultPageFormat (printSession, pageFormat) ) != noErr)
			goto cleanup;
			
		}

			//	Display the page setup dialog
			
	if ( (err = PMSessionPageSetupDialog (printSession, pageFormat, &accepted) ) != noErr)
		goto cleanup;

	if (accepted)
		{
				//	flatten the page format so we can keep it around
				
		if ((err = PMFlattenPageFormat (pageFormat, &newFlattenedPageFormat)) != noErr)
			goto cleanup;
		
		if ((err = WESetUserInfo (kPageFormatTag, (SInt32)newFlattenedPageFormat, we)) != noErr)
			goto cleanup;
			
		oldFlattenedPageFormat = UnlockAndDispose (oldFlattenedPageFormat);
		newFlattenedPageFormat = NULL;
		
		}

			//	clear result code
			
	err = noErr;

cleanup:
			//	close the print session
	if ( printSession != NULL )
		{
		PMRelease ( printSession ) ;
		printSession = NULL ;
		}

			//	clean up
			
	newFlattenedPageFormat = UnlockAndDispose (newFlattenedPageFormat);

			//	return result code
			
	return err;
	
}		// end "DoWePageSetup"



OSStatus DoImagePageSetup ()
			
{
	PMPrintSession	printSession = NULL;
	PMPageFormat	pageFormat = NULL;
	Boolean			accepted = FALSE;
	OSStatus			err;


			//	Begin a print session
			
	if ((err = PMCreateSession (&printSession)) != noErr)
		goto cleanup;

				//	no pre-existing page format: create one
				
	if ((err = PMCreatePageFormat (&pageFormat) ) != noErr)
		goto cleanup;
		
	if ((err = PMSessionDefaultPageFormat (printSession, pageFormat) ) != noErr)
		goto cleanup;

			//	Display the page setup dialog
			
	if ( (err = PMSessionPageSetupDialog (printSession, pageFormat, &accepted) ) != noErr)
		goto cleanup;

			//	clear result code
			
	err = noErr;

cleanup:
			//	close the print session
	if  (printSession != NULL)
		{
		PMRelease (printSession);
		printSession = NULL;
		
		}

			//	return result code
			
	return err;
	
}		// end "DoImagePageSetup"



OSStatus DoWePrint ( WindowRef inWindow )
{
	WEReference				we ;
	WEPrintOptions			printOptions ;
	Handle					flattenedPageFormat = NULL ;
	PMPrintSession			printSession = NULL ;
	PMPageFormat			pageFormat = NULL ;
	PMPrintSettings			printSettings = NULL ;
	WEPrintSession			wasteSession = NULL ;
	GrafPtr					printPort = NULL ;
	PageMarginRecHandle		hPageMargins = NULL ;
	PageMarginRec			pageMargins ;
	PMRect					pmRect ;
	Rect					paperRect ;
	Rect					printableArea ;
	UInt32					firstSelectedPage ;
	UInt32					lastSelectedPage ;
	SInt32					pageIndex ;
	SInt32					pageCount ;
	Boolean					accepted = false ;
	OSStatus				err ;


	we = gOutputTextH;

			//	start a new print session
			
	if ( ( err = PMCreateSession ( & printSession ) ) != noErr )
		{
		goto cleanup;
		}

			//	get the PMPageFormat object associated with our WASTE instance, if any
			
	if ( ( WEGetUserInfo ( kPageFormatTag, ( SInt32 * ) & flattenedPageFormat, we ) == noErr ) &&
		 ( flattenedPageFormat != NULL ) )
		{
				//	pre-existing flattened page format data: unflatten it
		if ( ( err = PMUnflattenPageFormat ( flattenedPageFormat, & pageFormat ) ) != noErr )
			{
			goto cleanup ;
			}
		}
	else
		{
				//	no pre-existing page format: create one
		if ( ( err = PMCreatePageFormat ( & pageFormat ) ) != noErr )
			{
			goto cleanup ;
			}
		if ( ( err = PMSessionDefaultPageFormat ( printSession, pageFormat ) ) != noErr )
			{
			goto cleanup ;
			}
		}

			//	get the page margins record associated with our WASTE instance
	if ( ( err = WEGetUserInfo ( kPageMarginsTag, ( SInt32 * ) & hPageMargins, we ) ) != noErr )
		{
		goto cleanup ;
		}
	pageMargins = ** hPageMargins ;

			//	create a print settings object
	if ( ( err = PMCreatePrintSettings ( & printSettings ) ) != noErr )
		{
		goto cleanup ;
		}
	if ( ( err = PMSessionDefaultPrintSettings ( printSession, printSettings ) ) != noErr )
		{
		goto cleanup ;
		}

			//	display the print dialog
	if ( ( err = PMSessionPrintDialog ( printSession, printSettings, pageFormat, & accepted ) ) != noErr )
		{
		goto cleanup ;
		}

			//	dialog accepted?
	err = userCanceledErr ;
	if ( ! accepted )
		{
		goto cleanup ;
		}

			//	get paper rectangle, rounded to nearest integral values
	if ( ( err = PMGetAdjustedPaperRect ( pageFormat, & pmRect ) ) != noErr )
		{
		goto cleanup ;
		}
	PMRectToRect ( & pmRect, & paperRect ) ;

			//	get printable page rectangle (actual page rect can't be larger than this)
	if ( ( err = PMGetAdjustedPageRect ( pageFormat, & pmRect ) ) != noErr )
		{
		goto cleanup ;
		}
	PMRectToRect ( & pmRect, & printableArea ) ;

			//	validate page margins
			
	ValidateMargins ( & paperRect, & printableArea, & pageMargins ) ;

			//	zero out the print options record (this is important for future compatibility!)
	
	BlockZero ( & printOptions, sizeof ( printOptions ) ) ;

			//	calculate page rectangle
			
	CalculatePageRect ( & paperRect, & pageMargins, & printOptions . pageRect ) ;

			//	copy it to a PMRect for PMBeginPage
			
	pmRect . left = printOptions . pageRect . left ;
	pmRect . top = printOptions . pageRect . top ;
	pmRect . right = printOptions . pageRect . right ;
	pmRect . bottom = printOptions . pageRect . bottom ;

			//	create a WASTE print session
			
	if ( ( err = WENewPrintSession ( & printOptions, we, & wasteSession ) ) != noErr )
		{
		goto cleanup ;
		}

			//	get page count
			
	pageCount = WECountPages ( wasteSession ) ;

			//	get page range selected by the user
			
	if ( ( err = PMGetFirstPage ( printSettings, & firstSelectedPage ) ) != noErr )
		{
		goto cleanup ;
		}
		
	if ( ( err = PMGetLastPage ( printSettings, & lastSelectedPage ) ) != noErr )
		{
		goto cleanup ;
		}

			//	sanity checks
			
	if ( lastSelectedPage > pageCount )
		{
		lastSelectedPage = pageCount ;
		}

	//	begin document
	
	if ( ( err = PMSessionBeginDocument ( printSession, printSettings, pageFormat ) ) != noErr )
		{
		goto cleanup ;
		}

			//	WASTE page indices are zero-based, while Printing Manager's are one-based
			
	firstSelectedPage -= 1 ;
	lastSelectedPage -= 1 ;

			//	page loop
	for ( pageIndex = firstSelectedPage ; pageIndex <= lastSelectedPage ; pageIndex ++ )
		{
				//	begin page
		if ( ( err = PMSessionBeginPage ( printSession, pageFormat, & pmRect ) ) != noErr )
			{
			goto cleanup ;
			}

		//	get the graphics port associated with the print context
		if ( ( err = PMSessionGetGraphicsContext ( printSession, NULL, (void**)&printPort ) ) != noErr )
		{
			goto cleanup ;
		}

		//	print this page
		if ( ( err = WEPrintPage ( pageIndex, printPort, NULL, wasteSession ) ) != noErr )
		{
			goto cleanup ;
		}

		//	end page
		if ( ( err = PMSessionEndPage ( printSession ) ) != noErr )
		{
			goto cleanup ;
		}
	}

	//	end document
	if ( ( err = PMSessionEndDocument ( printSession ) ) != noErr )
	{
		goto cleanup ;
	}

	//	clear result code
	err = noErr ;

cleanup :
	//	clean up
	if ( wasteSession )
	{
		WEDisposePrintSession ( wasteSession ) ;
		wasteSession = NULL ;
	}

	if ( printSettings )
	{
		PMRelease ( printSettings ) ;
		printSettings = NULL ;
	}

	if ( printSession )
	{
		PMRelease ( printSession ) ;
		printSession = NULL ;
	}

	//	return result code
	return err ;
	
}		// end "DoWePrint"
//#endif	// 
#else		// !PM_USE_SESSION_APIS

#pragma mark ====== Carbon Printing with the Old APIs ======

//	CARBON PRINTING WITH THE OLD (SESSIONLESS) APIS

//	Session APIs are the recommended APIs for Mac OS X and CarbonLib 1.1
//	AFAIK, they're not available in CarbonLib 1.0.x, though, so if you
//	need CarbonLib 1.0.x compatibility, you must use the "old" Carbon APIs

OSStatus DoWePageSetup (
			WEReference								we)
			
{
	PMPageFormat	pageFormat = NULL;
	Handle			oldFlattenedPageFormat = NULL;
	Handle			newFlattenedPageFormat = NULL;
	Boolean			accepted = FALSE;
	OSStatus		err ;

	if ( ( err = PMBegin ( ) ) != noErr )
	{
		goto cleanup ;
	}

	//	get the PMPageFormat object associated with our WASTE instance, if any
	if ( ( WEGetUserInfo ( kPageFormatTag, ( SInt32 * ) & oldFlattenedPageFormat, we ) == noErr ) &&
		 ( oldFlattenedPageFormat != NULL ) )
	{
		//	pre-existing flattened page format data: unflatten it
		if ( ( err = PMUnflattenPageFormat ( oldFlattenedPageFormat, & pageFormat ) ) != noErr )
		{
			goto cleanup ;
		}
	}
	else
	{
		//	no pre-existing page format: create one
		if ( ( err = PMNewPageFormat ( & pageFormat ) ) != noErr )
		{
			goto cleanup ;
		}
		if ( ( err = PMDefaultPageFormat ( pageFormat ) ) != noErr )
		{
			goto cleanup ;
		}
	}

	//	display the page setup dialog
	if ( ( err = PMPageSetupDialog ( pageFormat, & accepted ) ) != noErr )
	{
		goto cleanup ;
	}

	if ( accepted )
	{
		//	flatten the page format so we can keep it around
		if ( ( err = PMFlattenPageFormat ( pageFormat, & newFlattenedPageFormat ) ) != noErr )
		{
			goto cleanup ;
		}
		if ( ( err = WESetUserInfo ( kPageFormatTag, ( SInt32 ) newFlattenedPageFormat, we ) ) != noErr )
		{
			goto cleanup ;
		}
//		ForgetHandle ( & oldFlattenedPageFormat ) ;
		oldFlattenedPageFormat = UnlockAndDispose (oldFlattenedPageFormat);
		newFlattenedPageFormat = NULL ;
	}

	//	clear result code
	err = noErr ;

cleanup :
	//	clean up
	PMEnd ( ) ;
//	ForgetHandle ( & newFlattenedPageFormat ) ;
	newFlattenedPageFormat = UnlockAndDispose (newFlattenedPageFormat);

	//	return result code
	return err ;
}

OSStatus DoWePrint ( WindowRef inWindow )
{
	WEReference				we ;
	WEPrintOptions			printOptions ;
	Handle					flattenedPageFormat = NULL ;
	PMPageFormat			pageFormat = NULL ;
	PMPrintSettings			printSettings = NULL ;
	WEPrintSession			wasteSession = NULL ;
	PMPrintContext			printContext = NULL ;
	GrafPtr					printPort = NULL ;
	PageMarginRecHandle		hPageMargins = NULL ;
	PageMarginRec			pageMargins ;
	PMRect					pmRect ;
	Rect					paperRect ;
	Rect					printableArea ;
	UInt32					firstSelectedPage ;
	UInt32					lastSelectedPage ;
	UInt32					pageIndex ;
	UInt32					pageCount ;
	Boolean					accepted = false ;
	OSStatus				err ;

	we = gOutputTextH;

	if ( ( err = PMBegin ( ) ) != noErr )
	{
		goto cleanup ;
	}

	//	get the PMPageFormat object associated with our WASTE instance, if any
	if ( ( WEGetUserInfo ( kPageFormatTag, ( SInt32 * ) & flattenedPageFormat, we ) == noErr ) &&
		 ( flattenedPageFormat != NULL ) )
	{
		//	pre-existing flattened page format data: unflatten it
		if ( ( err = PMUnflattenPageFormat ( flattenedPageFormat, & pageFormat ) ) != noErr )
		{
			goto cleanup ;
		}
	}
	else
	{
		//	no pre-existing page format: create one
		if ( ( err = PMNewPageFormat ( & pageFormat ) ) != noErr )
		{
			goto cleanup ;
		}
		if ( ( err = PMDefaultPageFormat ( pageFormat ) ) != noErr )
		{
			goto cleanup ;
		}
	}

	//	get the page margins record associated with our WASTE instance
	if ( ( err = WEGetUserInfo ( kPageMarginsTag, ( SInt32 * ) & hPageMargins, we ) ) != noErr )
	{
		goto cleanup ;
	}
	pageMargins = ** hPageMargins ;

	//	create a print settings object
	if ( ( err = PMNewPrintSettings ( & printSettings ) ) != noErr )
	{
		goto cleanup ;
	}
	if ( ( err = PMDefaultPrintSettings ( printSettings ) ) != noErr )
	{
		goto cleanup ;
	}

	//	display the print dialog
	if ( ( err = PMPrintDialog ( printSettings, pageFormat, & accepted ) ) != noErr )
	{
		goto cleanup ;
	}

	//	dialog accepted?
	err = userCanceledErr ;
	if ( ! accepted )
	{
		goto cleanup ;
	}

	//	get paper rectangle, rounded to nearest integral values
	if ( ( err = PMGetAdjustedPaperRect ( pageFormat, & pmRect ) ) != noErr )
	{
		goto cleanup ;
	}
	PMRectToRect ( & pmRect, & paperRect ) ;

	//	get printable page rectangle (actual page rect can't be larger than this)
	if ( ( err = PMGetAdjustedPageRect ( pageFormat, & pmRect ) ) != noErr )
	{
		goto cleanup ;
	}
	PMRectToRect ( & pmRect, & printableArea ) ;

	//	validate page margins
	ValidateMargins ( & paperRect, & printableArea, & pageMargins ) ;

	//	zero out the print options record (this is important for future compatibility!)
	BlockZero ( & printOptions, sizeof ( printOptions ) ) ;

	//	calculate page rectangle
	CalculatePageRect ( & paperRect, & pageMargins, & printOptions . pageRect ) ;

	//	copy it to a PMRect for PMBeginPage
	pmRect . left = printOptions . pageRect . left ;
	pmRect . top = printOptions . pageRect . top ;
	pmRect . right = printOptions . pageRect . right ;
	pmRect . bottom = printOptions . pageRect . bottom ;

	//	create a WASTE print session
	if ( ( err = WENewPrintSession ( & printOptions, we, & wasteSession ) ) != noErr )
	{
		goto cleanup ;
	}

	//	get page count
	pageCount = WECountPages ( wasteSession ) ;

	//	get page range selected by the user
	if ( ( err = PMGetFirstPage ( printSettings, & firstSelectedPage ) ) != noErr )
	{
		goto cleanup ;
	}
	if ( ( err = PMGetLastPage ( printSettings, & lastSelectedPage ) ) != noErr )
	{
		goto cleanup ;
	}

	//	sanity checks
	if ( lastSelectedPage > pageCount )
	{
		lastSelectedPage = pageCount ;
	}

	//	set up the print context
	if ( ( err = PMBeginDocument ( printSettings, pageFormat, & printContext ) ) != noErr )
	{
		goto cleanup ;
	}

	//	WASTE page indices are zero-based, while Printing Manager's are one-based
	firstSelectedPage -= 1 ;
	lastSelectedPage -= 1 ;

	//	page loop
	for ( pageIndex = firstSelectedPage ; pageIndex <= lastSelectedPage ; pageIndex ++ )
	{
		if ( ( err = PMBeginPage ( printContext, & pmRect ) ) != noErr )
		{
			goto cleanup ;
		}

		//	get the graphics port associated with the print context
		if ( ( err = PMGetGrafPtr ( printContext, & printPort ) ) != noErr )
		{
			goto cleanup ;
		}

		if ( ( err = WEPrintPage ( pageIndex, printPort, NULL, wasteSession ) ) != noErr )
		{
			goto cleanup ;
		}

		if ( ( err = PMEndPage ( printContext ) ) != noErr )
		{
			goto cleanup ;
		}
	}

	//	clear result code
	err = noErr ;

cleanup :
	//	clean up
	if ( wasteSession )
	{
		WEDisposePrintSession ( wasteSession ) ;
		wasteSession = NULL ;
	}

	if ( printContext )
	{
		PMEndDocument ( printContext ) ;
		printContext = NULL ;
	}

	if ( printSettings )
	{
		PMDisposePrintSettings ( printSettings ) ;
		printSettings = NULL ;
	}

	PMEnd ( ) ;

	//	return result code
	return err ;
	
}	// end "DoWePrint"
#endif /*PM_USE_SESSION_APIS*/

#else	// !TARGET_API_MAC_CARBON

#pragma mark ====== Classic Printing ======
//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoWePageSetup
//
//	Software purpose:	The purpose of this routine is to call the page
// 						set up dialog for the text output window.	
//							This routine is for CLASSIC PRINTING.
//
//							NOTE.  Most of the code for this routine came from
//							WASTE's example code.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			WASTE						Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 07/18/2001	

OSStatus DoWePageSetup (
			WEReference								we)
	
{
	THPrint								hPrintRecord = NULL ;
	PageMarginRecHandle				hPageMargins = NULL ;
	Boolean								accepted = false ;
	OSStatus								err;


	PrOpen ();
	if ( ( err = PrError () ) != noErr )
		{
		goto cleanup;
		}

			//	Get the print record associated with our WASTE instance, if any
			
	if ( (WEGetUserInfo ( 'PREC', (SInt32*) &hPrintRecord, gOutputTextH) == noErr ) &&
		 ( hPrintRecord != NULL ) )
		{
				//	Pre-existing print record: validate it
				
		PrValidate (hPrintRecord);
		if ( ( err = PrError ( ) ) != noErr )
			{
			goto cleanup;
			}
			
		}
		
	else
		{
				//	No pre-existing print record: create one
		hPrintRecord = ( THPrint ) NewHandleClear ( sizeof (TPrint) ) ;
		if ( ( err = MemError ( ) ) != noErr )
			{
			goto cleanup ;
			}
		PrintDefault ( hPrintRecord ) ;
		if ( ( err = PrError ( ) ) != noErr )
			{
			goto cleanup;
			}
		}

			//	Get the page margins record associated with our WASTE instance
			
	if ( (err = WEGetUserInfo ('MARG', (SInt32*) &hPageMargins, gOutputTextH) ) != noErr)
		{
		goto cleanup;
		}

			//	Display the page setup dialog
			
	accepted = DoPrintDialog (hPrintRecord, hPageMargins, kPageSetupDialog);
	if ( ( err = PrError ( ) ) != noErr )
		{
		goto cleanup;
		}

	if (accepted)
		{
		//	keep the print record around
		if ( ( err = WESetUserInfo ('PREC', (SInt32)hPrintRecord, gOutputTextH) ) != noErr)
			{
			goto cleanup;
			}
			
		}

			//	Clear result code
			
	err = noErr ;

cleanup :
			//	Clean up
	PrClose ();

			//	Return result code
	return err;
	
}		// end "DoWePageSetup"



OSStatus DoWePrint (
				WindowRef 								inWindow)
				
{
	WEReference				we ;
	WEPrintOptions			printOptions ;
	THPrint					hPrintRecord = NULL ;
	WEPrintSession			wasteSession = NULL ;
	TPPrPortRef				printContext = NULL ;
	GrafPtr					printPort = NULL ;
	GrafPtr					savePort = NULL ;
	PageMarginRecHandle		hPageMargins = NULL ;
	PageMarginRec			pageMargins ;
	Rect					paperRect ;
	Rect					printableArea ;
	UInt32					firstSelectedPage ;
	UInt32					lastSelectedPage ;
	UInt32					pageIndex ;
	UInt32					pageCount ;
	Boolean					accepted = false ;
	OSStatus				err ;


	we = gOutputTextH;

			//	save the current graphics port
			
	GetPort (&savePort);

			//	open the print manager
			
	PrOpen ( ) ;
	if ( ( err = PrError ( ) ) != noErr )
		{
		goto cleanup ;
		}

			//	get the print record associated with our WASTE instance, if any
			
	if ( ( WEGetUserInfo ( kPrintRecordTag, ( SInt32 * ) &hPrintRecord, we ) == noErr ) &&
		 ( hPrintRecord != NULL ) )
	{
		//	pre-existing print record: validate it
		PrValidate ( hPrintRecord ) ;
		if ( ( err = PrError ( ) ) != noErr )
		{
			goto cleanup ;
		}
	}
	else
			goto cleanup ;

	//	get the page margins record associated with our WASTE instance
	if ( ( err = WEGetUserInfo ( kPageMarginsTag, ( SInt32 * ) & hPageMargins, we ) ) != noErr )
	{
		goto cleanup ;
	}
	pageMargins = ** hPageMargins ;

	//	display the print dialog
	accepted = DoPrintDialog ( hPrintRecord, NULL, kPrintDialog ) ;
	if ( ( err = PrError ( ) ) != noErr )
	{
		goto cleanup ;
	}

	//	dialog accepted?
	err = userCanceledErr ;
	if ( ! accepted )
	{
		goto cleanup ;
	}

	//	get paper rectangle
	paperRect = ( * hPrintRecord ) -> rPaper ;

	//	get printable page rectangle (actual page rect can't be larger than this)
	printableArea = ( * hPrintRecord ) -> prInfo . rPage ;

	//	validate page margins
	ValidateMargins ( & paperRect, & printableArea, & pageMargins ) ;

	//	zero out the print options record (this is important for future compatibility!)
	BlockZero ( & printOptions, sizeof ( printOptions ) ) ;

	//	calculate page rectangle
	CalculatePageRect ( & paperRect, & pageMargins, & printOptions . pageRect ) ;

	//	create a WASTE print session
	if ( ( err = WENewPrintSession ( & printOptions, we, & wasteSession ) ) != noErr )
	{
		goto cleanup ;
	}

	//	get page count
	pageCount = WECountPages ( wasteSession ) ;

	//	get page range selected by the user
	firstSelectedPage = ( * hPrintRecord ) -> prJob . iFstPage ;
	lastSelectedPage = ( * hPrintRecord ) -> prJob . iLstPage ;

	//	sanity checks
	if ( lastSelectedPage > pageCount )
	{
		lastSelectedPage = pageCount ;
	}

	//	set up the print context
	printContext = PrOpenDoc ( hPrintRecord, NULL, NULL ) ;
	if ( ( err = PrError ( ) ) != noErr )
	{
		goto cleanup ;
	}

	//	get the graphics port associated with the print context
	printPort = & printContext -> gPort ;

	//	WASTE page indices are zero-based, while Printing Manager's are one-based
	firstSelectedPage -= 1 ;
	lastSelectedPage -= 1 ;

	//	page loop
	for ( pageIndex = firstSelectedPage ; pageIndex <= lastSelectedPage ; pageIndex ++ )
	{
		PrOpenPage ( printContext, NULL ) ;
		if ( ( err = PrError ( ) ) != noErr )
		{
			goto cleanup ;
		}

		if ( ( err = WEPrintPage ( pageIndex, printPort, NULL, wasteSession ) ) != noErr )
		{
			goto cleanup ;
		}

		PrClosePage ( printContext ) ;
		if ( ( err = PrError ( ) ) != noErr )
		{
			goto cleanup ;
		}
	}

	//	clear result code
	err = noErr ;

cleanup :
	//	clean up
	if ( wasteSession )
	{
		WEDisposePrintSession ( wasteSession ) ;
		wasteSession = NULL ;
	}

	if ( printContext )
	{
		PrCloseDoc ( printContext ) ;
		printContext = NULL ;
	}

	PrClose ( ) ;

	//	restore the original graphics port
	SetPort (savePort);

	//	return result code
	return err ;
	
}		// end "DoWePrint"

//#endif

static void SetMarginField ( 
				SInt16 									inWhichSide, 
				Fixed 									inMargin, 
				PrCallbackData* 						inData)
				
{
	Str255					theString;
	
	
	if ( MeasureToString ( inMargin, inData -> preferredUnit, theString ) == noErr )
		{
//		SetEditFieldString ( inData -> theDialog, inWhichSide + inData -> numItems + kItemFirstField, theString ) ;
		LoadDItemString (inData->theDialog, 
								(SInt16)(inWhichSide+inData->numItems+kItemFirstField), 
								&theString);
		}
		
}		// end "SetMarginField"



static OSStatus GetMarginField ( SInt16 inWhichSide, Fixed * outMargin, PrCallbackData * inData )
{
	Str255						theString;
	
	DialogItemType				itemType;
	Handle						itemHandle;
	Rect							itemRect;

//	GetEditFieldString (inData->theDialog, inWhichSide+inData->numItems+kItemFirstField, theString);

	GetDialogItem (inData->theDialog, 
							inWhichSide+inData->numItems+kItemFirstField, 
							&itemType, 
							&itemHandle, 
							&itemRect);
							
	GetDialogItemText (itemHandle, theString);
	
	return StringToMeasure (theString, inData->preferredUnit, outMargin);
	
}		// end "GetMarginField"



static pascal Boolean FilterPrintDialogEvent ( DialogRef inDialog, EventRecord * ioEvent, DialogItemIndex * ioDialogItem )
{
	ModalFilterUPP filter = GetMyStandardDialogFilter ();

	//	we first call our own filter, then (if the event wasn't handled), the original filter proc
	if ( CallModalFilterProc ( filter, inDialog, ioEvent, ioDialogItem ) )
	{
		return true ;
	}

	if ( ( filter = sPrCallbackData -> origFilterProc ) != NULL )
	{
		return CallModalFilterProc ( filter, inDialog, ioEvent, ioDialogItem ) ;
	}

	return false;
	
}		// end "FilterPrintDialogEvent"



static pascal void HandlePrintDialogItem ( DialogRef inDialog, DialogItemIndex inDialogItem )
{
	PrCallbackData *	data = sPrCallbackData ;
	Fixed				margin ;
	SInt16				whichSide ;

	//	if one of the standard items was hit, call the original handler
	if ( ( inDialogItem <= data -> numItems ) && ( data -> origItemProc != NULL ) )
	{
		CallUserItemProc ( data -> origItemProc, inDialog, inDialogItem ) ;
	}

	//	if one of our edit fields has just been exited, check its contents
	if ( ( inDialogItem > 0 ) && ( inDialogItem != data -> recentEditField ) )
	{
		whichSide = ( data -> recentEditField - data -> numItems ) - kItemFirstField ;

		//	make sure we only validate our own fields
		if ( ( whichSide >= 0 ) && ( whichSide <= 3 ) )
		{
			if ( GetMarginField ( whichSide, & margin, data ) != noErr )
			{
				//	bad string: put the old string back and select the bad field
				SysBeep ( 0 ) ;
				SetMarginField ( whichSide, data -> margins [ whichSide ], data ) ;
				SelectDialogItemText ( inDialog, data -> recentEditField, 0, 0x7FFF ) ;
			}
			else
			{
				//	if the string is valid, save the margin value
				data -> margins [ whichSide ] = margin ;
			}
		}

		//	remember current edit field
		data -> recentEditField = GetDialogKeyboardFocusItem ( inDialog ) ;
	}
	
}		// end "HandlePrintDialogItem"



static pascal TPPrDlgRef InitPrintDialog ( THPrint )
{
	static ModalFilterUPP		eventFilter = NULL ;
	static UserItemUPP			itemHandler = NULL ;
	PrCallbackData *			data = sPrCallbackData ;
	TPrDlg *					prDialog = ( TPrDlg * ) data -> theDialog ;
	Handle						additionalItems ;
	SInt16						whichSide ;

	if ( eventFilter == NULL )
	{
		eventFilter = NewModalFilterProc ( FilterPrintDialogEvent ) ;
	}
	if ( itemHandler == NULL )
	{
		itemHandler = NewUserItemProc ( HandlePrintDialogItem ) ;
	}

	//	save original event filter and item handler functions
	data -> origFilterProc = prDialog -> pFltrProc ;
	data -> origItemProc = prDialog -> pItemProc ;

	//	count dialog items
	data -> numItems = CountDITL ( data -> theDialog ) ;

	//	install our own event filter
	prDialog -> pFltrProc = eventFilter ;

	if ( data -> additionalItemsID != 0 )
	{
		//	additional processing for the page setup dialog
		//	install our own item handler
		prDialog -> pItemProc = itemHandler ;

		//	get additional item list
		if ( ( additionalItems = GetResource ( kTypeDialogItemList, data -> additionalItemsID ) ) != NULL )
		{
			//	append the extra items at the bottom of the standard dialog
			DetachResource ( additionalItems ) ;
			AppendDITL ( data -> theDialog, additionalItems, appendDITLBottom ) ;
			DisposeHandle ( additionalItems ) ;

			//	the default unit is in (US) or cm (metric world)
			data -> preferredUnit = IsMetric ( ) ? kUnitCentimeter : kUnitInch ;

			//	fill in the margin edit fields
			for ( whichSide = 0 ; whichSide < 4 ; whichSide ++ )
			{
				SetMarginField ( whichSide, data -> margins [ whichSide ], data ) ;
			}
		}
	}

	return prDialog ;
	
}		// end "TPPrDlgRef"



Boolean DoPrintDialog (
				THPrint 								inPrintRecord, 
				PageMarginRecHandle 					inMargins, 
				PrintDialogStyle 						inStyle)
{
	static PDlgInitUPP		initer = NULL ;
	PrCallbackData			data ;
	Boolean					dialogResult ;

	if ( initer == NULL )
	{
		initer = NewPDlgInitProc (InitPrintDialog);
	}

	//	prepare parameter block for callbacks
	BlockZero ( & data, sizeof ( data ) ) ;
	sPrCallbackData = & data ;
	switch ( inStyle )
	{
		case kPrintDialog :
		{
			data . theDialog = ( DialogRef ) PrJobInit ( inPrintRecord ) ;
			break ;
		}

		case kPageSetupDialog :
		{
			data . theDialog = ( DialogRef ) PrStlInit ( inPrintRecord ) ;
			data . additionalItemsID = kDITLPageSetupExtras ;
			if ( inMargins )
			{
				BlockMoveData ( * inMargins, & data . margins, sizeof ( PageMarginRec ) ) ;
			}
			break ;
		}
	}

	if ( data . theDialog == NULL )
	{
		return false ;		//	emergency exit
	}

	//	display the dialog
	dialogResult = PrDlgMain ( inPrintRecord, initer ) ;

	//	if the dialog was accepted, copy back the modified margins
	if ( dialogResult && inMargins )
	{
		BlockMoveData ( & data . margins, * inMargins, sizeof ( PageMarginRec ) ) ;
	}

	return dialogResult;
	
}		// end "DoPrintDialog"


/*
static pascal Boolean MyStandardDialogFilter ( DialogRef inDialog, EventRecord * ioEvent, DialogItemIndex * ioDialogItem )
{
	GrafPtr				savePort;
	ModalFilterUPP		stdFilter = NULL;
	Boolean				retval = false;

	// set up the port
	GetPort( &savePort );
	SetPortDialogPort( inDialog );

	// 	intercept window events directed to windows behind the dialog
	if ( ( ioEvent -> what == updateEvt ) || ( ioEvent -> what == activateEvt ) )
	{
		if ( ( ( WindowRef ) ioEvent -> message) != GetDialogWindow ( inDialog ) )
		{
			DoWindowEvent ( ioEvent ) ;
		}
	}

	// is the default item a pushbutton?
	if ( GetDialogItemType ( inDialog, GetDialogDefaultItem ( inDialog ) ) == kButtonDialogItem )
	{
		// yes, so tell the Dialog Manager to care about its outline
		SetDialogDefaultItem ( inDialog, GetDialogDefaultItem ( inDialog ) ) ;
	}

	// this is something not in the original WASTE Demo App, but in the work that I've done
	// on my own projects, I've found it useful and helpful.

	// let's also make sure the cancel button can be handled...now, the cancel button
	// should be dialog item #2.  So, we get dialog item #2, check if it's a button.
	// if it fills these 2 criteria, it's cancel.  Even if the default item and the
	// cancel item are the same, still let them both be set this way so whatever keyboard
	// keys sthe user presses will be handled properly

	// pass the number "2" to GetDialogItemType...don't check for the cancel item (cause tho
	// cancel is defined as 2, we're not looking for cancel, we're looking for dialog item #2
	// this is just more readable code.

	// remember, this assumes that your cancel item will be item #2 (or at least the item
	// that you want to use for cancelling is #2), and that there are at least 2 items in
	// the dialog to begin with!

	if ( GetDialogItemType ( inDialog, kStdCancelItemIndex ) == kButtonDialogItem )
	{
		SetDialogCancelItem ( inDialog, kStdCancelItemIndex ) ;
	}

	//	call the standard Dialog Manager filter procedure

	if ( ( GetStdFilterProc( & stdFilter ) == noErr ) && stdFilter )
	{
		retval = CallModalFilterProc ( stdFilter, inDialog, ioEvent, ioDialogItem ) ;
	}

	//	restore the port
	SetPort ( savePort ) ;

	return retval ;
}		// "MyStandardDialogFilter"
*/


ModalFilterUPP GetMyStandardDialogFilter ( void )
{
	static ModalFilterUPP sFilterUPP = NULL ;

	if ( sFilterUPP == NULL )
	{
//		sFilterUPP = NewModalFilterProc (MyStandardDialogFilter);
		sFilterUPP = NewModalFilterProc (ProcessorDialogFilter);
	}

	return sFilterUPP ;
}
#endif	// TARGET_API_MAC_CARBON else...

#endif	// #if !use_mlte_for_text_window
