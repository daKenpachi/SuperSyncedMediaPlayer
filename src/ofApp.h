#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"

#include "TcpSyncManager.hpp"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxPanel m_panel;
    ofxLabel m_label;

    ofxPanel m_videoControlPanel;
    ofVideoPlayer m_videoPlayer;

    ofxXmlSettings m_xmlSettings;
    
    TcpSyncManager m_syncManager;

private:
    const std::string XML_TAG_VIDEO_FILE = "VideoFile";

    const int OF_KEY_SPACE = 32;
};