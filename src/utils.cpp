#include "utils.h"

std::string get_node_id(web::http::http_request& request) {
    auto path = web::uri::decode(request.relative_uri().path());
    auto path_vect = web::uri::split_path(path);
    return path_vect[path_vect.size() - 1];
}

bool path_match(const std::string& app_path, const std::string& request_path) {
    auto app_vect = web::uri::split_path(app_path);
    auto request_vect = web::uri::split_path(request_path);

    if (app_vect.size() != request_vect.size()) {
        return false;
    }

    const size_t n = app_vect.size();

    for (size_t i = 0; i < n; i++) {
        if (app_vect[i] != request_vect[i] && 
            !app_vect[i][0] == '{' && !app_vect[i][n-1] == '}') {
                return false;
            }
    }
    return true;
}
