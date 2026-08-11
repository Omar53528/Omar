#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Omega {

class FileSystem {
public:
    static void init(const std::string& basePath = "");
    static void shutdown();
    
    // قراءة وكتابة الملفات
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool appendToFile(const std::string& path, const std::string& content);
    
    // التحقق من وجود الملفات والمجلدات
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    
    // إنشاء وحذف المجلدات
    static bool createDirectory(const std::string& path);
    static bool deleteFile(const std::string& path);
    static bool deleteDirectory(const std::string& path);
    
    // الحصول على المعلومات
    static std::string getBasePath();
    static std::string getPrefPath(const std::string& org, const std::string& app);
    static std::string getCurrentDirectory();
    
    // البحث عن الملفات
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& extension = "");
    static std::vector<std::string> listDirectories(const std::string& directory);
    
    // مسارات مخصصة للأصول
    static std::string getAssetPath(const std::string& relativePath);
    static std::string getSavePath(const std::string& filename);
    
private:
    static std::string s_basePath;
    static std::string s_savePath;
};

} // namespace Omega
