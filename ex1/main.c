//di unger 206492746
//Daniel levi 318799640



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define PATH_MAX 2048

int compereFile( char* file1, char* file2) {
    char buff1[PATH_MAX + 1];
    char buff2[PATH_MAX + 1];
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
    charsr1 = read(fdin1, buff1, PATH_MAX);
    while(charsr1 > 0) {
        letters1 += charsr1;
        memset(buff1, 0, strlen(buff1));
        charsr1 = read(fdin1, buff1, PATH_MAX);
    }

    // Read from second file
    charsr2 = read(fdin2, buff2, PATH_MAX);
    while(charsr2 > 0) {
        letters2 += charsr2;
        memset(buff2, 0, strlen(buff2));
        charsr2 = read(fdin2, buff2, PATH_MAX);
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

    // Compare length of files
    if (letters1 != letters2) {
        return 2;
    } else {
        // Both of the files have the same size
        for (i = 0; i < letters1; i++) {
            c1 = readFile1[i];
            c2 = readFile2[i];
            if (c1 == c2)
                countSimilar++;
        }

        if (countSimilar == letters1)
            return 1;
        else return 2;
    }
}
//char *readFile(char *path)
//{
//    char *buffer = NULL;
//    long length;
//    FILE *f = fopen(path, "rb");
//
//    if (f)
//    {
//        fseek(f, 0, SEEK_END);
//        length = ftell(f);
//        fseek(f, 0, SEEK_SET);
//        buffer = malloc(length);
//        if (buffer)
//        {
//            fread(buffer, 1, length, f);
//        }
//        fclose(f);
//    }
//
//    return buffer;
//}

char **readConfiguration(char *path)
{
    char **fullConfig = (char **)malloc(1024);
    char *currLine = (char *)malloc(1024);
    int configFd = 0;
    int i = 0;
    char *ptr;

    if ((configFd = open(path, O_RDONLY)) == -1)
    {
        char error[] = "Could not open given file\n";
        write(2, error, sizeof(error));
        exit(-1);
    }
    if (read(configFd, currLine, 1024) == -1)
    {
        char error[20] = "Could not read file\n";
        write(2, error, sizeof(error));
        exit(-1);
    }

    ptr = strtok(currLine, "\n");
    while (ptr != NULL)
    {
        fullConfig[i] = ptr;
        ptr = strtok(NULL, "\n");
        i++;
    }
    return fullConfig;
}

bool isCFile(char *file)
{
    char *ext = strrchr(file, '.');
    if ((ext != NULL) && (!strcmp(ext + 1, "c")))
    {
        return true;
    }
    return false;
}

char *findFileDotC(char *studentPath)
{
    DIR *pDirStudent;
    struct dirent *pDirent;

    if ((pDirStudent = opendir(studentPath)) == NULL)
    {
        char error[] = "could not open student dir\n";
        write(2, error, sizeof(error));
        exit(-1);
    }

    char *string = malloc(2048);

    while ((pDirent = readdir(pDirStudent)) != NULL)
    {
        if (isCFile(pDirent->d_name))
        {
            strcpy(string, studentPath);
            strcat(string, "/");
            strcat(string, pDirent->d_name);
            return string;
        }
    }

    return NULL;
}

int tryToCompile(const char *cPath, const char *compiledPath)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // This is the child process.

        int null_fd = open("/dev/null", O_WRONLY);
        dup2(null_fd, STDOUT_FILENO);
        dup2(null_fd, STDERR_FILENO);
        close(null_fd);

        execl("/usr/bin/gcc", "gcc", cPath, "-o", compiledPath, NULL);

        perror("could not fork\n"); // we're not supposed to still be here
        exit(-1);
    }
    else
    {
        // This is the parent process.
        // Wait for the child process to finish.
        int status;
        waitpid(pid, &status, 0);

        // Check the exit status of the child process.
        if (WEXITSTATUS(status) == 0)
        {
            // The child process compiled the C file successfully.
            return 1;
        }
        else
        {
            // The child process was unable to compile the C file.
            return 0;
        }
    }
}

