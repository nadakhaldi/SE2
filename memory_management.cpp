/*
 * ============================================================
 *   MEMORY MANAGEMENT SIMULATOR — C++ VERSION
 *   TP3 SE2 — Mohamed Khider University, Biskra
 *   Academic Year 2025/2026
 * ============================================================ */

#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>

#pragma execution_character_set("utf-8");

using namespace std;

const int W = 62;

void printLine(char ch = '=', int len = W) {
    cout << string(len, ch) << "\n";
}

void printHeader(const string& title) {
    cout << "\n";
    printLine('=');
    cout << "  " << title << "\n";
    printLine('=');
}

int readPositiveInt(const string& prompt) {
    int val;
    while (true) {
        cout << "  " << prompt;
        if (cin >> val && val > 0) return val;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Must be a positive number. Try again.\n";
    }
}

int readStrategyChoice() {
    int val;
    while (true) {
        cout << "  Choose strategy (1-3): ";
        if (cin >> val && val >= 1 && val <= 3) return val;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid choice. Please enter 1, 2, or 3.\n";
    }
}

void pause() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

/* ============================================================
   PART 1 — FIXED PARTITION
   ============================================================ */

struct Partition {
    int  size;
    bool allocated;
    int  processId;
};

struct Process {
    int id;
    int size;
};

class FixedPartitionSimulator {
private:
    vector<Partition> partitions;
    vector<Process>   processes;
    vector<Process>   waitQueue; 

    int getProcessSize(int pid) const {
        for (const auto& p : processes)
            if (p.id == pid) return p.size;
        return 0;
    }

    void drainWaitQueue() {
        if (waitQueue.empty()) return;
        cout << "\n  Checking waiting queue (" << waitQueue.size() << " item(s))...\n";

        bool anyAllocated = true;
        while (anyAllocated && !waitQueue.empty()) {
            anyAllocated = false;
            for (int wi = 0; wi < (int)waitQueue.size(); wi++) {
                for (auto& part : partitions) {
                    if (!part.allocated && part.size >= waitQueue[wi].size) {
                        part.allocated = true;
                        part.processId = waitQueue[wi].id;
                        cout << "  [QUEUE] P" << waitQueue[wi].id
                             << " (" << waitQueue[wi].size << " KB) allocated.\n";
                        waitQueue.erase(waitQueue.begin() + wi);
                        anyAllocated = true;
                        break;
                    }
                }
                if (anyAllocated) break;
            }
        }
    }

public:
    void enterPartitions() {
        printHeader("[FIXED] ENTER PARTITIONS");
        int n = readPositiveInt("Number of partitions: ");
        partitions.clear();
        for (int i = 0; i < n; i++) {
            Partition p;
            p.size      = readPositiveInt("Partition " + to_string(i+1) + " size (KB): ");
            p.allocated = false;
            p.processId = -1;
            partitions.push_back(p);
        }
        cout << "\n  [OK] " << n << " partition(s) entered.\n";
    }

