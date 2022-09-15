#pragma once
#include <cpprest/http_listener.h>
#include "pg_backend.h"

void import_filesystem(
    web::http::http_request request, std::shared_ptr<PGBackend> pg_backend);

void get_node(
    web::http::http_request request, std::shared_ptr<PGBackend> pg_backend);

void delete_node(
    web::http::http_request request, std::shared_ptr<PGBackend> pg_backend);