#include <cpprest/http_listener.h>
#include <cpprest/base_uri.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include "pg_backend.h"


using HTTP_Handler = std::function<void(web::http::http_request, std::shared_ptr<PGBackend>)>;
using HTTP_Listener = web::http::experimental::listener::http_listener;


class WebApplication {
public:
    WebApplication(
        std::string host, std::string port, 
        std::string path, std::shared_ptr<PGBackend>
    );

    void add_route(
        const web::http::method& method, 
        const std::string& path, 
        const HTTP_Handler& handler
    );

    pplx::task<void> run();

    pplx::task<void> stop();

private:
    web::uri address;
    HTTP_Listener listener;
    std::shared_ptr<PGBackend> db_backend;
    
    std::map<web::http::method, std::map<std::string, HTTP_Handler>> routes;
};