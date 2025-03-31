#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <atomic>

using namespace std;

class DiningPhilosophers {
private:
    int numPhilosophers;               // Number of philosophers (and forks)
    vector<mutex> forks;               // Mutexes for each fork
    vector<thread> philosophers;       // Threads for each philosopher
    atomic<int> totalIterations = 0;   // Tracks the total number of iterations performed
    atomic<int> printedLines = 0;      // Tracks the number of lines printed
    const int maxPrintedLines = 30;     // Max number of lines that should be printed
    mutex outputMutex;                 // Mutex to protect output to console

    // Function to simulate a philosopher
    void philosopher(int id) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(100, 500);  // Random thinking and eating time

        while (totalIterations.load() < maxPrintedLines) {
            totalIterations.fetch_add(1); // Increment the number of iterations

            // Simulate thinking and print the status
            printStatus(id, "thinking");

            // Simulate hunger and print the status
            printStatus(id, "hungry");

            // Pick up forks (asymmetric locking to avoid deadlock)
            lockForks(id);

            // Simulate eating and print the status
            printStatus(id, "eating");

            // Release forks after eating
            releaseForks(id);

            // Sleep for a random time between thinking and eating
            this_thread::sleep_for(chrono::milliseconds(dist(gen)));
        }
    }

    // Print the philosopher's status (thinking, hungry, or eating)
    void printStatus(int id, const string& action) {
        lock_guard<mutex> lock(outputMutex);  // Lock to prevent multiple threads from printing simultaneously
        if (printedLines.load() < maxPrintedLines) {
            cout << "Philosopher " << id << " is " << action << "." << endl;
            printedLines.fetch_add(1);  // Increment the printed line counter
        }
    }

    // Lock the forks (asymmetric locking to avoid deadlock)
    void lockForks(int id) {
        if (id % 2 == 0) {
            forks[id].lock();
            forks[(id + 1) % numPhilosophers].lock();
        } else {
            forks[(id + 1) % numPhilosophers].lock();
            forks[id].lock();
        }
    }

    // Release the forks after eating
    void releaseForks(int id) {
        forks[id].unlock();
        forks[(id + 1) % numPhilosophers].unlock();
    }

public:
    // Constructor to initialize the number of philosophers and forks
    DiningPhilosophers(int num) : numPhilosophers(num), forks(num) {}

    // Start the philosopher threads
    void start() {
        for (int i = 0; i < numPhilosophers; ++i) {
            philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i);
        }

        // Wait for all threads to finish
        for (auto& p : philosophers) {
            p.join();
        }
    }
};


int main(int argc, char* argv[]) {

    // Check command line arguments for number of philosophers
    if (argc != 2 || stoi(argv[1]) < 2) {
        cerr << "Usage: " << argv[0] << " <number_of_philosophers>" << endl;
        return 1;
    }

    // Start the dining philosophers simulation with the specified number of philosophers
    DiningPhilosophers dp(stoi(argv[1]));
    dp.start();

    return 0;
}
