#include "request_handler.hpp"

HttpResponse RequestHandler::handler(const HttpRequest& request){
    HttpResponse respond;
    respond.protocol = request.version;
    if (request.method == "GET" && request.path == "/"){
        respond.statusCode = Success;
        respond.reasonPhase = "OK";
        respond.contentType = "text/html";
        respond.body = R"(<!DOCTYPE html>
                        <html>
                        <head><title>Success Connection</title></head>
                        <body><h1>Hi</h1><p>You have successfully connected to receive HTTP respond and send HTTP request.</p></body>
                        </html>)";
    } else {
        respond.statusCode = Success;
        respond.reasonPhase = "OK";
        respond.contentType = "text/html";
        respond.body = R"(<!DOCTYPE html>
                        <html>
                        <head><title>404 Not Found</title></head>
                        <body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body>
                        </html>)";
    }

    return respond;
};
