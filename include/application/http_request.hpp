#pragma once

#include <string>
#include <string_view>


enum class Method {
    GET,
    POST,
    PUT,
    DELETE
};

struct HttpRequest {
    Method method;
    std::string path;
    std::string version;
    std::string body;

    static HttpRequest parse(std::string_view data);
    static bool isComplete(std::string_view data);
};


