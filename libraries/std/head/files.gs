class File{
    adr fileName;
    int file;

    int write(adr text);
    int read(adr pointer, char delim, char term);
    int toArray(char delim, char term);
    int close();
};

File createFile(adr fileName);
File openFile(adr fileName);