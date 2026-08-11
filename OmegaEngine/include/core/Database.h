#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sqlite3.h>

namespace Omega {

class Database {
public:
    Database();
    ~Database();
    
    bool open(const std::string& filename);
    void close();
    bool isOpen() const;
    
    // تنفيذ الاستعلامات بدون نتائج
    bool execute(const std::string& query);
    
    // استعلام مع تحديد المعاملات
    bool executeWithParams(const std::string& query, const std::vector<std::string>& params);
    
    // استعلام يعيد نتائج
    struct Row {
        std::unordered_map<std::string, std::string> columns;
    };
    std::vector<Row> query(const std::string& sql);
    
    // دوال مساعدة للجدول
    bool createTable(const std::string& name, const std::vector<std::string>& columns);
    bool insert(const std::string& table, const std::unordered_map<std::string, std::string>& values);
    bool update(const std::string& table, const std::unordered_map<std::string, std::string>& values, const std::string& where);
    bool remove(const std::string& table, const std::string& where);
    std::vector<Row> select(const std::string& table, const std::string& where = "");
    
    // معاملات مسبقة الصنع
    void bindInt(int index, int value);
    void bindFloat(int index, float value);
    void bindString(int index, const std::string& value);
    
private:
    sqlite3* m_db = nullptr;
    std::string prepareStatement(const std::string& sql, int& paramCount);
};

} // namespace Omega
