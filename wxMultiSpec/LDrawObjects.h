// LDrawObjects.h - interface for CMDrawObject and derivatives 
// 
// Revised by Larry Biehl on 06/20/2017
// 
#ifndef __LDRAWOBJ_H__
#define __LDRAWOBJ_H__

class CMImageView;
class CMImageDoc;

//----------------------------------------------------------------------------

// CMDrawObject - base class for all 'drawable objects'

class CMDrawObject : public wxObject {
protected:
    CMDrawObject();

public:
    CMDrawObject(
            const wxRect& position);

    virtual ~CMDrawObject();

    virtual void Draw(
            CDC* pDC,
            CMImageView* imageViewCPtr);

    void Invalidate(void);

    virtual void OnOpen(
            CMImageView* pView);

    virtual void Remove();

#ifdef _DEBUG
    void AssertValid();
#endif       

    CMImageDoc* m_document;

    Boolean m_drawFlag;

protected:
    wxRect m_position;

    LOGPEN m_logpen;

    LOGBRUSH m_logbrush;

    static Rect s_viewRect;

}; // end "class CMDrawObject"


//----------------------------------------------------------------------------

// CMOutlineArea - class for outlining areas

class CMOutlineArea : public CMDrawObject {
public:
    CMOutlineArea();

    CMOutlineArea(const wxRect& position);

    virtual ~CMOutlineArea();

    void Clear(CMImageView* imageViewCPtr);

    virtual void Draw(CDC* pDC, CMImageView* imageViewCPtr);
    /*
                    Boolean						GetLineColumnSelection (
                                                                                            CMImageView*						imageViewCPtr,
                                                                                            LongRect*							selectionRectanglePtr,
                                                                                            Boolean								clearSelectionFlag,
                                                                                            Boolean								useThresholdFlag,
                                                                                            Boolean								adjustToBaseImageFlag);
											
                    Boolean						GetLineColumnSelection (
                                                                                            LongRect*							selectionRectanglePtr);
											
                    Boolean						GetOffscreenSelection (
                                                                                            Rect*									selectionRectanglePtr);
     */
    //		Boolean						GetSelectionExistsFlag (void);

    //		SInt16						GetTypeCode (void);

    void Invalidate(CMImageView* imageViewCPtr);
    /*
                    void							SetType (
                                                                                            SInt16								type);
		
                    virtual void				UpdateArea(
                                                                                            DisplaySpecsPtr					displaySpecsPtr,
                                                                                            const SelectionInfoPtr 			selectionInfoPtr,
                                                                                            const HPFieldPointsPtr    		selectionPointsPtr,
                                                                                            SInt16								startChannel,
                                                                                            SInt16								selectType);

                    virtual void				UpdateArea(
                                                                                            const Rect&							rectangle,
                                                                                            const LongRect&					lineColumnRect,
                                                                                            SInt16								selectType);
     */
#ifdef _DEBUG
    void AssertValid();
#endif

protected:

    //		Handle						m_selectInfoHandle;

    // For now these will not be used. The SelectInfo Structure will be used instead.
    // When the Mac version has been converted to classes then it will make sense
    // to use these variables. For now the need for both the selectInfo structure
    // and these member variables cause to much confusion.
    /*
                    enum Style {selection, field};
                    Style							m_style;
						
                    enum Type {none, rectangle, polygon};
                    Type							m_type;
	
                    LongRect 					m_lineColumnRectangle;
				
                    Rect 							m_offscreenRectangle;
		
                    Handle						m_polygonCoordinatesHandle;

                    SInt16						m_numberPolyPoints;

                    static SInt32				s_ThresholdSize;
     */
}; // end "class CMOutlineArea"

#endif // __LDRAWOBJ_H__
