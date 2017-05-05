#ifndef __SYSTEM_CALL__
#define __SYSTEM_CALL__

#include <string.h>
#include <sys/syscall.h>

#include "logger.h"
#include "common.h"

int syscall_table[1000] = {0};


#if __WORDSIZE == 32    //x86_32

//c or c++
int C[1000] = 
{
    SYS_access,         -1,
    SYS_brk,            -1,
    SYS_close,          -1,
    SYS_execve,          1,
    SYS_exit_group,     -1,
    SYS_fstat64,        -1,
    SYS_futex,          -1,
    SYS_gettimeofday,   -1,
    SYS_mmap2,          -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_read,           -1,
    SYS_set_thread_area,-1,
    SYS_uname,          -1,
    SYS_write,          -1,
    SYS_writev,         -1,
    -1
};

int CPP[1000] =
{
    SYS_access,         -1,
    SYS_brk,            -1,
    SYS_close,          -1,
    SYS_execve,         1,
    SYS_exit_group,     -1,
    SYS_fstat64,        -1,
    SYS_futex,          -1,
    SYS_gettimeofday,   -1,
    SYS_mmap2,          -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_read,           -1,
    SYS_set_thread_area,-1,
    SYS_uname,          -1,
    SYS_write,          -1,
    SYS_writev,         -1,
    -1
};

int JAVA[1000] =
{
    SYS_access,         -1,
    SYS_brk,            -1,
    SYS_clone,          -1,
    SYS_close,          -1,
    SYS_execve,         -1,
    SYS_exit,           -1,
    SYS_exit_group,     -1,
    SYS_fcntl64,        -1,
    SYS_fstat64,        -1,
    SYS_futex,          -1,
    SYS_getdents64,     -1,
    SYS_getegid32,      -1,
    SYS_geteuid32,      -1,
    SYS_getgid32,       -1,
    SYS_getrlimit,      -1,
    SYS_gettimeofday,   -1,
    SYS_getuid32,       -1,
    SYS_mmap,           -1,
    SYS_mmap2,          -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_open,           -1,
    SYS_read,           -1,
    SYS_readlink,       -1,
    SYS_rt_sigaction,   -1,
    SYS_rt_sigprocmask, -1,
    SYS_set_robust_list,-1,
    SYS_set_thread_area,-1,
    SYS_set_tid_address,-1,
    SYS_sigprocmask,    -1,
    SYS_stat64,         -1,
    SYS_ugetrlimit,     -1,
    SYS_uname,          -1,
    -1
};

int PYTHON[1000] = 
{
    SYS_access,         -1,
    SYS_arch_prctl,     -1,
    SYS_brk,            -1,
    SYS_clone,          -1,
    SYS_close,          -1,
    SYS_execve,         -1,
    SYS_exit_group,     -1,
    SYS_fstat,          -1,
    SYS_futex,          -1,
    SYS_getegid,        -1,
    SYS_geteuid,        -1,
    SYS_getgid,         -1,
    SYS_getrlimit,      -1,
    SYS_gettimeofday,   -1,
    SYS_getuid,         -1,
    SYS_mmap,           -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_open,           -1,
    SYS_read,           -1,
    SYS_readlink,       -1,
    SYS_rt_sigaction,   -1,
    SYS_rt_sigprocmask, -1,
    SYS_set_robust_list,-1,
    SYS_set_tid_address,-1,
    SYS_stat,           -1,
    SYS_uname,          -1,
    SYS_write,          -1,
    SYS_writev,         -1,
    SYS_time,           -1,
    SYS_readlink,       -1,
    -1
};

