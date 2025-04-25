#ifndef _USER_SQLITE_H
#define _USER_SQLITE_H

#include <sqlite3.h>
#include <iostream>

class UserSQLite{
public:
    UserSQLite();
    ~UserSQLite();
    //获取全部数据数量
    int get_row_count();
    //打开数据库
    void open_sqlite();
    //关闭数据库
    void close_sqlite();
    //清理资源
    void clean_resource();
    //获取名字
    std::string get_name_by_id(int id);
    //插入数据
    void insert_data(int label, std::string name);
    //实例化
    static UserSQLite* Instance();
private:
    sqlite3* m_db;
    sqlite3_stmt* m_stmt;
    char* m_errmsg;
    bool m_table_flag;
};

#endif
