#include "core/FileSystem.h"
#include "core/Log.h"
#include <SDL2/SDL_filesystem.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define DIR_SEPARATOR "\\"
#else
#include <dirent.h>
#include <unistd.h>
#define DIR_SEPARATOR "/"
#endif

namespace Omega {

std::string FileSystem::s_basePath = "";
std::string FileSystem::s_savePath = "";

void FileSystem::init(const std::string& basePath) {
    if (!basePath.empty()) {
        s_basePath = basePath;
    } else {
        char* path = SDL_GetBasePath();
        if (path) {
            s_basePath = path;
            SDL_free(path);
        }
    }
    
    // إعداد مسار الحفظ
    char* prefPath = SDL_GetPrefPath("OmegaEngine", "Game");
    if (prefPath) {
        s_savePath = prefPath;
        SDL_free(prefPath);
        createDirectory(s_savePath);
    }
    
    Log::info("FileSystem initialized. Base: " + s_basePath + ", Save: " + s_savePath);
}

void FileSystem::shutdown() {
    s_basePath.clear();
    s_savePath.clear();
}

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        Log::error("Failed to open file: " + path);
        return "";
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        Log::error("Failed to read file: " + path);
        return "";
    }
    
    return std::string(buffer.begin(), buffer.end());
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        Log::error("Failed to create file: " + path);
        return false;
    }
    
    file.write(content.c_str(), content.size());
    return file.good();
}

bool FileSystem::appendToFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        Log::error("Failed to open file for appending: " + path);
        return false;
    }
    
    file.write(content.c_str(), content.size());
    return file.good();
}

bool FileSystem::exists(const std::string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0;
}

bool FileSystem::isFile(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return false;
    return S_ISREG(info.st_mode);
}

bool FileSystem::isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return false;
    return S_ISDIR(info.st_mode);
}

bool FileSystem::createDirectory(const std::string& path) {
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0 || exists(path);
#else
    return mkdir(path.c_str(), 0755) == 0 || exists(path);
#endif
}

bool FileSystem::deleteFile(const std::string& path) {
#ifdef _WIN32
    return remove(path.c_str()) == 0;
#else
    return unlink(path.c_str()) == 0;
#endif
}

bool FileSystem::deleteDirectory(const std::string& path) {
#ifdef _WIN32
    return _rmdir(path.c_str()) == 0;
#else
    return rmdir(path.c_str()) == 0;
#endif
}

std::string FileSystem::getBasePath() {
    return s_basePath;
}

std::string FileSystem::getPrefPath(const std::string& org, const std::string& app) {
    char* path = SDL_GetPrefPath(org.c_str(), app.c_str());
    if (path) {
        std::string result = path;
        SDL_free(path);
        return result;
    }
    return "";
}

std::string FileSystem::getCurrentDirectory() {
    char path[1024];
#ifdef _WIN32
    if (_getcwd(path, sizeof(path))) {
        return std::string(path);
    }
#else
    if (getcwd(path, sizeof(path))) {
        return std::string(path);
    }
#endif
    return "";
}

std::vector<std::string> FileSystem::listFiles(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;
    
#ifdef _WIN32
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((directory + "/*").c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::string filename = findData.cFileName;
                if (extension.empty() || filename.size() >= extension.size() &&
                    filename.substr(filename.size() - extension.size()) == extension) {
                    files.push_back(directory + DIR_SEPARATOR + filename);
                }
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(directory.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) {
                std::string filename = entry->d_name;
                if (extension.empty() || filename.size() >= extension.size() &&
                    filename.substr(filename.size() - extension.size()) == extension) {
                    files.push_back(directory + DIR_SEPARATOR + filename);
                }
            }
        }
        closedir(dir);
    }
#endif
    
    return files;
}

std::vector<std::string> FileSystem::listDirectories(const std::string& directory) {
    std::vector<std::string> dirs;
    
#ifdef _WIN32
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((directory + "/*").c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                std::string dirname = findData.cFileName;
                if (dirname != "." && dirname != "..") {
                    dirs.push_back(directory + DIR_SEPARATOR + dirname);
                }
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(directory.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                std::string dirname = entry->d_name;
                if (dirname != "." && dirname != "..") {
                    dirs.push_back(directory + DIR_SEPARATOR + dirname);
                }
            }
        }
        closedir(dir);
    }
#endif
    
    return dirs;
}

std::string FileSystem::getAssetPath(const std::string& relativePath) {
    return s_basePath + "assets/" + relativePath;
}

std::string FileSystem::getSavePath(const std::string& filename) {
    return s_savePath + filename;
}

} // namespace Omega
