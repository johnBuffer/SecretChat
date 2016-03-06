#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "MessageManager.h"

void toUpperCase(std::string* s)
{
    std::string lower = "abcdefghijklmnopqrstuvwxyz";
    std::string upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::string s_out = *s; int n=s_out.length();

    for (int i(0); i<n; ++i)
    {
        if (lower.find(s_out[i]) != std::string::npos) s_out[i] = upper[lower.find(s_out[i])];
    }

    *s = s_out;
}

void setParameters(sf::RenderWindow* window, sf::Texture fond, std::string* i, std::string* p)
{
    sf::RenderTexture txtr;
    txtr.create(window->getSize().x, window->getSize().y, false);

    sf::Clock clock;
    clock.restart();

    sf::Font font;
    font.loadFromFile("font_slim.otf");

    sf::Text text, notice;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setColor(sf::Color::Black);

    notice.setFont(font);
    notice.setCharacterSize(16);
    notice.setColor(sf::Color::Black);

    sf::Text text_pseudo = text;

    std::string ip, pseudo;
    //ip = "86.66.215.224";
    //ip = "85.68.177.193";
    //ip = "127.0.0.1";
    pseudo = "";

    bool running(true);
    bool writting(true);
    bool erasing(false);

    int n_char = 0;
    int length_number[4] = {0, 0, 0, 0};
    short n_points = 0;

    int id_box = 0;

    while (running)
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Return)
                {
                    id_box++;
                    writting=false;
                }
                else if (event.key.code == sf::Keyboard::BackSpace)
                {
                    if (id_box == 0)
                    {
                        if (ip.length() > 0)
                        {
                            if (ip[ip.length()-1] != '.') n_char--;
                            else
                            {
                                n_points--;
                                n_char = length_number[n_points];
                            }
                            ip = ip.substr(0, ip.length()-1);
                        }
                    }
                    else if (id_box == 1) pseudo = pseudo.substr(0, pseudo.length()-1);

                    writting = false;
                    erasing = true;
                }
            }
            else if (event.type == sf::Event::KeyReleased) writting = true;
            else if (event.type == sf::Event::TextEntered && writting)
            {
                if (id_box == 0)
                {
                    if (event.text.unicode != 46 && n_char<3)
                    {
                        ip += event.text.unicode;
                        n_char++;
                    }
                    else if (event.text.unicode == 46 && n_points<3 && ip[ip.length()-1] != '.')
                    {
                        length_number[n_points] = n_char;
                        n_points++;
                        n_char = 0;
                        ip += event.text.unicode;
                    }
                }
                else if (id_box == 1)
                {
                    if (event.text.unicode != 20 && pseudo.length()<10) pseudo += event.text.unicode;
                }

                erasing = false;
            }
        }

        bool favorable = n_char > 0 && ip[ip.length()-1] != '.' && !erasing && n_points<3;
        if (favorable && (n_char%3 == 0 || ip[ip.length()-1] == '0' && n_char%3 == 1))
        {
            ip+=".";
            length_number[n_points] = n_char;
            n_points++;
            n_char = 0;
        }

        if (id_box > 0 && ip == "") ip = "you're the Boss";

        running = id_box<2;

        window->clear(sf::Color(240, 240, 240));
        txtr.draw(sf::Sprite(fond));
        txtr.display();
        window->draw(sf::Sprite(txtr.getTexture()));

        if (id_box == 0) notice.setString("Tip the IP"); else notice.setString("Enter your pseudo");
        notice.setPosition((window->getSize().x-notice.getGlobalBounds().width)/2, 100);
        window->draw(notice);

        int h_text_box = 30;

        sf::RectangleShape textBox(sf::Vector2f(800, h_text_box));
        if (id_box == 0) textBox.setFillColor(sf::Color(230, 230, 230));
        else textBox.setFillColor(sf::Color(200, 200, 200));
        textBox.setPosition(0, (window->getSize().y-h_text_box)/2);
        window->draw(textBox);

        if (id_box == 1) textBox.setFillColor(sf::Color(230, 230, 230));
        else textBox.setFillColor(sf::Color(200, 200, 200));
        textBox.setPosition(0, (window->getSize().y-h_text_box)/2+h_text_box+10);
        window->draw(textBox);

        text.setString(ip);
        text.setColor(sf::Color::Black);
        text.setPosition((window->getSize().x-text.getGlobalBounds().width)/2, (window->getSize().y-h_text_box+(h_text_box-text.getGlobalBounds().height)/2)/2);
        window->draw(text);

        text_pseudo.setString(pseudo);
        text_pseudo.setColor(sf::Color::Black);
        text_pseudo.setPosition((window->getSize().x-text_pseudo.getGlobalBounds().width)/2, (window->getSize().y-h_text_box+(h_text_box-text_pseudo.getGlobalBounds().height)/2)/2+h_text_box+10);
        window->draw(text_pseudo);

        sf::RectangleShape cursor(sf::Vector2f(1, h_text_box*0.8));
        cursor.setFillColor(sf::Color::Black);
        if (id_box==0) cursor.setPosition(text.getGlobalBounds().width+text.getPosition().x, (window->getSize().y-h_text_box+(h_text_box-h_text_box*0.8)/2)/2);
        else if (id_box==1) cursor.setPosition(text_pseudo.getGlobalBounds().width+text_pseudo.getPosition().x, (window->getSize().y-h_text_box+(h_text_box-h_text_box*0.8)/2)/2+h_text_box+10);

        if (clock.getElapsedTime().asMilliseconds() <= 500) window->draw(cursor);
        if (clock.getElapsedTime().asMilliseconds() >= 1000) clock.restart();

        window->display();
    }

    *i = ip;
    *p = pseudo;
}

std::vector<std::string> parse(std::string s, int length)
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

sf::Texture getWindowTexture(sf::RenderWindow &window)
{
    sf::Image fond = window.capture();
    sf::Texture texture;
    texture.loadFromImage(fond);

    return texture;
}

std::string getMyIp()
{
    std::string ip;
    return ip;
}

std::string numberToString(double d)
{
    std::string s;
    std::ostringstream sstr;

    sstr << d;
    s = sstr.str();

    return s;
}

void gradient(sf::RenderWindow &window, sf::Color color1, sf::Color color2, int l, int h, double x, double y)
{
    sf::VertexArray v_a(sf::Quads, 4);
    v_a[0].position = sf::Vector2f(x, y); v_a[0].color = color1;
    v_a[1].position = sf::Vector2f(x+l, y); v_a[1].color = color1;
    v_a[2].position = sf::Vector2f(x+l, y+h); v_a[2].color = color2;
    v_a[3].position = sf::Vector2f(x, y+h); v_a[3].color = color2;
    window.draw(v_a);
}

void close(sf::RenderWindow *window)
{
    double y = window->getSize().y;
    while (y > 1)
    {
        y -= y/4;
        window->setSize(sf::Vector2u(window->getSize().x, y));
        window->display();
    }

    window->close();
}

std::vector<std::string> split(std::string s, char c)
{
    std::vector<std::string> splitted;

    while (s.find(c) != std::string::npos)
    {
        splitted.push_back(s.substr(0, s.find(c)));
        s=s.substr(s.find(c)+1);
    }

    splitted.push_back(s);

    return splitted;
}

