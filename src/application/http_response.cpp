#include "http_response.hpp"
#include <string>

std::string HttpResponse::toHttpMessage(HttpResponse response){
    std::string message;

    message += response.protocol + " " + std::to_string(response.statusCode) + " " + response.reasonPhase + "\r\n";
    message += "Content-Type: " + response.contentType +"\r\n";
    message += "Content-Length: " + std::to_string(response.body.size()) + "\r\n";
    message += "\r\n"; // HTTP/1.1 seperate body
    message += response.body;

    return message;    
}
