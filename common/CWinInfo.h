// ===========================================================================
//	CWinInfo.h	c1995 Purdue Research Foundataion All rights reserved.
// ===========================================================================                 
  
#if !defined __MWININFO_H__  
	#define __MWININFO_H__ 
	

class	CMWindowInfo
	{
	public:
									CMWindowInfo();						// constructor
									
									CMWindowInfo(							// constructor
										Handle		fileInfoHandle);
										
		virtual 					~CMWindowInfo();		// destructor stubBoolean
		
		Handle 					GetWindowInfoHandle(void);
										
		void						ReleaseOffscreenSupportMemory(void);

		void						SetImageWindowCPtrInWindowInfo (
										CMImageWindow*			imageWindowCPtr);
		
		Boolean					m_initializedFlag; 
	
	protected:									
	
		Handle					m_windowInfoHandle;
		
	};
	
#endif	// !defined __MWININFO_H__ 
