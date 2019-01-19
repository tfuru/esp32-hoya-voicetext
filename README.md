# esp32-hoya-voicetext

#環境
[VSCode](https://code.visualstudio.com/)
[PlatformIO IDE for VSCode](https://docs.platformio.org/en/latest/ide/vscode.html)

# 必要API KEY
[VoiceText Web API](https://cloud.voicetext.jp/webapi) に登録して APIキーを取得する

# 必要ライブラリ導入
```
cd lib
git clone https://github.com/earlephilhower/ESP8266Audio
git clone https://github.com/Gianbacchio/ESP8266_Spiram
```

# コード修正
```c++:main.cpp
const char *SSID = "SSID";
const char *PASSWORD = "PASSWORD";

//https://cloud.voicetext.jp/webapi API_KEY
const String TTS_API_KEY = "API_KEY";
```