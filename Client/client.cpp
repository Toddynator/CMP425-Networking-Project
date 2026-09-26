/*	CMP425 / CMP501
    Lab 1 TCP client example - by Andrei Boiko

    A simple client that connects to a server and waits for
    a response. The server sends "hello" when the client first
    connects. Text typed is then sent to the server which echos
    it back, and the response is printed out.
*/

//#include <entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "utils.h";

constexpr auto WELCOME_MSG = "Hello client!";
constexpr auto MESSAGESIZE = 40;
auto serverPort = 53000;
//auto ip = sf::IpAddress::resolve("127.0.0.1"); // more readable option
sf::IpAddress serverIp(127, 0, 0, 1); // safer option as it avoids string parsing

int main()
{
    // Initialise the window and draw a simple shape to test that it works.
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Server");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(sf::Vector2f(300,200));

    Utils::printMsg("Client startup. Connecting to server...");

    // Create a TCP socket that we'll connect to the server.
    sf::TcpSocket socket;

    bool client_connected = false;
    sf::Time timeout = sf::seconds(2.0f);

    // Connect the socket to the server.
    while (!client_connected)
    {
        sf::Socket::Status status = socket.connect(serverIp, serverPort, timeout);
        if (status != sf::Socket::Status::Done)
        {
            Utils::printMsg("Error connecting to server!", error);

            // FIXME [DONE]: currently, the application will continue even if it fails to connect.
            // Handle this more gracefully.

            std::cout << "Press a key to reattempt connection\n";
            system("pause");
        }
        else {
            client_connected = true;
        }
    }
    
    
    // We'll use this array to hold the messages we exchange with the server.
    char buffer[MESSAGESIZE];
    std::size_t message_size;

    // We expect the server to send us a welcome message (WELCOME_MSG) when we connect.

    // Receive the message.
    if (socket.receive(buffer, sizeof(buffer), message_size) != sf::Socket::Status::Done)
    {
        // FIXME [NEEDS TESTED]: check for errors, and/or for unexpected message size.
        Utils::printMsg("Error receiving packets!", error);
    }
    if (message_size > (size_t)sizeof(buffer))
    {
        Utils::printMsg("Message is greater than the size of the buffer!");
    }

    Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);
    // Check it's what we expected.
    if (memcmp(buffer, WELCOME_MSG, strlen(WELCOME_MSG)) != 0)
    {
        Utils::printMsg("Expected \"" + std::string(WELCOME_MSG).substr(0, MESSAGESIZE) + "\" upon connection, but got something else!", error);
    }

    // Main loop will run while the window is open.
    while (window.isOpen())
    {
        // Check for window events.
        while (const std::optional event = window.pollEvent())
        {
            // If "close" button is pressed, close the window.
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Clear frame.
        window.clear();

        // Draw new frame.
        window.draw(shape);

        // Display Frame.
        window.display();

        // -----------------------------------------------------------------------------
        // Normally you want to draw frames after doing the input/logic/networking, 
        // but we do it first so that we could check that the window works as expected.
        // -----------------------------------------------------------------------------

        Utils::printMsg("Please enter a message: ");
        // Read a line of text from the user.
        std::string line;
        std::getline(std::cin, line);
        // Now "line" contains what the user typed (without the trailing \n).
        
        // Copy the line into the buffer, filling the rest with underscores.
        memset(buffer, '~', MESSAGESIZE);
        memcpy(buffer, line.c_str(), line.size());

        // FIXME [NEEDS TESTED]: if line.size() is bigger than the buffer it'll overflow (and likely corrupt memory)
        if (line.size() > (size_t)sizeof(buffer)) {
            Utils::printMsg("line is bigger than the buffer! Resizing");
            line.resize(sizeof(buffer));
        }


        Utils::printMsg("Sending message to server: ");
        Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);
        // Send the message to the server.
        if (socket.send(buffer, MESSAGESIZE) != sf::Socket::Status::Done)
        {
            Utils::printMsg("Error sending message!", error);
            // FIXME [DONE]: check for errors from send
        }

        // Read a response back from the server.
        if (socket.receive(buffer, sizeof(buffer), message_size) != sf::Socket::Status::Done)
        {
            Utils::printMsg("Error receiving message!", error);
            // FIXME [DONE]: check for errors from recieve
        }

        // Print the message we recieved.
        Utils::printMsg("Echo recieved: ", success);
        Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);

    }

    socket.disconnect();
}
