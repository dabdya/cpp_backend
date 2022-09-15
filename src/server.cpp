#include <condition_variable>
#include <iostream>
#include <signal.h>
#include <mutex>

#include "web_app.h"
#include "http_response.h"
#include "http_handler.h"
#include "db_service.h"


#include <libpq-fe.h>
#include <fmt/core.h>


static std::condition_variable condition;
static std::mutex mutex;

class InterruptHandler {
public:
    static void set_SIGINT() {
        signal(SIGINT, handler_SIGINT);        
    }

    static void wait_interrupt() {
        std::unique_lock<std::mutex> lock { mutex };
        condition.wait(lock);
        lock.unlock();
    }

private:
    static void handler_SIGINT(int signal){
        if (signal == SIGINT) {
            condition.notify_one();
        }
    }
};

void use_routes(WebApplication& app) {

    app.add_route(
        web::http::methods::POST, "/imports", import_filesystem);

    app.add_route(
        web::http::methods::DEL, "/delete/{id}", delete_node);

    app.add_route(
        web::http::methods::GET, "/nodes/{id}", get_node);
}

int main(int argc, char** argv) {
    PGParams params {"localhost", 5432, "disk", "iamdabdya", "Cy3brFt5"};
    auto pg_backend = std::make_shared<PGBackend>(params);

    WebApplication app("localhost", "8000", "", pg_backend);
    use_routes(app);

    InterruptHandler::set_SIGINT();

    app.run().wait();
    std::cout << "Server is ready to accept requests" << std::endl << std::flush;

    InterruptHandler::wait_interrupt();
    app.stop().wait();
}