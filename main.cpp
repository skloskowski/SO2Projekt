#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>
#include <atomic>

using namespace std;

class DiningPhilosophers {
private:
    int numPhilosophers;
    vector<mutex> forks;
    vector<thread> philosophers;
    atomic<int> totalIterations = 0;
    int maxIterations = 30;
    mutex outputMutex;

    void philosopher(int id) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(100, 500);

        while (true) {
            int currentIterations = totalIterations.load();
            if (currentIterations >= maxIterations) break;
            if (totalIterations.fetch_add(1) >= maxIterations) break;

            {
                lock_guard<mutex> lock(outputMutex);
                if (totalIterations.load() > maxIterations) break;
                cout << "Philosopher " << id << " is thinking." << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(dist(gen)));

            {
                lock_guard<mutex> lock(outputMutex);
                if (totalIterations.load() > maxIterations) break;
                cout << "Philosopher " << id << " is hungry." << endl;
            }

            // Asymmetric locking to prevent deadlock
            if (id % 2 == 0) {
                forks[id].lock();
                forks[(id + 1) % numPhilosophers].lock();
            } else {
                forks[(id + 1) % numPhilosophers].lock();
                forks[id].lock();
            }

            {
                lock_guard<mutex> lock(outputMutex);
                if (totalIterations.load() > maxIterations) break;
                cout << "Philosopher " << id << " is eating." << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(dist(gen)));

            // Put down forks
            forks[id].unlock();
            forks[(id + 1) % numPhilosophers].unlock();
        }
    }

public:
    DiningPhilosophers(int n) : numPhilosophers(n), forks(n) {}

    void start() {
        for (int i = 0; i < numPhilosophers; ++i) {
            philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i);
        }
        for (auto& p : philosophers) {
            p.join();
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <number_of_philosophers>\n";
        return 1;
    }

    int numPhilosophers = stoi(argv[1]);
    if (numPhilosophers < 2) {
        cerr << "Number of philosophers must be at least 2.\n";
        return 1;
    }

    DiningPhilosophers dp(numPhilosophers);
    dp.start();

    return 0;
}
