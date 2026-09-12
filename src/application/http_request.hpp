#include <cstring>
#include <iostream>
#include <string_view>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;

    static HttpRequest parse(std::string_view data);
};


