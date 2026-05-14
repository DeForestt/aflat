import task from "Utils/Task";

types(T)
task::<T> spawn(const adr imp_pointer);

void run();

types(T)
T blockOn(const task::<T> job);

any __runtimeGetJobs__();
