#include <stdio.h>

#define N 4
#define QUANTUM 2

struct Process
{
    int pid;
    int arrival;
    int burst;
    int priority;

    int remaining;
    int completion;
    int waiting;
    int turnaround;
    int response;

    int first_start;
};

/* Original process data */
struct Process original[N] =
{
    {1, 0, 8, 2},
    {2, 0, 5, 1},
    {3, 1, 6, 3},
    {4, 2, 4, 2}
};


/* ---------------------------------------------------
   ROUND ROBIN SCHEDULING
   --------------------------------------------------- */

void roundRobin()
{
    struct Process p[N];

    int i;

    for (i = 0; i < N; i++)
    {
        p[i] = original[i];
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        p[i].first_start = -1;
    }

    int queue[100];
    int front = 0;
    int rear = 0;

    int added[N] = {0};

    int time = 0;
    int completed = 0;
    int contextSwitches = 0;

    int current = -1;

    printf("\n====================================\n");
    printf("     NORMAL ROUND ROBIN\n");
    printf("====================================\n");

    while (completed < N)
    {
        /* Add newly arrived processes */
        for (i = 0; i < N; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }

        /* If queue is empty, CPU is idle */
        if (front == rear)
        {
            time++;
            continue;
        }

        int index = queue[front++];

        /* Context switch */
        if (current != -1 && current != index)
        {
            contextSwitches++;

            printf("Context Switch: P%d -> P%d\n",
                   p[current].pid,
                   p[index].pid);
        }

        current = index;

        /* First response time */
        if (p[index].first_start == -1)
        {
            p[index].first_start = time;
        }

        printf("Running P%d from %d to ",
               p[index].pid, time);

        int run;

        if (p[index].remaining > QUANTUM)
            run = QUANTUM;
        else
            run = p[index].remaining;

        time += run;
        p[index].remaining -= run;

        printf("%d\n", time);

        /* Add processes which arrived during execution */
        for (i = 0; i < N; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }

        /* Process finished */
        if (p[index].remaining == 0)
        {
            p[index].completion = time;
            completed++;
        }
        else
        {
            /* Put process back into ready queue */
            queue[rear++] = index;
        }
    }

    double avgWaiting = 0;
    double avgTurnaround = 0;
    double avgResponse = 0;

    printf("\nProcess Results:\n");

    printf("PID\tWaiting\tTurnaround\tResponse\n");

    for (i = 0; i < N; i++)
    {
        p[i].turnaround =
            p[i].completion - p[i].arrival;

        p[i].waiting =
            p[i].turnaround - p[i].burst;

        p[i].response =
            p[i].first_start - p[i].arrival;

        avgWaiting += p[i].waiting;
        avgTurnaround += p[i].turnaround;
        avgResponse += p[i].response;

        printf("P%d\t%d\t%d\t\t%d\n",
               p[i].pid,
               p[i].waiting,
               p[i].turnaround,
               p[i].response);
    }

    avgWaiting /= N;
    avgTurnaround /= N;
    avgResponse /= N;

    printf("\n------------------------------------\n");
    printf("Total Context Switches : %d\n",
           contextSwitches);

    printf("Average Waiting Time   : %.2f ms\n",
           avgWaiting);

    printf("Average Turnaround Time : %.2f ms\n",
           avgTurnaround);

    printf("Average Response Time  : %.2f ms\n",
           avgResponse);

    printf("CPU Utilization        : 100%%\n");
}


/* ---------------------------------------------------
   INTELLIGENT CONTEXT SWITCHING
   --------------------------------------------------- */

