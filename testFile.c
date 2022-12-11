#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIZE 1024

int main(int argc, char *argv[]) {
    char buff1[SIZE + 1];
    char buff2[SIZE + 1];
    char* file1 = argv[1];
    char* file2 = argv[2];
    int fdin1, fdin2;
    int charsr1, charsr2;
    int letters1 = 0, letters2 = 0;
    int countSimilar = 0;

    // Open first file
    fdin1 = open(file1, O_RDONLY);
    if(fdin1 < 0) {
        perror("Could not open first file");
        exit(-1);
    }

    // Open second file
    fdin2 = open(file2, O_RDONLY);
    if (fdin2 < 0) {
        perror("Could not open second file");
        exit(-1);
    }

    // Read from first file
    charsr1 = read(fdin1, buff1, SIZE);
    while(charsr1 > 0) {
        letters1 += charsr1;
        memset(buff1, 0, strlen(buff1));
        charsr1 = read(fdin1, buff1, SIZE);
    }

    // Read from second file
    charsr2 = read(fdin2, buff2, SIZE);
    while(charsr2 > 0) {
        letters2 += charsr2;
        memset(buff2, 0, strlen(buff2));
        charsr2 = read(fdin2, buff2, SIZE);
    }

    // Now we have the sizes of the files
    int i, j;
    char readFile1[letters1];
    char readFile2[letters2];
    char c1, c2;
    int diff;

    // Point to the beginning of the files
    lseek(fdin1, 0, SEEK_SET);
    lseek(fdin2, 0, SEEK_SET);

    // Read the whole files
    read(fdin1, readFile1, letters1);
    read(fdin2, readFile2, letters2);

    close(fdin1);
    close(fdin2);

    // Files are different
    if (letters1 != letters2) {
        return 2;
    } else {
        // Both of the files have the same size
        for (i = 0; i < letters1; i++) {
            c1 = readFile1[i];
            c2 = readFile2[i];
            if (c1 != c2)
            {
                return 2;
            }
        }
        return 1;
    }
}