# raylib-zip

Zip archive support for [raylib](https://www.raylib.com) using [kuba--/zip](https://github.com/kuba--/zip).

## Usage

``` c
#define RAYLIB_ZIP_IMPLEMENTATION
#include "raylib-zip.h"

int main() {
    InitWindow(800, 600, "raylib-zip");

    Zip zip = InitZip("resources.zip");

    Image image = LoadImageFromZip(zip, "player.png");
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    // ... game loop ...

    UnloadTexture(texture);
    CloseZip(zip);
    CloseWindow();
}
```

## API

``` c
Zip InitZip(const char* zipFile);
void CloseZip(Zip zip);
bool IsZipReady(Zip zip);
bool FileExistsInZip(Zip zip, const char* fileName);
int GetZipEntryCount(Zip zip);

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
