#include <stdio.h>
#define MAX 20
#define SIZE 10000

struct Process
{
    int pid, arrival, burst, priority;
    int remaining, completion, waiting, turnaround, response, first_start;
};

void inputProcesses(struct Process p[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        p[i].pid = i + 1;
        printf("\nProcess P%d\n", i + 1);
        printf("Arrival Time: ");
        if (scanf("%d", &p[i].arrival) != 1 || p[i].arrival < 0)
        {
            printf("Invalid arrival time.\n");
            return;
        }
        printf("Burst Time: ");
        if (scanf("%d", &p[i].burst) != 1 || p[i].burst <= 0)
        {
            printf("Invalid burst time.\n");
            return;
        }
        printf("Priority: ");
        if (scanf("%d", &p[i].priority) != 1)
        {
            printf("Invalid priority.\n");
            return;
        }
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        p[i].first_start = -1;
    }
}

void resetProcesses(struct Process p[], struct Process original[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        p[i] = original[i];
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        p[i].waiting = 0;
        p[i].turnaround = 0;
        p[i].response = 0;
        p[i].first_start = -1;
    }
}

int completed(struct Process p[], int n)
{
    int i;
    for (i = 0; i < n; i++)
        if (p[i].remaining > 0)
            return 0;
    return 1;
}

void printGantt(int pid[], int start[], int end[], int count)
{
    int i;
    printf("\nGANTT CHART\n");
    for (i = 0; i < count; i++)
        printf("--------");
    printf("-\n");
    for (i = 0; i < count; i++)
        printf("|  P%d  ", pid[i]);
    printf("|\n");
    for (i = 0; i < count; i++)
        printf("--------");
    printf("-\n");
    if (count > 0)
        printf("%-7d", start[0]);
    for (i = 0; i < count; i++)
        printf("%-8d", end[i]);
    printf("\n");
}

void display(struct Process p[], int n, int switches, char title[])
{
    int i;
    float wt = 0, tat = 0, rt = 0;
    printf("\n========== %s ==========\n", title);
    printf("PID\tAT\tBT\tPriority\tCT\tWT\tTAT\tRT\n");
    for (i = 0; i < n; i++)
    {
        p[i].turnaround = p[i].completion - p[i].arrival;
        p[i].waiting = p[i].turnaround - p[i].burst;
        p[i].response = p[i].first_start - p[i].arrival;
        printf("P%d\t%d\t%d\t%d\t\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].arrival, p[i].burst, p[i].priority,
               p[i].completion, p[i].waiting, p[i].turnaround,
               p[i].response);
        wt += p[i].waiting;
        tat += p[i].turnaround;
        rt += p[i].response;
    }
    printf("\nContext Switches: %d\n", switches);
    printf("Average Waiting Time: %.2f ms\n", wt / n);
    printf("Average Turnaround Time: %.2f ms\n", tat / n);
    printf("Average Response Time: %.2f ms\n", rt / n);
    printf("CPU Utilization: 100%%\n");
}

void roundRobin(struct Process original[], int n, int quantum)
{
    struct Process p[MAX];
    int queue[SIZE], added[MAX] = {0};
    int pid[SIZE], start[SIZE], end[SIZE], count = 0;
    int front = 0, rear = 0, time = 0, switches = 0;
    int i, current, run, last = -1;

    resetProcesses(p, original, n);
    printf("\n========================================\n");
    printf("ROUND ROBIN SCHEDULING\n");
    printf("========================================\n");

    while (!completed(p, n))
    {
        for (i = 0; i < n; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }
        if (front == rear)
        {
            time++;
            continue;
        }
        current = queue[front++];
        if (last != -1 && last != current)
        {
            switches++;
            printf("Context Switch: P%d -> P%d\n",
                   p[last].pid, p[current].pid);
        }
        last = current;
        if (p[current].first_start == -1)
            p[current].first_start = time;
        run = p[current].remaining < quantum ?
              p[current].remaining : quantum;
        printf("Running P%d from %d to %d\n",
               p[current].pid, time, time + run);
        pid[count] = p[current].pid;
        start[count] = time;
        end[count] = time + run;
        count++;
        time += run;
        p[current].remaining -= run;
        for (i = 0; i < n; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }
        if (p[current].remaining > 0)
            queue[rear++] = current;
        else
            p[current].completion = time;
    }
    printGantt(pid, start, end, count);
    display(p, n, switches, "ROUND ROBIN RESULTS");
}

void intelligentScheduling(struct Process original[], int n, int quantum)
{
    struct Process p[MAX];
    int queue[SIZE], added[MAX] = {0};
    int pid[SIZE], start[SIZE], end[SIZE], count = 0;
    int front = 0, rear = 0, time = 0, switches = 0;
    int i, j, current, run, higherPriority, last = -1;

    resetProcesses(p, original, n);
    printf("\n========================================\n");
    printf("INTELLIGENT CONTEXT SWITCHING\n");
    printf("========================================\n");

    while (!completed(p, n))
    {
        for (i = 0; i < n; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }
        if (front == rear)
        {
            time++;
            continue;
        }
        current = queue[front++];
        if (last != -1 && last != current)
        {
            switches++;
            printf("Context Switch: P%d -> P%d\n",
                   p[last].pid, p[current].pid);
        }
        last = current;
        if (p[current].first_start == -1)
            p[current].first_start = time;
        run = p[current].remaining < quantum ?
              p[current].remaining : quantum;
        printf("Running P%d from %d to %d\n",
               p[current].pid, time, time + run);
        pid[count] = p[current].pid;
        start[count] = time;
        end[count] = time + run;
        count++;
        time += run;
        p[current].remaining -= run;
        for (i = 0; i < n; i++)
        {
            if (!added[i] && p[i].arrival <= time)
            {
                queue[rear++] = i;
                added[i] = 1;
            }
        }
        if (p[current].remaining == 0)
        {
            p[current].completion = time;
        }
        else
        {
            higherPriority = 0;
            for (j = front; j < rear; j++)
            {
                if (p[queue[j]].priority < p[current].priority)
                {
                    higherPriority = 1;
                    break;
                }
            }
            if (p[current].remaining <= quantum && !higherPriority)
            {
                printf("Decision: Continue P%d (avoiding unnecessary switch)\n",
                       p[current].pid);
                printf("Running P%d from %d to %d\n",
                       p[current].pid, time,
                       time + p[current].remaining);
                pid[count] = p[current].pid;
                start[count] = time;
                end[count] = time + p[current].remaining;
                count++;
                time += p[current].remaining;
                p[current].remaining = 0;
                p[current].completion = time;
            }
            else
            {
                queue[rear++] = current;
            }
        }
    }
    printGantt(pid, start, end, count);
    display(p, n, switches, "INTELLIGENT SCHEDULING RESULTS");
}

int main()
{
    struct Process original[MAX];
    int n, quantum;

    printf("========================================\n");
    printf("INTELLIGENT CONTEXT SWITCHING SYSTEM\n");
    printf("========================================\n");
    printf("Enter number of processes (1-%d): ", MAX);
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX)
    {
        printf("Invalid number of processes.\n");
        return 1;
    }
    printf("Enter Time Quantum: ");
    if (scanf("%d", &quantum) != 1 || quantum <= 0)
    {
        printf("Invalid time quantum.\n");
        return 1;
    }

    inputProcesses(original, n);
    roundRobin(original, n, quantum);
    intelligentScheduling(original, n, quantum);

    printf("\n========================================\n");
    printf("SIMULATION COMPLETED\n");
    printf("========================================\n");
    return 0;
}