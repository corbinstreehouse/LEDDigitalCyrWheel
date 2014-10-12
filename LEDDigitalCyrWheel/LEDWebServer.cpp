//
//  LEDWebServer.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 10/12/14 .
//
//

#include "LEDWebServer.h"
#include "CWPatternSequenceManager.h"

void helloCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
    
    /* this line sends the standard "we're all OK" headers back to the
     browser */
    server.httpSuccess();
    
    /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
    if (type != WebServer::HEAD)
    {
        /* this defines some HTML text in read-only memory aka PROGMEM.
         * This is needed to avoid having the string copied to our limited
         * amount of RAM. */
        const char *helloMsg = "<html><body><h1>Hello, Costanza!</h1></body></html>";
        
        /* this is a special form of print that outputs from PROGMEM */
        server.print(helloMsg);
    }
}

void LEDWebServer::begin() {
    /* setup our default command that will be run when the user accesses
     * the root page on the server */
    setDefaultCommand(&helloCmd);
    
    /* run the same command if you try to load /index.html, a common
     * default page name */
    addCommand("index.html", &helloCmd);
    
    
    
    WebServer::begin();
}


void LEDWebServer::process() {
    // Anything extra here?
    processConnection();
}