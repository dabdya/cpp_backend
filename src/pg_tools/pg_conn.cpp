#include "pg_conn.h"
#include <stdexcept>

PGConnection::PGConnection(PGParams params) {
    this->params = params;
    connection_.reset(
        PQsetdbLogin(
            params.db_host.c_str(), std::to_string(params.db_port).c_str(), nullptr, nullptr, 
            params.db_name.c_str(), params.db_user.c_str(), params.db_password.c_str()
        ), 
        &PQfinish
    );

    if (PQstatus(connection_.get()) != CONNECTION_OK && PQsetnonblocking(connection_.get(), 1) != 0) {
       throw std::runtime_error(PQerrorMessage(connection_.get()));
    }
}

std::shared_ptr<PGconn> PGConnection::connection() const {
    return connection_;
}