    void enterProcesses() {
        printHeader("[FIXED] ENTER PROCESSES");
        
        if (partitions.empty()) {
            cout << "\n  [!] WARNING: Enter partitions FIRST for better planning.\n\n";
            return ;
        }
        
        int n = readPositiveInt("Number of processes: ");
        processes.clear();
        for (int i = 0; i < n; i++) {
            Process p;
            p.id   = i + 1;
            p.size = readPositiveInt("Process P" + to_string(i+1) + " size (KB): ");
            processes.push_back(p);
        }
        cout << "\n  [OK] " << n << " process(es) entered.\n";
    }

    
    void firstFitAllocation() {
    if (partitions.empty()) {
        cout << "\n  [!] Enter partitions first.\n";
        return;
    }

    if (processes.empty()) {
        cout << "\n  [!] Enter processes first.\n";
        return;
    }

    for (auto& p : partitions) {
        p.allocated = false;
        p.processId = -1;
    }

    waitQueue.clear();

    for (const auto& proc : processes) {

        bool allocatedNow = false;
        bool suitableBlockExists = false;

      
        int largestBlock = 0;
        for (const auto& part : partitions) {
            if (part.size > largestBlock)
                largestBlock = part.size;
        }

        // إذا process أكبر من أكبر partition => rejected
        if (proc.size > largestBlock) {
            cout << "\n  [REJECTED] P" << proc.id
                 << " (" << proc.size << " KB)"
                 << " > Largest Partition (" << largestBlock << " KB)\n";
            continue;
        }

        for (auto& part : partitions) {

            if (part.size >= proc.size) {
                suitableBlockExists = true;

                if (!part.allocated) {
                    part.allocated = true;
                    part.processId = proc.id;
                    allocatedNow = true;
                    break;
                }
            }
        }

        // إذا كاين partition مناسب بصح كامل معمرين => waiting queue
        if (!allocatedNow && suitableBlockExists) {
            waitQueue.push_back(proc);
        }
    }

    printHeader("[FIXED] ALLOCATION REPORT — FIRST FIT");

    cout << "\n";
    cout << "+-----------+----------+------------+--------------+----------------+\n";
    cout << "| Partition | Size(KB) | Process ID | ProcSize(KB) | Internal Frag. |\n";
    cout << "+-----------+----------+------------+--------------+----------------+\n";

    int totalFrag = 0;

    for (int i = 0; i < (int)partitions.size(); i++) {
        const auto& part = partitions[i];

        cout << "| " << left << setw(9) << (i + 1) << " | "
             << setw(8) << part.size << " | ";

        if (part.allocated) {
            int ps = getProcessSize(part.processId);
            int frag = part.size - ps;
            totalFrag += frag;

            cout << setw(10) << ("P" + to_string(part.processId)) << " | "
                 << setw(12) << ps << " | "
                 << setw(12) << frag << " KB |\n";
        }
        else {
            cout << setw(10) << "---" << " | "
                 << setw(12) << "---" << " | "
                 << setw(15) << "---" << " |\n";
        }
    }

    cout << "+-----------+----------+------------+--------------+----------------+\n";
    cout << "  Total Internal Fragmentation : "
         << totalFrag << " KB\n";

    if (!waitQueue.empty()) {
        cout << "\n";
        printLine('-');
        cout << "  WAITING QUEUE (" << waitQueue.size() << "):\n";
        printLine('-');

        for (const auto& p : waitQueue) {
            cout << "   - P" << p.id
                 << " (" << p.size << " KB)\n";
        }
    }
    else {
        cout << "\n  [OK] No waiting processes.\n";
    }
}

    void deallocate(int pid) {
        for (auto& part : partitions) {
            if (part.allocated && part.processId == pid) {
                cout << "  [OK] P" << pid << " freed — partition is now FREE.\n";
                part.allocated = false;
                part.processId = -1;
                drainWaitQueue();
                return;
            }
        }
        cout << "  [FAIL] Process P" << pid << " not found.\n";
    }

    void displayWaitQueue() const {
        if (waitQueue.empty()) {
            cout << "\n  [i] Waiting queue is empty.\n";
            return;
        }
        printHeader("[FIXED] WAITING QUEUE");
        cout << "\n";
        cout << "+------------+----------+\n";
        cout << "| Process ID | Size(KB) |\n";
        cout << "+------------+----------+\n";
        for (const auto& p : waitQueue)
            cout << "| P" << left << setw(9) << p.id << " | "
                 << setw(8) << p.size << " |\n";
        cout << "+------------+----------+\n";
    }

    void displayStatus() const {
        if (partitions.empty()) { cout << "\n  [!] No partitions defined.\n"; return; }
        printHeader("[FIXED] PARTITION STATUS");
        cout << "\n";
        cout << "+-----------+----------+------------+\n";
        cout << "| Partition | Size(KB) | Status     |\n";
        cout << "+-----------+----------+------------+\n";
        for (int i = 0; i < (int)partitions.size(); i++) {
            const auto& p = partitions[i];
            cout << "| " << left << setw(9) << (i+1) << " | "
                 << setw(8) << p.size << " | ";
            if (p.allocated)
                cout << left << setw(10) << ("P" + to_string(p.processId)) << " |\n";
            else
                cout << left << setw(10) << "FREE" << " |\n";
        }
        cout << "+-----------+----------+------------+\n";
    }

