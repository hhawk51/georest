#include "c_IsapiOstream.h"
#include "..\..\..\poco-1.3.5\Foundation\include\Poco\NumberFormatter.h"



c_IsapiOstreamBuf::c_IsapiOstreamBuf( EXTENSION_CONTROL_BLOCK * Ecb,bool Chunked ) : Poco::BufferedStreamBuf(8192,std::ios::out )
{
  m_Ecb = Ecb;
  m_Chunked = Chunked;
  
  m_HexLen.reserve(32);
}

void c_IsapiOstreamBuf::Close()
{
    //_session.write("0\r\n\r\n", 5);
    sync();
  if( m_Chunked )
  {
    DWORD len=5;    
    BOOL ret = m_Ecb->WriteClient(m_Ecb->ConnID, "0\r\n\r\n", &len, 0);    
  }
  
}

int c_IsapiOstreamBuf::writeToDevice( const char_type* buffer, std::streamsize length )
{
  DWORD len = (DWORD)length;
  if( m_Chunked )
  {
    BOOL ret;
    //m_HexLen.cle
    m_HexLen.clear();
    Poco::NumberFormatter::appendHex(m_HexLen,length);
    m_HexLen.append("\r\n");
    
    len = m_HexLen.size();
    ret = m_Ecb->WriteClient(m_Ecb->ConnID, (void*)m_HexLen.data(), &len, 0);
    
    len = (DWORD)length;
    ret = m_Ecb->WriteClient(m_Ecb->ConnID, (void*)buffer, &len, 0);
    
    len = 2;
    ret = m_Ecb->WriteClient(m_Ecb->ConnID, "\r\n", &len, 0);
  }
  else
  {
  
    BOOL ret = m_Ecb->WriteClient(m_Ecb->ConnID, (void*)buffer, &len, 0);
  }
  return length;
}

c_IsapiOstreamBuf::~c_IsapiOstreamBuf()
{
  sync();
}

c_IsapiIOS::c_IsapiIOS( EXTENSION_CONTROL_BLOCK * Ecb,bool Chunked ) : _buf(Ecb,Chunked)
{

}

c_IsapiIOS::~c_IsapiIOS()
{

}
c_IsapiOstream::c_IsapiOstream( EXTENSION_CONTROL_BLOCK * Ecb,bool Chunked ): c_IsapiIOS(Ecb,Chunked) , std::ostream(&_buf)
{

}

c_IsapiOstream::~c_IsapiOstream(void)
{
}

