#include "core/Packager.h"
#include <iostream>
#include <string>

/**
 * @brief أداة سطر الأوامر لإنشاء حزم الألعاب (Omega Packager Tool)
 * 
 * الاستخدام:
 * omega-pack.exe <source_dir> <output_package.opk>
 * 
 * مثال:
 * omega-pack.exe ../assets/my_game ../build/game.opk
 */

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source_directory> <output_package.opk>" << std::endl;
        std::cerr << "Example: " << argv[0] << " ../assets ../build/game.opk" << std::endl;
        return 1;
    }

    std::string sourceDir = argv[1];
    std::string outputPath = argv[2];

    Omega::PackageSystem packager;

    std::cout << "========================================" << std::endl;
    std::cout << "   Omega Engine Packager Tool v1.0" << std::endl;
    std::cout << "========================================" << std::endl;

    if (packager.createPackage(sourceDir, outputPath)) {
        std::cout << "\n[SUCCESS] Package created successfully!" << std::endl;
        std::cout << "Output: " << outputPath << std::endl;
        
        // اختبار سريع للتحميل
        std::cout << "\n[TEST] Verifying package integrity..." << std::endl;
        Omega::PackageSystem verifier;
        if (verifier.loadPackage(outputPath)) {
            std::cout << "[TEST] Package verification passed." << std::endl;
        } else {
            std::cerr << "[WARNING] Package verification failed (file might be locked or corrupted)." << std::endl;
        }

        return 0;
    } else {
        std::cerr << "\n[ERROR] Failed to create package." << std::endl;
        return 1;
    }
}
