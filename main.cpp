#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/async.hpp>
#include <boost/async/promise.hpp>
#include <boost/async/select.hpp>
#include <boost/async/this_thread.hpp>
#include <boost/beast.hpp>

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/system/detail/errc.hpp>
#include <boost/throw_exception.hpp>
#include <chrono>
#include <fmt/core.h>
#include <stdexcept>

namespace async = boost::async;
namespace beast = boost::beast;

using executor_type = async::use_op_t::executor_with_default<async::executor>;
using socket_type = typename boost::asio::ip::tcp::socket::rebind_executor<
    executor_type>::other;
using ssl_socket_type = boost::asio::ssl::stream<socket_type>;
using acceptor_type = typename boost::asio::ip::tcp::acceptor::rebind_executor<
    executor_type>::other;
using websocket_type = beast::websocket::stream<ssl_socket_type>;

async::promise<boost::asio::ip::tcp::resolver::results_type>
resolve(std::string_view host) {
  // Resolve the host
  boost::asio::ip::tcp::resolver resolve{async::this_thread::get_executor()};

  // Resolve timer
  boost::asio::system_timer resolve_timer{async::this_thread::get_executor()};
  resolve_timer.expires_after(std::chrono::seconds{10});
  switch (auto v = co_await async::select(
              resolve_timer.async_wait(async::use_op),
              resolve.async_resolve(host, "https", async::use_op));
          v.index()) {
  case 0:
    break;
  case 1:
    co_return boost::variant2::get<1>(v);
  }

  std::runtime_error e("resolver timed out");
  boost::throw_exception(e);
}

async::promise<ssl_socket_type> connect(std::string_view host,
                                        boost::asio::ssl::context &ctx) {
  auto endpoints = co_await resolve(host);

  // Timer for timeouts
  //boost::asio::system_timer t{async::this_thread::get_executor()};

  ssl_socket_type sock{async::this_thread::get_executor(), ctx};
  //bool connected{false};
  co_await sock.next_layer().async_connect(*endpoints.begin());

  // Connected, now do the handshake
  co_await sock.async_handshake(boost::asio::ssl::stream_base::client);
  co_return sock;
}

async::main co_main(int argc, char **argv) {
  boost::asio::ssl::context ctx{boost::asio::ssl::context::tls_client};
  auto conn = co_await connect("httpbin.org", ctx);
  fmt::print("connected\n");
  beast::http::request<beast::http::empty_body> req{beast::http::verb::get, "/get?bla=bla", 11};
  co_await beast::http::async_write(conn, req, async::use_op);

  // read the response
  beast::flat_buffer b;
  beast::http::response<beast::http::string_body> response;
  co_await beast::http::async_read(conn, b, response);

  // write the response
  fmt::print("{}", response.body());
  co_return 0;
}
