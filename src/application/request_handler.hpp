#include "http_request.hpp"
#include "http_response.hpp"

class RequestHandler{
    
    public:
        static HttpResponse handler(const HttpRequest& request);
};
