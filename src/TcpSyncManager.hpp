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

class TcpSyncManager : public ofThread
{
public:
    TcpSyncManager();
    virtual ~TcpSyncManager();
    
    void setup(const ofxXmlSettings& settings, ofVideoPlayer* const player);
    
    void playAllVideos();
    void pauseAllVideos();
    void stopAllVideos();

    static uint64_t getUnixTimestampMs();
    
    const std::string XML_TAG_TCP = "TCP";
    const std::string XML_TAG_PORT = "TCP:Port";
    const std::string XML_TAG_IP_ADDRESS = "TCP:Address";
    const std::string XML_TAG_TCP_MODE = "TCP:Mode";
    const std::string XML_TAG_TIME_OFFEST = "TimeOffset";
    
    const std::string MODE_SERVER = "Server";
    const std::string MODE_CLIENT = "Client";
    const std::string MODE_AUTO = "Automatic";

    const int MAX_HANDSHAKE_RETRIES = 5;

    bool isServer();
    std::string getConnectionInfo();
    
private:

    class WaitTimer : public ofThread
    {
    public:
        WaitTimer(TcpSyncManager* parent);
        virtual ~WaitTimer();
        void threadedFunction();
    private :
        ofTimer m_timer;
        TcpSyncManager* m_parent;
    };
   
    typedef enum {
        NO_ACTION,
        PLAY_ACTION,
        PAUSE_ACTION,
        STOP_ACTION
    } NextAction;

    typedef struct {
        std::vector<bool> validIds;
        std::vector<int> retries;

        bool hasId(unsigned int id) {
            return (id < validIds.size());
        }

        bool isIdValid(unsigned int id) {
            if (hasId(id)) {
               return validIds.at(id);
            }
            return false;
        }
        void addId(){
            validIds.push_back(false);
            retries.push_back(0);
        }
    } ClientValidationList;

    void update();
    void threadedFunction(); 
    void doAction();
    void calcNextActionTime();
    void setNextActionTime(uint64_t time);

    bool m_isServer;
    
    ofxTCPServer m_server;
    ofxTCPClient m_client;
    
    ClientValidationList m_validClientIds;
    
    ofVideoPlayer* m_player = NULL;
    
    const std::string CMD_PLAY = "SuperSync_Play";
    const std::string CMD_PAUSE = "SuperSync_Pause";
    const std::string CMD_STOP= "SuperSync_Stop";
    const std::string CMD_HELLO = "SuperSync_Hello";
    const std::string CMD_DELIMITER = ":::";

    int m_timeOffset;
    NextAction m_nextAction = NO_ACTION;
    WaitTimer m_waitTimer;
    uint64_t m_timeForAction = 0;

    void updateAsServer();
    void updateAsClient();
    void checkMessageAsServer(const std::string& msg, int clientid);
};

#endif /* TcpSyncManager_hpp */
