//
//  LEDWebServer.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 10/12/14 .
//
//

#include "LEDWebServer.h"
#include "CWPatternSequenceManager.h"

#if SD_CARD_SUPPORT
#include "SD.h"
#endif

#include <JsonGenerator.h>
#include <aJSON.h>

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
 
 ### POST /sequences/{name}/start
 Starts the sequence with that name, or restarts it if it is already playing
 
 ### DELETE /sequences/{name}
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
        const char *helloMsg = "<html><body><h1>corbin's<br><br>LED Cyr Wheel Server</h1></body></html>";
        
        /* this is a special form of print that outputs from PROGMEM */
        server.print(helloMsg);
    }
}

#define GET_SEQUENCE_MANAGER ((LEDWebServer *)&server)->getSequenceManager()

static inline void getSequencesList(WebServer &server) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    
    JsonArray<MAX_SEQUENCES> array;
    int count = manager->getNumberOfSequenceNames();
    if (count > MAX_SEQUENCES) {
        // only return max...sort of ugly
        count = MAX_SEQUENCES;
    }
    // deal with stack based memory..
    JsonObject<2> sequences[MAX_SEQUENCES];
    for (int i = 0; i < count; i++) {
        sequences[i]["name"] = manager->getSequenceNameAtIndex(i);
        sequences[i]["editable"] = manager->isSequenceEditableAtIndex(i);
        array.add(sequences[i]);
    }
    server.print(array);
}

static inline void getSequenceInfoAtIndex(WebServer &server, int i) {
    server.httpSuccess(JSON_CONTENT_TYPE);

    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    JsonObject<2> sequence;
    sequence["name"] = manager->getSequenceNameAtIndex(i);
    sequence["editable"] = manager->isSequenceEditableAtIndex(i);

    server.print(sequence);
}


static inline char *moveToNextPathComponent(char *p) {
    while (*p) {
        if (*p == '/') {
            // Overwrite it...we indirctly own the buffer
            *p = NULL;
            // Go past it
            p++;
            break;
        } else {
            p++;
        }
    }
    return p;
}

//static inline void addNewSequence(WebServer &server) {
//    server.httpSuccess(JSON_CONTENT_TYPE);
//    
//}

void commandGetSequences(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    bool result = false;
    if (type != WebServer::HEAD) {
        // See what was being asked for... (make sure we are at the end)
        if (url_tail == NULL || (*url_tail == NULL)) {
            getSequencesList(server);
            return; // Done (avoids sending at the end...yeah, i should refactor this)
        } else {
            CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
            // It should be an operation on a particular sequence.
            // Find the name
            // We are destructive on the buffer...
            char *sequenceName = url_tail;
            char *sequenceNameEnd = moveToNextPathComponent(sequenceName);
            size_t sequenceNameLength = sequenceNameEnd - sequenceName;
            int sequenceIndex = -1;
            if (sequenceNameLength > 0) {
                sequenceIndex = manager->getIndexOfSequenceName(sequenceName);
            }
            if (sequenceIndex >= 0) {
                // Find the action
                char *actionStart = sequenceNameEnd;
                char *actionEnd = moveToNextPathComponent(actionStart);
                size_t actionLength = actionEnd - actionStart;
//                DEBUG_PRINTF("NAME: %s, action: %s, len: %d;\r\n", sequenceName, actionStart, actionLength);
                if (actionLength > 0) {
                    if (strncmp("play", actionStart, 4) == 0) {
                        DEBUG_PRINTLN("PLAY");
                        manager->setCurrentSequenceAtIndex(sequenceIndex);
                        result = true;
                    } else if (strncmp("delete", actionStart, 6)== 0)  {
                        DEBUG_PRINTLN("delete");
                        result = manager->deleteSequenceAtIndex(sequenceIndex);
                    }
                } else {
                    // No action...return info on that action
                    // What we do is dependent on the method
                    if (type == WebServer::DELETE) {
                        result = manager->deleteSequenceAtIndex(sequenceIndex);
                    } else if (type == WebServer::GET) {
                        getSequenceInfoAtIndex(server, sequenceIndex);
                        result = true;
                    } else if (type == WebServer::POST) {
                        // Poasting a new sequence
//                        addNewSequence(server); // wait???
                    }
                }
            }
        }
    }
    if (result) {
        server.httpSuccess();
    } else {
        server.httpFail();
    }
}

