#pragma once

#include <fstream>

namespace NEngine::Helpers {
class FileWriter
{
public:
    explicit FileWriter(const char *file_name);

    ~FileWriter()
    {
        Save();
    }

    template <typename T>
    void Serialize(const T &val);
    bool Save();

private:
    std::ofstream mFile;
};
}  // namespace NEngine::Helpers