#include "db_service.h"
#include <fmt/core.h>
#include <libpq-fe.h>
#include <cpprest/http_listener.h>


bool node_is_exist(
    std::string& node_id, std::shared_ptr<PGConnection> conn) {

    std::string exist_query = fmt::format(
        "SELECT * FROM nodes WHERE id = '{}'", node_id);

    bool is_exist = false;

    if (!PQsendQuery(conn->connection().get(), exist_query.c_str())) {
        std::cout << PQerrorMessage(conn->connection().get()) << std::flush;
        throw std::runtime_error("SEND ERROR");
    }

    while (auto res = PQgetResult(conn->connection().get())) {
        if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
            is_exist = true;
        }
    }

    return is_exist;
}

web::json::value select_node(
    std::string node_id, std::shared_ptr<PGConnection> conn) {

    std::string get_query = fmt::format(
        "SELECT * FROM nodes WHERE id = '{}'", node_id);

    if (!PQsendQuery(conn->connection().get(), get_query.c_str())) {
         std::cout << PQerrorMessage(conn->connection().get()) << std::flush;
         throw std::runtime_error("SEND ERROR");
    }

    auto node = web::json::value::object();

    while (auto res = PQgetResult(conn->connection().get())) {
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            const size_t nfields = PQnfields(res);
            const size_t ntuples = PQntuples(res);

            if (ntuples == 0) {
                node = web::json::value::null();
                continue;
            }

            std::vector<std::string> field_names;
            for (size_t j = 0; j < nfields; j++) {
                field_names.push_back(PQfname(res, j));
            }

            for (size_t j = 0; j < nfields; j++) {
                node[field_names[j]] = 
                    web::json::value(PQgetvalue(res, 0, j));
            }
        }
    }

    return node;
}

void remove_node(std::string& node_id, std::shared_ptr<PGConnection> conn) {

    std::string delete_query = fmt::format(
        "DELETE FROM nodes WHERE id = '{}'", node_id);

    if (!PQsendQuery(conn->connection().get(), delete_query.c_str())) {
        std::cout << PQerrorMessage(conn->connection().get()) << std::flush;
        throw std::runtime_error("SEND ERROR");
    }

    while (auto res = PQgetResult(conn->connection().get())) {
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            throw std::runtime_error("EXECUTION ERROR");
        }
    }
}

void insert_nodes(
    const std::vector<web::json::value>& values, std::shared_ptr<PGConnection> conn) 
{
    std::vector<std::string> field_names = {
        "id", "type", "parentId", "url", "size", "updateDate"};

    std::ostringstream ss;
    ss << "INSERT INTO nodes (";

    for (size_t i = 0; i < field_names.size(); i++) {
        ss << " " << fmt::format("\"{}\"", field_names[i]);
        if (i < field_names.size() - 1) ss << ",";
    }

    ss << ") VALUES ";
    for (size_t i = 0; i < values.size(); i++) {
        const web::json::value& value = values[i];
        ss << "(";
        for (size_t j = 0; j < field_names.size(); j++) {
            const std::string& name = field_names[j];
            if (value.has_string_field(name)) {
                ss << fmt::format("'{}'", value.at(name).as_string());
            } else if (value.has_field(name)) {
                ss << value.at(name);
            } else {
                ss << web::json::value::null();
            }
            if (j < field_names.size() - 1) ss << ", ";
        }
        ss << ")";
        if (i < values.size() - 1) ss << ",";
    }

    std::string insert_query = ss.str();

    if (!PQsendQuery(conn->connection().get(), insert_query.c_str())) {
        std::cout << PQerrorMessage(conn->connection().get()) << std::flush;
        throw std::runtime_error("SEND ERROR");
    }

    while (auto res = PQgetResult(conn->connection().get())) {
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            throw std::runtime_error("EXECUTION ERROR");
        }
    }
}

