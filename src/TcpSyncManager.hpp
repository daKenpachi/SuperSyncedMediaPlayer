//
//  TcpSyncManager.hpp
//  SuperSyncedMediaPlayer
//
//  Created by FlaVor on 01.07.21.
//

#ifndef TcpSyncManager_hpp
#define TcpSyncManager_hpp

#include <stdio.h>
#include <ofxNetwork.h>
#include <ofxXmlSettings.h>

class TcpSyncManager
{
public:
    TcpSyncManager() {}
    virtual ~TcpSyncManager() {}
    
    void setup(const ofxXmlSettings& settings, ofVideoPlayer* const player);
    void update();
    
    void playAllVideos();
    void pauseAllVideos();
    
    const std::string XML_TAG_TCP = "TCP";
    const std::string XML_TAG_PORT = "TCP:Port";
    const std::string XML_TAG_IP_ADDRESS = "TCP:Address";
    const std::string XML_TAG_TCP_MODE = "TCP:Mode";
    
    const std::string MODE_SERVER = "Server";
    const std::string MODE_CLIENT = "Client";
    const std::string MODE_AUTO = "Automatic";
    
private:
    bool m_isServer;
    
    ofxTCPServer m_server;
    ofxTCPClient m_client;
    
    std::set<int> m_validClientIds;
    
    ofVideoPlayer* m_player = NULL;
    
    const std::string CMD_PLAY = "SuperSync_Play";
    const std::string CMD_PAUSE = "SuperSync_Pause";
    const std::string CMD_HELLO = "SuperSync_Hello";
};

#endif /* TcpSyncManager_hpp */
