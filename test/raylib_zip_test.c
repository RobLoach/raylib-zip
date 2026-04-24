#include <string.h>  // memcmp

#include "zip.h"
#define RAYLIB_ASSERT_LOG LOG_FATAL
#include "raylib-assert.h"

#define RAYLIB_ZIP_IMPLEMENTATION
#include "../raylib-zip.h"

int main() {
    // Create a test zip with content across directories
    const char* testContent = "Hello from raylib-zip!";
    struct zip_t* z = zip_open("test.zip", 6, 'w');
    AssertNotNull(z);
    AssertEqual(zip_entry_open(z, "hello.txt"), 0);
    AssertEqual(zip_entry_write(z, testContent, TextLength(testContent)), 0);
    zip_entry_close(z);
    AssertEqual(zip_entry_open(z, "images/player.png"), 0);
    AssertEqual(zip_entry_write(z, testContent, TextLength(testContent)), 0);
    zip_entry_close(z);
    AssertEqual(zip_entry_open(z, "images/sub/icon.png"), 0);
    AssertEqual(zip_entry_write(z, testContent, TextLength(testContent)), 0);
    zip_entry_close(z);
    AssertEqual(zip_entry_open(z, "images/bg.jpg"), 0);
    AssertEqual(zip_entry_write(z, testContent, TextLength(testContent)), 0);
    zip_entry_close(z);
    zip_close(z);

    // Load from file
    Zip zip = LoadZip("test.zip");
    Assert(IsZipReady(zip));
    AssertEqual(GetZipEntryCount(zip), 4);
    Assert(FileExistsInZip(zip, "hello.txt"));
    AssertNot(FileExistsInZip(zip, "missing.txt"));

    // Load file data
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip(zip, "hello.txt", &dataSize);
    AssertNotNull(data);
    AssertEqual(dataSize, (int)TextLength(testContent));
    AssertEqual(memcmp(data, testContent, dataSize), 0);
    UnloadFileDataFromZip(data);

    // Load file text
    char* text = LoadFileTextFromZip(zip, "hello.txt");
    AssertNotNull(text);
    AssertStringEqual(text, testContent);
    UnloadFileTextFromZip(text);

    // LoadDirectoryFilesFromZip: files at root only
    FilePathList root = LoadDirectoryFilesFromZip(zip, NULL);
    AssertEqual((int)root.count, 1);
    UnloadDirectoryFiles(root);

    // LoadDirectoryFilesFromZip: files directly in images/
    FilePathList images = LoadDirectoryFilesFromZip(zip, "images/");
    AssertEqual((int)images.count, 2);
    UnloadDirectoryFiles(images);

    // LoadDirectoryFilesFromZipEx: images/ recursively, .png only
    FilePathList pngs = LoadDirectoryFilesFromZipEx(zip, "images/", ".png", true);
    AssertEqual((int)pngs.count, 2);
    UnloadDirectoryFiles(pngs);

    // LoadDirectoryFilesFromZipEx: all files recursively
    FilePathList all = LoadDirectoryFilesFromZipEx(zip, NULL, NULL, true);
    AssertEqual((int)all.count, 4);
    UnloadDirectoryFiles(all);

    UnloadZip(zip);

    // Load from memory
    int zipDataSize = 0;
    unsigned char* zipData = LoadFileData("test.zip", &zipDataSize);
    AssertNotNull(zipData);
    Zip zipMem = LoadZipFromMemory(zipData, zipDataSize);
    Assert(IsZipReady(zipMem));
    char* memText = LoadFileTextFromZip(zipMem, "hello.txt");
    AssertNotNull(memText);
    AssertStringEqual(memText, testContent);
    UnloadFileTextFromZip(memText);
    UnloadZip(zipMem);
    UnloadFileData(zipData);

    return 0;
}