    void runMenu() {
        int ch;
        do {
            cout << "\n";
            printLine('-', 44);
            cout << "   FIXED PARTITION MENU\n";
            printLine('-', 44);
            cout << "   [1] Enter Partitions\n";
            cout << "   [2] Enter Processes\n";
            cout << "   [3] First Fit Allocation\n";
            cout << "   [4] Display Status\n";
            cout << "   [5] Deallocate Process\n";
            cout << "   [6] Waiting Queue\n";
            cout << "   [0] Back to Main Menu\n";
            printLine('-', 44);
            cout << "   Choice: ";
            if (!(cin >> ch)) { cin.clear(); cin.ignore(1000,'\n'); ch=-1; }

            int id;
            switch (ch) {
                case 1: enterPartitions();    break;
                case 2:
    if (partitions.empty()) {
        cout << "\n  [!] Enter partitions first (option 1).\n";
    } else {
        enterProcesses();
    }
                     break;
                
                case 3: firstFitAllocation(); break;
                case 4: displayStatus();      break;
                case 5:
                    cout << "  Process ID : "; cin >> id;
                    deallocate(id); break;
                case 6: displayWaitQueue();   break;
                case 0: break;
                default: cout << "  [!] Invalid choice.\n";
            }
        } while (ch != 0);
    }
};

/* ============================================================
   PART 2 — VARIABLE PARTITION
   ============================================================ */

struct Block {
    int  start;
    int  size;
    bool free;
    int  processId;
};

enum Strategy { FIRST_FIT = 1, BEST_FIT = 2, WORST_FIT = 3 };

class VariablePartitionSimulator {
private:
    vector<Block>  memory;
    vector<Block>  waitQueue;
    int            totalSize = 0;
    Strategy       currentStrategy = FIRST_FIT;

    string stratName(Strategy s) const {
        switch (s) {
            case FIRST_FIT: return "First Fit";
            case BEST_FIT:  return "Best Fit";
            case WORST_FIT: return "Worst Fit";
        }
        return "Unknown";
    }

    int mergeInternal() {
        int count = 0;
        for (int i = 0; i + 1 < (int)memory.size(); ) {
            if (memory[i].free && memory[i+1].free) {
                memory[i].size += memory[i+1].size;
                memory.erase(memory.begin() + i + 1);
                count++;
            } else {
                i++;
            }
        }
        return count;
    }

    void drainWaitQueue() {
        if (waitQueue.empty()) return;
        cout << "\n  Checking waiting queue (" << waitQueue.size() << " item(s))...\n";

        bool anyAllocated = true;
        while (anyAllocated && !waitQueue.empty()) {
            anyAllocated = false;
            for (int wi = 0; wi < (int)waitQueue.size(); wi++) {
                for (int bi = 0; bi < (int)memory.size(); bi++) {
                    if (memory[bi].free && memory[bi].size >= waitQueue[wi].size) {
                        if (memory[bi].size > waitQueue[wi].size) {
                            Block remainder;
                            remainder.start     = memory[bi].start + waitQueue[wi].size;
                            remainder.size      = memory[bi].size  - waitQueue[wi].size;
                            remainder.free      = true;
                            remainder.processId = -1;
                            memory[bi].size = waitQueue[wi].size;
                            memory.insert(memory.begin() + bi + 1, remainder);
                        }
                        memory[bi].free      = false;
                        memory[bi].processId = waitQueue[wi].processId;
                        cout << "  [QUEUE] P" << waitQueue[wi].processId
                             << " (" << waitQueue[wi].size << " KB) allocated.\n";
                        waitQueue.erase(waitQueue.begin() + wi);
                        anyAllocated = true;
                        break;
                    }
                }
                if (anyAllocated) break;
            }
        }
    }

public:
    void initMemory() {
        printHeader("[VAR] INITIALIZE MEMORY");
        totalSize = readPositiveInt("Total memory size (KB): ");
        
        printLine('-', 44);
        cout << "\n  SELECT ALLOCATION STRATEGY:\n";
        cout << "  (This strategy will be used for all processes)\n\n";
        cout << "   [1] First Fit\n";
        cout << "   [2] Best Fit\n";
        cout << "   [3] Worst Fit\n";
        printLine('-', 44);
        
        int stratChoice = readStrategyChoice();
        
        if (stratChoice == 1) currentStrategy = FIRST_FIT;
        else if (stratChoice == 2) currentStrategy = BEST_FIT;
        else if (stratChoice == 3) currentStrategy = WORST_FIT;
        
        cout << "\n  [OK] Strategy set to: " << stratName(currentStrategy) << "\n";
        cout << "  [OK] Memory initialized: " << totalSize << " KB\n\n";
        
        memory.clear();
        waitQueue.clear();

        Block b;
        b.start     = 0;
        b.size      = totalSize;
        b.free      = true;
        b.processId = -1;
        memory.push_back(b);
    }

