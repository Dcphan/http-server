#pragma once

#include <string>
#include <unordered_map>
#include "http_request.hpp"
#include "http_response.hpp"

using Handler = HttpResponse (*)(const HttpRequest&);

class Router {
    public:
        void GET(const std::string& path, Handler func);
        void POST(const std::string& path, Handler func);
        void PUT(const std::string& path, Handler func);
        void DELETE(const std::string& path, Handler func);
        Handler find(Method method, const std::string& path);

    private:
        std::unordered_map<Method, std::unordered_map<std::string, Handler>> routes;
};
