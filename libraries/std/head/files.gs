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

class File{
    FileInfo info;
    adr fileName;
    int file;

    int write(adr text);
    int read(adr pointer, char delim, char term);
    FileInfo getFileInfo();
    int close();
};

File createFile(adr fileName);
File openFile(adr fileName);