#include "MessageManager.h"

MessageManager::MessageManager()
{

}

void MessageManager::init()
{
    if (!font_slim.loadFromFile("font_slim.otf")) std::cout << "Cannot open font" << std::endl;
    if (!font_bold.loadFromFile("font_bold.otf")) std::cout << "Cannot open font" << std::endl;

    text.setFont(font_slim);
    text.setCharacterSize(14);
    text.setColor(sf::Color::Black);

    m_render_chatters.create(150, 500, false);
    m_render_chatters.clear(sf::Color(15, 16, 19));
    m_render_chatters.setSmooth(true);

    m_n_msgs = 0;

    m_buff.loadFromFile("new_message.wav");
    m_new_message.setBuffer(m_buff);
    m_new_message.setVolume(20);

    if (m_socket_sender.bind(sf::Socket::AnyPort) != sf::Socket::Done) std::cout << "Error..." << std::endl;

    m_has_host = false;
    m_infos_received = false;
}

void MessageManager::addMessage(std::string message, std::string ip)
{
    std::pair<std::string, std::string> msg;
    msg.first = ip;
    msg.second = message;
    m_unread_messages.push_back(msg);
    //this->processMessages();
}

std::pair<std::string, std::string> MessageManager::getLastMessage()
{
    std::pair<std::string, std::string> msg;

    if (m_unread_messages.size() > 0)
    {
        msg.first = m_unread_messages.back().first;
        msg.second = m_unread_messages.back().second;

        m_messages.push_back(m_unread_messages.back());
        m_unread_messages.pop_back();
    }

    return msg;
}

int MessageManager::update()
{
    //std::cout << m_contacts << std::endl << std::endl;
    if (m_unread_messages.size() > 0) this->processMessages();
    std::string to_erase = "";
    for (std::map<std::string,Chater>::iterator it=m_chaters.begin(); it!=m_chaters.end(); ++it)
    {
        if (it->second.hasQuits())
        {
            if (it->second.isComplete()) this->addMessage(it->second.getPseudo() + " quit", "Q");
            to_erase = it->first;
        }
        else if (it->second.isAfk() && !it->second.alreadyAfk() && !it->second.writting)
        {
            if (it->second.getPseudo().compare("me") != 0) this->addMessage(it->second.getPseudo() + " is now AFK", "A");
            it->second.setAfk();
        }
        if (it->second.writting) it->second.resetAfk();
        if (it->second.getTimeLastMessage() > 500) it->second.spaming = false;
    }
    if (to_erase.size() > 0) m_chaters.erase(to_erase);
    return m_n_msgs;
}

