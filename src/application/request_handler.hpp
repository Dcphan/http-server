#include "http_request.hpp"
#include "http_response.hpp"

class RequestHandler{
    
    public:
        HttpResponse handler(const HttpRequest& request);
};
