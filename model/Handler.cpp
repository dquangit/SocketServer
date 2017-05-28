#include "Handler.h"

using namespace SHDocument;

static Handler* instance;

Handler::Handler() {
    this->createFolderSHDocument();
}

// Check file
bool Handler::isExistFile(string target) {
    ifstream file(target.c_str());
    return file.good();
}

// Check dir
bool Handler::isExistDirectory(string pathDiractory) {
    DIR *SHDocment_Server = opendir(pathDiractory.c_str());
    if (SHDocment_Server) {
        closedir(SHDocment_Server);
        return true;
    }
    closedir(SHDocment_Server);
    return false;
}

//Check user is exist
bool Handler::isExistAccout(string userName) {
    return isExistFile(userName);
}

// If folder SHDocument_Server not exist then create this folder
void Handler:: createFolderSHDocument() {
    chdir("./");
    if (!isExistDirectory("SHDocument_Server")) {
        mkdir("SHDocument_Server", 0);
        system("chmod 775 SHDocument_Server/");
        chdir("SHDocument_Server");
        mkdir("ClientManagement", 0);
        system("chmod 775 ClientManagement/");
        mkdir("DocumentManagement", 0);
        system("chmod 775 DocumentManagement/");
        system("ls");
    }
}

// Sign up to new user
string Handler::signUp(char *username, char *password) {
    chdir("SHDocument_Server/ClientManagement/");
    string user = username;
    string userFile = user + ".txt";
    if (!Handler::isExistFile(userFile)) {
        std::ofstream outfile;
        outfile.open(userFile);
        outfile << password << std::endl;
        outfile.close();
        chdir("SHDocument_Server/DocumentManagement/");
        string nameFolder = "mkdir " + user;
        cout << nameFolder << endl;
        system(nameFolder.c_str());
        cout << "dang ky thanh cong\n";
        return "{\"status\":\"success\",\"message\":\"Dang ki thanh cong...\"}";
    }
    cout << "tai khoan ton tai\n";
    return "{\"status\":\"unsuccessful\",\"message\":\"Tai khoan nay da ton tai...\"}";
}

// Sign in
string Handler::signIn(char *username, char *password) {
    chdir("SHDocument_Server/ClientManagement");
    string user = username;
    string userFile = user + ".txt";
    if (!isExistFile(userFile)) {
        return "{\"status\":\"No user\",\"message\":\"Tai khoan khong ton tai...\"}";
    }
    ifstream fileUser;
    fileUser.open(userFile);
    string passwordGetOfFile;
    getline(fileUser, passwordGetOfFile);
    fileUser.close();
    if (passwordGetOfFile.compare(password) != 0) {
        return "{\"status\":\"Wrong password\",\"message\":\"Sai username hoac password...\"}";
    }
    return "{\"status\":\"success\",\"message\":\"Dang nhap thanh cong...\"}";
}

/**
 * Read all content
 *
 * @param pFile
 * @return string
 */
string readAllFile(ifstream &pFile) {
    pFile.clear();
    pFile.seekg(0);
    string lineOfFile;
    string result ( (std::istreambuf_iterator<char>(pFile) ),
                    (std::istreambuf_iterator<char>()    ) );

    return result;
}

/**
 * dataCheck[0] la kieu check
 * dataCheck[1] la file hoac nguoi dung de check
 *
 * @param dataCheck
 * @return
 */
string Handler::checkData(int checkOption, char *dataCheck) {
    chdir("SHDocument_Server/ClientManagement/");
    if (checkOption == CHECK_USER) {
        ifstream userFile;
        string user = dataCheck;
        string nameUserFile = user + ".txt";
        userFile.open(nameUserFile);

        if (!userFile.good()) {
            return "{\"status\":\"unsuccessful\",\"message\":\"Dang nhap thanh cong...\"}";
        }

        string lineOfFileUser;
        getline(userFile, lineOfFileUser);
        string listFile;
        listFile = "";

        while((getline(userFile, lineOfFileUser))) {
            listFile.append(lineOfFileUser);
            listFile.append("#");
        }

        listFile.substr(listFile.size() - 2, 1);
        userFile.close();
        if (listFile != "") {
            string result = "{\"status\":\"ListFileOfUser\",\"message\":\"";
            result += listFile + "\"}";
            result.c_str();
        }
        return "{\"status\":\"NoFileUpload\",\"message\":\"User nay chua upload file...\"}";
    }
    if (checkOption == CHECK_FILE) {
        ifstream file;
        DIR *pDir;
        struct dirent *ent;
        if ((pDir = opendir("SHDocument_Server/ClientManagement/")) != NULL) {
            while ((ent = readdir(pDir)) != NULL) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    file.open(ent->d_name);
                    string lineOfFile;
                    getline(file, lineOfFile);

                    while ((getline(file, lineOfFile))) {
                        if (lineOfFile == dataCheck) {
                            return "{\"status\":\"HaveFile\",\"message\":\"File nay ton tai tren he thong...\"}";
                        }
                    }
                }
            }
            return "{\"status\":\"DontHaveFile\",\"message\":\"Khong co file nay tren he thong...\"}";
        }
    }

    return "{\"status\":\"SERVER_ERROR\",\"massage\":\"SERVER_ERROR\"}";
}

string Handler::downloadFile(char *filename) {

}

string Handler::uploadFile(char *username, char *filename, char *content) {
    string path = "./SHDocument_Server/DocumentManagement/";
    path += username;
    chdir(path.c_str());

    ofstream fileUpload;
    fileUpload.open(filename);
    string contentToWrite = content;

    fileUpload << contentToWrite << endl;

    fileUpload.close();
    return "{\"status\":\"success\",\"message\":\"Ghi file thanh cong..\"}";
}

Handler* Handler::getInstance() {
    if (instance == NULL) {
        instance = new Handler();
    }
    return instance;
}

