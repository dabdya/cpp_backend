#pragma once
#include <libpq-fe.h>
#include <memory>
#include <string>


struct PGParams {
    std::string db_host;
    size_t      db_port;
    std::string db_name;
    std::string db_user;
    std::string db_password;
};

class PGConnection {
public:
    PGConnection(PGParams params);
    std::shared_ptr<PGconn> connection() const;
private:
    PGParams params;
    std::shared_ptr<PGconn> connection_;
};