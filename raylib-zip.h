/**********************************************************************************************
*
*   raylib-zip - Zip archive support for raylib using kuba--/zip.
*
*   Copyright 2026 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       raylib https://www.raylib.com
*       zip    https://github.com/kuba--/zip
*
*   LICENSE: zlib/libpng
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RAYLIB_ZIP_H_
#define RAYLIB_ZIP_H_

#ifndef RAYLIB_ZIP_API
    #ifdef __declspec
        #define RAYLIB_ZIP_API __declspec(dllexport)
    #else
        #define RAYLIB_ZIP_API extern
    #endif
#endif

#ifndef RAYLIB_ZIP_RAYLIB_H
    #define RAYLIB_ZIP_RAYLIB_H "raylib.h"
#endif
#include RAYLIB_ZIP_RAYLIB_H

#include <stdbool.h>

typedef struct Zip {
    void* zip;  // struct zip_t*
} Zip;

#ifdef __cplusplus
extern "C" {
#endif

RAYLIB_ZIP_API Zip LoadZip(const char* zipFile);
RAYLIB_ZIP_API Zip LoadZipFromMemory(const unsigned char* data, int dataSize);
RAYLIB_ZIP_API void UnloadZip(Zip zip);
RAYLIB_ZIP_API bool IsZipReady(Zip zip);
RAYLIB_ZIP_API bool FileExistsInZip(Zip zip, const char* fileName);
RAYLIB_ZIP_API int GetZipEntryCount(Zip zip);

RAYLIB_ZIP_API unsigned char* LoadFileDataFromZip(Zip zip, const char* fileName, int* dataSize);
RAYLIB_ZIP_API void UnloadFileDataFromZip(unsigned char* data);
RAYLIB_ZIP_API char* LoadFileTextFromZip(Zip zip, const char* fileName);
RAYLIB_ZIP_API void UnloadFileTextFromZip(char* text);

RAYLIB_ZIP_API FilePathList LoadDirectoryFilesFromZip(Zip zip, const char* dirPath);
RAYLIB_ZIP_API FilePathList LoadDirectoryFilesFromZipEx(Zip zip, const char* basePath, const char* filter, bool scanSubdirs);

RAYLIB_ZIP_API Image LoadImageFromZip(Zip zip, const char* fileName);
RAYLIB_ZIP_API Texture2D LoadTextureFromZip(Zip zip, const char* fileName);
RAYLIB_ZIP_API Wave LoadWaveFromZip(Zip zip, const char* fileName);
RAYLIB_ZIP_API Music LoadMusicStreamFromZip(Zip zip, const char* fileName);
RAYLIB_ZIP_API Font LoadFontFromZip(Zip zip, const char* fileName, int fontSize, int* codepoints, int codepointCount);
RAYLIB_ZIP_API Shader LoadShaderFromZip(Zip zip, const char* vsFileName, const char* fsFileName);

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_ZIP_H_

#ifdef RAYLIB_ZIP_IMPLEMENTATION
#ifndef RAYLIB_ZIP_IMPLEMENTATION_ONCE
#define RAYLIB_ZIP_IMPLEMENTATION_ONCE

#ifndef RAYLIB_ZIP_ZIP_H
    #define RAYLIB_ZIP_ZIP_H "zip.h"
#endif
#include RAYLIB_ZIP_ZIP_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

RAYLIB_ZIP_API Zip LoadZip(const char* zipFile) {
    Zip zip = {0};
    zip.zip = zip_open(zipFile, 0, 'r');
    if (zip.zip == NULL) {
        TraceLog(LOG_WARNING, "ZIP: Failed to open %s", zipFile);
    }
    return zip;
}

RAYLIB_ZIP_API Zip LoadZipFromMemory(const unsigned char* data, int dataSize) {
    Zip zip = {0};
    zip.zip = zip_stream_open((const char*)data, (size_t)dataSize, 0, 'r');
    if (zip.zip == NULL) {
        TraceLog(LOG_WARNING, "ZIP: Failed to open zip from memory");
    }
    return zip;
}

RAYLIB_ZIP_API void UnloadZip(Zip zip) {
    if (zip.zip != NULL) {
        zip_close((struct zip_t*)zip.zip);
    }
}

RAYLIB_ZIP_API bool IsZipReady(Zip zip) {
    return zip.zip != NULL;
}

RAYLIB_ZIP_API int GetZipEntryCount(Zip zip) {
    if (zip.zip == NULL) {
        return 0;
    }
    return (int)zip_entries_total((struct zip_t*)zip.zip);
}

RAYLIB_ZIP_API bool FileExistsInZip(Zip zip, const char* fileName) {
    if (zip.zip == NULL) {
        return false;
    }
    struct zip_t* z = (struct zip_t*)zip.zip;
    if (zip_entry_open(z, fileName) < 0) {
        return false;
    }
    zip_entry_close(z);
    return true;
}

RAYLIB_ZIP_API unsigned char* LoadFileDataFromZip(Zip zip, const char* fileName, int* dataSize) {
    *dataSize = 0;

    if (zip.zip == NULL) {
        TraceLog(LOG_WARNING, "ZIP: Not ready, call InitZip() first");
        return NULL;
    }

    struct zip_t* z = (struct zip_t*)zip.zip;

    if (zip_entry_open(z, fileName) < 0) {
        TraceLog(LOG_WARNING, "ZIP: Failed to open entry: %s", fileName);
        return NULL;
    }

    size_t size = (size_t)zip_entry_uncomp_size(z);
    if (size == 0) {
        zip_entry_close(z);
        return NULL;
    }

    unsigned char* data = (unsigned char*)RL_MALLOC(size);
    if (data == NULL) {
        zip_entry_close(z);
        return NULL;
    }

    ssize_t bytesRead = zip_entry_noallocread(z, data, size);
    zip_entry_close(z);

    if (bytesRead < 0) {
        RL_FREE(data);
        TraceLog(LOG_WARNING, "ZIP: Failed to read entry: %s", fileName);
        return NULL;
    }

    *dataSize = (int)size;
    return data;
}

RAYLIB_ZIP_API void UnloadFileDataFromZip(unsigned char* data) {
    RL_FREE(data);
}

RAYLIB_ZIP_API char* LoadFileTextFromZip(Zip zip, const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return NULL;
    }

    char* text = (char*)RL_MALLOC(dataSize + 1);
    if (text == NULL) {
        RL_FREE(data);
        return NULL;
    }

    memcpy(text, data, dataSize);
    text[dataSize] = '\0';
    RL_FREE(data);

    return text;
}

RAYLIB_ZIP_API void UnloadFileTextFromZip(char* text) {
    RL_FREE(text);
}

RAYLIB_ZIP_API FilePathList LoadDirectoryFilesFromZipEx(Zip zip, const char* basePath, const char* filter, bool scanSubdirs) {
    FilePathList result = {0};

    if (zip.zip == NULL) {
        return result;
    }

    struct zip_t* z = (struct zip_t*)zip.zip;
    int total = (int)zip_entries_total(z);
    size_t baseLen = (basePath != NULL) ? strlen(basePath) : 0;

    // First pass: count matching entries
    int count = 0;
    for (int i = 0; i < total; i++) {
        if (zip_entry_openbyindex(z, i) < 0) continue;
        const char* name = zip_entry_name(z);
        bool isDir = zip_entry_isdir(z);

        bool matches = !isDir && name != NULL;
        if (matches && baseLen > 0 && strncmp(name, basePath, baseLen) != 0) matches = false;
        if (matches && !scanSubdirs && strchr(name + baseLen, '/') != NULL) matches = false;
        if (matches && filter != NULL && !IsFileExtension(name, filter)) matches = false;

        zip_entry_close(z);
        if (matches) count++;
    }

    if (count == 0) return result;

    result.paths = (char**)RL_MALLOC(count * sizeof(char*));
    if (result.paths == NULL) return result;

    // Second pass: fill entries
    for (int i = 0; i < total; i++) {
        if (zip_entry_openbyindex(z, i) < 0) continue;
        const char* name = zip_entry_name(z);
        bool isDir = zip_entry_isdir(z);

        bool matches = !isDir && name != NULL;
        if (matches && baseLen > 0 && strncmp(name, basePath, baseLen) != 0) matches = false;
        if (matches && !scanSubdirs && strchr(name + baseLen, '/') != NULL) matches = false;
        if (matches && filter != NULL && !IsFileExtension(name, filter)) matches = false;

        if (matches) {
            size_t len = strlen(name);
            result.paths[result.count] = (char*)RL_MALLOC(len + 1);
            if (result.paths[result.count] != NULL) {
                memcpy(result.paths[result.count], name, len + 1);
                result.count++;
            }
        }

        zip_entry_close(z);
    }

    return result;
}

RAYLIB_ZIP_API FilePathList LoadDirectoryFilesFromZip(Zip zip, const char* dirPath) {
    return LoadDirectoryFilesFromZipEx(zip, dirPath, NULL, false);
}

RAYLIB_ZIP_API Image LoadImageFromZip(Zip zip, const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return (Image){0};
    }

    Image image = LoadImageFromMemory(GetFileExtension(fileName), data, dataSize);
    RL_FREE(data);
    return image;
}

RAYLIB_ZIP_API Texture2D LoadTextureFromZip(Zip zip, const char* fileName) {
    Image image = LoadImageFromZip(zip, fileName);
    if (image.data == NULL) {
        return (Texture2D){0};
    }

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

RAYLIB_ZIP_API Wave LoadWaveFromZip(Zip zip, const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return (Wave){0};
    }

    Wave wave = LoadWaveFromMemory(GetFileExtension(fileName), data, dataSize);
    RL_FREE(data);
    return wave;
}

RAYLIB_ZIP_API Music LoadMusicStreamFromZip(Zip zip, const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return (Music){0};
    }

    Music music = LoadMusicStreamFromMemory(GetFileExtension(fileName), data, dataSize);
    RL_FREE(data);
    return music;
}

RAYLIB_ZIP_API Font LoadFontFromZip(Zip zip, const char* fileName, int fontSize, int* codepoints, int codepointCount) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return GetFontDefault();
    }

    Font font = LoadFontFromMemory(GetFileExtension(fileName), data, dataSize, fontSize, codepoints, codepointCount);
    RL_FREE(data);
    return font;
}

RAYLIB_ZIP_API Shader LoadShaderFromZip(Zip zip, const char* vsFileName, const char* fsFileName) {
    char* vsCode = NULL;
    char* fsCode = NULL;

    if (vsFileName != NULL) {
        vsCode = LoadFileTextFromZip(zip, vsFileName);
    }
    if (fsFileName != NULL) {
        fsCode = LoadFileTextFromZip(zip, fsFileName);
    }

    Shader shader = LoadShaderFromMemory(vsCode, fsCode);

    if (vsCode != NULL) {
        RL_FREE(vsCode);
    }
    if (fsCode != NULL) {
        RL_FREE(fsCode);
    }

    return shader;
}

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_ZIP_IMPLEMENTATION_ONCE
#endif  // RAYLIB_ZIP_IMPLEMENTATION
