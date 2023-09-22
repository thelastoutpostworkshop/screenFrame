// Web server Functions
//
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "secrets.h"
#ifndef SCREEN_FRAME
#include "screenFrame.h"
#endif

WebServer server(80);

// Web server host name
const char *hostName = "frame";

// CSS for the web application
const char *css = "<style>\
                          .loading {\
                            background-color: #f3f3f3;\
                            cursor: not-allowed;\
                          }\
                          .spinner {\
                            display: inline-block;\
                            width: 40px;\
                            height: 40px;\
                            border: 5px solid rgba(0,0,0,.2);\
                            border-top-color: #008000;\
                            border-radius: 100%;\
                            animation: spin 1s linear infinite;\
                          }\
                          @keyframes spin {\
                            to { transform: rotate(360deg); }\
                          }\
                          .column {\
                            width: 50%;\
                            padding: 10px;\
                            box-sizing: border-box;\
                          }\
                          .separator {\
                            width: 2px;\
                            background: #ddd;\
                            height: 100%;\
                            position: absolute;\
                            left: 50%;\
                            top: 0;\
                          }\
                          .left { float: left; }\
                          .right { float: right; }\
                          h1 { text-align: center; }\
                    .label {\
                      font-weight: bold;\
                      font-size: 25px;\
                    }\
                    input[type=\"text\"] {\
                      background-color: #f3f3f3;\
                      border: 1px solid #ccc;\
                      padding: 10px;\
                      width: 50%;\
                      box-sizing: border-box;\
                      font-size: 25px;\
                    }\
                    input[type=\"submit\"] {\
                      background-color: #008800;\
                      color: white;\
                      border: none;\
                      padding: 10px;\
                      text-transform: uppercase;\
                      cursor: pointer;\
                      font-size: 25px;\
                      margin-top: 10px;\
                      width: 50%;\
                    }\
                    input[type=\"submit\"]:hover {\
                      background-color: #006600;\
                    }\
                          button {\
                            font-size: 40px;\
                            padding: 15px;\
                            width: 90%;\
                            box-sizing: border-box;\
                            margin: 20px 5%;\
                            border-radius: 25px;\
                            transition: background-color 0.3s, transform 0.3s;\
                          }\
                          button:hover {\
                            background-color: #ddd;\
                          }\
                          button:active {\
                            transform: scale(0.95);\
                          }\
                          </style>";

const char *htmlPageUpdate =
    "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
    "<style>"
    "body {"
    "    font-size: 40px;" /* Change the size as needed */
    "}"
    "input[type=submit], input[type=file], #homeButton {"
    "    font-size: 40px;"    /* Change the size as needed */
    "    padding: 10px 20px;" /* Change the padding as needed */
    "}"
    "progress {"
    "    width: 100%;"
    "    height: 100px;" /* Increase the height as needed */
    "}"
    "#successMessage {"
    "    display: none;" /* Initially hidden */
    "    color: green;"
    "}"
    "</style>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
    "</form>"
    "<progress id='prg' value='0' max='100'></progress>"
    "<div id='successMessage'>"
    "Upload completed successfully!"
    "<br>"
    "<button id='homeButton' onclick='window.location=\"/\"'>Return to Home</button>"
    "</div>"
    "<script>"
    "$('form').submit(function(e){"
    "    e.preventDefault();"
    "    var form = $('#upload_form')[0];"
    "    var data = new FormData(form);"
    "    $.ajax({"
    "        url: '/upload',"
    "        type: 'POST',"
    "        data: data,"
    "        contentType: false,"
    "        processData:false,"
    "        xhr: function() {"
    "            var xhr = new window.XMLHttpRequest();"
    "            xhr.upload.addEventListener('progress', function(evt) {"
    "                if (evt.lengthComputable) {"
    "                    var per = evt.loaded / evt.total;"
    "                    $('#prg').val(Math.round(per*100));"
    "                }"
    "            }, false);"
    "            return xhr;"
    "        },"
    "        success:function(d, s) {"
    "            $('#successMessage').show();" // Show the success message and button
    "        },"
    "        error: function (a, b, c) {"
    "        }"
    "    });"
    "});"
    "</script>";

// Structure to store commands available on the web application
struct Command
{
    const char *name;
    const char *endpoint;
    void (*handler)();
};

// List of Commands available on the web application
void handleHello(void);
void handleUpdate(void);
void handleLedFinder(void);
void handleDemo(void);
void handleSuspendAnimations(void);

// Fetch commands name, route and handling function to be called
Command fetchCommands[] = {
    {"Home", "/", handleHello},
    {"Demo", "/demo", handleDemo},
    {"Suspend Animations", "/suspendAnimations", handleSuspendAnimations},
    // {"Fast Random", "/show_fastrandom", handleShowFastRandom},
    // {"Darth Vader Breathing", "/darth_vader", handleDarthVaderBreathing},
    // {"Demo Add 1 Subscriber", "/demo_plus_one_subscriber", handleDemoPlusOneSubscriber},
    // {"Demo Remove 1 Subscriber", "/demo_minus_one_subscriber", handleDemoMinusOneSubscriber},
    // {"Demo Add 2 Subscriber", "/demo_plus_two_subscriber", handleDemoPlusTwoSubscriber},
};

// Post commands name, route and handling function to be called
const char *routeLedFinderForm = "/led_finder";
Command postCommands[] = {
    {"Upload code", "/update", handleUpdate},
    {"LED Finder", routeLedFinderForm, handleLedFinder},

};

