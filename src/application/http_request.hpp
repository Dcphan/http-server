#pragma once

#include <cstring>
#include <iostream>
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

    static HttpRequest parse(std::string_view data);
};


