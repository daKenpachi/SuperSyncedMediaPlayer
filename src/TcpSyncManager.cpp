//
//  TcpSyncManager.cpp
//  SuperSyncedMediaPlayer
//
//  Created by FlaVor on 01.07.21.
//

#include "TcpSyncManager.hpp"

TcpSyncManager::~TcpSyncManager()
{
    if (isThreadRunning()) {
        stopThread();
    }
}

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

    this->startThread();
}

void TcpSyncManager::update()
{
    if (m_isServer)
    {
        updateAsServer();
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

void TcpSyncManager::updateAsServer()
{
    if (m_server.isConnected())
    {
        for(unsigned int i = 0; i <  m_server.getLastID(); i++)
        {
            if (m_server.isClientConnected(i))
            {

                string msg = m_server.receive(i);
                if (!m_validClientIds.hasId(i)) {
                    // client not valid yet
                    m_validClientIds.addId(); // internal increment will follow client increment
                    ofLogNotice() << "Client " << i << " at " << m_server.getClientIP(i) << " tries to connect" << flush;
                }
                if (m_validClientIds.hasId(i))
                {
                    if (!m_validClientIds.isIdValid(i))
                    {
                        // check for handshake message
                        if (msg == CMD_HELLO)
                        {
                            m_validClientIds.validIds.at(i) = true;
                            ofLogNotice() << "Client " << i << " at " << m_server.getClientIP(i) << " successfully connected" << flush;
                            m_server.send(i, CMD_HELLO);
                        }
                        else if (msg != "")
                        {
                            m_validClientIds.retries.at(i) = m_validClientIds.retries.at(i) + 1;
                            if (m_validClientIds.retries.at(i) >= MAX_HANDSHAKE_RETRIES)
                            {
                                // client did not send right command - reject
                                ofLogWarning() << "Client " << i << " at " << m_server.getClientIP(i) << " was rejected due to handshake timeout"
                                               << " after " << m_validClientIds.retries.at(i) << " retries" << flush;
                                m_server.disconnectClient(i);
                            }
                        }
                    }
                    else
                    {
                        checkMessageAsServer(msg, i);
                    }
                }

            }
        }
    }
}

void TcpSyncManager::checkMessageAsServer(const std::string& msg, int clientid)
{
    if (msg == CMD_PLAY)
    {
        ofLogNotice() << "PLAY command received from client " << clientid << flush;
        playAllVideos();
    }
    else if (msg == CMD_PAUSE)
    {
        ofLogNotice() << "PAUSE command received from client " << clientid << flush;
        pauseAllVideos();
    }
    else if (msg != "")
    {
        ofLogWarning() << "Unkown message received from client " << clientid << ": " << msg << flush;
    }
}

void TcpSyncManager::playAllVideos()
{
    if (m_isServer)
    {
        if (m_server.isConnected())
        {
            ofLogNotice() << "sending PLAY command" << flush;

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

void TcpSyncManager::threadedFunction()
{
    {

        // start

        while (isThreadRunning()) {
            update();

        }

        // done
    }
}