void intelligentScheduling()
{
    struct Process p[N];

    int i;

    for (i = 0; i < N; i++)
    {
        p[i] = original[i];
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        p[i].first_start = -1;
    }

    int queue[100];
    int front = 0;
    int rear = 0;

    int added[N] = {0};

    int time = 0;
    int completed = 0;
    int contextSwitches = 0;

    int current = -1;

    printf("\n====================================\n");
    printf("   INTELLIGENT CONTEXT SWITCHING\n");
    printf("====================================\n");

    while (completed < N)
    {
        /* Add newly arrived processes */
        for (i = 0; i < N; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }

        if (current == -1)
        {
            if (front == rear)
            {
                time++;
                continue;
            }

            current = queue[front++];

            if (p[current].first_start == -1)
            {
                p[current].first_start = time;
            }
        }

        printf("Running P%d from %d to ",
               p[current].pid, time);

        int run;

        if (p[current].remaining > QUANTUM)
            run = QUANTUM;
        else
            run = p[current].remaining;

        time += run;
        p[current].remaining -= run;

        printf("%d\n", time);

        /* Add newly arrived processes */
        for (i = 0; i < N; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }

        /* Current process finished */
        if (p[current].remaining == 0)
        {
            p[current].completion = time;
            completed++;

            if (front < rear)
            {
                int next = queue[front++];

                contextSwitches++;

                printf("Context Switch: P%d -> P%d\n",
                       p[current].pid,
                       p[next].pid);

                current = next;

                if (p[current].first_start == -1)
                {
                    p[current].first_start = time;
                }
            }
            else
            {
                current = -1;
            }

            continue;
        }


        /*
           INTELLIGENT DECISION

           If the current process can finish within
           the next quantum, allow it to continue
           instead of immediately switching.

           However, if a higher-priority process is
           waiting, perform the switch.
        */

        int allowContinue = 0;

        if (p[current].remaining <= QUANTUM)
        {
            allowContinue = 1;

            /* Check for higher-priority process */
            for (i = front; i < rear; i++)
            {
                int waitingProcess = queue[i];

                if (p[waitingProcess].priority
                    < p[current].priority)
                {
                    allowContinue = 0;
                    break;
                }
            }
        }

        if (allowContinue)
        {
            printf("Decision: Continue P%d "
                   "(avoiding unnecessary switch)\n",
                   p[current].pid);

            continue;
        }


        /* Normal scheduling switch */

        if (front < rear)
        {
            queue[rear++] = current;

            int next = queue[front++];

            contextSwitches++;

            printf("Context Switch: P%d -> P%d\n",
                   p[current].pid,
                   p[next].pid);

            current = next;

            if (p[current].first_start == -1)
            {
                p[current].first_start = time;
            }
        }
    }


    double avgWaiting = 0;
    double avgTurnaround = 0;
    double avgResponse = 0;

    printf("\nProcess Results:\n");

    printf("PID\tWaiting\tTurnaround\tResponse\n");

    for (i = 0; i < N; i++)
    {
        p[i].turnaround =
            p[i].completion - p[i].arrival;

        p[i].waiting =
            p[i].turnaround - p[i].burst;

        p[i].response =
            p[i].first_start - p[i].arrival;

        avgWaiting += p[i].waiting;
        avgTurnaround += p[i].turnaround;
        avgResponse += p[i].response;

        printf("P%d\t%d\t%d\t\t%d\n",
               p[i].pid,
               p[i].waiting,
               p[i].turnaround,
               p[i].response);
    }

    avgWaiting /= N;
    avgTurnaround /= N;
    avgResponse /= N;

    printf("\n------------------------------------\n");
    printf("Total Context Switches : %d\n",
           contextSwitches);

    printf("Average Waiting Time   : %.2f ms\n",
           avgWaiting);

    printf("Average Turnaround Time : %.2f ms\n",
           avgTurnaround);

    printf("Average Response Time  : %.2f ms\n",
           avgResponse);

    printf("CPU Utilization        : 100%%\n");
}


/* ---------------------------------------------------
   MAIN
   --------------------------------------------------- */

int main()
{
    printf("\n");
    printf("=================================================\n");
    printf(" INTELLIGENT CONTEXT SWITCHING CASE STUDY\n");
    printf("=================================================\n");

    printf("\nProcesses Used:\n");

    printf("P1: Arrival = 0, Burst = 8, Priority = 2\n");
    printf("P2: Arrival = 0, Burst = 5, Priority = 1\n");
    printf("P3: Arrival = 1, Burst = 6, Priority = 3\n");
    printf("P4: Arrival = 2, Burst = 4, Priority = 2\n");

    printf("\nTime Quantum = %d ms\n", QUANTUM);

    roundRobin();

    intelligentScheduling();

    printf("\n=================================================\n");
    printf("             END OF EXPERIMENT\n");
    printf("=================================================\n");

    return 0;
}