static void returnError(WebServer &server) {
    server.httpFail();
}

static bool readFormDataAddSequence(WebServer &server) {
    bool result = false;
    //            Serial.println("found multi-part form data..................");
    char filenameBuffer[PATH_COMPONENT_BUFFER_LEN];
    if (server.skipToNextMultipartBoundaryStart()) {
        // Read parameters
        char name[16];
        char filename[16];
        while (server.readMultipartFormDataName(name, 16, filename, 16)) {
            // check for my specific data and content type
            if (strcmp(name, "new_file") == 0 && filename[0] != NULL && (strcmp(server.getContentType(), "application/x-pattern") == 0)) {
                Serial.printf(" - new file: %s, contentType: %s\n", filename, server.getContentType());
                // It's the file!
                // validate the filename length?? 8.3 format...
                CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
                // TODO: uppercase it...???
                if (manager->getIndexOfSequenceName(filename) == -1) {
                    // Doesn't exist (I think..)
                    // open up the file for writing... and write it.
                    const char *path = manager->getSequencePath();

#define STACK_BUFFER_SIZE 32
                    char fullFilenameWithPath[STACK_BUFFER_SIZE];
                    int pathLength = strlen(path);
                    // TODO: better error stuff
                    if ((pathLength + 1 + strlen(filename) + 1) > STACK_BUFFER_SIZE) {
                        return false;
                    }
                    
                    strcpy(fullFilenameWithPath, path);
                    strcpy(&fullFilenameWithPath[pathLength], filename);
                    DEBUG_PRINTLN("upload START!");
                    
                    File sequenceFile = SD.open(fullFilenameWithPath, FILE_WRITE);
//                    Serial.printf(" - creating file\n");
                    if (sequenceFile.name()) { // indicates creation..
//                        Serial.printf(" - creation worked\n");
                        result = true;
                        int totalRead = 0;
                        int amountRead = 0;
                        bool done = false;
                        do {
                            char buffer[256];
                            amountRead = 0;
                            done = server.readMultipartFormDataContent(buffer, 256, amountRead);
                            totalRead += amountRead;
//                            Serial.printf(" - read %d bytes, total: %d\n", amountRead, totalRead);
                            if (amountRead > 0) {
                                int amountWritten = sequenceFile.write(buffer, amountRead);
//                                Serial.printf(" - wrote %d bytes\n\n", amountWritten);
                            }
                        } while (amountRead > 0 && !done);
                        // close the file, and flush...refresh stuff...
                        
//                        Serial.printf(" - done reading...\n");
                        sequenceFile.close();
                        sequenceFile.flush();
                        DEBUG_PRINTLN("upload DONE!");
                    } else {
                        // TODO: error about not being able to write the file
                        
                    }
                } else {
                    // TODO: specify why we fail in an error , because the file already existed
                    
                }
            } else {
                // Serial.printf("\n **** skipping item with name: %s\n", name); // corbin
            }
        }
        
    }
    return result;
}

void commandAddSequence(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    bool success = false;
    if (type == WebServer::POST) {
//        Serial.printf("getContentType: %s\n", server.getContentType());
        if (server.getContentType() && strcmp(server.getContentType(), "multipart/form-data") == 0) {
            success = readFormDataAddSequence(server);
        }
    }
    if (success) {
        Serial.println("worked!");
        server.httpSuccess(JSON_CONTENT_TYPE);

        // Then, load the sequences after we sent the data
//        delay(100); // yeah, delay to give the send some time
        manager->loadSequencesFromDisk();
    } else {
        // reason?
        server.httpFail();
    }
}

