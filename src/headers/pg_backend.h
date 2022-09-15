#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <queue>
#include <condition_variable>
#include <libpq-fe.h>
#include "pg_conn.h"

class PGBackend {
public:
    PGBackend(PGParams params, size_t pool_size = 10);
    std::shared_ptr<PGConnection> get_connection();
    void free_connection(std::shared_ptr<PGConnection>);

private:
    void create_pool(PGParams params);
    
    std::mutex mutex;
    std::condition_variable condition;
    std::queue<std::shared_ptr<PGConnection>> pool;

    size_t pool_size;
};