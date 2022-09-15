#include "http_handler.h"
#include "db_service.h"
#include "http_response.h"
#include "utils.h"
#include <fmt/core.h>

void get_node(
    web::http::http_request request, std::shared_ptr<PGBackend> pg_backend) 
{
    std::string node_id = get_node_id(request);
    auto conn = pg_backend->get_connection();

    if (!node_is_exist(node_id, conn)) {
        response_not_found(request);
        return;
    }

    auto node = select_node(node_id, conn);

    auto children_vect = get_node_children(node_id, conn);
    auto arr = web::json::value::array(children_vect.size());

    for (size_t i = 0; i < children_vect.size(); i++) {
        arr[i] = children_vect[i];
    }

    node["children"] = arr;

    pg_backend->free_connection(conn);
    request.reply(web::http::status_codes::OK, node);
}
