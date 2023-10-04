# Testing system calls

## Running Tests for getreadcount

Running tests for this syscall is easy. Just do the following from
inside the `initial-xv6` directory:

```sh
prompt> ./test-getreadcounts.sh
```

If you implemented things correctly, you should get some notification
that the tests passed. If not ...

The tests assume that xv6 source code is found in the `src/` subdirectory.
If it's not there, the script will complain.

The test script does a one-time clean build of your xv6 source code
using a newly generated makefile called `Makefile.test`. You can use
this when debugging (assuming you ever make mistakes, that is), e.g.:

```sh
prompt> cd src/
prompt> make -f Makefile.test qemu-nox
```

You can suppress the repeated building of xv6 in the tests with the
`-s` flag. This should make repeated testing faster:

```sh
prompt> ./test-getreadcounts.sh -s
```

### Implementing `getreadcount`

- in `kernel/syscall.h`
    add the following entry
    ```
    #define SYS_getreadcount 22
    ```

- in `kernel/syscall.c`
    - add the following line to the `syscall` array
        ```
        [SYS_getreadcount] sys_getreadcount,
        ```
    - add the following line
        ```
        extern int sys_getreadcount(void);
        ```

- in `kernel/proc.h`
    add a new attribute `readcount` to the `proc` structure
    ```
    int readcount;        // number of times sys call read has been called
    ```

- in `kernel/sysproc.c`
    add the following function
    ```
    int
    sys_getreadcount(void)
    {
        return myproc()->readcount;
    }
    ```

- in `kernel/proc.c`
    initialise the `readcount` attribute of the `proc` structure to 0 in the `allocproc` and `freeproc` functions
    ```
    p->readcount = 0;
    ```

- in `kernel/syscall.c`
    - declare and initialise a variable `readcount`
        ```
        int readcount = 0;
        ```
    - if the sys call `read` is called, update the readcount variable and if the sys call `getreadcount` is called, update the readcount attribute of the process
        ```
        if(num==SYS_read)
            readcount++;
        if(num==SYS_getreadcount)
            p->readcount = readcount;
        ```

- in `user/user.h`
    add the following function definition
    ```
    int getreadcount(void);
    ```

- in `user/usys.pl`
    added the following entry
    ```
    entry("getreadcount");
    ```


---

## Running Tests for sigalarm and sigreturn

**After implementing both sigalarm and sigreturn**, do the following:
- Make the entry for `alarmtest` in `src/Makefile` inside `UPROGS`
- Run the command inside xv6:
    ```sh
    prompt> alarmtest
    ```

### Implementing `sigalarm` and `sigreturn`

- in `kernel/syscall.h`
    add the following entry
    ```
    #define SYS_sigalarm 24
    #define SYS_sigreturn 25
    ```

- in `kernel/syscall.c`
    - add the following line
        ```
        extern uint64 sys_sigalarm(void);
        extern uint64 sys_sigreturn(void);
        ```
    - add the following line to the `syscall` array
        ```
        [SYS_sigalarm] sys_sigalarm,
        [SYS_sigreturn] sys_sigreturn,
        ```

- in `user/user.h`
    add the following function definition
    ```
    int sigalarm(int, void* func);
    int sigreturn(void);
    ```

- in `user/usys.pl`
    added the following entry
    ```
    entry("sigalarm");
    entry("sigreturn");
    ```

- in `kernel/proc.h`
    add the following required new attributes to the `proc` structure
    ```
    int numticks;                    // number of ticks the process has run for
    int alarmflag;                   // flag to check if alarm is set
    int interval;                    // number of ticks after which alarm handler is to be called
    uint64 handler;                  // function pointer for alarm handler
    struct trapframe *trapframecopy; // copy of trapframe for sigreturn
    ```

- in `kernel/proc.c`
    - update the `allocproc` function to initialise the new attributes
        ```
        p->numticks = 0;
        p->alarmflag = 0;
        p->interval = 0;
        p->handler = 0;
        ```
    - updated the `freeproc` function as well

- in `kernel/sysproc.c`
    add the `sys_sigalarm` and `sys_sigreturn` functions
    ```
    int
    sys_sigalarm(void)
    {
        int interval;
        argint(0, &interval);

        uint64 handler;
        argaddr(1, &handler);

        struct proc *p = myproc();
        p->numticks = 0;
        p->interval = interval;
        p->handler = handler;

        return 0;
    }

    int
    sys_sigreturn(void)
    {
        struct proc *p = myproc();
        
        memmove(p->trapframe, p->trapframecopy, sizeof(struct trapframe));
        p->alarmflag = 0;
        
        return 0;
    }
    ```

- in `kernel/syscall.c`
    the return value of `sigreturn` gets stored in `trapframe->a0` and so here, its value is being restored
    ```
    if(num==SYS_sigreturn)
    {
        // restore a0
        p->trapframe->a0 = p->trapframecopy->a0;
        kfree((void*) p->trapframecopy);
    }
    ```

- in `kernel/trap.c`
    added code to update the number of ticks and handle accordingly
    ```
    // give up the CPU if this is a timer interrupt.
    if (which_dev == 2)
    {
        if (p->interval > 0)
        {
            if(p->alarmflag==0)
            {
                p->numticks++;
                if (p->numticks == p->interval)
                {
                    p->numticks = 0;
                    p->alarmflag = 1;
                    p->trapframecopy = (struct trapframe *)kalloc();
                    *(p->trapframecopy) = *(p->trapframe);
                    p->trapframe->epc = p->handler;
                }
            }
        }
        yield();
    }
    ```
    note: `which_dev` being 2 indicates timer interrupt

---

## Getting runtimes and waittimes for your schedulers
- Run the following command in xv6:
    ```sh
    prompt> schedulertest
    ```  
---

## Running tests for entire xv6 OS
- Run the following command in xv6:
    ```sh
    prompt> usertests
    ```

---