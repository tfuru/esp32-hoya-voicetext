#include "AudioFileSourceHTTPStreamPost.h"

AudioFileSourceHTTPStreamPost::AudioFileSourceHTTPStreamPost()
{
  pos = 0;
  reconnectTries = 0;
  saveURL[0] = 0;
  saveHeaders = std::map<std::string, std::string>();
  saveParms = "";
}

bool AudioFileSourceHTTPStreamPost::open(const char *url,std::map<std::string, std::string> headers,const String parms){
  pos = 0;

  http.begin(url);
  http.addHeader("Content-Length", String(parms.length()));
  std::map<std::string, std::string>::iterator it = headers.begin();
  while (it != headers.end()){
    std::string h = it->first;
    std::string v = it->second;
    http.addHeader(h.c_str(), v.c_str());
    it++;
  }
  http.setReuse(true);
  int code = http.POST(parms);
  if (code != HTTP_CODE_OK) {
    http.end();
    cb.st(STATUS_HTTPFAIL, PSTR("Can't open HTTP request"));
    return false;
  }
  size = http.getSize();
  strncpy(saveURL, url, sizeof(saveURL));
  saveURL[sizeof(saveURL)-1] = 0;
  saveHeaders = headers;
  saveParms = parms;
  return true;
}

AudioFileSourceHTTPStreamPost::~AudioFileSourceHTTPStreamPost()
{
  http.end();
}

uint32_t AudioFileSourceHTTPStreamPost::read(void *data, uint32_t len)
{
  if (data==NULL) {
    Serial.printf_P(PSTR("ERROR! AudioFileSourceHTTPStream::read passed NULL data\n"));
    return 0;
  }
  return readInternal(data, len, false);
}

uint32_t AudioFileSourceHTTPStreamPost::readNonBlock(void *data, uint32_t len)
{
  if (data==NULL) {
    Serial.printf_P(PSTR("ERROR! AudioFileSourceHTTPStream::readNonBlock passed NULL data\n"));
    return 0;
  }
  return readInternal(data, len, true);
}

uint32_t AudioFileSourceHTTPStreamPost::readInternal(void *data, uint32_t len, bool nonBlock)
{
retry:
  if (!http.connected()) {
    cb.st(STATUS_DISCONNECTED, PSTR("Stream disconnected"));
    http.end();
    for (int i = 0; i < reconnectTries; i++) {
      char buff[32];
      sprintf_P(buff, PSTR("Attempting to reconnect, try %d"), i);
      cb.st(STATUS_RECONNECTING, buff);
      delay(reconnectDelayMs);
      if (open(saveURL,saveHeaders,saveParms)) {
        cb.st(STATUS_RECONNECTED, PSTR("Stream reconnected"));
        break;
      }
    }
    if (!http.connected()) {
      cb.st(STATUS_DISCONNECTED, PSTR("Unable to reconnect"));
      return 0;
    }
  }
  if ((size > 0) && (pos >= size)) return 0;

  WiFiClient *stream = http.getStreamPtr();

  // Can't read past EOF...
  if ( (size > 0) && (len > (uint32_t)(pos - size)) ) len = pos - size;

  if (!nonBlock) {
    int start = millis();
    while ((stream->available() < (int)len) && (millis() - start < 500)) yield();
  }

  size_t avail = stream->available();
  if (!nonBlock && !avail) {
    cb.st(STATUS_NODATA, PSTR("No stream data available"));
    http.end();
    goto retry;
  }
  if (avail == 0) return 0;
  if (avail < len) len = avail;

  int read = stream->read(reinterpret_cast<uint8_t*>(data), len);
  pos += read;
  return read;
}

bool AudioFileSourceHTTPStreamPost::seek(int32_t pos, int dir)
{
  (void) pos;
  (void) dir;
  return false;
}

bool AudioFileSourceHTTPStreamPost::close()
{
  http.end();
  return true;
}

bool AudioFileSourceHTTPStreamPost::isOpen()
{
  return http.connected();
}

uint32_t AudioFileSourceHTTPStreamPost::getSize()
{
  return size;
}

uint32_t AudioFileSourceHTTPStreamPost::getPos()
{
  return pos;
}

String AudioFileSourceHTTPStreamPost::URLEncode(const char *msg)
{
    const char *hex = "0123456789ABCDEF";
    String encodedMsg = "";

    while (*msg != '\0')
    {
        if (('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') ||
            ('0' <= *msg && *msg <= '9') || *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~')
        {
            encodedMsg += *msg;
        }
        else
        {
            encodedMsg += '%';
            encodedMsg += hex[*msg >> 4];
            encodedMsg += hex[*msg & 0xf];
        }
        msg++;
    }
    return encodedMsg;
}
