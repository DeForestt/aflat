int beginTimer(int milliseconds);
int af_task_status(adr task);
void af_task_cancel(adr task);
void af_task_detach(adr task);
adr af_task_current_waker();
void af_task_wake(adr task);
