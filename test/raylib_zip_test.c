#include "raylib.h"

#define RAYLIB_ASSERT_LOG LOG_FATAL
#include "raylib-assert.h"

#define RAYLIB_ZIP_IMPLEMENTATION
#include "../raylib-zip.h"

int main() {
    const char* testContent = "Hello from raylib-zip!";

    // LoadZip
    {
        Zip zip = LoadZip("resources/test.zip");

        // IsZipValid
        {
            Assert(IsZipValid(zip));
        }

        // GetZipEntryCount
        {
            AssertGreaterEqual(GetZipEntryCount(zip), 4);
        }

        // FileExistsInZip
        {
            Assert(FileExistsInZip(zip, "hello.txt"));
            AssertNot(FileExistsInZip(zip, "missing.txt"));
        }

        // LoadFileDataFromZip
        {
            int dataSize = 0;
            unsigned char* data = LoadFileDataFromZip(zip, "hello.txt", &dataSize);
            AssertNotNull(data);
            AssertEqual(dataSize, (int)TextLength(testContent));
            UnloadFileData(data);
        }

        // LoadFileTextFromZip
        {
            char* text = LoadFileTextFromZip(zip, "hello.txt");
            AssertNotNull(text);
            AssertStringEqual(text, testContent);
            UnloadFileText(text);
        }

        // LoadImageFromZip
        {
            Image image = LoadImageFromZip(zip, "images/player.png");
            AssertImage(image);
            UnloadImage(image);
            Image notfound = LoadImageFromZip(zip, "images/notfound.png");
            AssertNot(IsImageValid(notfound));
        }

        // LoadDirectoryFilesFromZip
        {
            FilePathList root = LoadDirectoryFilesFromZip(zip, NULL);
            AssertEqual(root.count, 1);
            UnloadDirectoryFiles(root);

            FilePathList images = LoadDirectoryFilesFromZip(zip, "images/");
            AssertEqual(images.count, 2);
            UnloadDirectoryFiles(images);
        }

        // LoadDirectoryFilesFromZipEx
        {
            FilePathList pngs = LoadDirectoryFilesFromZipEx(zip, "images/", ".png", true);
            AssertEqual(pngs.count, 2);
            UnloadDirectoryFiles(pngs);

            FilePathList all = LoadDirectoryFilesFromZipEx(zip, NULL, NULL, true);
            AssertEqual(all.count, 4);
            UnloadDirectoryFiles(all);
        }

        UnloadZip(zip);
    }

    TraceLog(LOG_INFO, "ZIP: Tests pass");

    return 0;
}
