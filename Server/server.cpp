/*	CMP425 / CMP501
    Lab 1 TCP server example - by Andrei Boiko

    A simple TCP server that waits for a connection.
    When a connection is made, the server sends "hello" to the client.
    It then repeats back anything it receives from the client.
    All the calls are blocking -- so this program only handles
    one connection at a time.
*/

//#include <entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "utils.h";

constexpr auto WELCOME_MSG = "Hello client!";
constexpr auto MESSAGESIZE = 40;
auto serverPort = 53000;

int main()
{
    // Initialise the window and draw a simple shape to test that it works.
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Server");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(sf::Vector2f(300,200));

    Utils::printMsg("Echo server startup...");

    // Create a TCP socket that we'll uise to listen for incoming connections.
    sf::TcpListener listenerSocket;

    // Make the socket listen for connections.
    if (listenerSocket.listen(serverPort) == sf::Socket::Status::Done)
    {
        Utils::printMsg("Listening on port " + std::to_string(serverPort));
    }
    else
    {
        Utils::printMsg("Error binding listener socket!", error);
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

        // Create new socket object for commnication with the client.
        sf::TcpSocket clientSocket;

        // Accept a new connection to the server socket.
        // This will update the clientSocket with new connection.
        bool connection_made = false;
        while (!connection_made)
        {
            sf::Socket::Status status = listenerSocket.accept(clientSocket);
            if (status == sf::Socket::Status::Done)
            {
                std::string message = "Connection accepted from ";
                sf::IpAddress incoming_ip = *clientSocket.getRemoteAddress();
                auto port = clientSocket.getRemotePort();

                Utils::printMsg(message + incoming_ip.toString() + ":" + std::to_string(port), success);
                // We don't need the socket after we accepted 1 client. We can only handle 1 for now.
                listenerSocket.close();

                connection_made = true;
            }
            else
            {
                Utils::printMsg("Error accepting connection!", error);             
                             
                //window.close();
                // FIXME [NEEDS TESTED]: in a real server, we wouldn't want the server to crash if
                // it failed to accept a connection -- recover more effectively!

                Utils::printMsg("Reattempting connection after timeout.");
                sf::Clock timer;
                while (timer.getElapsedTime().asSeconds() < 5.0f) {
                    
                }
            }
        }

        // We'll use this array to hold the messages we exchange with the client.
        char buffer[MESSAGESIZE];

        // Fill the buffer with ~ characters to start with.
        memset(buffer, '~', MESSAGESIZE);
        // Replace part of the data with our welcome message.
        memcpy(buffer, WELCOME_MSG, strlen(WELCOME_MSG));

        // Send the welcome message to the client.
        Utils::printMsg("Sending welcome message...");
        Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);
        if (clientSocket.send(buffer, MESSAGESIZE) != sf::Socket::Status::Done)
        {
            // FIXME[NEEDS TESTED]: check for errors from send
            Utils::printMsg("Error sending welcome message!", error);
        }

        while (true)
        {
            std::size_t message_size;
            // Receive as much data from the client as will fit in the buffer.
            sf::Socket::Status socket_status = clientSocket.receive(buffer, sizeof(buffer), message_size);
            if (socket_status != sf::Socket::Status::Done)
            {
                // FIXME [NEEDS TESTED]: check for socket errors from recieve
                    // check for closed connection
                    // check for strange-sized message
                Utils::printMsg("Error receiving message!", error);
                if (socket_status == sf::Socket::Status::Disconnected) {
                    Utils::printMsg("Connection was disconnected!");
                }
                else if (message_size > (size_t)sizeof(buffer))
                {
                    Utils::printMsg("Message is greater than the size of the buffer!");
                }

            }
 
            // Print the message we recieved.
            Utils::printMsg("Message recieved: ", success);
            Utils::printMsg(std::string(buffer).substr(0, MESSAGESIZE), debug);

            // LAB_TASK: Reverse Message
            std::reverse(buffer, buffer + MESSAGESIZE);

            // Send the same data back to the client.
            Utils::printMsg("Sending echo...");
            if (clientSocket.send(buffer, MESSAGESIZE) != sf::Socket::Status::Done)
            {
                // FIXME[DONE]: check for errors from send
                Utils::printMsg("Error sending message!", error);
            }
        }
        // Cleanup the socket if not needed.
        clientSocket.disconnect();
    }
}
