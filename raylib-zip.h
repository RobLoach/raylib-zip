/**********************************************************************************************
*
*   raylib-zip - Zip archive support for raylib using kuba--/zip.
*
*   Copyright 2026 Rob Loach (@RobLoach)
*
*   VERSION: 0.1.1
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

/** A zip archive handle. */
typedef struct Zip {
    void* zip;   /**< Internal zip_t pointer from kuba--/zip. */
    void* data;  /**< Owned memory buffer, or NULL if externally managed. */
} Zip;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Open a zip archive from a file. Check the result with IsZipValid().
 *
 * @param zipFile Path to the zip file to open.
 * @return The loaded Zip, or empty on failure.
 *
 * @see IsZipValid()
 */
RAYLIB_ZIP_API Zip LoadZip(const char* zipFile);

/**
 * Open a zip archive from a memory buffer. Check the result with IsZipValid().
 *
 * @param data Pointer to the zip data in memory.
 * @param dataSize Size of the data in bytes.
 * @return The loaded Zip, or empty on failure.
 *
 * @see IsZipValid()
 */
RAYLIB_ZIP_API Zip LoadZipFromMemory(const unsigned char* data, int dataSize);

/**
 * Close and unload a zip archive.
 */
RAYLIB_ZIP_API void UnloadZip(Zip zip);

/**
 * Check whether a zip archive is open and ready to use.
 */
RAYLIB_ZIP_API bool IsZipValid(Zip zip);

/**
 * Check whether a file exists within the zip archive.
 *
 * @param fileName Path of the entry within the zip.
 */
RAYLIB_ZIP_API bool FileExistsInZip(Zip zip, const char* fileName);

/**
 * Get the total number of entries in the zip archive.
 */
RAYLIB_ZIP_API int GetZipEntryCount(Zip zip);

/**
 * Load raw file data from a zip entry. Free with UnloadFileData().
 *
 * @param fileName Path of the entry within the zip.
 * @param dataSize Optional output byte count of the loaded data. Use NULL if size is not required.
 */
RAYLIB_ZIP_API unsigned char* LoadFileDataFromZip(Zip zip, const char* fileName, int* dataSize);

/**
 * Load a text file from a zip entry as a null-terminated string. Free with UnloadFileTextFromZip().
 *
 * @param fileName Path of the entry within the zip.
 */
RAYLIB_ZIP_API char* LoadFileTextFromZip(Zip zip, const char* fileName);

/**
 * Load all file paths directly under dirPath (non-recursive). Free with UnloadDirectoryFiles().
 *
 * @param dirPath Directory prefix to filter entries, or NULL for root entries.
 */
RAYLIB_ZIP_API FilePathList LoadDirectoryFilesFromZip(Zip zip, const char* dirPath);

/**
 * Load file paths with extension filtering and optional recursive scan. Free with UnloadDirectoryFiles().
 *
 * @param basePath Directory prefix to filter entries, or NULL for all entries.
 * @param filter Semicolon-separated extension filter (e.g. ".png;.jpg"), or NULL for all files.
 * @param scanSubdirs When true, includes entries in subdirectories.
 */
RAYLIB_ZIP_API FilePathList LoadDirectoryFilesFromZipEx(Zip zip, const char* basePath, const char* filter, bool scanSubdirs);

/**
 * Load an Image from a zip entry.
 */
RAYLIB_ZIP_API Image LoadImageFromZip(Zip zip, const char* fileName);

/**
 * Load a Texture2D from a zip entry. Requires an open window.
 */
RAYLIB_ZIP_API Texture2D LoadTextureFromZip(Zip zip, const char* fileName);

/**
 * Load a Wave from a zip entry.
 */
RAYLIB_ZIP_API Wave LoadWaveFromZip(Zip zip, const char* fileName);

/**
 * Load a Music stream from a zip entry.
 */
RAYLIB_ZIP_API Music LoadMusicStreamFromZip(Zip zip, const char* fileName);

/**
 * Load a Font from a zip entry.
 *
 * @param fontSize Desired font size in pixels.
 * @param codepoints Array of codepoints to load, or NULL for default.
 * @param codepointCount Number of codepoints in the array.
 */
RAYLIB_ZIP_API Font LoadFontFromZip(Zip zip, const char* fileName, int fontSize, int* codepoints, int codepointCount);

/**
 * Load a Shader from zip entries.
 *
 * @param vsFileName Vertex shader entry path, or NULL to use the default.
 * @param fsFileName Fragment shader entry path, or NULL to use the default.
 */
RAYLIB_ZIP_API Shader LoadShaderFromZip(Zip zip, const char* vsFileName, const char* fsFileName);

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_ZIP_H_

#ifdef RAYLIB_ZIP_IMPLEMENTATION
#ifndef RAYLIB_ZIP_IMPLEMENTATION_ONCE
#define RAYLIB_ZIP_IMPLEMENTATION_ONCE

#include <stdlib.h>

#define MINIZ_NO_STDIO
#define MINIZ_NO_TIME
#define MINIZ_NO_ARCHIVE_WRITING_APIS
#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES

#ifdef ZIP_ENABLE_DEFLATE
#undef ZIP_ENABLE_DEFLATE
#endif
#define ZIP_ENABLE_DEFLATE 0

#ifndef RAYLIB_ZIP_ZIP_H
    #define RAYLIB_ZIP_ZIP_H "zip.h"
