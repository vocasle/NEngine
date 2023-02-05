
#include "NEngine/Helpers/FileWriter.h"

namespace NEngine::Helpers {

FileWriter::FileWriter(const char *file_name)
    : mFile(file_name, std::ios_base::binary)
{
}

bool
FileWriter::Save()
{
    return mFile.flush() ? true : false;
}
}  // namespace NEngine::Helpers
