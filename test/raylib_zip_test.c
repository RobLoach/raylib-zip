#include <assert.h>
#include <string.h>

#include "zip.h"

#define RAYLIB_ZIP_IMPLEMENTATION
#include "../raylib-zip.h"

int main() {
    // Create a test zip with some content across directories
    const char* testContent = "Hello from raylib-zip!";
    struct zip_t* z = zip_open("test.zip", 6, 'w');
    assert(z != NULL);
    assert(zip_entry_open(z, "hello.txt") == 0);
    assert(zip_entry_write(z, testContent, strlen(testContent)) == 0);
    zip_entry_close(z);
    assert(zip_entry_open(z, "images/player.png") == 0);
    assert(zip_entry_write(z, testContent, strlen(testContent)) == 0);
    zip_entry_close(z);
    assert(zip_entry_open(z, "images/sub/icon.png") == 0);
    assert(zip_entry_write(z, testContent, strlen(testContent)) == 0);
    zip_entry_close(z);
    assert(zip_entry_open(z, "images/bg.jpg") == 0);
    assert(zip_entry_write(z, testContent, strlen(testContent)) == 0);
    zip_entry_close(z);
    zip_close(z);

    // Load from file
    Zip zip = LoadZip("test.zip");
    assert(IsZipReady(zip));
    assert(GetZipEntryCount(zip) == 4);
    assert(FileExistsInZip(zip, "hello.txt"));
    assert(!FileExistsInZip(zip, "missing.txt"));

    // Load file data
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, "hello.txt", &dataSize);
    assert(data != NULL);
    assert(dataSize == (int)strlen(testContent));
    assert(memcmp(data, testContent, dataSize) == 0);
    UnloadFileDataFromZip(data);

    // Load file text
    char* text = LoadFileTextFromZip(zip, "hello.txt");
    assert(text != NULL);
    assert(strcmp(text, testContent) == 0);
    UnloadFileTextFromZip(text);

    // LoadDirectoryFilesFromZip: all files at root (no subdir)
    FilePathList root = LoadDirectoryFilesFromZip(zip, NULL);
    assert(root.count == 1);  // only hello.txt
    UnloadDirectoryFiles(root);

    // LoadDirectoryFilesFromZip: files directly in images/
    FilePathList images = LoadDirectoryFilesFromZip(zip, "images/");
    assert(images.count == 2);  // player.png and bg.jpg, not sub/icon.png
    UnloadDirectoryFiles(images);

    // LoadDirectoryFilesFromZipEx: images/ recursively, .png only
    FilePathList pngs = LoadDirectoryFilesFromZipEx(zip, "images/", ".png", true);
    assert(pngs.count == 2);  // player.png and sub/icon.png
    UnloadDirectoryFiles(pngs);

    // LoadDirectoryFilesFromZipEx: all files recursively
    FilePathList all = LoadDirectoryFilesFromZipEx(zip, NULL, NULL, true);
    assert(all.count == 4);
    UnloadDirectoryFiles(all);

    UnloadZip(zip);

    // Load from memory
    int zipDataSize = 0;
    unsigned char* zipData = LoadFileData("test.zip", &zipDataSize);
    assert(zipData != NULL);
    Zip zipMem = LoadZipFromMemory(zipData, zipDataSize);
    assert(IsZipReady(zipMem));
    char* memText = LoadFileTextFromZip(zipMem, "hello.txt");
    assert(memText != NULL);
    assert(strcmp(memText, testContent) == 0);
    UnloadFileTextFromZip(memText);
    UnloadZip(zipMem);
    UnloadFileData(zipData);

    return 0;
}
