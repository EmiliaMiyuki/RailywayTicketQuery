#ifndef __MIYUKI_NETWORK_HPP__
#define __MIYUKI_NETWORK_HPP__

#include <boost/asio.hpp>
#include <sstream>
#include <cstring>

namespace asio = boost::asio;

void http_get(const char * server, const char * path, char * desc, const char * cookie = nullptr, const char * referer = nullptr, const char * post_content = nullptr) {
	using boost::asio::ip::tcp;
	asio::io_service io_service;

	try {
    	tcp::resolver resolver(io_service);
    	tcp::resolver::query query(server, "http");
	    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    	tcp::socket socket(io_service);
    	boost::asio::connect(socket, endpoint_iterator);

    	boost::asio::streambuf request;
    	std::ostream request_stream(&request);
    	request_stream << ( post_content ? "POST " : "GET ") << path << " HTTP/1.0\r\n";
    	request_stream << "Host: " << server << "\r\n";
    	if (cookie)
    		request_stream << "Cookie: " << cookie << "\r\n";
    	if (referer)
    		request_stream << "Referer: " << referer << "\r\n";
    	request_stream << "Accept: */*\r\n";
    	request_stream << "Connection: close\r\n\r\n";

    	// Send the request.
    	asio::write(socket, request);

    	// Read the response
    	boost::asio::streambuf response;
    	boost::asio::read_until(socket, response, "\r\n");

    	// Check that response is OK.
    	std::istream response_stream(&response);
    	std::string http_version;
    	response_stream >> http_version;
    	unsigned int status_code;
    	response_stream >> status_code;
    	std::string status_message;
    	std::getline(response_stream, status_message);
    	if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
      		std::cout << "Invalid response\n";
      		return;
    	}

    	// Read the response headers, which are terminated by a blank line.
    	boost::asio::read_until(socket, response, "\r\n\r\n");

    	// Process the response headers.
    	std::string header;
    	while (std::getline(response_stream, header) && header != "\r")
    		;//std::cout << header << "\n";

    	if (status_code != 200) {
      		std::cout << "Response returned with status code " << status_code << "\n";
    		return;
    	}

    	std::stringstream ss;
     	// Write whatever content we already have to output.
    	if (response.size() > 0)
     		ss << &response;
    	// Read until EOF, writing data to output as we go.
    	boost::system::error_code error;
    	while (boost::asio::read(socket, response,
        	boost::asio::transfer_at_least(1), error))
      	ss << &response;
      	strcpy(desc, ss.str().c_str());
    	if (error != boost::asio::error::eof)
    		throw boost::system::system_error(error);
    }
    catch (std::exception e) {
    	std::cout << e.what();
    }
}

#endif