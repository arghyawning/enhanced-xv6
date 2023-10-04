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

- in `kernel/proc.h`
    add a new attribute `readcount` to the `proc` structure
    ```
    struct proc
    {
    struct spinlock lock;

    // p->lock must be held when using these:
    enum procstate state; // Process state
    void *chan;           // If non-zero, sleeping on chan
    int killed;           // If non-zero, have been killed
    int xstate;           // Exit status to be returned to parent's wait
    int pid;              // Process ID
    int readcount;


    // wait_lock must be held when using this:
    struct proc *parent; // Parent process
    .
    .
    .
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
    initialise the `readcount` attribute of the `proc` structure to 0
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