#elif defined __x86_64__
int C[1000] =
{
    SYS_access,         -1,
    SYS_arch_prctl,     -1,
    SYS_brk,            -1,
    SYS_close,          -1,
    SYS_execve,          1,
    SYS_exit_group,     -1,
    SYS_fstat,          -1,
    SYS_futex,          -1,
    SYS_gettimeofday,   -1,
    SYS_mmap,           -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_read,           -1,
    SYS_set_thread_area,-1,
    SYS_uname,          -1,
    SYS_write,          -1,
    SYS_writev,         -1,
    SYS_time,           -1,
    SYS_readlink,       -1,
    -1
};

int CPP[1000] =
{
    SYS_access,         -1,
    SYS_arch_prctl,     -1,
    SYS_brk,            -1,
    SYS_close,          -1,
    SYS_execve,          1,
    SYS_exit_group,     -1,
    SYS_fstat,          -1,
    SYS_futex,          -1,
    SYS_gettimeofday,   -1,
    SYS_mmap,           -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_read,           -1,
    SYS_set_thread_area,-1,
    SYS_uname,          -1,
    SYS_write,          -1,
    SYS_writev,         -1,
    SYS_time,           -1,
    SYS_readlink,       -1,
    -1
};

int JAVA[1000] =
{
    SYS_access,         -1,
    SYS_arch_prctl,     -1,
    SYS_brk,            -1,
    SYS_clone,          -1,
    SYS_close,          -1,
    SYS_execve,         -1,
    SYS_exit_group,     -1,
    SYS_fstat,          -1,
    SYS_futex,          -1,
    SYS_getegid,        -1,
    SYS_geteuid,        -1,
    SYS_getgid,         -1,
    SYS_getrlimit,      -1,
    SYS_gettimeofday,   -1,
    SYS_getuid,         -1,
    SYS_mmap,           -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_open,           -1,
    SYS_read,           -1,
    SYS_readlink,       -1,
    SYS_rt_sigaction,   -1,
    SYS_rt_sigprocmask, -1,
    SYS_set_robust_list,-1,
    SYS_set_tid_address,-1,
    SYS_stat,           -1,
    SYS_uname,          -1,
    SYS_write,          -1,
    SYS_writev,         -1,
    SYS_time,           -1,
    SYS_readlink,       -1,
    -1
};

int PYTHON[1000] = 
{
    SYS_access,         -1,
    SYS_arch_prctl,     -1,
    SYS_brk,            -1,
    SYS_clone,          -1,
    SYS_close,          -1,
    SYS_execve,         -1,
    SYS_exit_group,     -1,
    SYS_fstat,          -1,
    SYS_futex,          -1,
    SYS_getegid,        -1,
    SYS_geteuid,        -1,
    SYS_getgid,         -1,
    SYS_getrlimit,      -1,
    SYS_gettimeofday,   -1,
    SYS_getuid,         -1,
    SYS_mmap,           -1,
    SYS_mremap,         -1,
    SYS_mprotect,       -1,
    SYS_munmap,         -1,
    SYS_lseek,          -1,
    SYS_open,           -1,
    SYS_read,           -1,
    SYS_readlink,       -1,
    SYS_rt_sigaction,   -1,
    SYS_rt_sigprocmask, -1,
    SYS_set_robust_list,-1,
    SYS_set_tid_address,-1,
    SYS_stat,           -1,
    SYS_uname,          -1,
    SYS_write,          -1,
    SYS_writev,         -1,
    SYS_time,           -1,
    SYS_readlink,       -1,
    -1
};
#endif


void init_syscall_table (int lang) {
    int *arr = NULL;
    switch (lang) {
        case LANG::C:
            arr = C;
            break;
        case LANG::CPP:
            arr = CPP;
            break;
        case LANG::JAVA:
            arr = JAVA;
            break;
        case LANG::PYTHON:
        	arr = PYTHON;
         	break;
        default:
            LOG_WARNING("Unknown language: %d", lang);
            break;
    }
    memset(syscall_table, 0, sizeof(syscall_table));
    for (int i = 0; arr[i] >= 0; i += 2) {
        syscall_table[arr[i]] = arr[i + 1];
    }
}

#endif