    bool allocate(int pid, int size) {
        if (memory.empty()) {
            cout << "  [!] Initialize memory first.\n";
            return false;
        }

        // REJECTED: حجمه أكبر منsize of memory — مستحيل يتحمل حتى لو فارغة
        if (size > totalSize) {
            cout << "  [REJECTED] P" << pid << " (" << size
                 << " KB) — exceeds total memory (" << totalSize << " KB).\n";
            return false;
        }

        int selectedIdx = -1;
        int bestVal     = -1;

        for (int i = 0; i < (int)memory.size(); i++) {
            if (!memory[i].free || memory[i].size < size) continue;

            if (currentStrategy == FIRST_FIT) {
                selectedIdx = i;
                break;
            } else if (currentStrategy == BEST_FIT) {
                int waste = memory[i].size - size;
                if (bestVal == -1 || waste < bestVal) {
                    bestVal     = waste;
                    selectedIdx = i;
                }
            } else if (currentStrategy == WORST_FIT) {
                if (memory[i].size > bestVal) {
                    bestVal     = memory[i].size;
                    selectedIdx = i;
                }
            }
        }

        if (selectedIdx == -1) {
            Block w; w.start = 0; w.size = size;
            w.free = true; w.processId = pid;
            waitQueue.push_back(w);
            cout << "  [QUEUE] P" << pid << " (" << size
                 << " KB) added to waiting queue.\n";
            return false;
        }

        Block& sel = memory[selectedIdx];
        if (sel.size > size) {
            Block remainder;
            remainder.start     = sel.start + size;
            remainder.size      = sel.size  - size;
            remainder.free      = true;
            remainder.processId = -1;
            sel.size = size;
            memory.insert(memory.begin() + selectedIdx + 1, remainder);
        }

        memory[selectedIdx].free      = false;
        memory[selectedIdx].processId = pid;

        int s = memory[selectedIdx].start;
        cout << "  [OK] P" << pid << " (" << size << " KB) allocated at "
             << s << " - " << (s + size) << " KB\n";
        return true;
    }

    bool deallocate(int pid) {
        for (auto& blk : memory) {
            if (!blk.free && blk.processId == pid) {
                cout << "  [OK] P" << pid << " freed — "
                     << blk.size << " KB at " << blk.start << " KB\n";
                blk.free      = true;
                blk.processId = -1;
                mergeInternal();
                drainWaitQueue();
                return true;
            }
        }
        cout << "  [FAIL] Process P" << pid << " not found.\n";
        return false;
    }

    void mergeBlocks() {
        int n = mergeInternal();
        cout << "  [OK] Merged " << n << " adjacent free block(s).\n";
        displayMemory();
    }

    void displayMemory() const {
        if (memory.empty()) { cout << "  [!] Memory not initialized.\n"; return; }

        printHeader("[VAR] MEMORY LAYOUT");
        cout << "\n";
        cout << "+-----------+----------+-----------+------------+\n";
        cout << "| Start(KB) | Size(KB) |  Status   | Process ID |\n";
        cout << "+-----------+----------+-----------+------------+\n";

        for (const auto& blk : memory) {
            cout << "| " << left << setw(9)  << blk.start << " | "
                 << setw(8)  << blk.size  << " | ";
            if (blk.free)
                cout << setw(9) << "FREE"  << " | " << setw(10) << "---" << " |\n";
            else
                cout << setw(9) << "ALLOC" << " | "
                     << setw(10) << ("P" + to_string(blk.processId)) << " |\n";
        }
        cout << "+-----------+----------+-----------+------------+\n";

        int total = 0;
        for (const auto& b : memory) total += b.size;
        cout << "\n  [";
        for (const auto& blk : memory) {
            int bar = max(1, blk.size * 40 / total);
            cout << string(bar, blk.free ? '.' : '#');
        }
        cout << "]\n  # = Allocated   . = Free\n";
    }

