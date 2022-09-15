#pragma once
#include <cpprest/http_listener.h>
#include <string>

std::string get_node_id(web::http::http_request& request);

bool path_match(const std::string& app_path, const std::string& request_path);