#include "IniWriter.h"
#include <iostream>
#include <Windows.h> 
CIniWriter::CIniWriter(char* szFileName)
{
 memset(m_szFileName, 0x00, 255);
 memcpy(m_szFileName, szFileName, strlen(szFileName));
}
bool CIniWriter::WriteInteger(char* szSection, char* szKey, int iValue)
{
 char szValue[255];
 bool	returnFlag;

 sprintf_s(szValue, "%d", iValue);
 returnFlag = WritePrivateProfileString((LPCWSTR)szSection, (LPCWSTR)szKey, (LPCWSTR)szValue, (LPCWSTR)m_szFileName);
 return (returnFlag);
}
void CIniWriter::WriteFloat(char* szSection, char* szKey, float fltValue)
{
 char szValue[255];
 sprintf_s(szValue, "%f", fltValue);
 WritePrivateProfileString((LPCWSTR)szSection,  (LPCWSTR)szKey, (LPCWSTR)szValue, (LPCWSTR)m_szFileName); 
}
void CIniWriter::WriteBoolean(char* szSection, char* szKey, bool bolValue)
{
 char szValue[255];
 sprintf_s(szValue, "%s", bolValue ? "True" : "False");
 WritePrivateProfileString((LPCWSTR)szSection,  (LPCWSTR)szKey, (LPCWSTR)szValue, (LPCWSTR)m_szFileName); 
}
void CIniWriter::WriteString(char* szSection, char* szKey, char* szValue)
{
 WritePrivateProfileString((LPCWSTR)szSection,  (LPCWSTR)szKey, (LPCWSTR)szValue, (LPCWSTR)m_szFileName);
}

