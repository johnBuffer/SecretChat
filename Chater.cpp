#include "Chater.h"

Chater::Chater()
{
    m_pseudo = "";
}

Chater::Chater(std::string pseudo, std::string ip, int port)
{
    m_pseudo = pseudo;
    m_ip = ip;
    m_port = port;

    this->resetClocks();
    m_afk = false;
    spaming=false;
    writting = false;
    y = 0;
    host = false;

    m_has_pseudo = false;
}

bool Chater::hasQuits()
{
    return m_clock_quits.getElapsedTime().asSeconds()>2;
}

bool Chater::isAfk()
{
    return m_clock_afk.getElapsedTime().asSeconds()>30;
}

void Chater::resetClocks()
{
    m_afk = false;
    m_clock_afk.restart();
    m_clock_quits.restart();
}
