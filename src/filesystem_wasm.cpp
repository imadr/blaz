#include "filesystem.h"

namespace blaz {

pair<Error, str> read_whole_file(str path) {
    FILE* file;
    u64 file_size;
    char* buffer;

    file = fopen(path.c_str(), "r");
    if (file == NULL) {
        return std::make_pair(Error("Failed to open file '" + path + "'"), "");
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    buffer = (char*)malloc((file_size + 1) * sizeof(char));
    if (buffer == NULL) {
        fclose(file);
        return std::make_pair(Error("Failed to allocate memory '" + path + "'"), "");
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);
    std::string file_content = buffer;
    free(buffer);

    return std::make_pair(Error(), file_content);
}

pair<Error, vec<u8>> read_whole_file_binary(str path) {
    FILE* file;
    u64 file_size;
    char* buffer;

    vec<u8> file_content;

    file = fopen(path.c_str(), "rb");
    if (file == NULL) {
        return std::make_pair(Error("Failed to open file '" + path + "'"), file_content);
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    buffer = (char*)malloc((file_size + 1) * sizeof(char));
    if (buffer == NULL) {
        fclose(file);
        return std::make_pair(Error("Failed to allocate memory '" + path + "'"), file_content);
    }

    fread(buffer, 1, file_size, file);
    file_content.assign(buffer, buffer + file_size);
    free(buffer);

    fclose(file);

    return std::make_pair(Error(), file_content);
}

}  // namespace blaz