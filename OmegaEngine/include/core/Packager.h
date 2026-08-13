#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <cstring>

namespace Omega {

    /**
     * @brief هيكل يمثل ملفاً داخل الحزمة
     */
    struct PackedFile {
        std::string path;          // المسار النسبي داخل الحزمة
        std::string virtualPath;   // المسار الافتراضي للوصول
        size_t size;               // حجم البيانات
        size_t offset;             // الإزاحة داخل ملف الحزمة
        std::vector<uint8_t> data; // البيانات (للتحميل في الذاكرة إذا لزم الأمر)
        
        // تشفير بسيط (XOR) للحماية الأساسية
        void encrypt(uint8_t key) {
            for (auto& byte : data) {
                byte ^= key;
            }
        }
    };

    /**
     * @brief نظام إدارة حزم الألعاب (Omega Packager)
     * يستخدم std::filesystem لاجتياز المجلدات وتجميع الملفات
     */
    class PackageSystem {
    public:
        static constexpr uint32_t MAGIC_NUMBER = 0x4F504B00; // "OPK\0"
        static constexpr uint8_t ENCRYPTION_KEY = 0xA7;      // مفتاح تشفير بسيط

        PackageSystem() = default;
        ~PackageSystem() = default;

        /**
         * @brief إنشاء حزمة جديدة من مجلد أصول
         * @param sourceDir المجلد المصدر (مثل assets/)
         * @param outputPath مسار ملف الحزمة الناتج (مثل game.opk)
         * @return true إذا نجح الإنشاء
         */
        bool createPackage(const std::string& sourceDir, const std::string& outputPath) {
            std::cout << "[Packager] Starting package creation from: " << sourceDir << std::endl;
            
            std::vector<PackedFile> files;
            namespace fs = std::filesystem;

            if (!fs::exists(sourceDir)) {
                std::cerr << "[Packager] Error: Source directory does not exist!" << std::endl;
                return false;
            }

            // اجتياز المجلد بشكل متكرر
            for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
                if (entry.is_regular_file()) {
                    PackedFile pFile;
                    
                    // الحصول على المسار النسبي
                    fs::path relPath = fs::relative(entry.path(), sourceDir);
                    pFile.path = relPath.string();
                    pFile.virtualPath = "/" + pFile.path; // مسار افتراضي يبدأ بـ /
                    
                    // قراءة الملف
                    std::ifstream inFile(entry.path(), std::ios::binary | std::ios::ate);
                    if (!inFile) {
                        std::cerr << "[Packager] Failed to open: " << entry.path() << std::endl;
                        continue;
                    }

                    pFile.size = inFile.tellg();
                    inFile.seekg(0, std::ios::beg);
                    
                    pFile.data.resize(pFile.size);
                    if (!inFile.read(reinterpret_cast<char*>(pFile.data.data()), pFile.size)) {
                        std::cerr << "[Packager] Failed to read: " << entry.path() << std::endl;
                        continue;
                    }
                    inFile.close();

                    // تشفير البيانات
                    pFile.encrypt(ENCRYPTION_KEY);

                    files.push_back(std::move(pFile));
                    std::cout << "  + Added: " << pFile.path << " (" << pFile.size << " bytes)" << std::endl;
                }
            }

            if (files.empty()) {
                std::cerr << "[Packager] Warning: No files found to pack." << std::endl;
                // نستمر لإنشاء ملف فارغ أو نرجع خطأ حسب الرغبة
            }

            // كتابة ملف الحزمة
            return writePackageFile(outputPath, files);
        }

