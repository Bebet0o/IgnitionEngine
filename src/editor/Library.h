#pragma once
#include <string>
#include <vector>

struct LibraryFile
{
    std::string name;
    std::string path;
    bool isDirectory;
};

class LibraryPanel
{
public:
    LibraryPanel(const std::string& assetsDir = "assets/");
    void Render();
private:
    void Refresh();
    std::string m_assetsDir;
    std::vector<LibraryFile> m_files;
    int m_selected = -1;
};
