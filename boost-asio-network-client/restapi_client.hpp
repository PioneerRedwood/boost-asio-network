#pragma once
#include "predef.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban {
class restapi_client
{
public:
	restapi_client(io::io_context& context, std::string host, std::string port)
		: context_(context), resolver_(context), socket_(context)
		, host_(host), port_(port)
	{}

	bool get_account_info(std::string target, std::unordered_map<std::string, std::string>& result_map)
	{
		try
		{
			auto const results = resolver_.resolve(host_, port_);
			io::connect(socket_, results.begin(), results.end());

			http::request<http::string_body> req{ http::verb::get, target, 11 };
			req.set(http::field::host, host_);
			req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

			beast::error_code ec;
			http::write(socket_, req, ec);
			if (ec)
			{
				throw beast::system_error{ ec };
				return false;
			}

			http::response<http::string_body> res;

			http::read(socket_, buffer, res, ec);

			if (!ec && res.result() == http::status::ok)
			{
				// {"id":"0"}
				using namespace boost::property_tree;
				ptree pt;

				std::stringstream ss;
				ss << res.body();
				read_json(ss, pt);
				for (ptree::const_iterator iter = pt.begin(); iter != pt.end(); ++iter)
				{
					result_map.try_emplace(iter->first, iter->second.get_value<std::string>());
				}

				socket_.shutdown(tcp::socket::shutdown_both, ec);
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << "\n";
			return false;
		}
	}

private:
	io::io_context& context_;
	tcp::resolver resolver_;
	tcp::socket socket_;

	std::string host_;
	std::string port_;
	
	beast::flat_buffer buffer;
};
}