        /**
         * @brief تحميل حزمة موجودة للقراءة منها
         * @param packagePath مسار ملف الحزمة
         * @return true إذا نجح التحميل
         */
        bool loadPackage(const std::string& packagePath) {
            std::ifstream file(packagePath, std::ios::binary);
            if (!file) {
                std::cerr << "[Packager] Failed to open package: " << packagePath << std::endl;
                return false;
            }

            // قراءة الرأس (Header)
            uint32_t magic;
            file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
            if (magic != MAGIC_NUMBER) {
                std::cerr << "[Packager] Invalid package format (Bad Magic Number)." << std::endl;
                return false;
            }

            uint32_t fileCount;
            file.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));

            m_files.clear();
            m_files.reserve(fileCount);

            for (uint32_t i = 0; i < fileCount; ++i) {
                PackedFile pFile;
                
                // قراءة طول المسار ثم المسار نفسه
                uint32_t pathLen;
                file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
                
                std::vector<char> pathBuffer(pathLen);
                file.read(pathBuffer.data(), pathLen);
                pFile.virtualPath = std::string(pathBuffer.begin(), pathBuffer.end());
                
                file.read(reinterpret_cast<char*>(&pFile.size), sizeof(pFile.size));
                file.read(reinterpret_cast<char*>(&pFile.offset), sizeof(pFile.offset));

                // نخزن الموقع الحالي للعودة إليه لاحقاً عند القراءة الفعلية للبيانات
                // لا نقرأ البيانات كلها في الذاكرة الآن لتوفير الرام (Lazy Loading)
                m_fileIndex[pFile.virtualPath] = &m_files.emplace_back(std::move(pFile));
            }

            m_packageStream = std::move(file);
            m_isLoaded = true;
            
            std::cout << "[Packager] Package loaded successfully: " << packagePath 
                      << " (" << fileCount << " files)" << std::endl;
            return true;
        }

        /**
         * @brief قراءة ملف من الحزمة المحملة
         * @param virtualPath المسار الافتراضي للملف (مثال: /textures/player.png)
         * @return vector يحتوي البيانات، أو فارغ إذا فشل
         */
        std::vector<uint8_t> readFile(const std::string& virtualPath) {
            if (!m_isLoaded) {
                std::cerr << "[Packager] No package loaded." << std::endl;
                return {};
            }

            auto it = m_fileIndex.find(virtualPath);
            if (it == m_fileIndex.end()) {
                // محاولة مع مسار بدون slash في البداية
                std::string altPath = (virtualPath[0] == '/') ? virtualPath.substr(1) : "/" + virtualPath;
                it = m_fileIndex.find(altPath);
                if (it == m_fileIndex.end()) {
                    std::cerr << "[Packager] File not found in package: " << virtualPath << std::endl;
                    return {};
                }
            }

            const PackedFile* pFile = it->second;
            
            // قراءة البيانات من الإزاحة المحددة
            std::vector<uint8_t> buffer(pFile->size);
            
            // حفظ الموقع الحالي
            std::streampos currentPos = m_packageStream.tellg();
            
            m_packageStream.seekg(pFile->offset, std::ios::beg);
            m_packageStream.read(reinterpret_cast<char*>(buffer.data()), pFile->size);
            
            // استعادة الموقع
            m_packageStream.seekg(currentPos, std::ios::beg);

            if (!m_packageStream) {
                std::cerr << "[Packager] Read error for file: " << virtualPath << std::endl;
                return {};
            }

            // فك التشفير
            // نعمل على نسخة لأننا لا نريد تعديل البيانات الأصلية في الذاكرة إذا كانت مشتركة
            // لكن هنا نقرأ في buffer محلي لذا آمن
            uint8_t key = ENCRYPTION_KEY;
            for (auto& byte : buffer) {
                byte ^= key;
            }

            return buffer;
        }

        bool isLoaded() const { return m_isLoaded; }

    private:
        bool writePackageFile(const std::string& path, const std::vector<PackedFile>& files) {
            std::ofstream out(path, std::ios::binary);
            if (!out) {
                std::cerr << "[Packager] Cannot create output file: " << path << std::endl;
                return false;
            }

            // كتابة الرأس
            uint32_t magic = MAGIC_NUMBER;
            out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
            
            uint32_t count = static_cast<uint32_t>(files.size());
            out.write(reinterpret_cast<const char*>(&count), sizeof(count));

            // حساب إزاحة البيانات (بعد جدول الملفات)
            // حجم الجدول التقريبي: 4 (len) + path_len + 8 (size+offset) لكل ملف
            size_t currentOffset = sizeof(magic) + sizeof(count);
            
            // أولاً نحسب حجم الجدول لنحدد أين تبدأ البيانات
            // سنكتب الجدول أولاً، ثم البيانات
            // لكننا نحتاج لمعرفة الإزاحة قبل الكتابة، لذا سنكتب الجدول مؤقتاً أو نحسبه
            
            std::vector<size_t> offsets;
            offsets.reserve(files.size());

            // حساب أحجام المسارات لتحديد الإزاحة بدقة
            size_t tableSize = 0;
            for (const auto& f : files) {
                uint32_t pathLen = static_cast<uint32_t>(f.virtualPath.size());
                tableSize += sizeof(uint32_t) + pathLen + sizeof(size_t) + sizeof(size_t);
            }
            
            currentOffset += tableSize;

            // كتابة جدول الملفات
            for (const auto& f : files) {
                uint32_t pathLen = static_cast<uint32_t>(f.virtualPath.size());
                out.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
                out.write(f.virtualPath.c_str(), pathLen);
                out.write(reinterpret_cast<const char*>(&f.size), sizeof(f.size));
                out.write(reinterpret_cast<const char*>(&currentOffset), sizeof(currentOffset)); // كتابة الإزاحة الحالية

                offsets.push_back(currentOffset);
                currentOffset += f.size;
            }

            // كتابة البيانات
            for (size_t i = 0; i < files.size(); ++i) {
                out.write(reinterpret_cast<const char*>(files[i].data.data()), files[i].size);
            }

            out.close();
            std::cout << "[Packager] Package created successfully: " << path 
                      << " (Total size: " << currentOffset << " bytes)" << std::endl;
            return true;
        }

        std::vector<PackedFile> m_files;
        std::unordered_map<std::string, PackedFile*> m_fileIndex;
        std::ifstream m_packageStream;
        bool m_isLoaded = false;
    };

} // namespace Omega
