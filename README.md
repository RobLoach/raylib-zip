# raylib-zip

Zip archive support for [raylib](https://www.raylib.com), using [`kuba--/zip`](https://github.com/kuba--/zip) and miniz.

## Usage

raylib-zip is a header-only library. Copy `raylib-zip.h`, `zip.h`, `miniz.h`, and `zip.c` into your project, then in **one** C file define the implementation before including:

``` c
#define RAYLIB_ZIP_IMPLEMENTATION
#include "raylib-zip.h"

int main() {
    InitWindow(800, 600, "raylib-zip");

    Zip zip = LoadZip("resources.zip");

    Texture2D texture = LoadTextureFromZip(zip, "player.png");

    UnloadTexture(texture);
    UnloadZip(zip);
    CloseWindow();
}
```

The implementation pulls in `zip.c` automatically. To use a different path, define `RAYLIB_ZIP_ZIP_C` before including:

``` c
#define RAYLIB_ZIP_ZIP_C "path/to/zip.c"
#define RAYLIB_ZIP_IMPLEMENTATION
#include "raylib-zip.h"
```

## API

``` c
Zip LoadZip(const char* zipFile);                                    // Open a zip archive from a file
Zip LoadZipFromMemory(const unsigned char* data, int dataSize);      // Open a zip archive from memory
void UnloadZip(Zip zip);                                             // Close and unload a zip archive
bool IsZipValid(Zip zip);                                            // Check if a zip archive is open and ready
bool FileExistsInZip(Zip zip, const char* fileName);                 // Check if a file exists in the zip
int GetZipEntryCount(Zip zip);                                       // Get the total number of entries in the zip
FilePathList LoadDirectoryFilesFromZip(Zip zip, const char* dirPath);
FilePathList LoadDirectoryFilesFromZipEx(Zip zip, const char* basePath, const char* filter, bool scanSubdirs);
unsigned char* LoadFileDataFromZip(Zip zip, const char* fileName, int* dataSize);
char* LoadFileTextFromZip(Zip zip, const char* fileName);
Image LoadImageFromZip(Zip zip, const char* fileName);
Texture2D LoadTextureFromZip(Zip zip, const char* fileName);
Wave LoadWaveFromZip(Zip zip, const char* fileName);
Music LoadMusicStreamFromZip(Zip zip, const char* fileName);
Font LoadFontFromZip(Zip zip, const char* fileName, int fontSize, int* codepoints, int codepointCount);
Shader LoadShaderFromZip(Zip zip, const char* vsFileName, const char* fsFileName);
```

## Development

``` bash
cmake -B build
cmake --build build
ctest --test-dir build
```

## License

[zlib/libpng](LICENSE)
