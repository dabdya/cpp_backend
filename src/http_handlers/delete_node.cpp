#include "http_handler.h"
#include "db_service.h"
#include "http_response.h"
#include "utils.h"
#include <fmt/core.h>

void delete_node(
    web::http::http_request request, std::shared_ptr<PGBackend> pg_backend) 
{
    std::string node_id = get_node_id(request);
    auto conn = pg_backend->get_connection();

    if (!node_is_exist(node_id, conn)) {
        response_not_found(request);
        return;
    }

    remove_node(node_id, conn);
    delete_node_children(node_id, conn);

    pg_backend->free_connection(conn);
    request.reply(web::http::status_codes::OK);
}
