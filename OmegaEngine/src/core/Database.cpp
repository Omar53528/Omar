#include "core/Database.h"
#include "core/Log.h"
#include <sstream>

namespace Omega {

Database::Database() = default;

Database::~Database() {
    close();
}

bool Database::open(const std::string& filename) {
    if (m_db) {
        Log::warn("Database already open");
        return false;
    }
    
    int rc = sqlite3_open(filename.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        Log::error("Failed to open database: " + std::string(sqlite3_errmsg(m_db)));
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }
    
    Log::info("Database opened: " + filename);
    return true;
}

void Database::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
        Log::info("Database closed");
    }
}

bool Database::isOpen() const {
    return m_db != nullptr;
}

bool Database::execute(const std::string& query) {
    if (!m_db) {
        Log::error("Database not open");
        return false;
    }
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, query.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        Log::error("SQL error: " + std::string(errMsg));
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool Database::executeWithParams(const std::string& query, const std::vector<std::string>& params) {
    if (!m_db) return false;
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        Log::error("Failed to prepare statement: " + std::string(sqlite3_errmsg(m_db)));
        return false;
    }
    
    for (size_t i = 0; i < params.size(); i++) {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_STATIC);
    }
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    
    return success;
}

std::vector<Database::Row> Database::query(const std::string& sql) {
    std::vector<Row> results;
    
    if (!m_db) return results;
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        Log::error("Query failed: " + std::string(sqlite3_errmsg(m_db)));
        return results;
    }
    
    int colCount = sqlite3_column_count(stmt);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Row row;
        for (int i = 0; i < colCount; i++) {
            const char* colName = sqlite3_column_name(stmt, i);
            const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.columns[colName] = value ? value : "";
        }
        results.push_back(row);
    }
    
    sqlite3_finalize(stmt);
    return results;
}

bool Database::createTable(const std::string& name, const std::vector<std::string>& columns) {
    std::stringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS " << name << " (";
    
    for (size_t i = 0; i < columns.size(); i++) {
        ss << columns[i];
        if (i < columns.size() - 1) ss << ", ";
    }
    
    ss << ")";
    
    return execute(ss.str());
}

bool Database::insert(const std::string& table, const std::unordered_map<std::string, std::string>& values) {
    std::stringstream cols, vals;
    cols << "(";
    vals << "(";
    
    bool first = true;
    for (const auto& [key, value] : values) {
        if (!first) {
            cols << ", ";
            vals << ", ";
        }
        cols << key;
        vals << "'" << value << "'";
        first = false;
    }
    
    cols << ")";
    vals << ")";
    
    std::string sql = "INSERT INTO " + table + " " + cols.str() + " VALUES " + vals.str();
    return execute(sql);
}

bool Database::update(const std::string& table, const std::unordered_map<std::string, std::string>& values, const std::string& where) {
    std::stringstream ss;
    ss << "UPDATE " << table << " SET ";
    
    bool first = true;
    for (const auto& [key, value] : values) {
        if (!first) ss << ", ";
        ss << key << "='" << value << "'";
        first = false;
    }
    
    if (!where.empty()) {
        ss << " WHERE " << where;
    }
    
    return execute(ss.str());
}

bool Database::remove(const std::string& table, const std::string& where) {
    std::string sql = "DELETE FROM " + table;
    if (!where.empty()) {
        sql += " WHERE " + where;
    }
    return execute(sql);
}

std::vector<Database::Row> Database::select(const std::string& table, const std::string& where) {
    std::string sql = "SELECT * FROM " + table;
    if (!where.empty()) {
        sql += " WHERE " + where;
    }
    return query(sql);
}

void Database::bindInt(int index, int value) {
    // للتطبيق المستقبلي مع prepared statements
}

void Database::bindFloat(int index, float value) {
    // للتطبيق المستقبلي مع prepared statements
}

void Database::bindString(int index, const std::string& value) {
    // للتطبيق المستقبلي مع prepared statements
}

} // namespace Omega
