#pragma once
#include <cpprest/http_listener.h>

void response_bad_request(web::http::http_request& request);

void response_not_found(web::http::http_request& request);