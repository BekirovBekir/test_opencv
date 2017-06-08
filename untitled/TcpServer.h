#pragma once
// By Oleksiy Grechnyev

#include <thread>
#include <mutex>

#include <boost/asio.hpp>

/// The TCP server module which sends data to all connected clients
class TcpServer
{
public: //==== Parameters ======
    /// Max buffer size in bytes
    static constexpr int BUFFER_SIZE = 1014*128;
public: //==== Constructor, destructor =====
    /// Constructor with port number
    TcpServer(int port);

    /// Destructor
    ~TcpServer();

    TcpServer(const TcpServer &) = delete;

    TcpServer & operator= (const TcpServer &) = delete;

public: //==== Methods =======
    /// Send data to all presently connected TCP clients
    void sendData(const void * data, int len);

private: //==== Methods =======
    /// The master server thread
    void serverThread();

private: //===== Data =====
    /// Boost Asio IO service
    boost::asio::io_service ios;

    /// The tcp port to use
    int port;

    /// The thread object corresponding to serverThread()
    std::thread thr = std::thread(&TcpServer::serverThread, this);

    /// Mutex protecting buffer and len

    std::mutex dataMutex;

    /// The buffer
    const void *buffer[BUFFER_SIZE];

    /// Length of the data to send
    int len = -1;

    /// From 1 to 100, incremented by sendData, or 0 if no data, 666 to shut down
    int marker = 0;
};

