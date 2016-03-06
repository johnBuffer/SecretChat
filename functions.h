#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

void setParameters(sf::RenderWindow* window, sf::Texture fond, std::string* i, std::string* p);
//void checkForMessage(std::pair<sf::UdpSocket*, MessageManager*> args);
std::vector<std::string> parse(std::string s, int length);
sf::Texture getWindowTexture(sf::RenderWindow &window);
void toUpperCase(std::string* s);
std::string numberToString(double d);
void gradient(sf::RenderWindow &window, sf::Color color1, sf::Color color2, int l, int h, double x, double y);
void close(sf::RenderWindow *window);
std::vector<std::string> split(std::string s, char c);


#endif // FUNCTIONS_H_INCLUDED