int runCompiledFile(const char *compiledPath, const char *inputPath)
{
    char outputPath[PATH_MAX];
    strcpy(outputPath, compiledPath);
    strcat(outputPath, ".output");

    pid_t pid = fork();
    int output_fd = open(outputPath, O_WRONLY | O_CREAT, 0644);
//    int input_fd = open("/Users/adi/Desktop/adit/input.txt", O_RDONLY);


    if (pid == 0)
    {
        // This is the child process.

//        output_fd = open(outputPath, O_WRONLY | O_CREAT, 0644);
        if (output_fd < 0)
        {
            perror("could not pipe student output to file\n");
            exit(EXIT_FAILURE);
        }

        // Redirect stdout and stderr to the output file.
        dup2(output_fd, STDOUT_FILENO);
        dup2(output_fd, STDERR_FILENO);
        close(output_fd);

        // Redirect stdin to the input file.
        int input_fd = open(inputPath, O_RDONLY);
        if (input_fd < 0)
        {
            perror("could not pipe student input from file");
            exit(EXIT_FAILURE);
        }

        // Redirect stdin to the input file.
        dup2(input_fd, STDIN_FILENO);
        close(input_fd);

        execl(compiledPath, NULL);

        perror("could not fork\n"); // we're not supposed to still be here
        exit(-1);
    }
    else
    {
        // This is the parent process.
        // Wait for the child process to finish.
        int status;
        waitpid(pid, &status, 0);

        // return the exit status of the child process.
        return WEXITSTATUS(status);
    }
}

int giveGrade(const char *pathToFildDotC, const char *inputPath)
{
    if (!pathToFildDotC)
        return 0;

    char compiledPath[PATH_MAX];
    strcpy(compiledPath, pathToFildDotC);
    strcat(compiledPath, ".compiled");

    int didCompile = tryToCompile(pathToFildDotC, compiledPath);
    if (!didCompile)
        return 10;

    int runStatusCode = runCompiledFile(compiledPath, inputPath);

    if (runStatusCode == 0) return 100;

    return 49;
}

int main(int argc, char **argv)
{
    DIR *pDirStudents;
    struct dirent *pDirent;

    char *argv_1 = argv[1];

    char **fullConfig = readConfiguration(argv_1);
    char *studentsPath = fullConfig[0];

    const char* inputPath = fullConfig[1]; // fullConfig[1];

    char originalCwd[PATH_MAX];
    getcwd(originalCwd, sizeof(originalCwd));

    if ((pDirStudents = opendir(studentsPath)) == NULL)
    {
        char error[] = "could not open students dir\n";
        write(2, error, sizeof(error));
        exit(-1);
    }
    //open result file
    FILE *csv_file = fopen("result", "w");
    if (csv_file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    while ((pDirent = readdir(pDirStudents)) != NULL)
    {
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0 || strcmp(pDirent->d_name, ".DS_Store") == 0)
        {
            continue;
        }

        char userPath[1024];
        strcpy(userPath, studentsPath);
        strcat(userPath, "/");
        strcat(userPath, pDirent->d_name);

        //write the data to cvs:

        char *cFile = findFileDotC(userPath);
        int grade = giveGrade(cFile, inputPath);
         if (grade == 0){
             fprintf(csv_file, "%s,%d \n", pDirent->d_name, 0);
         }
        char *correctOutput = fullConfig[2];
        char *outputStudent = malloc(2048);
        strcpy(outputStudent, cFile);
        strcat(outputStudent,".compiled.output");
//        char* outputStudent = "/Users/adi/Desktop/adit/student/adi/main.c.compiled.output";
        int compere = compereFile(outputStudent,correctOutput);
        if (compere == 2) {
            fprintf(csv_file, "%s,%d \n", pDirent->d_name, 0);
        } else {
            fprintf(csv_file, "%s,%d \n", pDirent->d_name, 100);

        }

    }
}



