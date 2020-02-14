#include <thread>
#include "TestClient.h"

TestClient::TestClient()
{
    client.ConnectToIP("127.0.0.1", 24000);
}

int TestClient::Update()
{
    auto events = client.GetNetworkEvents();
    while(!events.empty())
    {
        switch (events.front().eventType)
        {
            case netlib::NetworkEvent::EventType::ONCONNECT:
            {
                std::thread tr(&TestClient::GetInput, this);
                tr.detach();
                break;
            }
            case netlib::NetworkEvent::EventType::MESSAGE:
            {
                auto test = events.front();
                std::cout << events.front().data.data() << std::endl;
                break;
            }
            case netlib::NetworkEvent::EventType::ONDISCONNECT:
            {
                std::cout << "Remotely disconnected from server." << std::endl;
                break;
            }
            case netlib::NetworkEvent::EventType::ONLOBBYJOIN:
            {
                std::cout << "Joined new Lobby: " << client.GetCurrentLobbyInfo().name << std::endl;
                break;
            }

            default:
            {
                break;
            }
        }
        events.pop();
    }
    return 0;

}

void TestClient::GetInput()
{

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    inputRunning = true;
    std::string input = "";
    std::getline(std::cin, input);
    while(inputRunning)
    {
        std::cout << "";
        std::getline(std::cin, input);
        if(input == "#getlobbies")
        {
            std::cout << "Current open lobbies:" << std::endl;
            auto lobbies = client.GetAllLobbyInfo();
            for(auto& lobby : lobbies)
            {
                std::cout << "Name: " << lobby.name << " ID: " << lobby.lobbyID
                          << " (" << lobby.clientsInRoom << "/" << lobby.maxClientsInRoom << ")" <<  std::endl;
                std::cout << "Clients: " << std::endl;
                for(auto& member : lobby.memberInfo)
                {
                    std::cout << "\t Name: " << member.name << " ID: " << member.uid << std::endl;
                }
            }
            std::cout << std::endl;
        }
        else if(input == "#cl")
        {
            std::cout << "Enter New Lobby Name: ";
            std::getline(std::cin, input);
            client.CreateLobby(input);
        }
        else if(input == "#joinlobby")
        {
            std::cout << "Enter New Lobby Number to Join: ";
            std::getline(std::cin, input);
            client.JoinLobby(std::atoi(input.data()));
        }
        else
        {
            input = std::to_string(client.GetUID()) + ": " + input;
            client.SendMessageToServer(input.c_str(), input.size() + 1);
        }
    }

}