void update_nodes(
    const std::vector<web::json::value>& values, std::shared_ptr<PGConnection> conn) 
{

    std::vector<std::string> field_names = {
        "parentId", "url", "size", "updateDate"};

    const std::string ID_NAME = "id";

    std::ostringstream ss;
    ss << "UPDATE nodes as N1 SET";

    for (size_t i = 0; i < field_names.size(); i++) {
        const std::string& name = field_names[i];
        ss << fmt::format(" \"{}\" = N2.\"{}\"", name, name);
        if (i < field_names.size() - 1) {
            ss << ",";
        }
    }
    
    ss << " FROM (VALUES";

    for (size_t i = 0; i < values.size(); i++) {
        const auto& node = values[i];
        ss << fmt::format("('{}',", node.at(ID_NAME).as_string());

        for (size_t j = 0; j < field_names.size(); j++) {
            if (node.has_string_field(field_names[j])) {
                ss << fmt::format(" '{}'", 
                node.at(field_names[j]).as_string());
            } else if (node.has_field(field_names[j])) {
                ss << node.at(field_names[j]);
            } else {
                ss << web::json::value::null();
            }

            if (field_names[j] == "updateDate") {
                ss << "::TIMESTAMP";
            }

            if (field_names[j] == "size") {
                ss << "::INT";
            }

            if (j < field_names.size() - 1) ss << ",";
            if (j == field_names.size() - 1) ss << ")";
        }
        if (i < values.size() - 1) ss << ",";
    }
    ss << fmt::format(") as N2(\"{}\", ", ID_NAME);
    for (size_t i = 0; i < field_names.size(); i++) {
        ss << " " << fmt::format("\"{}\"", field_names[i]);
        if (i < field_names.size() - 1) {
            ss << ",";
        }
    }

    ss << fmt::format(") WHERE N2.\"{}\" = N1.\"{}\"", ID_NAME, ID_NAME);
    std::string update_query = ss.str();

    if (!PQsendQuery(conn->connection().get(), update_query.c_str())) {
        std::cout << PQerrorMessage(conn->connection().get()) << std::flush;
        throw std::runtime_error("SEND ERROR");
    }

    while (auto res = PQgetResult(conn->connection().get())) {
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            throw std::runtime_error("EXECUTION ERROR");
        }
    }
}

std::vector<web::json::value> get_node_children(
    const std::string& node_id, std::shared_ptr<PGConnection> conn) 
{
    std::vector<web::json::value> children;

    std::string children_query = fmt::format(
        "SELECT * FROM nodes WHERE \"parentId\" = '{}'", node_id);

    if (!PQsendQuery(conn->connection().get(), children_query.c_str())) {
        std::cout << PQerrorMessage(conn->connection().get()) << std::flush;
        throw std::runtime_error("SEND ERROR");
    }

    while (auto res = PQgetResult(conn->connection().get())) {
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            const size_t nfields = PQnfields(res);
            const size_t ntuples = PQntuples(res);

            std::vector<std::string> field_names;
            for (size_t j = 0; j < nfields; j++) {
                field_names.push_back(PQfname(res, j));
            }

            for (size_t i = 0; i < ntuples; i++) {
                auto obj = web::json::value::object();
                for (size_t j = 0; j < nfields; j++) {
                    obj[field_names[j]] = web::json::value(PQgetvalue(res, i, j));
                }
                children.push_back(obj);
            }
        } else {
            throw std::runtime_error("EXECUTION ERROR");
        }
    }

    for (auto& child : children) {
        if (child["type"].as_string() == "FOLDER") {
            auto children_vect = get_node_children(child["id"].as_string(), conn);
            auto arr = web::json::value::array(children_vect.size());

            for (size_t i = 0; i < children_vect.size(); i++) {
                arr[i] = children_vect[i];
            }
            child["children"] = arr;
        } else {
            child["children"] = web::json::value::null();
        }
    }

    return children;
}

void delete_node_children(std::string node_id, std::shared_ptr<PGConnection> conn) 
{
    auto children_vect = get_node_children(node_id, conn);
    std::string children_query = fmt::format(
        "DELETE FROM nodes WHERE \"parentId\" = '{}'", node_id);

    if (!PQsendQuery(conn->connection().get(), children_query.c_str())) {
        std::cout << PQerrorMessage(conn->connection().get()) << std::flush;
        throw std::runtime_error("SEND ERROR");
    }

    while (auto res = PQgetResult(conn->connection().get())) {
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            throw std::runtime_error("EXECUTION ERROR");
        }
    }

    for (auto& child : children_vect) {
        if (child["type"].as_string() == "FOLDER") {
            delete_node_children(child["id"].as_string(), conn);
        }
    }
}
