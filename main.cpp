#include "src/HttpServer.hpp"

HttpResponse homeHandler(const HttpRequest& request)
{
    HttpResponse response;
    response.protocol = request.version;
    response.statusCode = Success;
    response.reasonPhase = "OK";
    response.contentType = "text/html";
    response.body = R"(<!DOCTYPE html>
<html>
<head><title>Success Connection</title></head>
<body>
    <h1>Hi</h1>
    <p>You have successfully connected to receive HTTP response and send HTTP request.</p>
</body>
</html>)";

    return response;
}

HttpResponse notFoundHandler(const HttpRequest& request)
{
    HttpResponse response;
    response.protocol = request.version;
    response.statusCode = NotFound;      // Prefer 404 instead of Success
    response.reasonPhase = "Not Found";
    response.contentType = "text/html";
    response.body = R"(<!DOCTYPE html>
<html>
<head><title>404 Not Found</title></head>
<body>
    <h1>404 Not Found</h1>
    <p>The requested URL was not found on this server.</p>
</body>
</html>)";

    return response;
}

int main()
{
    HttpServer server;

    server.GET("/", homeHandler);

    // Optional: set a default handler if your server supports it

    server.run(8080);
}
