#include <signal.h>
#include <unistd.h>
#ifdef __APPLE__
#include <execinfo.h>
#endif

#ifdef DEBUG
static void segv_handler(int sig)
{
#ifdef __APPLE__
    void *bt[64];
    int n = backtrace(bt, 64);
    backtrace_symbols_fd(bt, n, STDERR_FILENO);
#else
    const char msg[] = "Segmentation fault\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
#endif
    _exit(128 + sig);
}

void start_debug_segv_backtrace(void)
{
    signal(SIGSEGV, segv_handler);
}
#endif
