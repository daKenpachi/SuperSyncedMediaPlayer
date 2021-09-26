#include "ofApp.h"
#include <chrono>

//--------------------------------------------------------------
void ofApp::setup(){
    // ofSetVerticalSync(true);

    m_xmlSettings.loadFile("settings.xml");
    string loggingFile = m_xmlSettings.getValue(XML_TAG_LOGGING_FILE, "");
    if (loggingFile != "")
    {
        ofLogToFile(loggingFile, true);
    }
    ofLogNotice() << LOG_HEADER;

    std::string videoName = m_xmlSettings.getValue(XML_TAG_VIDEO_FILE,"fingers.mov");
    m_videoPlayer.load(videoName);
    
    m_syncManager.setup(m_xmlSettings, &m_videoPlayer);
    
    double volume = m_xmlSettings.getValue(XML_TAG_VIDEO_VOLUME, 1.0);
    m_videoPlayer.setLoopState(OF_LOOP_NONE);
    m_videoPlayer.setVolume(volume);
    m_videoPlayer.setPaused(true);
    m_videoPlayer.setFrame(0);
    
    m_infoPanel.setup();
    m_infoPanel.add(m_infoLabel1.setup("Client/Server", ""));
    m_infoPanel.add(m_infoLabel2.setup("Video", "stopped"));
}

//--------------------------------------------------------------
void ofApp::update(){
    static bool restarting = false;
    m_videoPlayer.update();
    bool movieDone = m_videoPlayer.getIsMovieDone();
    int frame = m_videoPlayer.getCurrentFrame();
    if (movieDone && m_syncManager.isServer()) {
        if(!restarting) {
            ofLogNotice() << "Restarting Videos" << std::flush;
            m_syncManager.playAllVideos();
            restarting = true;
        }
    }
    else {
        restarting = false;
    }
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    m_videoPlayer.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    if (m_syncManager.isServer())
    {
        m_infoLabel1.setName("Server");
    }
    else
    {
        m_infoLabel1.setName("Client");
    }
    m_infoLabel1 = m_syncManager.getConnectionInfo();
    
    if (m_videoPlayer.isPlaying())
    {
        if (m_videoPlayer.isPaused())
        {
            m_infoLabel2 = "paused";
        }
        else
        {
            m_infoLabel2 = "playing";
        }
    }
    else
    {
        m_infoLabel2 = "stopped";
    }
    if (m_infoPanelCountdown > 0)
    {
        m_infoPanel.draw();
        m_infoPanelCountdown --;
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == OF_KEY_SPACE)
    {
        if(m_videoPlayer.isPlaying())
        {
            m_syncManager.pauseAllVideos(); // this will pause or unpause
        }
        else {
            m_syncManager.playAllVideos();
        }
    }
    else if(key == OF_KEY_BACKSPACE) {
         m_syncManager.stopAllVideos();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    m_infoPanelCountdown = INFO_PANEL_COUNT_DOWN_VALUE;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
