//
//  LEDWebServer.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 10/12/14 .
//
//

#include "LEDWebServer.h"
#include "CWPatternSequenceManager.h"

#include <JsonGenerator.h>

/* REST API documentation:

 ### GET /status
 Status information of what sequence is currently loaded and the state it is at in the sequence.

 ### GET /command/next_pattern
 ### GET /command/prior_pattern
 
 ### GET /command/next_sequence
 ### GET /command/prior_sequence
 ### GET /command/restart_sequence // restarts the currrent sequence


 
 ### GET /command/start_calibration
 ### GET /command/end_calibration

 
 
 ### GET /sequences
 Returns a list of sequence names on the card, including the default ones.

 ### GET /sequences/{name}/
 Returns information on that particular sequence (requires a read of the SD card)
 
 ### GET /sequences/{name}/start
 Starts the sequence with that name, or restarts it if it is already playing
 
 ### DELETE /sequences/{name}/delete
 Delete the sequence with that name.
 
 ### POST /sequences/add
    + an upload of the data adds the said filename sequence. Must end in .pat.
 
 
 

 
 
 */

#define JSON_CONTENT_TYPE "application/json"

#define WEB_SERVER_CHECK_INTERVAL 100 // every 100ms
#define MAX_SEQUENCES 32 // TODO: allow more.......

using namespace ArduinoJson::Generator;

// should use uinstd.h to define sbrk but on Arduino Due this causes a conflict
extern int getFreeRam();

void defaultCommand(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
//    Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
    
    server.httpSuccess();
        
    if (type != WebServer::HEAD)
    {
        /* this defines some HTML text in read-only memory aka PROGMEM.
         * This is needed to avoid having the string copied to our limited
         * amount of RAM. */
        const char *helloMsg = "<html><body><h1>Hello, Costanza! <br><br> LED Cyr Wheel Server</h1></body></html>";
        
        /* this is a special form of print that outputs from PROGMEM */
        server.print(helloMsg);
    }
}

#define GET_SEQUENCE_MANAGER ((LEDWebServer *)&server)->getSequenceManager()

void commandGetSequences(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    if (type != WebServer::HEAD) {
        CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
        
        JsonArray<MAX_SEQUENCES> array;
        int count = manager->getNumberOfSequenceNames();
        if (count > MAX_SEQUENCES) {
            // only return max...sort of ugly
            count = MAX_SEQUENCES;
        }
        for (int i = 0; i < count; i++) {
            array.add(manager->getSequenceNameAtIndex(i));
        }
        server.print(array);
    }
}

void commandNextPattern(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess();
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->nextPatternItem();
}

void commandPriorPattern(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess();
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->priorPatternItem();
}

void commandNextSequence(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess();
    DEBUG_PRINTLN("starting load of next sequence...");
    
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->loadNextSequence();
    DEBUG_PRINTLN("...done load of next sequence; sending response");
}

void commandPriorSequence(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess();
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->loadPriorSequence();
}

void commandRestartSequence(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess();
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->restartCurrentSequence();
}

LEDWebServer::LEDWebServer(const char *urlPrefix, uint16_t port) : WebServer(urlPrefix, port) {
    
}

void LEDWebServer::begin() {
    /* setup our default command that will be run when the user accesses
     * the root page on the server */
    setDefaultCommand(&defaultCommand);
    
    /* run the same command if you try to load /index.html, a common
     * default page name */
    addCommand("index.html", &defaultCommand);
    
    addCommand("sequences", &commandGetSequences);
    addCommand("command/next_pattern", &commandNextPattern);
    addCommand("command/prior_pattern", &commandPriorPattern);
    addCommand("command/next_sequence", &commandNextSequence);
    addCommand("command/prior_sequence", &commandPriorSequence);
    addCommand("command/restart_sequence", &commandRestartSequence);
    
    WebServer::begin();
}

void LEDWebServer::process() {
    // Slow down the normal processing to not pound SPI
    uint32_t now = millis();
    if ((now - m_lastProcessTime) >= WEB_SERVER_CHECK_INTERVAL) {   // Main loop runs at 50Hz
        m_lastProcessTime = now;
        processConnection();
    }
}