void MessageManager::processMessages()
{
    int h_char = 20;

    //m_render_msgs->clear(sf::Color(21, 22, 26));
    std::pair<std::string, std::string> msg = getLastMessage();

    if (msg.second.find(" ") != std::string::npos)
    {
        if (msg.second.size() > 0) // message utilisateur
        {
            if (msg.first.size() > 1)
            {
                std::string pseudo_sender = msg.second.substr(0, msg.second.find(" "));

                if (m_chaters.find(msg.first) != m_chaters.end())
                {
                    m_chaters[msg.first].spaming = (m_chaters[msg.first].getTimeLastMessage() < 500 && m_chaters[msg.first].getTimeLastMessage() > 100);
                    m_chaters[msg.first].resetClocks();
                    m_chaters[msg.first].writting = false;
                }

                sf::Text text_pseudo = text;
                text_pseudo.setFont(font_bold);
                toUpperCase(&pseudo_sender);
                text_pseudo.setString(pseudo_sender);
                text_pseudo.setColor(sf::Color::White);
                text_pseudo.setPosition(10, m_n_msgs*h_char);
                text_pseudo.setCharacterSize(14);
                int decal = text_pseudo.getGlobalBounds().width+5;
                m_render_msgs->draw(text_pseudo);

                if (msg.first.find("127.0.0.1") == std::string::npos) m_new_message.play();

                std::size_t pos = msg.second.find(" ");
                msg.second = msg.second.substr(pos+1);
                std::vector<std::string> toPost = parse(msg.second, 30);
                text.setColor(sf::Color(110, 110, 112));
                for (int i(0); i<toPost.size(); ++i)
                {
                    if (i==0)
                    {
                        time_t rawtime;
                        struct tm * timeinfo;
                        char buffer [80];

                        time (&rawtime);
                        timeinfo = localtime (&rawtime);

                        strftime(buffer,80,"%I:%M %p",timeinfo);
                        text.setCharacterSize(12);
                        text.setString(buffer);
                        text.setPosition(m_window->getSize().x-text.getGlobalBounds().width-20, m_n_msgs*h_char+7);
                        m_render_msgs->draw(text);
                        text.setCharacterSize(14);
                        //toPost[0] = "- "+toPost[0];
                    }

                    text.setString(toPost[i]);
                    text.setPosition(12+decal, m_n_msgs*h_char);
                    m_render_msgs->draw(text);
                    m_n_msgs++;
                    decal = 0;
                }
            }
            else
            {
                if (msg.first.compare("Q") == 0) text.setColor(sf::Color::Red);
                else if (msg.first.compare("J") == 0) text.setColor(sf::Color(0, 72, 255));
                else if (msg.first.compare("A") == 0) text.setColor(sf::Color(255, 150, 0));
                text.setString(msg.second);
                text.setPosition(10, m_n_msgs*h_char);
                m_render_msgs->draw(text);
                m_n_msgs++;
            }
        }
    }
    else if (msg.second.find("_rapport_presence") != std::string::npos) // rapport présence
    {
        if (m_chaters.find(msg.first) != m_chaters.end())
        {
            if (!m_chaters[msg.first].isComplete()) // mise à jour eventuelle
            {
                std::cout << "Mise à jour" << std::endl;
                std::string pseudo_sender = msg.second.substr(0, msg.second.find("_"));
                std::string port = msg.second.substr(msg.second.find("_")+1);
                int port_sender = atoi(port.substr(0, port.find("_")).c_str());
                m_chaters[msg.first] = Chater(pseudo_sender, msg.first.substr(0, msg.first.find("_")), port_sender);
                m_chaters[msg.first].setComplete();
                toUpperCase(&pseudo_sender);
                this->addMessage(pseudo_sender+" joined", "J");
            }
            else
            {
                m_chaters[msg.first].resetQuits();
                if (msg.second.find("w") != std::string::npos) m_chaters[msg.first].writting = true; else m_chaters[msg.first].writting = false;
            }
        }
        else // nouveau contact
        {
            std::cout << "Ajout contact " << msg.first << std::endl;
            std::string pseudo_sender = msg.second.substr(0, msg.second.find("_"));
            std::string port = msg.second.substr(msg.second.find("_")+1);
            int port_sender = atoi(port.substr(0, port.find("_")).c_str());
            m_chaters[msg.first] = Chater(pseudo_sender, msg.first.substr(0, msg.first.find("_")), port_sender);
            m_chaters[msg.first].setComplete();
            toUpperCase(&pseudo_sender);
            this->addMessage(pseudo_sender+" joined", "J");

            m_contacts += m_chaters[msg.first].getIp()+","+numberToString(m_chaters[msg.first].getPort())+"_";
            m_socket_sender.send(m_contacts.c_str(), m_contacts.length()+1, m_chaters[msg.first].getIp(), m_chaters[msg.first].getPort());
            /*if (m_contacts.find(m_chaters[msg.first].getIp()) == std::string::npos && !m_chaters[msg.first].host)*/

        }
    }
    else if (!m_infos_received)// reception liste contacts
    {
        //this->addMessage("Infos contact reçues", "J");
        std::string ips_txt = msg.second;
        std::vector<std::string> ips = split(ips_txt, '_');
        for (int i(0); i<ips.size(); ++i)
        {
            std::string ip=ips[i].substr(0, ips[i].find(","));
            std::string port=ips[i].substr(ips[i].find(",")+1);
            //std::cout << "New : " << ips[i] << std::endl << std::endl;
            std::cout << "Contact recu " << ip+"_"+port << std::endl;
            if (ips[i].size() > 0) m_chaters[ip+"_"+port] = Chater("Connexion en cours...", ip, atoi(port.c_str()));
        }

        m_infos_received = true;
    }

    m_render_msgs->display();
}