    void fragmentationReport() const {
        int totalFree = 0, largestFree = 0, freeCount = 0;
        for (const auto& blk : memory) {
            if (blk.free) {
                totalFree += blk.size;
                freeCount++;
                if (blk.size > largestFree) largestFree = blk.size;
            }
        }
        int extFrag = totalFree - largestFree;
        float ratio = (totalFree > 0) ? (float)extFrag / totalFree * 100 : 0.0f;

        printHeader("[VAR] EXTERNAL FRAGMENTATION REPORT");
        cout << "\n";
        cout << "  Total Free Memory      : " << totalFree    << " KB\n";
        cout << "  Largest Free Block     : " << largestFree  << " KB\n";
        cout << "  Number of Free Blocks  : " << freeCount    << "\n";
        cout << "  External Fragmentation : " << extFrag      << " KB\n";
        cout << fixed << setprecision(2);
        cout << "  Fragmentation Ratio    : " << ratio        << "%\n";
        cout << "\n  Formula: Ext.Frag = Total Free - Largest Free\n";
        cout << "           " << extFrag << " = " << totalFree
             << " - " << largestFree << "\n";
    }

    void displayWaitQueue() const {
        if (waitQueue.empty()) {
            cout << "\n  [i] Waiting queue is empty.\n";
            return;
        }
        printHeader("[VAR] WAITING QUEUE");
        cout << "\n";
        cout << "+------------+----------+\n";
        cout << "| Process ID | Size(KB) |\n";
        cout << "+------------+----------+\n";
        for (const auto& w : waitQueue)
            cout << "| P" << left << setw(9) << w.processId << " | "
                 << setw(8) << w.size << " |\n";
        cout << "+------------+----------+\n";
    }

    void memoryStatistics() const {
        int totalMem = 0, usedMem = 0, freeMem = 0;
        int allocBlocks = 0, freeBlocks = 0, largestFree = 0;
        for (const auto& blk : memory) {
            totalMem += blk.size;
            if (blk.free) {
                freeMem += blk.size;
                freeBlocks++;
                if (blk.size > largestFree) largestFree = blk.size;
            } else {
                usedMem += blk.size;
                allocBlocks++;
            }
        }
        int extFrag = freeMem - largestFree;
        float util  = (totalMem > 0) ? (float)usedMem / totalMem * 100 : 0.0f;

        printHeader("[VAR] MEMORY STATISTICS");
        cout << "\n";
        cout << "+-------------------------------+------------------+\n";
        cout << "| Metric                        | Value            |\n";
        cout << "+-------------------------------+------------------+\n";
        cout << "| Total Memory                  | " << left << setw(14) << (to_string(totalMem) + " KB") << "   |\n";
        cout << "| Used Memory                   | " << setw(14) << (to_string(usedMem)  + " KB") << "   |\n";
        cout << "| Free Memory                   | " << setw(14) << (to_string(freeMem)  + " KB") << "   |\n";
        cout << fixed << setprecision(2);
        cout << "| Memory Utilization            | " << setw(13) << util << " %   |\n";
        cout << "| Allocated Blocks              | " << setw(14) << allocBlocks     << "   |\n";
        cout << "| Free Blocks                   | " << setw(14) << freeBlocks      << "   |\n";
        cout << "| Largest Free Block            | " << setw(14) << (to_string(largestFree) + " KB") << "   |\n";
        cout << "| External Fragmentation        | " << setw(14) << (to_string(extFrag) + " KB") << "   |\n";
        cout << "+-------------------------------+------------------+\n";
        
        if (!waitQueue.empty()) {
            cout << "\n  ╔═══════════════════════════════════════╗\n";
            cout << "  ║  PROCESSES IN WAITING QUEUE (" << left << setw(2) << waitQueue.size() << ")     ║\n";
            cout << "  ╚═══════════════════════════════════════╝\n";
            cout << "\n  +--------+----------+----------+\n";
            cout << "  | Order  | Process  | Size(KB) |\n";
            cout << "  +--------+----------+----------+\n";
            for (int i = 0; i < (int)waitQueue.size(); i++) {
                cout << "  | " << left << setw(6) << (i+1) << " | "
                     << "P" << setw(7) << waitQueue[i].processId << " | "
                     << setw(8) << waitQueue[i].size << " |\n";
            }
            cout << "  +--------+----------+----------+\n";
        } else {
            cout << "\n  ✓ Waiting Queue: EMPTY (all processes allocated)\n";
        }
    }

