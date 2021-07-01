//
//  TcpSyncManager.cpp
//  SuperSyncedMediaPlayer
//
//  Created by FlaVor on 01.07.21.
//

#include "TcpSyncManager.hpp"



void TcpSyncManager::setup(const ofxXmlSettings& settings, ofVideoPlayer* const player)
{
    int port = settings.getValue(XML_TAG_PORT, 0);
    std::string ipAddress = settings.getValue(XML_TAG_IP_ADDRESS, "localhost");
    std::string tcpMode = settings.getValue(XML_TAG_TCP_MODE, "Client");
    
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
    m_player = player;
}

void TcpSyncManager::update()
{
    
}
