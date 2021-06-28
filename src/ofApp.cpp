#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // ofSetVerticalSync(true);

    m_xmlSettings.loadFile("settings.xml");
    std::string videoName = m_xmlSettings.getValue(XML_TAG_VIDEO_FILE,"fingers.mov");
    int port = m_xmlSettings.getValue(XML_TAG_PORT, 0);
    std::string ipAddress = m_xmlSettings.getValue(XML_TAG_IP_ADDRESS, "localhost");
    std::string tcpMode = m_xmlSettings.getValue(XML_TAG_TCP_MODE, "Client");
    
    if (tcpMode == MODE_CLIENT){
        m_isServer = false;
        bool success = m_client.setup(ipAddress, port);
    }
    else if (tcpMode == MODE_SERVER) {
        m_isServer = true;
        bool success = m_server.setup(port);
    }
    else {
        // TODO this is not working - inform user!
    }

    m_videoPlayer.load(videoName);
    m_videoPlayer.setVolume(0.5);
    m_videoPlayer.play();

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
            m_videoPlayer.setPaused(true);
        }
        else {
            m_videoPlayer.setPaused(false);
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
