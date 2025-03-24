#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

struct Process {
    int id, arrival, burst, priority;
    int waiting_time = 0, turnaround_time = 0, completion_time = 0;
};

// Function to display the Gantt Chart
void displayGanttChart(vector<pair<int, int>> ganttChart) {
    cout << "\nGantt Chart:\n";
    cout << "------------------------------------------------------\n";
    for (auto& entry : ganttChart) {
        cout << "| P" << entry.first << " ";
    }
    cout << "|\n";

    cout << "------------------------------------------------------\n";
    int time = 0;
    for (auto& entry : ganttChart) {
        cout << setw(3) << time << "    ";
        time = entry.second;
    }
    cout << setw(3) << time << "\n";
}

// Function to display process execution results
void displayResults(vector<Process>& processes, vector<pair<int, int>> ganttChart) {
    double total_wt = 0, total_tat = 0;

    // Sort by process ID before displaying results
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.id < b.id;
    });

    cout << "\nProcess Execution Results:\n";
    cout << "-------------------------------------------------------------\n";
    cout << "PID  Arrival  Burst  Priority  Waiting  Turnaround  Completion\n";
    cout << "-------------------------------------------------------------\n";

    for (auto& p : processes) {
        cout << setw(3) << p.id << setw(9) << p.arrival << setw(7) << p.burst;
        cout << setw(9) << p.priority << setw(9) << p.waiting_time;
        cout << setw(11) << p.turnaround_time << setw(12) << p.completion_time << "\n";
        total_wt += p.waiting_time;
        total_tat += p.turnaround_time;
    }

    cout << "-------------------------------------------------------------\n";
    cout << "Average Waiting Time: " << fixed << setprecision(2) << total_wt / processes.size() << "\n";
    cout << "Average Turnaround Time: " << fixed << setprecision(2) << total_tat / processes.size() << "\n";

    displayGanttChart(ganttChart);
}

// First-Come, First-Served (FCFS) Scheduling
void fcfs(vector<Process>& processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrival < b.arrival;
    });

    int time = 0;
    vector<pair<int, int>> ganttChart;

    for (auto& p : processes) {
        if (time < p.arrival) time = p.arrival;
        ganttChart.push_back({p.id, time + p.burst});
        p.waiting_time = time - p.arrival;
        p.turnaround_time = p.waiting_time + p.burst;
        p.completion_time = time + p.burst;
        time += p.burst;
    }

    displayResults(processes, ganttChart);
}

// Shortest Job First (SJF) - Non-Preemptive
void sjf(vector<Process>& processes) {
    int time = 0, completed = 0;
    vector<bool> done(processes.size(), false);
    vector<pair<int, int>> ganttChart;

    while (completed < processes.size()) {
        int idx = -1, min_burst = INT_MAX;

        for (int i = 0; i < processes.size(); i++) {
            if (!done[i] && processes[i].arrival <= time && processes[i].burst < min_burst) {
                min_burst = processes[i].burst;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        ganttChart.push_back({processes[idx].id, time + processes[idx].burst});
        processes[idx].waiting_time = time - processes[idx].arrival;
        processes[idx].turnaround_time = processes[idx].waiting_time + processes[idx].burst;
        processes[idx].completion_time = time + processes[idx].burst;
        time += processes[idx].burst;
        done[idx] = true;
        completed++;
    }

    displayResults(processes, ganttChart);
}

// Round Robin Scheduling
void roundRobin(vector<Process>& processes, int quantum) {
    queue<int> q;
    vector<int> remaining_burst(processes.size());
    vector<bool> inQueue(processes.size(), false);
    vector<pair<int, int>> ganttChart;
    int time = 0;

    for (int i = 0; i < processes.size(); i++) remaining_burst[i] = processes[i].burst;

    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].arrival == 0) {
            q.push(i);
            inQueue[i] = true;
        }
    }

    while (!q.empty()) {
        int i = q.front();
        q.pop();
        inQueue[i] = false;

        int exec_time = min(quantum, remaining_burst[i]);
        ganttChart.push_back({processes[i].id, time + exec_time});
        remaining_burst[i] -= exec_time;
        time += exec_time;

        for (int j = 0; j < processes.size(); j++) {
            if (processes[j].arrival <= time && remaining_burst[j] > 0 && !inQueue[j]) {
                q.push(j);
                inQueue[j] = true;
            }
        }

        if (remaining_burst[i] > 0) {
            q.push(i);
            inQueue[i] = true;
        } else {
            processes[i].turnaround_time = time - processes[i].arrival;
            processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst;
            processes[i].completion_time = time;
        }
    }

    displayResults(processes, ganttChart);
}

// Priority Scheduling (Non-Preemptive)
void priorityScheduling(vector<Process>& processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        if (a.priority == b.priority) return a.arrival < b.arrival;
        return a.priority < b.priority;
    });

    int time = 0;
    vector<pair<int, int>> ganttChart;

    for (auto& p : processes) {
        if (time < p.arrival) time = p.arrival;
        ganttChart.push_back({p.id, time + p.burst});
        p.waiting_time = time - p.arrival;
        p.turnaround_time = p.waiting_time + p.burst;
        p.completion_time = time + p.burst;
        time += p.burst;
    }

    displayResults(processes, ganttChart);
}

int main() {
    int n, choice, quantum;
    cout << "Enter the number of processes: ";
    cin >> n;

    vector<Process> processes(n);
    cout << "Enter process details (ID Arrival Burst [Priority if needed]):\n";

    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "Process " << i + 1 << " -> Arrival Time: ";
        cin >> processes[i].arrival;
        cout << "Burst Time: ";
        cin >> processes[i].burst;
    }

    cout << "\nChoose Scheduling Algorithm:\n";
    cout << "1. First-Come, First-Served (FCFS)\n";
    cout << "2. Shortest Job First (SJF)\n";
    cout << "3. Round Robin\n";
    cout << "4. Priority Scheduling\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 3) {
        cout << "Enter Time Quantum for Round Robin: ";
        cin >> quantum;
    }

    switch (choice) {
        case 1: fcfs(processes); break;
        case 2: sjf(processes); break;
        case 3: roundRobin(processes, quantum); break;
        case 4: priorityScheduling(processes); break;
        default: cout << "Invalid choice!\n";
    }

    return 0;
}