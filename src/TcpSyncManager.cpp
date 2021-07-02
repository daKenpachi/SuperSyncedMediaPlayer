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
    
    if (tcpMode == MODE_CLIENT)
    {
        m_isServer = false;
        bool success = m_client.setup(ipAddress, port);
        if (success) {
            m_client.send(CMD_HELLO);
            ofLogNotice() << "Successfully connected to " << ipAddress << ":" << port << " as client" << flush;
        }
        else {
            ofLogError() << "Could not connect to server at " << ipAddress << ":" << port << flush;
        }
    }
    else if (tcpMode == MODE_SERVER) 
    {
        m_isServer = true;
        bool success = m_server.setup(port);
        if (success) {
            ofLogNotice() << "Successfully opened server at " << ipAddress << ":" << port << flush;
        }
        else {
            ofLogError()  << "Could not open server at " << ipAddress << ":" << port << flush;
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
                if (m_server.isClientConnected(i))
                {

                    string msg = m_server.receive(i);
                    if (m_validClientIds.find(i) != m_validClientIds.end())
                    {
                        // client has to validate itself
                        if (msg == CMD_HELLO)
                        {
                            m_validClientIds.insert(i);
                            ofLogError()  << "Client " << i << " connected successfully" << flush;
                        }
                        else
                        {
                            // TODO - reject client?
                        }
                    }
                    else if (msg == CMD_PLAY)
                    {
                        ofLogNotice() << "PLAY command received from client " << i << flush;
                        playAllVideos();
                    }
                    else if (msg == CMD_PAUSE)
                    {
                        ofLogNotice() << "PAUSE command received from client " << i << flush;
                        pauseAllVideos();
                    }
                    else if (msg != "")
                    {
                        ofLogWarning() << "Unkown message received from client " << i << ": " << msg << flush;
                    }
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
                ofLogNotice() << "PLAY received from server" << flush;
            }
            else if (msg == CMD_PAUSE)
            {
                m_player->setPaused(true);
                ofLogNotice() << "PAUSE received from server" << flush;
            }
            else if (msg != "")
            {
                ofLogWarning()  << "Unkown message received from server: " << msg << flush;
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
            ofLogNotice() << "sending PLAY command" << flush;
            //for (auto iter = m_validClientIds.begin(); iter != m_validClientIds.end(); iter++)
            //{
            //    m_server.send(*iter, CMD_PLAY);
            //}
            for (int i = 0; i < m_server.getLastID(); i++)
            {
                if (m_server.isClientConnected(i)) {

                    m_server.send(i, CMD_PLAY);
                }
            }
            m_player->setPaused(false);
        }
    }
    else {
        if (m_client.isConnected())
        {
            m_client.send(CMD_PLAY);
        }
    }
}

void TcpSyncManager::pauseAllVideos()
{
    if (m_isServer)
    {
        if (m_server.isConnected())
        {
            ofLogNotice() << "sending PAUSE command" << flush;
            //for (auto iter = m_validClientIds.begin(); iter != m_validClientIds.end(); iter++)
            //{
            //    m_server.send(*iter, CMD_PAUSE);
            //}
            for (int i = 0; i < m_server.getLastID(); i++)
            {
                if (m_server.isClientConnected(i)) {

                    m_server.send(i, CMD_PAUSE);
                }
            }
        }
        m_player->setPaused(true);
    }
    else {
        if (m_client.isConnected())
        {
            m_client.send(CMD_PAUSE);
        }
    }
}
