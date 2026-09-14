#include <string>

enum StatusCode : int {
    Success = 200,
    BadRequest = 400,
    NotFound = 404
};

struct HttpResponse {

    // Status Line
    std::string protocol;
    StatusCode statusCode;
    std::string reasonPhase;

    // Headers
    std::string contentType;
    std::string body;
};
