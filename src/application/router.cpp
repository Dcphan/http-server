#include "router.hpp"

namespace {
bool matchesPattern(const std::string& pattern, const std::string& path) {
    size_t patternStart = 0;
    size_t pathStart = 0;

    while (patternStart < pattern.size() && pathStart < path.size()) {
        size_t patternEnd = pattern.find('/', patternStart);
        size_t pathEnd = path.find('/', pathStart);
        if (patternEnd == std::string::npos) patternEnd = pattern.size();
        if (pathEnd == std::string::npos) pathEnd = path.size();

        std::string patternPart = pattern.substr(patternStart, patternEnd - patternStart);
        std::string pathPart = path.substr(pathStart, pathEnd - pathStart);
        bool parameter = patternPart.size() >= 2 && patternPart.front() == '{' && patternPart.back() == '}';
        if (!parameter && patternPart != pathPart) return false;
        if (parameter && pathPart.empty()) return false;

        patternStart = patternEnd + 1;
        pathStart = pathEnd + 1;
        if (patternEnd == pattern.size() || pathEnd == path.size()) {
            return patternEnd == pattern.size() && pathEnd == path.size();
        }
    }

    return patternStart == pattern.size() && pathStart == path.size();
}
}

void Router::GET(const std::string& path, Handler func){
    this->routes[Method::GET][path] = func;
}

void Router::PUT(const std::string& path, Handler func){
    this->routes[Method::PUT][path] = func;
}

void Router::POST(const std::string& path, Handler func){
    this->routes[Method::POST][path] = func;
}

void Router::DELETE(const std::string& path, Handler func){
    this->routes[Method::DELETE][path] = func;
}

Handler Router::find(Method method, const std::string& path){
    auto methodRoutes = this->routes.find(method);
    if (methodRoutes == this->routes.end()) return nullptr;

    auto exactRoute = methodRoutes->second.find(path);
    if (exactRoute != methodRoutes->second.end()) return exactRoute->second;

    for (const auto& [pattern, handler] : methodRoutes->second) {
        if (matchesPattern(pattern, path)) return handler;
    }
    return nullptr;
}
