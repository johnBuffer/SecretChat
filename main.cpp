#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>

#include "MessageManager.h"
#include "functions.h"
#include "Chater.h"

void checkForMessage(std::pair<sf::UdpSocket*, MessageManager*> args)
{
    std::string ip;

    char data[1000];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port_sender;
    std::string pseudo_sender;

    while (true)
    {
        args.first->receive(data, 1000, received, sender, port_sender);
        args.second->addMessage(data, sender.toString()+"_"+numberToString(port_sender));
    }
}

int main()
{
    MessageManager messageManager;
    messageManager.init();
    sf::Clock clock;
    sf::Clock clock_state;
    sf::Clock clock_info;

    sf::ContextSettings settings; settings.antialiasingLevel = 2;

    std::map<std::string, Chater> chaters;

    std::string ip, pseudo, message;
    int port(8080), my_port;
    sf::UdpSocket socket_sender;
    sf::UdpSocket socket_recever;
    if (socket_sender.bind(sf::Socket::AnyPort) != sf::Socket::Done) std::cout << "Error..." << std::endl;
    if (socket_recever.bind(8080) != sf::Socket::Done)
    {
        socket_recever.bind(sf::Socket::AnyPort);
    }

    my_port = socket_recever.getLocalPort();
    messageManager.setMyPort(my_port);

    std::pair<sf::UdpSocket*, MessageManager*> args; args.first = &socket_recever; args.second = &messageManager;
    sf::Thread thread(&checkForMessage, args);
    thread.launch();

    sf::Font font;
    font.loadFromFile("font_slim.otf");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(14);
    text.setColor(sf::Color::Black);

    sf::RenderWindow window(sf::VideoMode(680, 400), "RSAChat", sf::Style::None, settings);
    double x_win = window.getSize().x; double y_win = window.getSize().y;
    window.setVerticalSyncEnabled(true);
    setParameters(&window, getWindowTexture(window), &ip, &pseudo);
    messageManager.setDefaultIp(ip);
    messageManager.setWindow(&window);

    sf::RenderTexture render_msgs;
    render_msgs.create(window.getSize().x, 8192, false);
    render_msgs.clear(sf::Color(21, 22, 26));
    messageManager.setRenderTexture(&render_msgs);

    sf::RectangleShape scrowller;
    scrowller.setFillColor(sf::Color(110, 110, 112));

    int h_title = 30;
    int cote = 14; double red_quit = 200;
    sf::RectangleShape quit(sf::Vector2f(h_title-cote, h_title-cote));
    quit.setPosition(window.getSize().x-(h_title-cote)-cote/2, cote/2);

    double h_text_box = 20;
    double y_reader = 0;
    double reader_height = y_win-h_text_box-h_title;
    int n_msgs = 0;

    clock_info.restart();

    sf::Vector2i lastPos(0, 0);
    sf::Vector2i pos_win = window.getPosition();

    int h_line = 20;
    int n_line_message=1;
    int h_char = 20;
    double red_color_text_box = 0;
    bool scrowlling(false);
    bool clic(false);
    bool moving_window(false);
    bool writting(true);
    bool want_close(false);
    bool inWindow(true);
    double delta_scrowller=0;
    double x_text_close = x_win;
    bool run = true;
    bool show_cursor(true);
    int delta_bottom = 15;

    while (run)
    {
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Return)
                {
                    if (message.size() > 0)
                    {
                        message = pseudo+" "+message;
                        //socket_sender.send(message.c_str(), message.length()+1, ip, port);
                        messageManager.sendMessage(message);
                        message = "";
                        scrowlling=false;
                    }
                    else
                        red_color_text_box = 150;

                    writting = false;
                }
                else if (event.key.code == sf::Keyboard::BackSpace)
                {
                    if (message.length() > 0) message = message.substr(0, message.length()-1);
                    writting = false;
                }
            }
            else if (event.type == sf::Event::KeyReleased) writting = true;
            else if (event.type == sf::Event::TextEntered && writting)
            {
                if (event.text.unicode < 256) message += event.text.unicode;
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                double x = localPosition.x;
                double y = localPosition.y;
                scrowlling = scrowller.getGlobalBounds().contains(x, y);
                delta_scrowller = (scrowller.getPosition().y+scrowller.getSize().y-y-h_title+17);
                clic = true;

                want_close = quit.getGlobalBounds().contains(localPosition.x, localPosition.y);
                if (y < h_title && !want_close)
                {
                    moving_window=true;
                    lastPos = sf::Mouse::getPosition();
                    pos_win = window.getPosition();
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                clic = false;
                moving_window = false;

                if (want_close && quit.getGlobalBounds().contains(localPosition.x, localPosition.y))
                {
                    thread.terminate();
                    run = false;
                }
            }
            else if (event.type == sf::Event::MouseWheelMoved)
            {
                scrowlling = true;
                if (inWindow) y_reader -= event.mouseWheel.delta*20;
            }
            else if (event.type == sf::Event::MouseLeft)
            {
                clic = false;
                inWindow = false;
            }
            else if (event.type == sf::Event::MouseEntered)inWindow = true;
            else if (event.type == sf::Event::LostFocus) show_cursor = false;
            else if (event.type == sf::Event::GainedFocus) show_cursor = true;
        }

        window.clear();

        if (moving_window)
        {
            int x = sf::Mouse::getPosition().x;
            int y = sf::Mouse::getPosition().y;

            window.setPosition(sf::Vector2i(pos_win.x+x-lastPos.x, pos_win.y+y-lastPos.y));
        }

        if (scrowlling && clic)
        {
            double y = localPosition.y;
            y_reader = (y+delta_scrowller)/reader_height*n_msgs*h_char;
        }

        text.setString("Exit ?");
        if (quit.getGlobalBounds().contains(localPosition.x, localPosition.y))
        {
            x_text_close += (x_win-text.getGlobalBounds().width-5-22-x_text_close)/4;
            if (red_quit < 245) red_quit+=11;
        }
        else x_text_close += (x_win-x_text_close)/4;

        if (clock_info.getElapsedTime().asMilliseconds() >= 500)
        {
            std::string rapport = pseudo+"_"+numberToString(my_port)+"_"+"rapport_presence_";
            if (message.length()>0) rapport += "_w";
            messageManager.sendMessage(rapport);
            //socket_sender.send(rapport.c_str(), rapport.length()+1, ip, 8080);
            //rapport = "_rapport_presence_|me|"+numberToString(my_port);
            //socket_sender.send(rapport.c_str(), rapport.length()+1, "127.0.0.1", my_port);
            clock_info.restart();
        }

        n_msgs = messageManager.update();
        if (y_reader < n_msgs*(h_char)+delta_bottom && !scrowlling) y_reader += (n_msgs*(h_char)+delta_bottom-y_reader)/4;
        if (y_reader > n_msgs*(h_char))
        {
            y_reader += (n_msgs*(h_char)-y_reader)/4;
            if (!clic) scrowlling = false;
        }
        if (y_reader - reader_height < 0 && n_msgs > 0) y_reader += (-y_reader + reader_height)/4;

        sf::Sprite reader(render_msgs.getTexture());
        reader.setPosition(0, h_title);
        reader.setTextureRect(sf::IntRect(0, y_reader-reader_height, window.getSize().x, reader_height));
        window.draw(reader);

        scrowller.setSize(sf::Vector2f(10, reader_height/(n_msgs*h_char+delta_bottom)*reader_height-10));
        scrowller.setPosition(window.getSize().x-12-2, (y_reader-reader_height)/(n_msgs*h_char+delta_bottom)*reader_height+h_title+5);
        sf::CircleShape topScrowller(5);
        topScrowller.setPosition(window.getSize().x-12-2, (y_reader-reader_height)/(n_msgs*h_char+delta_bottom)*reader_height+h_title);
        topScrowller.setFillColor(sf::Color(110, 110, 110));
        window.draw(topScrowller);
        topScrowller.move(0, scrowller.getSize().y);
        window.draw(topScrowller);
        window.draw(scrowller);

        text.setCharacterSize(14);
        std::vector<std::string> parsed_message = parse(message, 40);
        n_line_message = parsed_message.size();
        if (n_line_message==0) n_line_message=1;
        if (h_text_box < h_line*n_line_message) h_text_box+=(n_line_message*h_line-h_text_box)/4;
        else if (h_text_box > h_line*n_line_message) h_text_box+=(n_line_message*h_line-h_text_box)/4;

        sf::RectangleShape textBox(sf::Vector2f(x_win, h_text_box));
        textBox.setFillColor(sf::Color(20+red_color_text_box, 20, 20));
        textBox.setPosition(0, y_win-h_text_box);
        window.draw(textBox);

        if (red_color_text_box > 0) red_color_text_box-=red_color_text_box/10;

        sf::Color color(0, 0, 0, 150);
        gradient(window, color, sf::Color(0, 0, 0, 0), x_win, -40, 0, y_win-h_text_box);

        sf::Sprite chatters = messageManager.updateChatters();
        chatters.setPosition(x_win-150-12-5, h_title);
        window.draw(chatters);

        text.setColor(sf::Color::White);
        text.setString("message  :");
        text.setPosition(15, y_win-h_text_box+(h_text_box-12)/2-5);
        window.draw(text);

        if (n_line_message > 2)
        {
            text.setCharacterSize(12);
            text.setColor(sf::Color(100, 100, 100));
            text.setString(numberToString(message.length()) + " characters");
            text.setPosition((100-text.getGlobalBounds().width)/2, y_win-h_text_box+(h_text_box-12)/2+12);
            window.draw(text);
            text.setCharacterSize(14);
        }

        text.setColor(sf::Color::White);
        for (int i(0); i<n_line_message; ++i)
        {
            text.setString(parsed_message[i]);
            text.setPosition(100, y_win-h_text_box+(h_line-12)/2+(h_line)*i-5);
            window.draw(text);
        }

        sf::RectangleShape cursor(sf::Vector2f(1, h_line*0.8));
        cursor.setFillColor(sf::Color::White);
        cursor.setPosition(100+text.getGlobalBounds().width, y_win-h_line+(h_line-h_line*0.8)/2);

        //title bar
        sf::RectangleShape title(sf::Vector2f(window.getSize().x, h_title));
        title.setFillColor(sf::Color(10, 10, 10));
        window.draw(title);

        color=sf::Color(110, 110, 112, 100); gradient(window, color, sf::Color(0, 0, 0, 0), x_win, -2, 0, h_title);

        text.setColor(sf::Color(255, 150, 0));
        text.setPosition(5, 5);
        std::string p = pseudo;
        text.setCharacterSize(16);
        text.setString("RSAChat - " + p);
        window.draw(text);

        text.setString("Exit ?");
        text.setColor(sf::Color(255, 150, 0));
        text.setPosition(x_text_close, 5);
        window.draw(text);

        sf::RectangleShape hidder(sf::Vector2f(h_title, h_title-7));
        hidder.setPosition(quit.getPosition().x, 0);
        hidder.setFillColor(sf::Color(10, 10, 10));
        window.draw(hidder);

        if (red_quit > 150) red_quit-=2;
        quit.setFillColor(sf::Color(255, red_quit, 0));
        window.draw(quit);

        if (clock.getElapsedTime().asMilliseconds() <= 500 && show_cursor) window.draw(cursor);
        if (clock.getElapsedTime().asMilliseconds() >= 1000) clock.restart();

        window.display();
    }

    close(&window);

    return 0;
}
