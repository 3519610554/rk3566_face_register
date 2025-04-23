#ifndef _BACKEND_SQLITE_H
#define _BACKEND_SQLITE_H

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>

struct Backend_Info{
    int id;
    std::string time;
    std::string base64;
};

class BackendSQLite{
public:
    //实例化
    static BackendSQLite* Instance();
    //插入数据
    int insert_data(std::string time, std::string base64);
    //删除数据
    void delete_by_id(int id);
    //获取数据长度
    int get_row_count();
    //获取全部数据
    void get_all_data(std::vector<Backend_Info> &data);
    //获取全部id
    void get_all_id(std::vector<int> id_data);
protected:
    //打开数据库
    void open_sqlite();
    //关闭数据库
    void close_sqlite();
    //清理资源
    void clean_resource();
private:
    sqlite3* m_db;
    sqlite3_stmt* m_stmt;
    char* m_errmsg;
    bool m_table_flag;
};

#endif
