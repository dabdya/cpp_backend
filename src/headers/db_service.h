#pragma once
#include "pg_conn.h"
#include <cpprest/http_listener.h>

#include <vector>
#include <string>

bool node_is_exist(
    std::string& node_id, std::shared_ptr<PGConnection> conn);

web::json::value select_node(std::string node_id, std::shared_ptr<PGConnection> conn);

void remove_node(std::string& node_id, std::shared_ptr<PGConnection> conn);

void update_nodes(
    const std::vector<web::json::value>& values, std::shared_ptr<PGConnection> conn);

void insert_nodes(
    const std::vector<web::json::value>& values, std::shared_ptr<PGConnection> conn);

std::vector<web::json::value> get_node_children(
    const std::string& node_id, std::shared_ptr<PGConnection> conn);

void delete_node_children(std::string node_id, std::shared_ptr<PGConnection> conn);

void create_table(std::shared_ptr<PGConnection> conn);