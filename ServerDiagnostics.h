/*
 * Tutorial can be found at https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/
 *
 */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

bool wifiConnected = false;
int wifiMessage = 0;          // displays IP message when wifi first connects
int newVersionAvailable = 0;

const char* PARAM_INPUT_OPTION = "option";
const char* PARAM_INPUT_EFFECTS = "effect";
const char* PARAM_INPUT_AUDIO = "audio";
const char* PARAM_INPUT_STATIC = "static";
const char* PARAM_INPUT_PLAYLISTS = "playlists";
const char* PARAM_INPUT_STATE = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>AuroraDrop</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!--<link rel="icon" href="data:image/gif;base64,R0lGODlhEAAQAMQAAORHHOVSKudfOulrSOp3WOyDZu6QdvCchPGolfO0o/XBs/fNwfjZ0frl3/zy7////wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAkAABAALAAAAAAQABAAAAVVICSOZGlCQAosJ6mu7fiyZeKqNKToQGDsM8hBADgUXoGAiqhSvp5QAnQKGIgUhwFUYLCVDFCrKUE1lBavAViFIDlTImbKC5Gm2hB0SlBCBMQiB0UjIQA7">-->
  <link rel="icon" type="image/x-icon" href="/test.bmp" />
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem; color:#ffc600; text-shadow: -1px 0 black, 0 1px black, 1px 0 black, 0 -1px black; margin-bottom: 0.1rem;}
    h3 {font-size: 1.5rem; color:black; margin: 0.0rem;}
    h4 {font-size: 1.0rem; color:black; margin-bottom: 1.2rem; margin-top: 1.5rem;}
    h5 {font-size: 0.8rem; margin-bottom: 0.2rem; margin-top: 0.4rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 60px; height: 36px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 24px; width: 24px; left: 6px; bottom: 6px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    .numeric {position: relative; display: inline-block; width: 60px; height: 36px; margin-bottom: 0.3rem; } 
    .inputBox {background-color: #ccc;}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(24px); -ms-transform: translateX(24px); transform: translateX(24px)}
    .row {display: flex;}
    .column {flex: 25;}
	}
  </style>
</head>
<body>
  <h2>AuroraDrop</h2>
  <h3>A U D I O&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;V I S U A L I S E R</h3>
  <br />
  <a href="/">Home</a> - <a href="/bitmap">Render</a>
  <br />
  <div class="row">
    <div class="column">
      <h4>General</h4>
%SLIDERPLACEHOLDER%
    </div>

    <div class="column">
      <h4>Inital Effects</h4>
      <h5>Concurrent</h5>
      <label class="numeric"><input type="number" value="1" min="0" max="2" size="4" onchange="changeNumber(this)" id="1"><span class="inputBox"></span></label>
%EFFECTS_PLACEHOLDER%
    </div>

    <div class="column">
      <h4>Audio Effects</h4>
      <h5>Concurrent</h5>
      <label class="numeric"><input type="number" value="2" min="0" max="4" size="4" onchange="changeNumber(this)" id="2"><span class="inputBox"></span></label>
%AUDIO_PLACEHOLDER%
    </div>

    <div class="column">
      <h4>Static Effects</h4>
      <h5>Concurrent</h5>
      <label class="numeric"><input type="number" value="2" min="0" max="4" size="4" onchange="changeNumber(this)" id="3"><span class="inputBox"></span></label>
%STATIC_PLACEHOLDER%
<br />
%BUTTONPLACEHOLDER%
    </div>
  </div> 
<script>
function toggleOptionCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?option="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?option="+element.id+"&state=0", true); }
  xhr.send();
}
function toggleEffectCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?effect="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?effect="+element.id+"&state=0", true); }
  xhr.send();
}
function toggleAudioCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?audio="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?audio="+element.id+"&state=0", true); }
  xhr.send();
}
function toggleStaticCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?static="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?static="+element.id+"&state=0", true); }
  xhr.send();
}
function changeNumber(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update?playlists="+element.id+"&state="+element.value, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

const char bitmap_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>AuroraDrop Render</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" type="image/x-icon" href="/test.bmp" />
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem; color:#ffc600; text-shadow: -1px 0 black, 0 1px black, 1px 0 black, 0 -1px black; margin-bottom: 0.1rem;}
    h3 {font-size: 1.5rem; color:black; margin: 0.0rem;}
    h5 {margin-bottom: 0.2rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    div {font-family: Arial; display: inline-block; text-align: center;}
    .bmp {position: absolute; border: 1px solid #000; height: 256px; width: 256px;}
    .grid {position: relative; background-image: 
			linear-gradient( to right, rgba( 0, 0, 0, 0.9 ) 1px, transparent 1px ),
			linear-gradient( to bottom, rgba( 0, 0, 0, 0.9 ) 1px, transparent 1px );
		  background-size: 4px 4px;
		  border: 1px solid #000;
		  height: 256px;
		  width: 256px;
  </style>
</head>
<body>
  <h2>AuroraDrop</h2>
  <h3>A U D I O&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;V I S U A L I S E R</h3>
  <br />
  <a href="/">Home</a> - <a href="/bitmap">Render</a>
  <br />
  <br />
  %BITMAPPLACEHOLDER%
  <div class="bmp">
    <img src="/test.bmp" width="256" height="256"/>
  </div>
  <div class="grid">
    &nbsp;<br />
  </div>
  <br /><br />
  <a href="/bitmap">Refresh</a>&nbsp;&nbsp;-&nbsp;&nbsp;<a href="/favicon">Use as Favicon</a>
</body>
</html>
)rawliteral";

String optionState(bool option){
  if(option){
    return "checked";
  }
  else {
    return "";
  }
}


// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "SLIDERPLACEHOLDER"){
    String buttons = "";
    buttons += "<h5>Show Diagnostics</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"1\" " + optionState(option1Diagnostics) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Lock Frame Rate</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"2\" " + optionState(option2LockFps) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Show Render Time</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"3\" " + optionState(option3ShowRenderTime) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Pause Effect Cycling</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"4\" " + optionState(option4PauseCycling) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Lian-Li 120 Mode</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"5\" " + optionState(option5LianLi120Mode) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Disable Initial Effects</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"6\" " + optionState(option6DisableInitialEffects) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Disable Audio Patterns</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"7\" " + optionState(option7DisableAudio) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Disable Static Patterns</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"8\" " + optionState(option8DisableStatic) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Not Used</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"9\" " + optionState(option9DisableFinalEffects) + "><span class=\"slider\"></span></label>";
    buttons += "<h5>Disable Caleido Effects</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"10\" " + optionState(option10DisableCaleidoEffects) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if (var == "BUTTONPLACEHOLDER"){
    String buttons2 = "";
    buttons2 += "<h5>TEST</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleOptionCheckbox(this)\" id=\"10\" " + optionState(option1Diagnostics) + "><span class=\"slider\"></span></label>";
    return buttons2;
  }

  if (var == "EFFECTS_PLACEHOLDER"){
    String buttons4 = "";
    int num = playlistInitialEffects[0].getPatternCount();
    for (int i=0; i<num; i++) {
      char* yy = playlistInitialEffects[0].getItemName(i);
      bool isEnabled = playlistInitialEffects[0].getItemEnabled(i);
      String mystring(yy);
      buttons4 += "<h5>" + mystring + "</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleEffectCheckbox(this)\" id=\"" + i + "\" " + optionState(isEnabled) + "><span class=\"slider\"></span></label>";
    }
    return buttons4;
  }

  if (var == "AUDIO_PLACEHOLDER"){
    String buttons3 = "";
    int num = playlistAudio[0].getPatternCount();
    for (int i=0; i<num; i++) {
      char* yy = playlistAudio[0].getItemName(i);
      bool isEnabled = playlistAudio[0].getItemEnabled(i);
      String mystring(yy);
      buttons3 += "<h5>" + mystring + "</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleAudioCheckbox(this)\" id=\"" + i + "\" " + optionState(isEnabled) + "><span class=\"slider\"></span></label>";
    }
    return buttons3;
  }

  if (var == "STATIC_PLACEHOLDER"){
    String buttons = "";
    int num = playlistStatic[0].getPatternCount();
    for (int i=0; i<num; i++) {
      char* yy = playlistStatic[0].getItemName(i);
      bool isEnabled = playlistStatic[0].getItemEnabled(i);
      String mystring(yy);
      buttons += "<h5>" + mystring + "</h5><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleStaticCheckbox(this)\" id=\"" + i + "\" " + optionState(isEnabled) + "><span class=\"slider\"></span></label>";
    }
    return buttons;
  }


//patternsAudio[0].listPatterns();



  return String();
}


const String test = "";

// replaces placeholder with bitmaps from spiffs
String bitmapProcessor(const String& var){
  
  //effects.OldGenerateBase64EncodedBitmap();
  effects.GenerateBitmap();

  if(var == "BITMAPPLACEHOLDER"){
    String buttons = "";
    //buttons += "<p><img width='256' height='256' src='data:image/png;base64," + effects.base64EncodedBmp + "' alt='Aurora Drop Bitmap Dynamic' /></p>";

    buttons += "<p>Dynamic: <img width='256' height='256' src='data:image/bmp;base64,";
    //buttons +=  effects.base64EncodedBmp;
    //for (int i=0;i<800;i++) {   // 16458
    //  buttons += (char)bmpData[i];
    //}
    buttons +=  "' alt='Aurora Drop Bitmap Dynamic' /></p>";

    //WORKING ->>  buttons += "<p><img src='data:image/png;base64," + effects.base64EncodedBmp + "' alt='Aurora Drop Bitmap Dynamic' /></p>";
    return String();
    //return buttons;
  }
  return String();
}



void checkWifiStatus() {
    if (WiFi.status() == WL_CONNECTED && !wifiConnected) {
      wifiConnected = true;
      wifiMessage = 1000;                // when non zero, displays connected info for x renders
      Serial.println(WiFi.localIP());

#ifdef VERNO
      // get version info from git repository and flash message if there is an update
      HTTPClient http;
      
      String serverName = "https://raw.githubusercontent.com/";
      String serverPath = serverName + "uklooney/AuroraDrop/main/Version.txt";
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        if (payload != VERNO) {
          newVersionAvailable = 1500;        // when non zero, displays
        }
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
#endif

      // set route for root web page
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
      });

      // set route for rendered bitmap web page
      server.on("/bitmap", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", bitmap_html, bitmapProcessor);
      });

      // set route for only test bitmap image
      server.on("/test.bmp", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test.bmp", "image/bmp");
      });

      // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
      server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String inputMessage1;
        String inputMessage2;
        // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
        if (request->hasParam(PARAM_INPUT_OPTION) && request->hasParam(PARAM_INPUT_STATE)) {
          inputMessage1 = request->getParam(PARAM_INPUT_OPTION)->value();
          inputMessage2 = request->getParam(PARAM_INPUT_STATE)->value();
          
          //digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
          Serial.print(inputMessage1.toInt());
          Serial.print(",");
          Serial.println(inputMessage2.toInt());
          switch (inputMessage1.toInt()) {
            case 1:
              option1Diagnostics = inputMessage2.toInt();
              break;
            case 2:
              option2LockFps = inputMessage2.toInt();
              break;
            case 3:
              option3ShowRenderTime = inputMessage2.toInt();
              break;
            case 4:
              option4PauseCycling = inputMessage2.toInt();
              break;
            case 5:
              option5LianLi120Mode = inputMessage2.toInt();
              break;
            case 6:
              option6DisableInitialEffects = inputMessage2.toInt();
              break;
            case 7:
              option7DisableAudio = inputMessage2.toInt();
              break;
            case 8:
              option8DisableStatic = inputMessage2.toInt();
              break;
            case 9:
              option9DisableFinalEffects = inputMessage2.toInt();
              break;
            case 10:
              option10DisableCaleidoEffects = inputMessage2.toInt();
              break;
          }
        }

        // audio patterns enable/disable
        if (request->hasParam(PARAM_INPUT_EFFECTS) && request->hasParam(PARAM_INPUT_STATE)) {
          inputMessage1 = request->getParam(PARAM_INPUT_EFFECTS)->value();
          inputMessage2 = request->getParam(PARAM_INPUT_STATE)->value();
          // enable/disable chosen pattern in each playlist
          for (int i = 0; i < MAX_PLAYLISTS_EFFECT; i++) {
            playlistInitialEffects[i].setItemEnabled(inputMessage1.toInt(), inputMessage2.toInt());
          }
        }

        // audio patterns enable/disable
        if (request->hasParam(PARAM_INPUT_AUDIO) && request->hasParam(PARAM_INPUT_STATE)) {
          inputMessage1 = request->getParam(PARAM_INPUT_AUDIO)->value();
          inputMessage2 = request->getParam(PARAM_INPUT_STATE)->value();
          // enable/disable chosen pattern in each playlist
          for (int i = 0; i < MAX_PLAYLISTS_AUDIO; i++) {
            playlistAudio[i].setItemEnabled(inputMessage1.toInt(), inputMessage2.toInt());
          }
        }


        // static patterns enable/disable
        if (request->hasParam(PARAM_INPUT_STATIC) && request->hasParam(PARAM_INPUT_STATE)) {
          inputMessage1 = request->getParam(PARAM_INPUT_STATIC)->value();
          inputMessage2 = request->getParam(PARAM_INPUT_STATE)->value();
          // enable/disable chosen pattern in each playlist
          for (int i = 0; i < MAX_PLAYLISTS_STATIC; i++) {
            playlistStatic[i].setItemEnabled(inputMessage1.toInt(), inputMessage2.toInt());
          }
        }

        // number of simultaneous palylists 
        if (request->hasParam(PARAM_INPUT_PLAYLISTS) && request->hasParam(PARAM_INPUT_STATE)) {
          inputMessage1 = request->getParam(PARAM_INPUT_PLAYLISTS)->value();
          inputMessage2 = request->getParam(PARAM_INPUT_STATE)->value();
          switch (inputMessage1.toInt()) {
            case 1:
              maxPlaylistsInitialEffect = inputMessage2.toInt();
              break;
            case 2:
              maxPlaylistsAudio = inputMessage2.toInt();
              break;
            case 3:
              maxPlaylistsStatic = inputMessage2.toInt();
              break;
          }
        }

        //else {
        //  inputMessage1 = "No message sent";
        //  inputMessage2 = "No message sent";
        //}


        Serial.print("Option: ");
        Serial.print(inputMessage1);
        Serial.print(" - Set to: ");
        Serial.println(inputMessage2);
        request->send(200, "text/plain", "OK");
      });

      // Start server
      server.begin();    
    }

}


