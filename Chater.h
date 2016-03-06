#ifndef CHATER_H_INCLUDED
#define CHATER_H_INCLUDED

#include <string>
#include <SFML/Graphics.hpp>

class Chater
{
public:
    Chater();
    Chater(std::string pseudo, std::string ip, int port);

    bool hasQuits();
    bool isAfk();
    bool alreadyAfk() const {return m_afk;};

    void resetClocks();

    void resetAfk() {m_clock_afk.restart(); m_afk = false;};
    void resetQuits() {m_clock_quits.restart();};
    void setAfk() {m_afk = true;};

    double getTimeLastMessage() const {return m_clock_afk.getElapsedTime().asMilliseconds();};
    std::string getIp() const {return m_ip;};
    int getPort() const {return m_port;};
    void setComplete() {m_has_pseudo = true;};
    bool isComplete() const {return m_has_pseudo;};

    std::string getPseudo() const {return m_pseudo;};
    double y;
    bool writting;
    bool spaming;
    bool host;

private:
    std::string m_pseudo;
    bool m_afk;

    int m_port;
    std::string m_ip;

    sf::Clock m_clock_quits;
    sf::Clock m_clock_afk;

    bool m_has_pseudo;
};

#endif // CHATER_H_INCLUDED
