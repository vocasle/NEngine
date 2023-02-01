#pragma once

namespace NEngine::Helpers {
class FileWriter
{
public:
    explicit FileWriter(const char *file_name);
    bool Save() const;
};
}  // namespace NEngine::Helpers