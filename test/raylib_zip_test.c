#include <string.h>  // memcmp

#include "zip.h"
#define RAYLIB_ASSERT_LOG LOG_FATAL
#include "raylib-assert.h"

#define RAYLIB_ZIP_IMPLEMENTATION
#include "../raylib-zip.h"

int main() {
    const char* testContent = "Hello from raylib-zip!";

    // Create a test zip with content across directories
    {
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
    }

    // LoadZip
    {
        Zip zip = LoadZip("test.zip");
        Assert(IsZipReady(zip));
        AssertEqual(GetZipEntryCount(zip), 4);
        Assert(FileExistsInZip(zip, "hello.txt"));
        AssertNot(FileExistsInZip(zip, "missing.txt"));

        // LoadFileDataFromZip
        {
            int dataSize = 0;
            unsigned char* data = LoadFileDataFromZip(zip, "hello.txt", &dataSize);
            AssertNotNull(data);
            AssertEqual(dataSize, (int)TextLength(testContent));
            AssertEqual(memcmp(data, testContent, dataSize), 0);
            UnloadFileDataFromZip(data);
        }

        // LoadFileTextFromZip
        {
            char* text = LoadFileTextFromZip(zip, "hello.txt");
            AssertNotNull(text);
            AssertStringEqual(text, testContent);
            UnloadFileTextFromZip(text);
        }

        // LoadDirectoryFilesFromZip
        {
            FilePathList root = LoadDirectoryFilesFromZip(zip, NULL);
            AssertEqual((int)root.count, 1);
            UnloadDirectoryFiles(root);

            FilePathList images = LoadDirectoryFilesFromZip(zip, "images/");
            AssertEqual((int)images.count, 2);
            UnloadDirectoryFiles(images);
        }

        // LoadDirectoryFilesFromZipEx
        {
            FilePathList pngs = LoadDirectoryFilesFromZipEx(zip, "images/", ".png", true);
            AssertEqual((int)pngs.count, 2);
            UnloadDirectoryFiles(pngs);

            FilePathList all = LoadDirectoryFilesFromZipEx(zip, NULL, NULL, true);
            AssertEqual((int)all.count, 4);
            UnloadDirectoryFiles(all);
        }

        UnloadZip(zip);
    }

    // LoadZipFromMemory
    {
        int zipDataSize = 0;
        unsigned char* zipData = LoadFileData("test.zip", &zipDataSize);
        AssertNotNull(zipData);
        Zip zip = LoadZipFromMemory(zipData, zipDataSize);
        Assert(IsZipReady(zip));

        char* text = LoadFileTextFromZip(zip, "hello.txt");
        AssertNotNull(text);
        AssertStringEqual(text, testContent);
        UnloadFileTextFromZip(text);

        UnloadZip(zip);
        UnloadFileData(zipData);
    }

    return 0;
}
