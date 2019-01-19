#include <Arduino.h>
#include <base64.h>

#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>

#include <WiFi.h>
#include "AudioFileSourceHTTPStreamPost.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

#include "config.h"

// Enter your WiFi setup here:
const char *SSID = WIFI_SSID;
const char *PASSWORD = WIFI_PASSWORD;

//https://cloud.voicetext.jp/webapi API_KEY
const String API_KEY = TTS_API_KEY;

const char * TTS_TEXT = "おはようございます";

const char *URL="https://api.voicetext.jp/v1/tts";
std::map<std::string, std::string> headers;

const float VOLUME = (80.0F);
const uint32_t AUDIO_FILE_SOURCE_BUFFER = 48*1024;

AudioGeneratorMP3 *mp3;
AudioFileSourceHTTPStreamPost *file;
AudioFileSourceBuffer *buff;
AudioOutputI2SNoDAC *out;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

void initMP3(){
  file = new AudioFileSourceHTTPStreamPost();
  file->open(URL,headers,"text="+AudioFileSourceHTTPStreamPost::URLEncode(TTS_TEXT)+"&speaker=hikari&volume=200&speed=120&format=mp3");
  buff = new AudioFileSourceBuffer(file, AUDIO_FILE_SOURCE_BUFFER);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
  out = new AudioOutputI2SNoDAC();
  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  mp3->begin(buff, out);
  out->SetGain(VOLUME/100.0F);
}

void setup()
{  
  Serial.begin(115200);
  delay(1000);

  Serial.print("Connecting to WiFi ");

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(SSID, PASSWORD);

  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("Connected");

  headers["Authorization"] = ("Basic " + base64::encode(API_KEY + ":")).c_str();
  headers["Content-Type"] = "application/x-www-form-urlencoded";

  initMP3();
}

void loop()
{
  static int lastms = 0;

  if (mp3->isRunning()) {
    if (millis()-lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
     }
    if (!mp3->loop()){
      delay(1000);
      mp3->stop();
    } 
  } else {
    Serial.printf("MP3 done\n");
    delay(3000);
    //ループ再生する場合
    //initMP3();
  }
}

