#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE (32 * 1024 * 1024)

void overwrite_file(const char *filename);
void wait_for_enter(void);

int main(void)
{
    char filename[1024];

    fprintf(stderr, "Drag your file here:\n");

    if (!fgets(filename, sizeof(filename), stdin)) {
        fprintf(stderr, "Input error.\n");
        wait_for_enter();
        return 1;
    }

    filename[strcspn(filename, "\r\n")] = '\0';

    /* Windows drag & drop usually adds quotes */
    size_t len = strlen(filename);

    if (len >= 2 && filename[0] == '"' && filename[len - 1] == '"') {
        memmove(filename, filename + 1, len - 2);
        filename[len - 2] = '\0';
    }

    FILE *file = fopen(filename, "rb");

    if (!file) {
        fprintf(stderr, "File not found or cannot be opened.\n");
        wait_for_enter();
        return 1;
    }

    fprintf(stderr, "File found.\n");

    /* IMPORTANT: close the validation handle */
    fclose(file);

    overwrite_file(filename);

    wait_for_enter();

    return 0;
}

void overwrite_file(const char *filename)
{
    FILE *file = fopen(filename, "rb+");

    if (!file) {
        fprintf(stderr, "Cannot open file for overwriting.\n");
        return;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Seek error.\n");
        fclose(file);
        return;
    }

    long size = ftell(file);

    if (size < 0) {
        fprintf(stderr, "Could not get file size.\n");
        fclose(file);
        return;
    }

    rewind(file);

    if (size == 0) {
        fclose(file);

        if (remove(filename) == 0) {
            fprintf(stderr, "Empty file deleted.\n");
        } else {
            perror("Delete error");
        }

        return;
    }

    unsigned char *buffer = malloc(BUFFER_SIZE);

    if (!buffer) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return;
    }

    /* 32 MB buffer trust me bro */
    memset(buffer, 0xFF, BUFFER_SIZE);

    long written = 0;

    while (written < size) {
        size_t chunk;

        if ((size - written) > BUFFER_SIZE) {
            chunk = BUFFER_SIZE;
        } else {
            chunk = (size_t)(size - written);
        }

        size_t result = fwrite(buffer, 1, chunk, file);

        if (result != chunk) {
            fprintf(stderr, "Write error.\n");
            break;
        }

        written += (long)result;
    }

    if (fflush(file) != 0) {
        fprintf(stderr, "Flush error.\n");
    }

    fclose(file);
    free(buffer);

    if (written == size) {
        fprintf(stderr, "Overwrite complete: %ld bytes.\n", written);

        if (remove(filename) == 0) {
            fprintf(stderr, "File overwritten and deleted.\n");
        } else {
            perror("Delete error");
            fprintf(stderr, "errno: %d\n", errno);
        }
    }
}

void wait_for_enter(void)
{
    fprintf(stderr, "\nPress ENTER to exit...");
    getchar();
}