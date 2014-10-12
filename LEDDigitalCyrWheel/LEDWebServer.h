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
#include "WebServer.h"


class CWPatternSequenceManager;

class LEDWebServer : WebServer {
private:
    CWPatternSequenceManager *m_sequenceManager;
public:
    LEDWebServer(const char *urlPrefix = "", uint16_t port = 80) : WebServer(urlPrefix, port) { };

    inline void setSequenceManager(CWPatternSequenceManager *m) { m_sequenceManager = m; };
    void begin();
    
    void process();
    
    
};



#endif /* defined(__LEDDigitalCyrWheel__LEDWebServer__) */
