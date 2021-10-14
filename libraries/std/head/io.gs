struct FileInfo{
    int deviceId;
    int iNodeNum;
    int mode;
    int nLink;
    int uid;
    int gid;
    int sDeviceId;
    int size;
    int blkSize;
    int blocks;
    int atime;
    int mtime;
    int ctime;
};

FileInfo getFileInfo(int file);

struct File{
    FileInfo info;
    adr fileName;
    int file;
};
File createFile(adr fileName);
File openFile(adr fileName);


int print(adr input);
int readString(adr pointer);
int pullInt(adr prompt, int sigfigs);
int printInt(int input);
int writeTo(File file, adr text);
int readFrom(File file, adr pointer, char delim, char term);
int closFile(File file);
int toInt(char input);
char toChar(int input);

