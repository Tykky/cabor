#include "filesystem.h"
#include "../logging/logging.h"
#include "../debug/cabor_debug.h"

#include <stdio.h>
#include <stdlib.h>

cabor_file cabor_load_file(const char* filename)
{
    FILE* file_handle = fopen(filename, "rb");
    if (file_handle == NULL)
    {
        CABOR_LOG_ERR_F("Failed to open %s", filename);
        CABOR_RUNTIME_ERROR("failed to open file");
    }

    fseek(file_handle, 0, SEEK_END);
    size_t file_size = ftell(file_handle);
    rewind(file_handle);

    cabor_file file =
    {
        .file_memory = CABOR_MALLOC(file_size * sizeof(char))
    };

    size_t result = fread(file.file_memory.mem, sizeof(char), file_size, file_handle);

    if (result != file_size)
    {
        CABOR_LOG_ERR_F("Read %d bytes when actual file size was %d bytes", result, file_size);
        CABOR_RUNTIME_ERROR("Failed to read file!");
    }

    return file;
}

void cabor_dump_file_to_disk(cabor_file* file, const char* filename)
{
    FILE* fp = fopen(filename, "w");
    size_t result = fwrite(file->file_memory.mem, sizeof(char), file->file_memory.size, fp);
    CABOR_ASSERT(result == file->file_memory.size, "Failed to write all bytes to a file");
    fclose(fp);
}

void cabor_destroy_file(cabor_file* file)
{
    CABOR_FREE(&file->file_memory);
    file->file_memory.mem = NULL;
    file->file_memory.size = 0;
}

char cabor_read_byte_from_file(cabor_file* file, size_t idx)
{
    CABOR_ASSERT(idx < file->file_memory.size, "File out of bounds!");
    return ((char*)file->file_memory.mem)[idx];
}
