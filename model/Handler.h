#ifndef HANDLEFILE_HANDLE_H
#define HANDLEFILE_HANDLE_H

#include <unistd.h>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <string.h>

#define CHECK_USER 1
#define CHECK_FILE 2

using namespace std;

namespace SHDocument {
    class Handler {
    private:
        void createFolderSHDocument();
        Handler();

    public:
        static Handler* getInstance();

        static bool isExistFile(string filename);
        static bool isExistAccout(string userName);
        static bool isExistDirectory(string pathDiractory);

        string signIn(char *username, char *password);
        string signUp(char *username, char *password);
        string checkData(int checkOption, char *dataCheck);
        string downloadFile(char *filename);
        string uploadFile(char *username, char *filename, char *content);
    };
}
#endif //HANDLEFILE_HANDLE_H
