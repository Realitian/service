/**************************************************************
 * Project          Augmented Reality Visit Assistance
 * (c) Copyright    2017
 * Company          Infinte Reality Solution LLC
 *                  All rights reserved
 **************************************************************/

#ifndef SENSOR_PROC_H
#define SENSOR_PROC_H

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <functional>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket);

  void start();

private:
  tcp::socket socket_;
};

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      socket_(io_service)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket_))->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

#endif // SENSOR_PROC_H
