#ifndef MESSAGEMANAGER_H_INCLUDED
#define MESSAGEMANAGER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "Chater.h"
#include <time.h>
#include "functions.h"

class MessageManager
{
public:
    MessageManager();
    void init();
    void setRenderTexture(sf::RenderTexture* render_msgs) {m_render_msgs = render_msgs;};
    void setWindow(sf::RenderWindow* window) {m_window = window;};
    void setDefaultIp(std::string ip) {m_default_ip = ip;};
    void setMyPort(int port) {m_my_port = port;};

    void addMessage(std::string message, std::string ip);
    std::vector< std::pair<std::string, std::string> > getMessages() {return m_messages;};
    std::pair<std::string, std::string> getLastMessage();

    int update();
    sf::Sprite updateChatters();

    void sendMessage(std::string message);

private:
    sf::UdpSocket m_socket_sender;

    bool m_infos_received;

    std::vector< std::pair<std::string, std::string> > m_messages;
    std::vector< std::pair<std::string, std::string> > m_unread_messages;

    std::map<std::string, Chater> m_chaters;
    std::string m_contacts;
    std::string m_default_ip;

    void processMessages();
    std::vector<std::string> parse(std::string s, int length);

    sf::Font font_slim, font_bold;
    sf::Text text;
    sf::RenderTexture* m_render_msgs;
    sf::RenderTexture m_render_chatters;
    sf::RenderWindow* m_window;

    sf::SoundBuffer m_buff;
    sf::Sound m_new_message;

    bool m_has_host;
    int m_n_msgs;
    int m_my_port;
};

#endif // MESSAGEMANAGER_H_INCLUDED
