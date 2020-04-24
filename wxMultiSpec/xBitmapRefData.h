//	File:						XBitmapRefData.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/22/2019
//
//	Language:				C++
//
//	System:					MacOS Operating Systems
//
//	Brief description:	This file is the definition for the wxBitmapRefData class
//								It comes from the wxWidgets code. It is needed to get access
//								to some of the parameters in this class
//								It is only needed for the MacOS version of MultiSpec
//
//------------------------------------------------------------------------------------

/*
	// For wxWidgets 3.1.1
	class WXDLLEXPORT wxBitmapRefData: public wxGDIRefData
	{
		 friend class WXDLLIMPEXP_FWD_CORE wxIcon;
		 friend class WXDLLIMPEXP_FWD_CORE wxCursor;
	public:
		 wxBitmapRefData (int width , int height , int depth, double logicalscale = 1.0);
		 wxBitmapRefData (CGContextRef context);
		 wxBitmapRefData (CGImageRef image, double scale);
		 wxBitmapRefData ();
		 wxBitmapRefData (const wxBitmapRefData &tocopy);

		 virtual ~wxBitmapRefData ();

		 virtual bool IsOk() const wxOVERRIDE { return m_ok; }

		 void Free();
		 void SetOk( bool isOk) { m_ok = isOk; }

		 void SetWidth( int width ) { m_width = width; }
		 void SetHeight( int height ) { m_height = height; }
		 void SetDepth( int depth ) { m_depth = depth; }

		 int GetWidth() const { return m_width; }
		 int GetHeight() const { return m_height; }
		 int GetDepth() const { return m_depth; }
		 double GetScaleFactor() const { return m_scaleFactor; }
		 void *GetRawAccess() const;
		 void *BeginRawAccess();
		 void EndRawAccess();

		 bool HasAlpha () const { return m_hasAlpha; }
		 void UseAlpha ( bool useAlpha );

		 bool IsTemplate() const { return m_isTemplate; }
		 void SetTemplate(bool is) { m_isTemplate = is; }

	public:
	#if wxUSE_PALETTE
		 wxPalette     m_bitmapPalette;
	#endif // wxUSE_PALETTE

		 wxMask *      m_bitmapMask; // Optional mask
		 CGImageRef    CreateCGImage() const;

		 // returns true if the bitmap has a size that
		 // can be natively transferred into a true icon
		 // if no is returned GetIconRef will still produce
		 // an icon but it will be generated via a PICT and
		 // rescaled to 16 x 16
		 bool          HasNativeSize();

	#ifndef __WXOSX_IPHONE__
		 // caller should increase ref count if needed longer
		 // than the bitmap exists
		 IconRef       GetIconRef();
	#endif

		 CGContextRef  GetBitmapContext() const;

		 int           GetBytesPerRow() const { return m_bytesPerRow; }
		 private :
		 bool Create(int width , int height , int depth, double logicalscale);
		 bool Create( CGImageRef image, double scale );
		 bool Create( CGContextRef bitmapcontext);
		 void Init();

		 int           m_width;
		 int           m_height;
		 int           m_bytesPerRow;
		 int           m_depth;
		 bool          m_hasAlpha;
		 wxMemoryBuffer m_memBuf;
		 int           m_rawAccessCount;
		 bool          m_ok;
		 mutable CGImageRef    m_cgImageRef;
		 bool          m_isTemplate;

	#ifndef __WXOSX_IPHONE__
		 IconRef       m_iconRef;
	#endif

		 CGContextRef  m_hBitmap;
		 double        m_scaleFactor;
	};
*/

		// For wxWidgets 3.1.2
class WXDLLEXPORT wxBitmapRefData: public wxGDIRefData
{
    friend class WXDLLIMPEXP_FWD_CORE wxIcon;
    friend class WXDLLIMPEXP_FWD_CORE wxCursor;
public:
    wxBitmapRefData (int width , int height , int depth, double logicalscale = 1.0);
    wxBitmapRefData (CGContextRef context);
    wxBitmapRefData (CGImageRef image, double scale);
    wxBitmapRefData (WXImage image);
    wxBitmapRefData ();
    wxBitmapRefData (const wxBitmapRefData &tocopy);

    virtual ~wxBitmapRefData();

    bool IsOk() const;

    void Free();

    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;
    int GetBytesPerRow() const;
    bool HasAlpha() const;
    WXImage GetImage() const;
    double GetScaleFactor() const { return m_scaleFactor; }

    const void *GetRawAccess() const;
    void *GetRawAccess();
    void *BeginRawAccess();
    void EndRawAccess();

    void UseAlpha( bool useAlpha );

    bool IsTemplate() const { return m_isTemplate; }
    void SetTemplate(bool is) { m_isTemplate = is; }

public:
#if wxUSE_PALETTE
    wxPalette     m_bitmapPalette;
#endif // wxUSE_PALETTE

    wxMask *      m_bitmapMask; // Optional mask
    CGImageRef    CreateCGImage() const;

    // returns true if the bitmap has a size that
    // can be natively transferred into a true icon
    // if no is returned GetIconRef will still produce
    // an icon but it will be generated via a PICT and
    // rescaled to 16 x 16
    bool          HasNativeSize();

#if wxOSX_USE_ICONREF
    // caller should increase ref count if needed longer
    // than the bitmap exists
    IconRef       GetIconRef();
#endif

    CGContextRef  GetBitmapContext() const;

    void SetSelectedInto(wxDC *dc);
    wxDC *GetSelectedInto() const;

private :
    bool Create(int width , int height , int depth, double logicalscale);
    bool Create( CGImageRef image, double scale );
    bool Create( CGContextRef bitmapcontext);
    bool Create( WXImage image);
    void Init();

    void EnsureBitmapExists() const;

    void FreeDerivedRepresentations();

    WXImage    m_nsImage;
    int           m_rawAccessCount;
    mutable CGImageRef    m_cgImageRef;
    bool          m_isTemplate;

#if wxOSX_USE_ICONREF
    IconRef       m_iconRef;
#endif

    wxCFRef<CGContextRef>  m_hBitmap;
    double        m_scaleFactor;
    wxDC*         m_selectedInto;
};

