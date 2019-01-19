# esp32-hoya-voicetext
VoiceText Web API を ESP32 で最小構成で動かすサンプル。  
オーディオコーディックICやI2S DACが不要です。 

# ハードウェア
[Adafruit HUZZAH32](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather)
- 適当なESP32開発ボードでいいと思います。  

[2SC1815](http://akizukidenshi.com/catalog/g/gI-00881/)
- 適当なトランジスタでいいと思います。

[スピーカー 8Ω 0.2W](http://akizukidenshi.com/catalog/g/gP-09012/)
- 適当なスピーカーでいいと思います。

# 回路
参考サイトの "mp3再生プログラム(No DAC)" 欄の画像をのように ESP32 の IO22 と トラジスタ とスピーカーを接続する  

 [ESP32でmp3単純再生 - すらりん日記](https://blog.techlab-xe.net/archives/6026)

# ソフトウェア開発環境
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

# コード修正 箇所
```c++:main.cpp
const char *SSID = "SSID";
const char *PASSWORD = "PASSWORD";

//https://cloud.voicetext.jp/webapi API_KEY
const String TTS_API_KEY = "API_KEY";
```
