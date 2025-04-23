#include "BackendSQLite.h"
#include "File.h"
#include "sqlite3.h"

#define SQL_FILE_PATH           (util::File::get_currentWorking_directory()+"/sql/")
#define SQL_FILE                (SQL_FILE_PATH + "BackendSQLite.db")

BackendSQLite* BackendSQLite::Instance(){

    static BackendSQLite backend_sqlite;

    return &backend_sqlite;
}

int BackendSQLite::insert_data(std::string time, std::string base64){

    open_sqlite();
    std::string sql = "INSERT INTO images (time, base64) VALUES (?, ?);";

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        close_sqlite();
        return 0;
    }

    sqlite3_bind_text(m_stmt, 1, time.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(m_stmt, 2, base64.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(m_stmt);

    sqlite3_int64 row_id = 0;
    if (rc != SQLITE_DONE){
        std::cerr << "execution failed: " << sqlite3_errmsg(m_db) << std::endl;
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
        std::cerr << "failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        close_sqlite();
        return;
    }

    sqlite3_bind_int(m_stmt, 1, id);
    rc = sqlite3_step(m_stmt);
    if (rc != SQLITE_DONE){
        std::cerr << "execution failed: " << sqlite3_errmsg(m_db) << std::endl;
    }

    clean_resource();
}

int BackendSQLite::get_row_count(){

    open_sqlite();
    std::string sql = "SELECT COUNT(*) FROM images;";

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
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

void BackendSQLite::get_all_data(std::vector<Backend_Info> &data){
    
    open_sqlite();
    std::string sql = "SELECT id, time, base64 FROM images;";
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
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
}

void BackendSQLite::open_sqlite(){

    util::File::create_file(SQL_FILE_PATH.c_str(), nullptr);
    int rc = sqlite3_open(SQL_FILE.c_str(), &m_db);
    if (rc){
        std::cerr << "cannot open sqlite: " << rc << std::endl;
        return;
    }
    if (m_table_flag)
        return;
    const char* sql_create_table = "CREATE TABLE IF NOT EXISTS images (id INTEGER PRIMARY KEY, time TEXT, base64 TEXT);";
    rc = sqlite3_exec(m_db, sql_create_table, nullptr, nullptr, &m_errmsg);
    if (rc) {
        std::cerr << "create table failed: " << sqlite3_errmsg(m_db) << std::endl;
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

