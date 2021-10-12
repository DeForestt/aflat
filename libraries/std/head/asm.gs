adr brk(adr size);
int sys_write(int file, adr text, int bytes);
int sys_create(adr path, int mode);
int sys_open(adr path, int flags, int mode);