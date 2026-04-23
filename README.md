# raylib-zip

Zip archive support for [raylib](https://www.raylib.com) using [kuba--/zip](https://github.com/kuba--/zip).

## Usage

``` c
#define RAYLIB_ZIP_IMPLEMENTATION
#include "raylib-zip.h"

int main() {
    InitWindow(800, 600, "raylib-zip");

    InitZip("resources.zip");

    Image image = LoadImageFromZip("player.png");
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    // ... game loop ...

    UnloadTexture(texture);
    CloseZip();
    CloseWindow();
}
```

## API

``` c
bool InitZip(const char* zipFile);
void CloseZip(void);
bool IsZipReady(void);
bool FileExistsInZip(const char* fileName);
int GetZipEntryCount(void);

unsigned char* LoadFileDataFromZip(const char* fileName, int* dataSize);
void UnloadFileDataFromZip(unsigned char* data);
char* LoadFileTextFromZip(const char* fileName);
void UnloadFileTextFromZip(char* text);

Image LoadImageFromZip(const char* fileName);
Texture2D LoadTextureFromZip(const char* fileName);
Wave LoadWaveFromZip(const char* fileName);
Music LoadMusicStreamFromZip(const char* fileName);
Font LoadFontFromZip(const char* fileName, int fontSize, int* codepoints, int codepointCount);
Shader LoadShaderFromZip(const char* vsFileName, const char* fsFileName);
```

## Development

``` bash
cmake -B build
cmake --build build
ctest --test-dir build
```

## License

[zlib/libpng](LICENSE)
