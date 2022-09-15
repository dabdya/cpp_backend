#include "pg_backend.h"

PGBackend::PGBackend(PGParams params, size_t pool_size) {
    this->pool_size = pool_size;
    create_pool(params);
}

void PGBackend::create_pool(PGParams params) {
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < pool_size; i++) {
        pool.emplace(std::make_shared<PGConnection>(params));
    }
}

std::shared_ptr<PGConnection> PGBackend::get_connection() {
    std::unique_lock<std::mutex> lock(mutex);

    while (pool.empty()) {
        condition.wait(lock);
    }

    auto conn_ = pool.front();
    pool.pop();

    return conn_;
}

void PGBackend::free_connection(std::shared_ptr<PGConnection> conn_)
{
    std::unique_lock<std::mutex> lock(mutex);
    pool.push(conn_);
    lock.unlock();
    condition.notify_one();
}