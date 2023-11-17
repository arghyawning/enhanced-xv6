#ifdef MLFQ
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "./mlfq.h"

struct mlfqueue mlfq;

void initmlfq()
{
    for (int i = 0; i < NMLFQ; i++)
    {
        mlfq.npq[i] = 0;

        for (int j = 0; j < NPROC; j++)
            mlfq.pqs[i][j] = 0;
    }
}

void push(int q, struct proc *p)
{
    if (mlfq.npq[q] == NPROC)
        panic("MLFQ is full");

    mlfq.pqs[q][mlfq.npq[q]] = p;
    mlfq.npq[q]++;

    p->qpresent = 1;
    p->qno = q;
    p->qwaittime = 0;
    p->runtime = 0;
}

struct proc *pop(int q)
{
    if (mlfq.npq[q] == 0)
        panic("MLFQ is empty");

    struct proc *p = mlfq.pqs[q][0];

    for (int i = 0; i < mlfq.npq[q] - 1; i++)
        mlfq.pqs[q][i] = mlfq.pqs[q][i + 1];

    mlfq.pqs[q][mlfq.npq[q] - 1] = 0;
    mlfq.npq[q]--;

    p->qpresent = 0;
    p->runtime = 0;
    return p;
}

void remove(int q, struct proc *p)
{
    if (mlfq.npq[q] == 0)
    {
        panic("MLFQ is empty");
        return;
    }

    int i;
    for (i = 0; i < mlfq.npq[q]; i++)
    {
        if (mlfq.pqs[q][i] == p)
            break;
    }

    if (i == mlfq.npq[q])
        panic("Process not found in MLFQ");

    for (; i < mlfq.npq[q] - 1; i++)
        mlfq.pqs[q][i] = mlfq.pqs[q][i + 1];

    mlfq.pqs[q][mlfq.npq[q] - 1] = 0;
    mlfq.npq[q]--;
    p->qpresent = 0;
}

#endif