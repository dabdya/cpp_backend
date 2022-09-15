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

void init_db(std::shared_ptr<PGBackend> pg_backend) {
    auto conn = pg_backend->get_connection();

    create_table(conn);
    pg_backend->free_connection(conn);
}

struct args {
    std::string host;
    std::string port;
};

args parse_args(int argc, char** argv) {
    args args_;
    for (int i = 1; i < argc; ++i) {
        if (i == 1) args_.host = argv[i];
        else args_.port = argv[i];
    }
    return args_;
}

int main(int argc, char** argv) {
    PGParams params {"localhost", 5432, "disk", "iamdabdya", "Cy3brFt5"};
    auto pg_backend = std::make_shared<PGBackend>(params);

    init_db(pg_backend);

    args args_ = parse_args(argc, argv);
    WebApplication app(args_.host, args_.port, pg_backend);

    use_routes(app);

    InterruptHandler::set_SIGINT();

    app.run().wait();
    std::cout << "Server is ready to accept requests " 
    << args_.host << " " << args_.port << std::endl << std::flush;

    InterruptHandler::wait_interrupt();
    app.stop().wait();
}