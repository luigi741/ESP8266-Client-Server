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
String homePage = "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/bulma/0.4.3/css/bulma.min.css'><nav class='navbar' style='background-color: #00FFFF'> <div class='navbar-brand' style='color: blue'> <a style='font-size: 24px' class='navbar-item'>The Birds: CIS 4930 - Embedded Systems</a> </div></nav><br><div class='container is-fluid'> <div class='tile is-ancestor'> <div class='tile is-vertical is-8'> <div class='tile'> <div class='tile is-parent is-vertical'> <article class='tile is-child notification is-primary' style='background-color: #A232FE'> <p class='title'>LED Controller</p> <p class='subtitle'>Turn on some LEDs!</p> <a class='button is-success is-large' href='/button1' style='margin-right: 20px'>ON</a> <a class='button is-danger is-large' href='/button2'>OFF</a> </article> <article class='tile is-child notification is-warning'> <p class='title'>Timer</p> <p class='subtitle'>This specifies a value to send to a timer.</p> <div class='field'> <label class='label'>Time Increments</label> <p class='control'> <span class='select'> <select id='timerSelect'> <option id='opt1' value='10'>10 Seconds</option> <option id='opt2' value='20'>20 Seconds</option> <option id='opt3' value='30'>30 Seconds</option> </select> </span> </p> </div> <div> <p> <button id='timerSubmit' class='button is-primary'>Submit</button> </p> </div> </article> </div> <div class='tile is-parent'> <article class='tile is-child notification is-info' style='background-color: #6BB8FD'> <p class='title'>RGB LEDs</p> <p class='subtitle'>Change the colors</p> <!-- <figure class='image is-4by3'> <img src='http://bulma.io/images/placeholders/640x480.png'> </figure> --> <br> <div> <a href='/red' style='margin-left: 80px; background-color: red; color: white' class='button is-large' type='button' name='red'>RED</a> <a href='/green' style='margin-left: 30px; background-color: green; color: white' class='button is-large' type='button' name='green'>GREEN</a> <a href='/blue' style='margin-left: 30px; background-color: blue; color: white' class='button is-large' type='button' name='blue'>BLUE</a> </div> <br> <div> <a href='/purple' style='margin-left: 120px; background-color: purple; color: white' class='button is-large' type='button' name='purple'>PURPLE</a> <a href='/teal' style='margin-left: 30px; background-color: aqua' class='button is-large' type='button' name='teal'>TEAL</a> </div> </article> </div> </div> <div class='tile is-parent'> <article class='tile is-child notification is-danger'> <p class='title'>LCD Communications</p> <p class='subtitle'>Send a message to the LCD!</p> <div class='content'> <form action='/textbox' method='get'> Text: <input type='text' name='textbox' , id='myText'> <input class='button is-success is-small' type='submit' , id='myButton' value='Submit' onclick='textBoxPrint()'> </form> </div> </article> </div> </div> <div class='tile is-parent'> <article class='tile is-child notification is-success'> <div class='content'> <p class='title'>Temperature Sensor</p> <p class='subtitle'>Retrieves ambient temperature</p> <div class='content'> <p id='cardTime'>Suh Dude</p> </div> </div> </article> </div> </div></div><footer class='footer'> <div class='container'> <div class='content has-text-centered'> <p>Built with HTML/JavaScript using Bulma CSS Framework</p> </div> </div></footer><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script><script type='text/javascript'> function consoleLog() { console.log('Clicked on button!'); } function textBoxPrint() { var textBox = document.getElementById('myText').value; console.log(textBox); } $(document).ready(function() { $('#myButton').click(function() { var textBox = document.getElementById('myText').value; console.log('Clicked My Button!'); $.get('/textbox', { text: textBox }); }); $('#timerSubmit').click(function() { var RequestHTTP = new XMLHttpRequest(); var timer = $('#timerSelect').val(); if (timer == '10') { console.log('Timer = 10'); RequestHTTP.open('GET', '/timer1', true); RequestHTTP.send(); } else if (timer == '20') { RequestHTTP.open('GET', '/timer2', true); RequestHTTP.send(); } else if (timer == '30') { RequestHTTP.open('GET', '/timer3', true); RequestHTTP.send(); } }); });</script>";
const char* host = "192.168.45.73";

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }
    WiFi.begin("USF CONNECT", "ReSearcH2012");  //Connect to the WiFi network

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