std::vector<std::string> MessageManager::parse(std::string s, int length)
{
    std::vector<std::string> parsed;
    while (s.length() > length)
    {
        parsed.push_back(s.substr(0, length));
        s = s.substr(length);
    }
    parsed.push_back(s);

    return parsed;
}

sf::Sprite MessageManager::updateChatters()
{
    m_render_chatters.clear(sf::Color(50, 50, 50, 150));

    double n_chatters = 0;

    std::vector<Chater*> online;
    std::vector<Chater*> afk;

    for (std::map<std::string,Chater>::iterator it=m_chaters.begin(); it!=m_chaters.end(); ++it)
    {
        if (it->second.isComplete())
        {
            if (it->second.alreadyAfk()) afk.push_back(&(it->second));
            else online.push_back(&(it->second));
        }
    }

    for (int i(0); i<online.size(); ++i)
    {
        std::string pseudo = online[i]->getPseudo();
        text.setString(pseudo);
        text.setColor(sf::Color::White);
        sf::Color color;
        color = sf::Color(0, 255, 120);
        sf::CircleShape c(5);
        c.setFillColor(color);

        online[i]->y += (20.0*n_chatters-online[i]->y)/8.0;
        text.setPosition(25, online[i]->y);
        c.setPosition(11, online[i]->y+5);

        m_render_chatters.draw(c);
        m_render_chatters.draw(text);

        if (online[i]->spaming)
        {
            text.setString("[SPAM] ");
            text.setStyle(sf::Text::Bold);
            text.setColor(sf::Color::Red);
            text.setPosition(150-text.getGlobalBounds().width-5, online[i]->y);
            m_render_chatters.draw(text);
            text.setStyle(sf::Text::Regular);
        }
        else if (online[i]->writting)
        {
            text.setString("writing...");
            text.setStyle(sf::Text::Italic);
            text.setColor(color);
            text.setPosition(150-text.getGlobalBounds().width-5, online[i]->y);
            m_render_chatters.draw(text);
            text.setStyle(sf::Text::Regular);
        }

        n_chatters++;
    }

    for (int i(0); i<afk.size(); ++i)
    {
        std::string pseudo = afk[i]->getPseudo();
        text.setString(pseudo);
        text.setColor(sf::Color::White);
        sf::Color color;
        color = sf::Color(255, 150, 0);
        sf::CircleShape c(5);
        c.setFillColor(color);

        afk[i]->y += (20.0*n_chatters-afk[i]->y)/8.0;
        text.setPosition(25, afk[i]->y);
        c.setPosition(11, afk[i]->y+5);

        m_render_chatters.draw(c);
        m_render_chatters.draw(text);

        n_chatters++;
    }

    m_render_chatters.display();

    sf::Sprite sprite(m_render_chatters.getTexture());
    sprite.setTextureRect(sf::IntRect(0, 0, 150, n_chatters*20));

    return sprite;
}

void MessageManager::sendMessage(std::string message)
{
    for (std::map<std::string,Chater>::iterator it=m_chaters.begin(); it!=m_chaters.end(); ++it)
    {
        /*if (it->second.isComplete())*/ m_socket_sender.send(message.c_str(), message.length()+1, it->second.getIp(), it->second.getPort());
    }

    if (m_chaters.size() == 0)
    {
        m_socket_sender.send(message.c_str(), message.length()+1, m_default_ip, 8080);
    }
}



