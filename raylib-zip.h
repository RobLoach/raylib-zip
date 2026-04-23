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

#ifdef __cplusplus
extern "C" {
#endif

RAYLIB_ZIP_API bool InitZip(const char* zipFile);
RAYLIB_ZIP_API void CloseZip(void);
RAYLIB_ZIP_API bool IsZipReady(void);
RAYLIB_ZIP_API bool FileExistsInZip(const char* fileName);
RAYLIB_ZIP_API int GetZipEntryCount(void);

RAYLIB_ZIP_API unsigned char* LoadFileDataFromZip(const char* fileName, int* dataSize);
RAYLIB_ZIP_API void UnloadFileDataFromZip(unsigned char* data);
RAYLIB_ZIP_API char* LoadFileTextFromZip(const char* fileName);
RAYLIB_ZIP_API void UnloadFileTextFromZip(char* text);

RAYLIB_ZIP_API Image LoadImageFromZip(const char* fileName);
RAYLIB_ZIP_API Texture2D LoadTextureFromZip(const char* fileName);
RAYLIB_ZIP_API Wave LoadWaveFromZip(const char* fileName);
RAYLIB_ZIP_API Music LoadMusicStreamFromZip(const char* fileName);
RAYLIB_ZIP_API Font LoadFontFromZip(const char* fileName, int fontSize, int* codepoints, int codepointCount);
RAYLIB_ZIP_API Shader LoadShaderFromZip(const char* vsFileName, const char* fsFileName);

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

#ifndef RAYLIB_ZIP_RAYLIB_H
    #define RAYLIB_ZIP_RAYLIB_H "raylib.h"
#endif
#include RAYLIB_ZIP_RAYLIB_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static struct zip_t* _raylib_zip = NULL;

RAYLIB_ZIP_API bool InitZip(const char* zipFile) {
    if (_raylib_zip != NULL) {
        CloseZip();
    }
    _raylib_zip = zip_open(zipFile, 0, 'r');
    if (_raylib_zip == NULL) {
        TraceLog(LOG_WARNING, "ZIP: Failed to open %s", zipFile);
        return false;
    }
    return true;
}

RAYLIB_ZIP_API void CloseZip(void) {
    if (_raylib_zip != NULL) {
        zip_close(_raylib_zip);
        _raylib_zip = NULL;
    }
}

RAYLIB_ZIP_API bool IsZipReady(void) {
    return _raylib_zip != NULL;
}

RAYLIB_ZIP_API int GetZipEntryCount(void) {
    if (_raylib_zip == NULL) {
        return 0;
    }
    return (int)zip_entries_total(_raylib_zip);
}

RAYLIB_ZIP_API bool FileExistsInZip(const char* fileName) {
    if (_raylib_zip == NULL) {
        return false;
    }
    if (zip_entry_open(_raylib_zip, fileName) < 0) {
        return false;
    }
    zip_entry_close(_raylib_zip);
    return true;
}

RAYLIB_ZIP_API unsigned char* LoadFileDataFromZip(const char* fileName, int* dataSize) {
    *dataSize = 0;

    if (_raylib_zip == NULL) {
        TraceLog(LOG_WARNING, "ZIP: Not initialized, call InitZip() first");
        return NULL;
    }

    if (zip_entry_open(_raylib_zip, fileName) < 0) {
        TraceLog(LOG_WARNING, "ZIP: Failed to open entry: %s", fileName);
        return NULL;
    }

    size_t size = (size_t)zip_entry_uncomp_size(_raylib_zip);
    if (size == 0) {
        zip_entry_close(_raylib_zip);
        return NULL;
    }

    unsigned char* data = (unsigned char*)RL_MALLOC(size);
    if (data == NULL) {
        zip_entry_close(_raylib_zip);
        return NULL;
    }

    ssize_t bytesRead = zip_entry_noallocread(_raylib_zip, data, size);
    zip_entry_close(_raylib_zip);

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

RAYLIB_ZIP_API char* LoadFileTextFromZip(const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(fileName, &dataSize);
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

RAYLIB_ZIP_API Image LoadImageFromZip(const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(fileName, &dataSize);
    if (data == NULL) {
        return (Image){0};
    }

    Image image = LoadImageFromMemory(GetFileExtension(fileName), data, dataSize);
    RL_FREE(data);
    return image;
}

RAYLIB_ZIP_API Texture2D LoadTextureFromZip(const char* fileName) {
    Image image = LoadImageFromZip(fileName);
    if (image.data == NULL) {
        return (Texture2D){0};
    }

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

RAYLIB_ZIP_API Wave LoadWaveFromZip(const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(fileName, &dataSize);
    if (data == NULL) {
        return (Wave){0};
    }

    Wave wave = LoadWaveFromMemory(GetFileExtension(fileName), data, dataSize);
    RL_FREE(data);
    return wave;
}

RAYLIB_ZIP_API Music LoadMusicStreamFromZip(const char* fileName) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(fileName, &dataSize);
    if (data == NULL) {
        return (Music){0};
    }

    Music music = LoadMusicStreamFromMemory(GetFileExtension(fileName), data, dataSize);
    RL_FREE(data);
    return music;
}

RAYLIB_ZIP_API Font LoadFontFromZip(const char* fileName, int fontSize, int* codepoints, int codepointCount) {
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(fileName, &dataSize);
    if (data == NULL) {
        return GetFontDefault();
    }

    Font font = LoadFontFromMemory(GetFileExtension(fileName), data, dataSize, fontSize, codepoints, codepointCount);
    RL_FREE(data);
    return font;
}

RAYLIB_ZIP_API Shader LoadShaderFromZip(const char* vsFileName, const char* fsFileName) {
    char* vsCode = NULL;
    char* fsCode = NULL;

    if (vsFileName != NULL) {
        vsCode = LoadFileTextFromZip(vsFileName);
    }
    if (fsFileName != NULL) {
        fsCode = LoadFileTextFromZip(fsFileName);
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
