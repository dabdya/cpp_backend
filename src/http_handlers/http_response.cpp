#include <cpprest/http_listener.h>
#include "http_response.h"

void response_bad_request(web::http::http_request& request) {
    auto response = web::json::value::object();
    response["message"] = web::json::value::string("Validation Failed");
    response["code"] = 400;
    request.reply(web::http::status_codes::BadRequest, response);
}

void response_not_found(web::http::http_request& request) {
    auto response = web::json::value::object();
    response["message"] = web::json::value::string("Item not found");
    response["code"] = 404;
    request.reply(web::http::status_codes::NotFound, response);
}