#endif
#include RAYLIB_ZIP_ZIP_H

#ifndef RAYLIB_ZIP_ZIP_C
    #define RAYLIB_ZIP_ZIP_C "zip.c"
#endif
#include RAYLIB_ZIP_ZIP_C

#ifdef __cplusplus
extern "C" {
#endif

RAYLIB_ZIP_API Zip LoadZip(const char* zipFile) {
    int dataSize = 0;
    unsigned char* data = LoadFileData(zipFile, &dataSize);
    if (data == NULL) {
        TraceLog(LOG_WARNING, "ZIP: Failed to load %s", zipFile);
        return (Zip){0};
    }
    Zip zip = LoadZipFromMemory(data, dataSize);
    zip.data = data;
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
    if (zip.data != NULL) {
        MemFree(zip.data);
    }
}

RAYLIB_ZIP_API bool IsZipValid(Zip zip) {
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
    if (dataSize != NULL) {
        *dataSize = 0;
    }

    if (zip.zip == NULL) {
        TraceLog(LOG_WARNING, "ZIP: Not ready, call LoadZip() first");
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
        MemFree(data);
        TraceLog(LOG_WARNING, "ZIP: Failed to read entry: %s", fileName);
        return NULL;
    }

    if (dataSize != NULL) {
        *dataSize = (int)size;
    }
    return data;
}

RAYLIB_ZIP_API char* LoadFileTextFromZip(Zip zip, const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return NULL;
    }

    char* text = (char*)RL_MALLOC(dataSize + 1);
    if (text == NULL) {
        UnloadFileData(data);
        return NULL;
    }

    memcpy(text, data, dataSize);
    text[dataSize] = '\0';
    UnloadFileData(data);

    return text;
}

RAYLIB_ZIP_API FilePathList LoadDirectoryFilesFromZipEx(Zip zip, const char* basePath, const char* filter, bool scanSubdirs) {
    FilePathList result = {0};

    if (zip.zip == NULL) {
        return result;
    }

    struct zip_t* z = (struct zip_t*)zip.zip;
    int total = (int)zip_entries_total(z);
    unsigned int baseLen = (basePath != NULL) ? TextLength(basePath) : 0;

    // First pass: count matching entries
    int count = 0;
    for (int i = 0; i < total; i++) {
        if (zip_entry_openbyindex(z, i) < 0) continue;
        const char* name = zip_entry_name(z);
        bool isDir = zip_entry_isdir(z);

        bool matches = !isDir && name != NULL;
        if (matches && baseLen > 0 && TextFindIndex(name, basePath) != 0) matches = false;
        if (matches && !scanSubdirs && TextFindIndex(name + baseLen, "/") >= 0) matches = false;
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
        if (matches && baseLen > 0 && TextFindIndex(name, basePath) != 0) matches = false;
        if (matches && !scanSubdirs && TextFindIndex(name + baseLen, "/") >= 0) matches = false;
        if (matches && filter != NULL && !IsFileExtension(name, filter)) matches = false;

        if (matches) {
            unsigned int len = TextLength(name);
            result.paths[result.count] = (char*)RL_MALLOC(len + 1);
            if (result.paths[result.count] != NULL) {
                TextCopy(result.paths[result.count], name);
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
    UnloadFileData(data);
    return image;
}

RAYLIB_ZIP_API Image LoadImageAnimFromZip(Zip zip, const char *fileName, int *frames) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return (Image){0};
    }

    Image image = LoadImageAnimFromMemory(GetFileExtension(fileName), data, dataSize, frames);
    UnloadFileData(data);
    return image;
}

RAYLIB_ZIP_API Texture2D LoadTextureFromZip(Zip zip, const char* fileName) {
    Image image = LoadImageFromZip(zip, fileName);
    if (!IsImageValid(image)) {
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
    UnloadFileData(data);
    return wave;
}

RAYLIB_ZIP_API Music LoadMusicStreamFromZip(Zip zip, const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return (Music){0};
    }

    Music music = LoadMusicStreamFromMemory(GetFileExtension(fileName), data, dataSize);
    UnloadFileData(data);
    return music;
}

RAYLIB_ZIP_API Font LoadFontFromZip(Zip zip, const char* fileName, int fontSize, int* codepoints, int codepointCount) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, fileName, &dataSize);
    if (data == NULL) {
        return (Font){0};
    }

    Font font = LoadFontFromMemory(GetFileExtension(fileName), data, dataSize, fontSize, codepoints, codepointCount);
    UnloadFileData(data);
    return font;
}

RAYLIB_ZIP_API Shader LoadShaderFromZip(Zip zip, const char* vsFileName, const char* fsFileName) {
    unsigned char* vsCode = NULL;
    unsigned char* fsCode = NULL;

    if (vsFileName != NULL) {
        vsCode = LoadFileDataFromZip(zip, vsFileName, NULL);
    }
    if (fsFileName != NULL) {
        fsCode = LoadFileDataFromZip(zip, fsFileName, NULL);
    }

    Shader shader = LoadShaderFromMemory((const char*)vsCode, (const char*)fsCode);

    if (vsCode != NULL) {
        UnloadFileData(vsCode);
    }
    if (fsCode != NULL) {
        UnloadFileData(fsCode);
    }

    return shader;
}

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_ZIP_IMPLEMENTATION_ONCE
#endif  // RAYLIB_ZIP_IMPLEMENTATION
