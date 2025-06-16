#include "BackendSQLite.h"
#include "File.h"
#include "sqlite3.h"
#include <algorithm>
#include <spdlog/spdlog.h>

#define SQL_FILE_PATH           "../sql/"
#define SQL_FILE                SQL_FILE_PATH"BackendSQLite.db"

BackendSQLite* BackendSQLite::Instance(){

    static BackendSQLite backend_sqlite;

    return &backend_sqlite;
}

int BackendSQLite::insert_data(std::string time, std::string base64){

    open_sqlite();
    std::string sql = "INSERT INTO images (time, base64) VALUES (?, ?);";

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return 0;
    }

    sqlite3_bind_text(m_stmt, 1, time.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(m_stmt, 2, base64.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(m_stmt);

    sqlite3_int64 row_id = 0;
    if (rc != SQLITE_DONE){
        spdlog::error("execution failed: {}", sqlite3_errmsg(m_db));
    }else {
        row_id = sqlite3_last_insert_rowid(m_db);
    }

    clean_resource();

    return row_id;
}

void BackendSQLite::delete_by_id(int id){

    open_sqlite();
    std::string sql = "DELETE FROM images WHERE id = ?;";
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return;
    }

    sqlite3_bind_int(m_stmt, 1, id);
    rc = sqlite3_step(m_stmt);
    if (rc != SQLITE_DONE){
        spdlog::error("execution failed: {}", sqlite3_errmsg(m_db));
    }

    clean_resource();
}

int BackendSQLite::get_row_count(){

    open_sqlite();
    std::string sql = "SELECT COUNT(*) FROM images;";

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
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

bool BackendSQLite::get_data_by_id(int target_id, Backend_Info &info){

    open_sqlite();

    std::string sql = "SELECT id, time, base64 FROM images WHERE id = ?;";
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return false;
    }
    sqlite3_bind_int(m_stmt, 1, target_id);
    bool state = false;
    if (sqlite3_step(m_stmt) == SQLITE_ROW) {
        info.id = sqlite3_column_int(m_stmt, 0);
        info.time = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, 1));
        info.base64 = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, 2));

        state = true;
    }
    clean_resource();
    return state;
}

void BackendSQLite::get_all_data(std::vector<Backend_Info> &data){
    
    open_sqlite();
    std::string sql = "SELECT id, time, base64 FROM images;";
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return;
    }
    while (sqlite3_step(m_stmt) == SQLITE_ROW){
        Backend_Info info;
        info.id = sqlite3_column_int(m_stmt, 0);
        info.time = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, 1));
        info.base64 = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, 2));
        
        data.push_back(info);
    }
    clean_resource();
    std::reverse(data.begin(), data.end());
}

void BackendSQLite::get_all_id(std::vector<int> &id_data){

    open_sqlite();
    std::string sql = "SELECT id FROM images;";
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        spdlog::error("failed to prepare statement: {}", sqlite3_errmsg(m_db));
        close_sqlite();
        return;
    }
    while (sqlite3_step(m_stmt) == SQLITE_ROW){
        int id = sqlite3_column_int(m_stmt, 0);
        id_data.push_back(id);
    }
    clean_resource();
    std::reverse(id_data.begin(), id_data.end());
}

void BackendSQLite::open_sqlite(){

    util::create_file(SQL_FILE_PATH, nullptr);
    int rc = sqlite3_open(SQL_FILE, &m_db);
    if (rc){
        spdlog::error("cannot open sqlite: {}", rc);
        return;
    }
    if (m_table_flag)
        return;
    const char* sql_create_table = "CREATE TABLE IF NOT EXISTS images (id INTEGER PRIMARY KEY, time TEXT, base64 TEXT);";
    rc = sqlite3_exec(m_db, sql_create_table, nullptr, nullptr, &m_errmsg);
    if (rc) {
        spdlog::error("create table failed: {}", sqlite3_errmsg(m_db));
        return;
    }
    m_table_flag = true;
}

void BackendSQLite::close_sqlite(){

    sqlite3_close(m_db);
}

void BackendSQLite::clean_resource(){

    sqlite3_finalize(m_stmt);
    close_sqlite();
}

