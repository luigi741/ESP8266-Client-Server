// https://techtutorialsx.com/2016/10/03/esp8266-setting-a-simple-http-webserver/

#include <dummy.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);
HTTPClient http;
WiFiClient client;

// HTML string for our webpage
String homePage = "<head><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script><link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/bulma/0.4.3/css/bulma.min.css'><meta charset='utf-8'/></head><body onLoad='startTime()'><nav class='navbar' style='background-color: #00FFFF'><div class='navbar-brand' style='color: blue'><a style='font-size: 24px' class='navbar-item' href='/'>The Birds: CIS 4930 - Embedded Systems</a></div><div class='nav-right'><p id='cardTime' class='navbar-item' style='font-size: 24px'></p></div></nav><br><div class='container is-fluid'><div class='tile is-ancestor'><div class='tile is-vertical is-8'><div class='tile'><div class='tile is-parent is-vertical'><article class='tile is-child notification is-primary' style='background-color: #A232FE'><p class='title'>LED Controller</p><p class='subtitle'>Turn on some LEDs!</p><a class='button is-success is-large' href='/button1' style='margin-right: 20px'>ON</a><a class='button is-danger is-large' href='/button2'>OFF</a></article><article class='tile is-child notification is-warning'><p class='title'>Timer</p><p class='subtitle'>This specifies a value to send to a timer.</p><div class='field'><label class='label'>Time Increments</label><p class='control'><span class='select'><select id='timerSelect'><option id='opt1' value='10'>10 Seconds</option><option id='opt2' value='20'>20 Seconds</option><option id='opt3' value='30'>30 Seconds</option></select></span></p></div><div><p><button id='timerSubmit' class='button is-primary'>Submit</button></p></div></article></div><div class='tile is-parent'><article class='tile is-child notification is-info' style='background-color: #6BB8FD'><p class='title'>RGB LEDs</p><p class='subtitle'>Change the colors</p><br><div class='columns'><div class='column'><a href='/red' style='background-color: red; color: white' class='button is-large is-fullwidth' type='button' name='red'>RED</a></div><div class='column'><a href='/green' style='background-color: green; color: white' class='button is-large is-fullwidth' type='button' name='green'>GREEN</a></div><div class='column'><a href='/blue' style='background-color: blue; color: white' class='button is-large is-fullwidth' type='button' name='blue'>BLUE</a></div></div><br><div class='columns'><div class='column'><a href='/purple' style='background-color: purple; color: white' class='button is-large is-fullwidth' type='button' name='purple'>PURPLE</a></div><div class='column'></div><div class='column'><a href='/teal' style='background-color: aqua' class='button is-large is-fullwidth' type='button' name='teal'>TEAL</a></div></div></article></div></div><div class='tile is-parent'><article class='tile is-child notification is-danger'><p class='title'>Ultrasonic Sensor</p><p class='subtitle'>Use sonar to detect distance</p><div class='columns'><div class='column'><a class='button' style='background-color: blue; color: white'>Column 1</a></div><div class='column'><a class='button is-warning'>Column 2</a></div></div></article></div></div><div class='tile is-parent'><article class='tile is-child notification is-success'><div class='content'><p class='title'>Motion Detection</p><p class='subtitle'>Enable IR sensor to detect movement</p><div class='content'></div></div></article></div></div></div><footer class='footer'><div class='container'><div class='content has-text-centered'><p>Built with HTML/JavaScript using Bulma CSS Framework</p></div></div></footer><script type='text/javascript'>function consoleLog(){console.log('Clicked on button!');}function textBoxPrint(){var textBox=document.getElementById('myText').value;console.log(textBox);}function startTime(){var today=new Date();var h=today.getHours();var meridiem;if(h >= 12){meridiem = 'PM';}else if(h >= 0 && h < 12){meridiem = 'AM';}if(h > 12){h-=12;}var m=today.getMinutes();var s=today.getSeconds();m=checkTime(m);s=checkTime(s);document.getElementById('cardTime').innerHTML=h + ':' + m + ':' + s + ' ' + meridiem;var t=setTimeout(startTime,500);}function checkTime(i){if(i < 10){i = '0' + i};return i;}$(document).ready(function(){$('#myButton').click(function() {var textBox=document.getElementById('myText').value;console.log('Clicked My Button!');$.get('/textbox', {text: textBox});});$('#timerSubmit').click(function(){var RequestHTTP=new XMLHttpRequest();var timer = $('#timerSelect').val();if(timer=='10'){console.log('Timer=10');RequestHTTP.open('GET', '/timer1', true);RequestHTTP.send();}else if(timer=='20'){RequestHTTP.open('GET', '/timer2', true);RequestHTTP.send();}else if(timer=='30'){RequestHTTP.open('GET', '/timer3', true);RequestHTTP.send();}});});</script></body>";
const char* host = "192.168.45.73";

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }
    WiFi.begin("Galaxy Note5 WiFi", "luigi278678");  //Connect to the WiFi network

    while (WiFi.status() != WL_CONNECTED) {  // Wait for connection
        delay(1000);
        Serial.println("Waiting to connect...");
    }

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //Print the local IP

    server.on("/button1", []() {      //Define the handling function for the path
        server.send(200, "text/html", homePage);
        Serial.print("x");
    });

    server.on("/button2", []() {      //Define the handling function for the path
        server.send(200, "text/html", homePage);
        Serial.print("y");
    });

    server.on("/textbox", []() {
        server.send(200, "text/html", homePage);
        Serial.print("Entered text!");
        /*http.begin("http://192.168.43.75/textbox");
        int httpCode = http.GET();
//        Serial.print("HTTP Code" + httpCode);

        // https://stackoverflow.com/questions/37664360/arduino-ide-esp8266-json-decode-from-url
        String JSON = "";
        bool httpBody = false;

        while (client.available()) {
            String line = client.readStringUntil('\r');
            if (!httpBody && line.charAt(1) == '{') {
                httpBody = true;
            }

            if (httpBody) {
                JSON += line;
            }
        }
        StaticJsonBuffer<400> jsonBuffer;
        Serial.println("Got data: ");
        Serial.println(JSON);
        JsonObject& root = jsonBuffer.parseObject(JSON);
        String data = root["url"];
        root.printTo(Serial);*/
        // https://stackoverflow.com/questions/34078497/esp8266-wificlient-simple-http-get
    });

    server.on("/timer1", []() {
        server.send(200, "text/html", homePage);
        Serial.print("1");
    });

    server.on("/timer2", []() {
        server.send(200, "text/html", homePage);
        Serial.print("2");    
    });

    server.on("/timer3", []() {
        server.send(200, "text/html", homePage);
        Serial.print("3");    
    });

    server.on("/red", []() {
        server.send(200, "text/html", homePage);    
        Serial.print("r");
    });

    server.on("/green", []() {
        server.send(200, "text/html", homePage);  
        Serial.print("g");  
    });

    server.on("/blue", []() {
        server.send(200, "text/html", homePage);
        Serial.print("b");
    });

    server.on("/purple", []() {
        server.send(200, "text/html", homePage);    
        Serial.print("p");
    });

    server.on("/teal", []() {
        server.send(200, "text/html", homePage);    
        Serial.print("t");
    });

    server.on("/white", []() {
        server.send(200, "text/html", homePage);    
        Serial.print("w");
    });

    server.on("/sonar", []() {
        server.send(200, "text/html", homePage);
        Serial.print("d");   
    });

    server.on("/motion", []() {
        server.send(200, "text/html", homePage);
        Serial.print("m");    
    });

    server.on("/", handleRootPath);      //Associate the handler function to the path
    server.begin();                                       //Start the server
    Serial.println("Server listening");
}

void loop() {
    server.handleClient();         //Handling of incoming requests
 /*   HTTPClient http;
//    http.begin("http://192.168.43.75/");
    http.begin("http://192.168.1.16/");
//    int httpCode = http.GET();
//    Serial.println(httpCode);

//    String intData = http.getString();
//    Serial.println(intData);

//    http.end();
//    delay(1000);

    // https://www.arduino.cc/en/Tutorial/WebServer


    // https://techtutorialsx.com/2016/07/17/esp8266-http-get-requests/
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http2;
//        http2.begin("http://192.168.43.75/textbox");
        http2.begin("http://192.168.1.16/textbox");
        int httpCode = http2.GET();

        if (httpCode > 0 ) {
            String payload = http2.getString();
            Serial.println(payload);
        }
        http2.end();
    }
    delay(3000);*/
}

void handleRootPath() {            //Handler for the rooth path
    server.send(200, "text/html", homePage);    
}