// Functions to handle post data by the forms on the web application
const char *uploadEndpoint = "/upload";
const char *ledSetEndPoint = "/led_set";

// Build a list of commands available to display on the web application
String commandsList(void)
{
    String commandList = css;

    commandList += "<div class='separator'></div>\
                          <div class='column left'>\
                          <h1>Commands</h1>";
    for (Command &cmd : fetchCommands)
    {
        if (cmd.endpoint == "/update")
        {
            commandList += "<button style='font-size: 40px; padding: 15px; width: 90%; box-sizing: border-box; margin: 20px 5%; border-radius: 25px;' onclick=\"location.href='";
            commandList += cmd.endpoint;
            commandList += "'\">";
        }
        else
        {
            if (cmd.endpoint == "/suspendAnimations")
            {
                if (suspendAnimations)
                {
                    cmd.name = "Enable Animations";
                }
                else
                {
                    cmd.name = "Suspend Animations";
                }
            }
            commandList += "<button id='";
            commandList += cmd.endpoint;
            commandList += "' style='font-size: 40px; padding: 15px; width: 90%; box-sizing: border-box; margin: 20px 5%; border-radius: 25px;' onclick='this.classList.add(\"loading\"); this.innerHTML=\"<div class=spinner></div>\"; fetch(\"";
            commandList += cmd.endpoint;
            commandList += "\").then(response => response.text()).then(data => { document.body.innerHTML = data; this.classList.remove(\"loading\"); })'>";
        }

        commandList += cmd.name;
        commandList += "</button><br/>";
    }
    commandList += "</div>\
                    <div class='column right'>\
                    <h1>Settings Configuration</h1>";
    for (Command &cmd : postCommands)
    {
        commandList += "<button style='font-size: 40px; padding: 15px; width: 90%; box-sizing: border-box; margin: 20px 5%; border-radius: 25px;' onclick=\"location.href='";
        commandList += cmd.endpoint;
        commandList += "'\">";

        commandList += cmd.name;
        commandList += "</button><br/>";
    }
    commandList += "</div>";

    return commandList;
}

void sendPlainText(const char *fmt, ...)
{
    char buf[256]; // Buffer to hold the formatted string. Adjust the size as needed.
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    server.send(200, "text/plain", buf);
}

void handleHello(void)
{
    String body = "<div style='border: 2px solid #008800; background-color: #e6f7e6; padding: 10px; border-radius: 10px; margin: 10px; font-size: 24px;'>\
                   URL: " +
                  String(hostName) + ".local<br/>" +
                  "IP address: " + WiFi.localIP().toString() + "<br/>" +
                  "Connected to: " + String(ssid) +
                  "</div>" + commandsList();
    server.send(200, "text/html", body);
}

void handleSuspendAnimations(void)
{
    suspendAnimations = !suspendAnimations;
    handleHello();
}

void handleDemo(void)
{
    frameDemo();
    server.send(200);
}

void handleUpdate(void)
{
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", htmlPageUpdate);
}

void handleNotFound()
{
    String message = "Command Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void handleLedFinder(void)
{
    server.sendHeader("Connection", "close");

    String html = css;
    html += "<form action='";
    html += ledSetEndPoint;
    html += "' method=\"post\">\
                       <label class='label'>Set LED Number:</label><br>\
                       <input type=\"text\" name=\"led_number\" size=\"50\">\
                       <br>\
                       <input type=\"submit\" value=\"Submit\">\
                     </form>";
    html += "<a href='/' class='home-button'>Home</a>"; // Home button placed after the submit button
    server.send(200, "text/html", html);
}

void handleLedSet()
{
    if (server.hasArg("led_number"))
    {
        pixels.clear();
        pixels.show();
        String channelId = server.arg("led_number");
        int startPos = 0;
        int endPos = channelId.indexOf(',');

        while (endPos != -1)
        {
            String numberStr = channelId.substring(startPos, endPos);
            int number = numberStr.toInt();
            pixels.setPixelColor(number, COLOR_WHITE);

            startPos = endPos + 1;
            endPos = channelId.indexOf(',', startPos);
        }

        // Handle the last number (or the only number if there's just one)
        int number = channelId.substring(startPos).toInt();
        pixels.setPixelColor(number, COLOR_WHITE);
        pixels.show();

        // Redirect back to the form page
        server.sendHeader("Location", routeLedFinderForm); // Replace "/path_to_form_page" with the actual path to your form page
        server.send(303);                                  // HTTP 303 See Other
    }
    else
    {
        server.send(400, "text/plain", "Missing data in request.");
    }
}

// Setup of routes for handling requests coming from the web application
void setupCommands(void)
{
    // Register the command handlers with the web server
    for (Command &cmd : fetchCommands)
    {
        server.on(cmd.endpoint, HTTP_GET, cmd.handler);
    }
    for (Command &cmd : postCommands)
    {
        server.on(cmd.endpoint, HTTP_GET, cmd.handler);
    }

    server.onNotFound(handleNotFound);

    /*handling uploading firmware file */
    server.on(
        uploadEndpoint, HTTP_POST, []()
        {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); },
        []()
        {
            HTTPUpload &upload = server.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                { // start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                { // true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });

    // Handling LED Set
    server.on(ledSetEndPoint, HTTP_POST, handleLedSet);
}

void initWebServer()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        pixels.fill(COLOR_RED);
        pixels.show();
        delay(100);
        pixels.clear();
        pixels.show();
        delay(100);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin(hostName))
    {
        Serial.println("MDNS responder started");
    }

    setupCommands();

    server.begin();
    Serial.println("HTTP server started");
}
