#define NMLFQ 4
#define AGEWHEN 20

struct mlfqueue
{
    int npq[NMLFQ];                 // number of processes in each queue
    struct proc *pqs[NMLFQ][NPROC]; // the queues
};

void push(int q, struct proc *p);
struct proc *pop(int q);
void remove(int q, struct proc *p);

extern struct mlfqueue mlfq;

void initmlfq();