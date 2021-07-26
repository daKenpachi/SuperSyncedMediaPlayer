#include "ofApp.h"
#include <chrono>

//--------------------------------------------------------------
void ofApp::setup(){
    // ofSetVerticalSync(true);

    m_xmlSettings.loadFile("settings.xml");
    string loggingFile = m_xmlSettings.getValue(XML_TAG_LOGGING_FILE, "");
    if (loggingFile != "")
    {
//#ifndef NDEBUG
//        ofLogToFile(loggingFile, true);
//#endif
        ofLogNotice() << LOG_HEADER;
    }

    std::string videoName = m_xmlSettings.getValue(XML_TAG_VIDEO_FILE,"fingers.mov");
    m_videoPlayer.load(videoName);
    
    m_syncManager.setup(m_xmlSettings, &m_videoPlayer);
    
    m_videoPlayer.setVolume(0.5);
    m_videoPlayer.setFrame(0);

    //m_panel.setup();
    //m_panel.add(m_label.setup("Labename", "Ahoi, Ophelia"));
}

//--------------------------------------------------------------
void ofApp::update(){
    m_videoPlayer.update();
    //m_videoPlayer.setSpeed(0.6);
}

//--------------------------------------------------------------
void ofApp::draw(){
    m_videoPlayer.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    //m_panel.draw();
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
        if(m_videoPlayer.isPlaying()) {
            m_syncManager.stopAllVideos();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