    void compactMemory() {
        if (memory.empty()) {
            cout << "\n  [!] Initialize memory first.\n";
            return;
        }

        printHeader("[BONUS] MEMORY COMPACTION");
        cout << "\n  Before Compaction:\n";
        displayMemory();

        vector<Block> allocated;
        for (const auto& blk : memory)
            if (!blk.free) allocated.push_back(blk);

        memory.clear();
        int newAddress = 0;

        for (auto& blk : allocated) {
            blk.start = newAddress;
            memory.push_back(blk);
            newAddress += blk.size;
        }

        if (newAddress < totalSize) {
            Block freeBlock;
            freeBlock.start     = newAddress;
            freeBlock.size      = totalSize - newAddress;
            freeBlock.free      = true;
            freeBlock.processId = -1;
            memory.push_back(freeBlock);
        }

        cout << "\n  After Compaction:\n";
        displayMemory();
        cout << "\n  [OK] Memory compacted — external fragmentation eliminated!\n";
    }

    void runMenu() {
        int ch;
        do {
            cout << "\n";
            printLine('-', 52);
            cout << "   VARIABLE PARTITION MENU\n";
            
            if (!memory.empty()) {
                cout << "   Current Strategy: " << stratName(currentStrategy) << "\n";
            }
            printLine('-', 52);
            cout << "   [1] Initialize Memory (Choose Strategy)\n";
            cout << "   [2] Allocate Process\n";
            cout << "   [3] Deallocate Process\n";
            cout << "   [4] Merge Free Blocks\n";
            cout << "   [5] Fragmentation Report\n";
            cout << "   [6] Display Memory\n";
            cout << "   [7] Memory Statistics\n";
            cout << "   [8] Waiting Queue\n";
            cout << "   [9] Memory Compaction (BONUS)\n";
            cout << "   [0] Back to Main Menu\n";
            printLine('-', 52);
            cout << "   Choice: ";
            if (!(cin >> ch)) { cin.clear(); cin.ignore(1000,'\n'); ch=-1; }

            if ((ch >= 2 && ch <= 8) && memory.empty()) {
                cout << "  [!] Initialize memory first (choose strategy).\n";
                continue;
            }

            int id, size;
            switch (ch) {
                case 1:  initMemory(); break;
                case 2:
                    id   = readPositiveInt("Process ID : ");
                    size = readPositiveInt("Size (KB)  : ");
                    allocate(id, size); break;
                case 3:
                    cout << "  Process ID : "; cin >> id;
                    deallocate(id); break;
                case 4:  mergeBlocks();         break;
                case 5:  fragmentationReport(); break;
                case 6:  displayMemory();       break;
                case 7:  memoryStatistics();    break;
                case 8:  displayWaitQueue();    break;
                case 9:  compactMemory();       break;
                case 0:  break;
                default: cout << "  [!] Invalid choice.\n";
            }
        } while (ch != 0);
    }
};

/* ============================================================
   MAIN
   ============================================================ */

int main() {
    FixedPartitionSimulator    fixed;
    VariablePartitionSimulator variable;

    cout << "\n";
    printLine('*');
    cout << "       MEMORY MANAGEMENT SIMULATOR\n";
    cout << "       TP3 SE2 — Biskra University 2025/2026\n";
    printLine('*');

    int ch;
    do {
        cout << "\n";
        printLine('=', 44);
        cout << "   MAIN MENU\n";
        printLine('=', 44);
        cout << "   [1] Fixed Partition Simulator\n";
        cout << "   [2] Variable Partition Simulator\n";
        cout << "   [0] Exit\n";
        printLine('=', 44);
        cout << "   Choice: ";
        if (!(cin >> ch)) { cin.clear(); cin.ignore(1000,'\n'); ch=-1; }

        switch (ch) {
            case 1: fixed.runMenu();    break;
            case 2: variable.runMenu(); break;
            case 0: cout << "\n  Goodbye!\n\n"; break;
            default: cout << "  [!] Invalid choice.\n";
        }
    } while (ch != 0);

    return 0;
}
