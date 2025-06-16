#include "UserSQLite.h"
#include "File.h"
#include <spdlog/spdlog.h>

#define SQL_FILE_PATH           "../sql/"
#define SQL_FILE                (SQL_FILE_PATH"UserSQLite.db")

UserSQLite::UserSQLite(){

    m_table_flag = false;
}

UserSQLite::~UserSQLite(){

    close_sqlite();
}

int UserSQLite::get_row_count(){
    
    open_sqlite();
    const char* sql = "SELECT COUNT(*) FROM people;";

    int rc = sqlite3_prepare_v2(m_db, sql, -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return 0;
    }

    int count = 0;
    if (sqlite3_step(m_stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(m_stmt, 0);
    }

    clean_resource();

    return count;
}

void UserSQLite::open_sqlite(){

    util::create_file(SQL_FILE_PATH, nullptr);
    int rc = sqlite3_open(SQL_FILE, &m_db);
    if (rc){
        spdlog::error("cannot open sqlite: {}", rc);
        return;
    }
    if (m_table_flag)
        return;
    const char* sql_create_table = "CREATE TABLE IF NOT EXISTS people (id INTEGER PRIMARY KEY, name TEXT);";
    rc = sqlite3_exec(m_db, sql_create_table, nullptr, nullptr, &m_errmsg);
    if (rc) {
        spdlog::error("create table failed: {}", sqlite3_errmsg(m_db));
        return;
    }
    m_table_flag = true;
}

void UserSQLite::close_sqlite(){

    sqlite3_close(m_db);
}

void UserSQLite::clean_resource(){

    sqlite3_finalize(m_stmt);
    close_sqlite();
}

std::string UserSQLite::get_name_by_id(int id){

    open_sqlite();
    const char* sql = "SELECT name FROM people WHERE id = ?;";

    int rc = sqlite3_prepare_v2(m_db, sql, -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return "";
    }

    sqlite3_bind_int(m_stmt, 1, id);

    std::string name;
    if (sqlite3_step(m_stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(m_stmt, 0);
        if (text) {
            name = reinterpret_cast<const char*>(text);
        }
    }

    clean_resource();

    return name;
}

void UserSQLite::insert_data(int label, std::string name){

    open_sqlite();
    std::string sql = "INSERT INTO people (id, name) VALUES (?, ?);";

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return;
    }

    sqlite3_bind_int(m_stmt, 1, label);
    sqlite3_bind_text(m_stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(m_stmt);
    if (rc != SQLITE_DONE){
        spdlog::error("execution failed: {}", sqlite3_errmsg(m_db));
    }

    clean_resource();
}

UserSQLite* UserSQLite::Instance(){

    static UserSQLite user_sqlite;

    return &user_sqlite;
}
