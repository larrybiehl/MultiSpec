// LImageDoc.h
//
// Revised by Larry Biehl on 06/20/2017
//
#ifndef __DOCIMAGGH__
#	define __DOCIMAGGH__

#include "LDrawObjects.h"

#include "wx/docview.h"
#include "wx/cmdproc.h"

class CMImageDoc : public wxDocument {
   DECLARE_DYNAMIC_CLASS(CMImageDoc)
private:
   bool m_updateflag;
public:
   CMImageDoc();
   ~CMImageDoc();
   
   //virtual bool OnSaveDocument(const wxString& filename);
   //virtual bool OnOpenDocument(const wxString& filename);
   virtual bool IsModified(void) const;
   virtual void Modify(bool mod);
   //bool DoOpenDocument(const wxString& file);
   //virtual bool DoSaveDocument(const wxString& file);
   virtual bool SaveAs();
   virtual bool Close();

   void Add(CMDrawObject* pObject);

   void Draw(CDC* pDC, CMImageView* pImageView);

   Boolean GetDisplayCoordinatesFlag() {
      return m_displayCoordinatesFlag;
   }

   void SetDisplayCoordinatesFlag(
			bool inputSetting) {
      m_displayCoordinatesFlag = inputSetting;
   }

   CMImageWindow* GetDocImageWindowCPtr() {
      return m_imageWindowCPtr;
   }

   wxSize GetDocSize() {
      return m_sizeDoc;
   }

   CMImageFrame* GetImageFrameCPtr() {
      return m_imageFrameCPtr;
   }

   CMOutlineArea* GetSelectionAreaCPtr() {
      return m_selectionAreaCPtr;
   }

   //bool OnCloseDocument(void);

   void Remove(CMDrawObject* pObject);

   void SetDocSize(
           SInt16 height,
           SInt16 width);

   void SetImageFrameCPtr(CMImageFrame* imageFrameCPtr);

   void SetImageWindowCPtr(CMImageWindow* imageWindowCPtr);
   bool SetImageWindowCPtrV(CMImageView* imageViewCPtr);
   Boolean OnViewCoordinatesBar();
   Boolean GetCoordinateFlag();

protected:
   void InitDocument();

   bool OnNewDocument();

   bool OnOpenDocument(const wxString &pszPathName);

   Boolean m_initializedFlag;

   Boolean m_displayCoordinatesFlag;

   wxList m_objects;

   wxSize m_sizeDoc;

   CMImageWindow* m_imageWindowCPtr;

   CMOutlineArea* m_selectionAreaCPtr;

   CMImageFrame* m_imageFrameCPtr;

   CMImageView* m_imageViewCPtr;

};

#endif
