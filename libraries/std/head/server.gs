int serve(int port, adr handler, * adr data);
int serve_pool(int port, adr handler, * adr data, int workerCount);
int af_server_default_worker_count();
int serve_sync(int port, adr handler, * adr data);
