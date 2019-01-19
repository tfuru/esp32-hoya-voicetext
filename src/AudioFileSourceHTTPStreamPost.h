#ifndef _AUDIOFILESOURCEHTTPSTREAMPOST_H
#define _AUDIOFILESOURCEHTTPSTREAMPOST_H

#include <map>
#include <utility>

#include <Arduino.h>
#ifdef ESP32
  #include <HTTPClient.h>
#else
  #include <ESP8266HTTPClient.h>
#endif
#include "AudioFileSource.h"

class AudioFileSourceHTTPStreamPost : public AudioFileSource
{
  public:
    AudioFileSourceHTTPStreamPost();
    virtual ~AudioFileSourceHTTPStreamPost() override;
    
    virtual bool open(const char *url,std::map<std::string, std::string> headers,const String parms);
    virtual uint32_t read(void *data, uint32_t len) override;
    virtual uint32_t readNonBlock(void *data, uint32_t len) override;
    virtual bool seek(int32_t pos, int dir) override;
    virtual bool close() override;
    virtual bool isOpen() override;
    virtual uint32_t getSize() override;
    virtual uint32_t getPos() override;
    bool SetReconnect(int tries, int delayms) { reconnectTries = tries; reconnectDelayMs = delayms; return true; }

    enum { STATUS_HTTPFAIL=2, STATUS_DISCONNECTED, STATUS_RECONNECTING, STATUS_RECONNECTED, STATUS_NODATA };
    static String URLEncode(const char *msg);
  private:
    virtual uint32_t readInternal(void *data, uint32_t len, bool nonBlock);

    HTTPClient http;
    int pos;
    int size;
    int reconnectTries;
    int reconnectDelayMs;
    char saveURL[128];
    std::map<std::string, std::string> saveHeaders;
    String saveParms;
};


#endif

