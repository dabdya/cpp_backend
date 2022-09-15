#include "web_app.h"
#include <fmt/core.h>
#include "pg_backend.h"
#include "utils.h"

WebApplication::WebApplication(
    std::string host, std::string port, std::string path, std::shared_ptr<PGBackend> pg_backend) 
{
    address = web::uri(fmt::format("http://{}:{}/{}", host, port, path));
    listener = HTTP_Listener(address);
    db_backend = pg_backend;
}

void WebApplication::add_route(
    const web::http::method& method, const std::string& path, const HTTP_Handler& handler) {
    routes[method][path] = handler;
}

pplx::task<void> WebApplication::run() {
    auto& routes_ = routes;
    auto& db_backend_ = db_backend;
        auto handler = [&](web::http::http_request request) {
            auto relativePath = web::uri::decode(request.relative_uri().path());
            auto method = request.method();
            if (routes_.count(method) && routes_.at(method).count(relativePath)) {
                routes_.at(method).at(relativePath)(request, db_backend_);
            }

            if (routes_.count(method)) {
                for (const auto& [path, handler_] : routes_.at(method)) {
                    if (path_match(path, relativePath)) {
                        handler_(request, db_backend_);
                    }
                }
            }
            request.reply(web::http::status_codes::NotFound);
        };

        for (const auto& [method, _]: routes) {
            listener.support(method, handler);
        }

        return listener.open();
}

pplx::task<void> WebApplication::stop() {
    return listener.close();
}
