#include "http_request.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include <cstring>

HttpRequest HttpRequest::parse(std::string_view data){
    HttpRequest request;

    size_t firstSpace = data.find(' ');
    size_t secondSpace = data.find(' ', firstSpace + 1);
    size_t endLine = data.find("\r\n");

    std::string_view method = data.substr(0, firstSpace); 

    if (method == "GET"){
        request.method = Method::GET;
    } else if(method == "POST"){
        request.method = Method::POST;
    } else if (method == "PUT") {
        request.method = Method::PUT;
    } else if (method == "DELETE"){
        request.method = Method::DELETE;
    }
    request.path = std::string(data.substr(firstSpace + 1, secondSpace - firstSpace - 1));
    request.version = std::string(data.substr(secondSpace + 1, endLine - secondSpace - 1));

    return request;
}
