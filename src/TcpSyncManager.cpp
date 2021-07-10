//
//  TcpSyncManager.cpp
//  SuperSyncedMediaPlayer
//
//  Created by FlaVor on 01.07.21.
//

#include "TcpSyncManager.hpp"

TcpSyncManager::TcpSyncManager(): m_waitTimer(this)
{
}

TcpSyncManager::~TcpSyncManager()
{
    if (m_waitTimer.isThreadRunning())
    {
        m_waitTimer.waitForThread();
    }
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
    m_waitTimer.startThread();
    ofLogNotice() << "Current system time ms: " << ofGetCurrentTime().getAsMilliseconds();
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
            if (msg == "")
            {
                // Nothing to do here
            }
            else if (ofIsStringInString(msg,CMD_PLAY))
            {
                std::vector<std::string> msgParts = ofSplitString(msg, CMD_DELIMITER);
                m_nextAction = PLAY_ACTION;
                setNextActionTime(ofToInt64(msgParts.at(1)));
                ofLogNotice() << "PLAY received from server: " << msg << flush;
            }
            else if (ofIsStringInString(msg, CMD_PAUSE))
            {
                std::vector<std::string> msgParts = ofSplitString(msg, CMD_DELIMITER);
                m_nextAction = PAUSE_ACTION;
                setNextActionTime(ofToInt64(msgParts.at(1)));
                
                ofLogNotice() << "PAUSE received from server" << msg << flush;
            }
            else
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
            calcNextActionTime();
            std::string playCommand = CMD_PLAY + CMD_DELIMITER + ofToString(m_timeForAction);

            for (int i = 0; i < m_server.getLastID(); i++)
            {
                if (m_server.isClientConnected(i)) {

                    m_server.send(i, playCommand);
                }
            }
            m_nextAction = PLAY_ACTION;
            ofLogNotice() << "sended: " << playCommand << flush;
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

            calcNextActionTime();
            std::string pauseCommand = CMD_PAUSE + CMD_DELIMITER + ofToString(m_timeForAction);

            for (int i = 0; i < m_server.getLastID(); i++)
            {
                if (m_server.isClientConnected(i)) {

                    m_server.send(i, pauseCommand);
                }
            }
            m_nextAction = PAUSE_ACTION;
        }
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
        // start

        while (isThreadRunning()) {
            update();
        }

}

void TcpSyncManager::doAction()
{
    setNextActionTime(0);
    switch (m_nextAction)
    {
    case PLAY_ACTION:
        m_player->setFrame(0);
        m_player->play();
        break;
    case PAUSE_ACTION:
        if (m_player->isPaused())
        {
            m_player->setPaused(false);
        }
        else {
            m_player->setPaused(true);
        }
        break;
    case STOP_ACTION:
        if (m_player->isPlaying())
        {
            m_player->stop();
        }
        break;
    case NO_ACTION:
    default:
        break;
    }
    m_nextAction = NO_ACTION;
}

void TcpSyncManager::calcNextActionTime()
{
    uint64_t currentTimeMillis = ofGetCurrentTime().getAsMilliseconds();
    setNextActionTime(((currentTimeMillis / 100) * 100) + TIME_OFFSET_FOR_COMMANDS);
}

void TcpSyncManager::setNextActionTime(uint64_t time)
{
    m_waitTimer.lock();
    m_timeForAction = time;
    m_waitTimer.unlock();
}

TcpSyncManager::WaitTimer::WaitTimer(TcpSyncManager* parent)
{
    m_parent = parent;
    m_timer.setPeriodicEvent(1000000);
}

TcpSyncManager::WaitTimer::~WaitTimer()
{
    if (isThreadRunning())
    {
        stopThread();
    }
}

void TcpSyncManager::WaitTimer::threadedFunction()
{

    while (isThreadRunning()) {
        m_timer.waitNext();
        if (m_parent->m_timeForAction != 0)
        {
            uint64_t currentTime = ofGetCurrentTime().getAsMilliseconds();
            if (currentTime >= m_parent->m_timeForAction)
            {
                ofLogNotice() << currentTime << " - " << m_parent->m_timeForAction << flush;
                m_parent->doAction();
            }
        }
    }
}
