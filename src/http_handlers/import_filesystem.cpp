#include "http_handler.h"
#include "db_service.h"
#include "http_response.h"
#include "utils.h"
#include <fmt/core.h>

bool node_is_valid(web::json::value& node_obj) {
    if (!node_obj.has_string_field("id") ||
        !node_obj.has_string_field("type")) {
        return false;
    }

    if (node_obj["id"].as_string().size() == 0) {
        return false;
    }

    if (node_obj["type"].as_string() == "FOLDER" && 
        (node_obj.has_string_field("url") || 
        node_obj.has_integer_field("size"))) {
        return false;
    }

    if (node_obj["type"].as_string() == "FILE" && 
        (!node_obj.has_string_field("url") || 
        !node_obj.has_integer_field("size"))) {
        return false;
    }

    if (node_obj.has_string_field("url") && 
        node_obj["url"].as_string().size() > 255) {
        return false;
    }

    if (node_obj.has_integer_field("size") &&
        node_obj["size"].as_integer() <= 0) {
        return false;
    }

    if (node_obj.has_field("parentId") &&
        !node_obj.has_string_field("parentId") && 
        !node_obj.at("parentId").is_null()) {
        return false;
    }

    return true;
}

bool import_is_valid(web::json::value& json) {
    if (!json.has_array_field("items") || 
        !json.has_string_field("updateDate")) {
        return false;
    }

    // TODO: Check ISO 8601 for date: json["updateDate"]

    for (size_t i = 0; i < json["items"].size(); i++) {
        auto& node_obj = json["items"][i];
        if (!node_is_valid(node_obj)) {
            return false;
        }
    }

    return true;
}

bool node_type_changed(
    web::json::value& nodes, std::shared_ptr<PGConnection> conn) 
{
    for (size_t i = 0; i < nodes.size(); i++) {
        auto node_obj = nodes[i];

        std::string node_id = fmt::format(
            "{}", node_obj["id"].as_string());
        auto node_db = select_node(node_id, conn);

        if (node_db != web::json::value::null() && 
            node_db["type"].as_string() != node_obj["type"].as_string()) 
        {
            return true;
        }
    }

    return false;
}

void import_filesystem(
    web::http::http_request request, std::shared_ptr<PGBackend> pg_backend) 
{
    web::json::value json = request.extract_json().get();

    auto conn = pg_backend->get_connection();

    const std::string NODES = "items";
    const std::string DATE = "updateDate";

    if (!import_is_valid(json) || 
        node_type_changed(json[NODES], conn)) 
    {
        response_bad_request(request);
        return;
    }

    std::vector<web::json::value> update_values;
    std::vector<web::json::value> insert_values;

    for (size_t i = 0; i < json[NODES].size(); i++) {
        auto node_obj = json[NODES][i];
        node_obj[DATE] = json[DATE];

        std::string node_id = fmt::format(
            "{}", node_obj["id"].as_string());

        if (node_is_exist(node_id, conn)) {
            update_values.push_back(node_obj); 
        } else {
            insert_values.push_back(node_obj);
        }
    }

    if (insert_values.size()) {
        insert_nodes(insert_values, conn);
    }

    if (update_values.size()) {
        update_nodes(update_values, conn);
    }

    pg_backend->free_connection(conn);
    request.reply(web::http::status_codes::OK);
}
