.needs <std>

class fstat{
    long st_dev;
    long st_ino;
    int st_mode;
    char 4 alligner;
    long st_nlink;
    int st_uid;
    int st_gid;
    long st_rdev;
    long st_size;
    long st_blksize;
    long st_blocks;
    Times st_atime;
    Times st_mtime;
    Times st_ctime;
};

class File{
    adr fileName;
    int file;

    int write(adr text);
    int read(adr pointer, char delim, char term);
    adr toArray(char delim, char term);
    adr toList(char delim, char term);
    int close();
};

File createFile(adr fileName);
File openFile(adr fileName);