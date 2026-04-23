#include <assert.h>
#include <string.h>

#include "zip.h"

#define RAYLIB_ZIP_IMPLEMENTATION
#include "../raylib-zip.h"

int main() {
    // Create a test zip with some content
    const char* testContent = "Hello from raylib-zip!";
    struct zip_t* zip = zip_open("test.zip", 6, 'w');
    assert(zip != NULL);
    assert(zip_entry_open(zip, "hello.txt") == 0);
    assert(zip_entry_write(zip, testContent, strlen(testContent)) == 0);
    zip_entry_close(zip);
    zip_close(zip);

    // Initialize
    assert(InitZip("test.zip"));
    assert(IsZipReady());
    assert(GetZipEntryCount() == 1);
    assert(FileExistsInZip("hello.txt"));
    assert(!FileExistsInZip("missing.txt"));

    // Load file data
    int dataSize = 0;
    unsigned char* data = LoadFileDataFromZip("hello.txt", &dataSize);
    assert(data != NULL);
    assert(dataSize == (int)strlen(testContent));
    assert(memcmp(data, testContent, dataSize) == 0);
    UnloadFileDataFromZip(data);

    // Load file text
    char* text = LoadFileTextFromZip("hello.txt");
    assert(text != NULL);
    assert(strcmp(text, testContent) == 0);
    UnloadFileTextFromZip(text);

    CloseZip();
    assert(!IsZipReady());

    return 0;
}
