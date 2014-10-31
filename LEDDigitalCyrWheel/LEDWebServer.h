//
//  LEDWebServer.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 10/12/14 .
//
//

#ifndef __LEDDigitalCyrWheel__LEDWebServer__
#define __LEDDigitalCyrWheel__LEDWebServer__

#define WEBDUINO_FAVICON_DATA ""
#define WEBDUINO_COMMANDS_COUNT 14
#define WEBDUINO_OUTPUT_BUFFER_SIZE 64

#include "WebServer.h"

class CWPatternSequenceManager;

class LEDWebServer : public WebServer {
private:
    CWPatternSequenceManager *m_sequenceManager;
    uint32_t m_lastProcessTime;
public:
    LEDWebServer(const char *urlPrefix = "", uint16_t port = 80);

    inline void setSequenceManager(CWPatternSequenceManager *m) { m_sequenceManager = m; };
    inline CWPatternSequenceManager *getSequenceManager() { return m_sequenceManager; };

    void begin();
    
    void process();
    
    
};



#endif /* defined(__LEDDigitalCyrWheel__LEDWebServer__) */
