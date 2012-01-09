#ifndef _c_IsapiOstream_h
#define _c_IsapiOstream_h

#include "Poco/Foundation.h"
#include "Poco/StreamUtil.h"
#include "Poco/BufferedStreamBuf.h"
#include <iosfwd>

#include "HttpExt.h"


class c_IsapiOstreamBuf : public Poco::BufferedStreamBuf
{
public:
  c_IsapiOstreamBuf();
  c_IsapiOstreamBuf(EXTENSION_CONTROL_BLOCK * Ecb,bool Chunked);
  ~c_IsapiOstreamBuf();
  void Close();
  
protected:
  virtual int readFromDevice(char_type* buffer, std::streamsize length)
  {
    return 0;
  }

  virtual int writeToDevice(const char_type* buffer, std::streamsize length);
  

  
protected:
  EXTENSION_CONTROL_BLOCK * m_Ecb;
  bool m_Chunked;
  std::string m_HexLen;
};


class c_IsapiIOS: public virtual std::ios
  /// The base class for CountingInputStream and CountingOutputStream.
  ///
  /// This class is needed to ensure the correct initialization
  /// order of the stream buffer and base classes.
{
public:
  c_IsapiIOS(EXTENSION_CONTROL_BLOCK * Ecb,bool Chunked);
  /// Creates the basic stream and leaves it unconnected.

  ~c_IsapiIOS();
  /// Destroys the stream.

  

  c_IsapiOstreamBuf* rdbuf();
  /// Returns a pointer to the underlying streambuf.
  
  void Close() { _buf.Close(); }

protected:
  c_IsapiOstreamBuf _buf;
  
};

class c_IsapiOstream : public c_IsapiIOS, public std::ostream
{
public:
  c_IsapiOstream(EXTENSION_CONTROL_BLOCK * Ecb,bool Chunked);
  ~c_IsapiOstream(void);  
  
  
};

#endif
