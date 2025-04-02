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
    const int maxIterations = 10;
    mutex outputMutex;

    void philosopher(int id) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(100, 500);

        while (true) {
            if (totalIterations.load() >= maxIterations) break;

            {
                lock_guard<mutex> lock(outputMutex);
                cout << "Philosopher " << id << " is thinking." << endl;
            }

            this_thread::sleep_for(chrono::milliseconds(dist(gen)));

            {
                lock_guard<mutex> lock(outputMutex);
                cout << "Philosopher " << id << " is hungry." << endl;
            }

            int leftFork = id;
            int rightFork = (id + 1) % numPhilosophers;

            if (id % 2 == 0) swap(leftFork, rightFork);

            // Try acquiring forks to avoid deadlock
            while (true) {
                if (forks[leftFork].try_lock()) {
                    if (forks[rightFork].try_lock()) {
                        break;
                    } else {
                        forks[leftFork].unlock();
                    }
                }

                this_thread::sleep_for(chrono::milliseconds(50));
            }

            {
                lock_guard<mutex> lock(outputMutex);
                cout << "Philosopher " << id << " is eating." << endl;
            }

            this_thread::sleep_for(chrono::milliseconds(dist(gen)));

            forks[rightFork].unlock();
            forks[leftFork].unlock();

            if (totalIterations.fetch_add(1) >= maxIterations) break;
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