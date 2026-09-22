#include "router.hpp"

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
    return this->routes[method][path];
}
