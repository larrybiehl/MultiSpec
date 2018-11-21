
#ifndef __DOCTEXTH__
#define __DOCTEXTH__

#include "wx/docview.h"
#include "wx/cmdproc.h"

class CMTextDoc: public wxDocument
{
	DECLARE_DYNAMIC_CLASS(CMTextDoc)
	private:
	public:

   virtual bool Close();
	virtual bool IsModified(void) const;
	virtual void Modify(bool mod);
	virtual bool OnOpenDocument(const wxString& filename);
	virtual bool OnSaveDocument(const wxString& filename);
	//virtual bool SaveAs();

	CMTextDoc(void) {}
	~CMTextDoc(void) {}
};

#endif
