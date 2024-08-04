#include <boost/beast.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
using namespace std::chrono_literals;

//------------------------------------------------------------------------------

class EchoServer
{
private:
  // This function produces an HTTP response for the given
  // request. The type of the response object depends on the
  // contents of the request, so the interface requires the
  // caller to pass a generic lambda for receiving the response.
  template <
    class Body,
    class Allocator,
    class Send>
  void handle_request(http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send)
  {
    // Returns a bad request response
    auto const bad_request =
      [&req](beast::string_view why) {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
      };

    // Request path must be absolute and not contain "..".
    if (req.target().empty() ||
      req.target()[0] != '/' ||
      req.target().find("..") != beast::string_view::npos)
      return send(bad_request("Illegal request-target"));

    // Cache the size since we need it after the move
    auto const size = req.body().size();

    // Respond to GET request
    if (size == 0)
    {
      http::response<http::empty_body> res;
      res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
      res.content_length(0);
      res.keep_alive(req.keep_alive());
      return send(std::move(res));
    }
    else
    {
      http::string_body::value_type body = req.body();
      http::response<http::string_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())};
      res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set(http::field::content_type, req[http::field::content_type]);
      res.content_length(size);
      res.keep_alive(req.keep_alive());
      return send(std::move(res));
    }
  }

  // Report a failure
  void fail(beast::error_code ec, char const *what)
  {
    std::cout << what << ": " << ec.message() << std::endl;
  }

  // This is the C++11 equivalent of a generic lambda.
  // The function object is used to send an HTTP message.
  template <class Stream>
  struct send_lambda
  {
    Stream &stream_;
    bool &close_;
    beast::error_code &ec_;

    explicit send_lambda(
      Stream &stream,
      bool &close,
      beast::error_code &ec)
      : stream_(stream), close_(close), ec_(ec)
    {
    }

    template <
      bool isRequest,
      class Body,
      class Fields
    >
    void operator()(http::message<isRequest, Body, Fields> &&msg) const
    {
      // Determine if we should close the connection after
      close_ = msg.need_eof();

      // We need the serializer here because the serializer requires
      // a non-const file_body, and the message oriented version of
      // http::write only works with const messages.
      http::serializer<isRequest, Body, Fields> sr{msg};
      http::write(stream_, sr, ec_);
    }
  };

  // Handles an HTTP server connection
  void do_session(tcp::socket socket)
  {
    bool close = false;
    beast::error_code ec;

    // This buffer is required to persist across reads
    beast::flat_buffer buffer;

    // This lambda is used to send messages
    send_lambda<tcp::socket> lambda{socket, close, ec};

    for (;;)
    {
      // Read a request
      http::request<http::string_body> req;
      http::read(socket, buffer, req, ec);
      if (ec == http::error::end_of_stream)
        break;
      if (ec)
        return fail(ec, "read");

      // Send the response
      handle_request(std::move(req), lambda);
      if (ec)
        return fail(ec, "write");
      if (close)
        break;
    }

    // Send a TCP shutdown
    socket.shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
  }

  net::io_context ioc;
  std::unique_ptr<std::thread> thread;

public:
  EchoServer() : ioc{1}, thread{nullptr} {}

  void start()
  {
    thread = std::make_unique<std::thread>(
      [this]{
        // The io_context is required for all I/O
        const auto address = net::ip::make_address("0.0.0.0");
        const auto port = 8281;
        tcp::endpoint ep{address, port};
        // The acceptor receives incoming connections
        tcp::acceptor acceptor{ioc, ep};

        // This will receive the new connection
        tcp::socket socket{ioc};

        // Block until we get a connection
        acceptor.accept(socket);

        // Launch the session, transferring ownership of the socket
        do_session(std::move(socket));
      }
    );

    std::this_thread::sleep_for(100ms);
  }

  void stop()
  {
    thread->join();
    thread.release();
  }
};
