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
        if (success) {
            m_client.send(CMD_HELLO);
            ofLogNotice() << "Successfully connected to " << ipAddress << ":" << port << " as client";
        }
        else {
            ofLogError() << "Could not connect to server at " << ipAddress << ":" << port;
        }
    }
    else if (tcpMode == MODE_SERVER) {
        m_isServer = true;
        bool success = m_server.setup(port);
        if (success) {
            ofLogNotice() << "Successfully opened server at " << ipAddress << ":" << port;
        }
        else {
            ofLogError()  << "Could not open server at " << ipAddress << ":" << port;
        }
        
    }
    else {
        ofLogError()  << "No valid TCP setting!";
    }
    m_player = player;
}

void TcpSyncManager::update()
{
    if (m_isServer)
    {
        if (m_server.isConnected())
        {
            for(int i = 0; i <  m_server.getLastID(); i++)
            {
                string msg = m_server.receive(i);
                if (m_validClientIds.find(i) != m_validClientIds.end())
                {
                    // client has to validate itself
                    //if (msg == CMD_HELLO)
                    //{
                        m_validClientIds.insert(i);
                        ofLogError()  << "Client " << i << " connected successfully";
                    //}
                    //else
                    //{
                        // TODO - reject client?
                    //}
                }
                else
                {
                    // TODO client valid - react to message
                }
            }
        }
    }
    else
    {
        if (m_client.isConnected())
        {
            string msg = m_client.receive();
            if (msg == CMD_PLAY)
            {
                m_player->setPaused(false);
            }
            else if (msg == CMD_PAUSE)
            {
                m_player->setPaused(true);
            }
            else if (msg != "")
            {
                ofLogError()  << "Unkown message received from server: " << msg;
            }
        }
    }
}

void TcpSyncManager::playAllVideos()
{
    if (m_isServer)
    {
        if (m_server.isConnected())
        {
            for (auto iter = m_validClientIds.begin(); iter != m_validClientIds.end(); iter++)
            {
                m_server.send(*iter, CMD_PLAY);
            }
            m_player->setPaused(false);
        }
    }
}

void TcpSyncManager::pauseAllVideos()
{
    if (m_isServer)
    {
        if (m_server.isConnected())
        {
            for (auto iter = m_validClientIds.begin(); iter != m_validClientIds.end(); iter++)
            {
                m_server.send(*iter, CMD_PAUSE);
            }
        }
        m_player->setPaused(true);
    }
}
