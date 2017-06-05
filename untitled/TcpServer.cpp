#include <chrono>
#include <string>
#include <iostream>
#include <cstdlib>

#include "TcpServer.h"

//===================================================

TcpServer::TcpServer(int port) : port(port)
{
}
//===================================================

TcpServer::~TcpServer()
{
}


//===================================================

void TcpServer::sendData(const void *data, int len)
{

    if (len > BUFFER_SIZE)
        throw std::runtime_error("TcpServer: Buffer overflow !");

    // Copy the data to the buffer safely under mutex
    dataMutex.lock();
    memcpy(buffer, data, len);
    this->len = len;
    ++marker;
    if (marker > 100)
        marker = 1;

    dataMutex.unlock();
}
//===================================================

void TcpServer::serverThread()
{
    using namespace std;
    using boost::asio::ip::tcp;

    tcp::acceptor a(ios, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        tcp::socket sock(ios);
        a.accept(sock);

        // The session thread for each session
        thread([this]( tcp::socket sock){
            cout << "New connection !" << endl;

            int prevMarker = -1; // Previous marker to compare to

            for (;;){
                boost::system::error_code err;

                // Send message only if the marker has changed
                if (marker && (marker != prevMarker) ) {
//                    cout << marker << endl;
                    dataMutex.lock();
                    boost::asio::write(sock, boost::asio::buffer(buffer, len), err);
                    dataMutex.unlock();

                    if (err) {
//                        cerr << "TCP error ! Disconnecting !" << endl;
                        break;
                    }
                }

                prevMarker = marker;

                // Yield a bit of CPU power
                this_thread::sleep_for(chrono::milliseconds(10));
            }
            cout << "Connection ended !" << endl;

        }, move(sock) ).detach();
    }
}
