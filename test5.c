#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Parse /proc/stat and return total and idle CPU ticks
int get_cpu_ticks(unsigned long long *total, unsigned long long *idle)
{
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    char line[256];
    if (!fgets(line, sizeof(line), fp)) {fclose(fp); return -1;}

    // Skip the "cpu" prefix
    unsigned long long user, nice, system, idle_t, iowait, irq, softirq, steal;
    sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle_t, &iowait, &irq, &softirq, &steal);

    *idle  = idle_t + iowait;
    *total = user + nice + system + idle_t + iowait + irq + softirq + steal;

    fclose(fp);
    return 0;
}

int main()
{
    unsigned long long total1, idle1, total2, idle2;

    if (get_cpu_ticks(&total1, &idle1)) { perror("read /proc/stat"); return 1; }
    usleep(500000);   // wait 0.5s
    if (get_cpu_ticks(&total2, &idle2)) { perror("read /proc/stat"); return 1; }

    double total_delta = total2 - total1;
    double idle_delta  = idle2 - idle1;
    double usage = 100.0 * (1.0 - idle_delta / total_delta);

    printf("CPU usage: %.1f%%\n", usage);
    return 0;
}
