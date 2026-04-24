# raylib-zip

Zip archive support for [raylib](https://www.raylib.com) using [kuba--/zip](https://github.com/kuba--/zip).

## Usage

``` c
#define RAYLIB_ZIP_IMPLEMENTATION
#include "raylib-zip.h"

int main() {
    InitWindow(800, 600, "raylib-zip");

    Zip zip = LoadZip("resources.zip");

    Texture2D texture = LoadTextureFromZip(zip, "player.png");

    // ... game loop ...

    UnloadTexture(texture);
    UnloadZip(zip);
    CloseWindow();
}
```

## API

### Archive

``` c
Zip LoadZip(const char* zipFile);                                    // Open a zip archive from a file
Zip LoadZipFromMemory(const unsigned char* data, int dataSize);      // Open a zip archive from memory
void UnloadZip(Zip zip);                                             // Close and unload a zip archive
bool IsZipReady(Zip zip);                                            // Check if a zip archive is open and ready
bool FileExistsInZip(Zip zip, const char* fileName);                 // Check if a file exists in the zip
int GetZipEntryCount(Zip zip);                                       // Get the total number of entries in the zip
```

### Directory

``` c
FilePathList LoadDirectoryFilesFromZip(Zip zip, const char* dirPath);
FilePathList LoadDirectoryFilesFromZipEx(Zip zip, const char* basePath, const char* filter, bool scanSubdirs);
void UnloadDirectoryFiles(FilePathList files);                        // From raylib
```

`LoadDirectoryFilesFromZipEx` supports semicolon-separated extension filters (e.g. `".png;.jpg"`) and optional recursive scanning. Both functions return a `FilePathList` compatible with raylib's `UnloadDirectoryFiles()`.

### File Data

``` c
unsigned char* LoadFileDataFromZip(Zip zip, const char* fileName, int* dataSize);
void UnloadFileDataFromZip(unsigned char* data);
char* LoadFileTextFromZip(Zip zip, const char* fileName);
void UnloadFileTextFromZip(char* text);
```

### Assets

``` c
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