void commandNextPattern(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->nextPatternItem();
}

void commandPriorPattern(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->priorPatternItem();
}

void commandNextSequence(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    DEBUG_PRINTLN("starting load of next sequence...");
    
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->loadNextSequence();
    DEBUG_PRINTLN("...done load of next sequence; sending response");
}

void commandPriorSequence(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->loadPriorSequence();
}

void commandRestartSequence(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->restartCurrentSequence();
}

void commandStartCalibration(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->startCalibration();
}

void commandEndCalibration(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // TODO: send back the calibration state...
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->endCalibration();

}

void commandCancelCalibration(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // TODO: send back the calibration state...
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->cancelCalibration();
    
}

void commandStartSavingData(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    // We don't send any data back.. we just do it
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->startRecordingData();
}

void commandEndSavingData(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    server.httpSuccess(JSON_CONTENT_TYPE);
    CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
    manager->endRecordingData();
    
}

void commandSetDynamicPatternItem(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
    Serial.println("dynamic test");
    // Read in the type..
    if (strcmp(JSON_CONTENT_TYPE, server.getContentType()) == 0) {
//        Serial.println("got json");
//        aJsonClientStream jsonStream = aJsonClientStream(server.getClient());
//        aJsonObject *rootObject = aJson.parse(&jsonStream);
//        
//        char *string = aJson.print(rootObject);
//        if (string != NULL) {
//            Serial.println("got str");
//            Serial.println(string);
//        }
//        else {
//            Serial.println("nuttin");
//        }
//
        
        CWPatternSequenceManager *manager = GET_SEQUENCE_MANAGER;
        
        
        server.httpSuccess(JSON_CONTENT_TYPE);
    } else {
        Serial.printf("no json???; %s\n", server.getContentType());
        server.httpFail();
    }
    // TODO: stop using the other JSON library.....as it isn't as dynamic as I'd like or as easy to use for parsing
}

LEDWebServer::LEDWebServer(const char *urlPrefix, uint16_t port) : WebServer(urlPrefix, port) {
    
}

void LEDWebServer::begin() {
    DEBUG_PRINTLN("Webserver begin");
    /* setup our default command that will be run when the user accesses
     * the root page on the server */
    setDefaultCommand(&defaultCommand);
    
    /* run the same command if you try to load /index.html, a common
     * default page name */
    addCommand("index.html", &defaultCommand);
    
//    addCommand("sequences", &commandGetSequences);
    addCommand("sequences/add", &commandAddSequence);
    addCommand("sequences/", &commandGetSequences);
    addCommand("command/next_pattern", &commandNextPattern);
    addCommand("command/prior_pattern", &commandPriorPattern);
    addCommand("command/next_sequence", &commandNextSequence);
    addCommand("command/prior_sequence", &commandPriorSequence);
    addCommand("command/restart_sequence", &commandRestartSequence);
 
    addCommand("command/start_calibration", &commandStartCalibration);
    addCommand("command/end_calibration", &commandEndCalibration);
    addCommand("command/cancel_calibration", &commandCancelCalibration);
    
    addCommand("command/dynamic_pattern_item", &commandSetDynamicPatternItem);
    /// get state...
    
//    addCommand("command/is_calibrating", &commandEndCalibration);
    
    addCommand("command/start_saving_gyro_data", &commandStartSavingData);
    addCommand("command/end_saving_gyro_data", &commandEndSavingData); // 12
    
//    addCommand("gyro_data_files", )
    
    WebServer::begin();
}

void LEDWebServer::process() {
    // Slow down the normal processing to not pound SPI
    uint32_t now = millis();
    if ((now - m_lastProcessTime) >= WEB_SERVER_CHECK_INTERVAL) {   // Main loop runs at 50Hz
        m_lastProcessTime = now;
 //       DEBUG_PRINTF("%d\n", now);
        processConnection();
    }
}