# raylib-zip

Zip archive support for [raylib](https://www.raylib.com) using [kuba--/zip](https://github.com/kuba--/zip).

## Usage

``` c
#define RAYLIB_ZIP_IMPLEMENTATION
#include "raylib-zip.h"

int main() {
    InitWindow(800, 600, "raylib-zip");

    Zip zip = LoadZip("resources.zip");

    Image image = LoadImageFromZip(zip, "player.png");
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    // ... game loop ...

    UnloadTexture(texture);
    UnloadZip(zip);
    CloseWindow();
}
```

## API

``` c
Zip LoadZip(const char* zipFile);
Zip LoadZipFromMemory(const unsigned char* data, int dataSize);
void UnloadZip(Zip zip);
bool IsZipReady(Zip zip);
bool FileExistsInZip(Zip zip, const char* fileName);
int GetZipEntryCount(Zip zip);

FilePathList LoadDirectoryFilesFromZip(Zip zip, const char* dirPath);
FilePathList LoadDirectoryFilesFromZipEx(Zip zip, const char* basePath, const char* filter, bool scanSubdirs);

unsigned char* LoadFileDataFromZip(Zip zip, const char* fileName, int* dataSize);
void UnloadFileDataFromZip(unsigned char* data);
char* LoadFileTextFromZip(Zip zip, const char* fileName);
void UnloadFileTextFromZip(char* text);

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
