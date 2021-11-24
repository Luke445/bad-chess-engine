#include "Threads.h"
#include "Gui.h"
#include "Computer.h"
#include "EnhancedBoard.h"

using namespace std;

Threads::Threads(function<void(Move *)> func, Move *sharedMove) {
    // create worker threads
    waitingThreads = 0;
    int num_threads = thread::hardware_concurrency();
    for (int i = 0; i < num_threads; i++)
    {
        threads.push_back(thread(&Threads::threadWaitLoop, this));
    }

    // initialize control thread
    controlThread = thread(func, sharedMove);
}

void Threads::threadWaitLoop() {
    function<void()> func;
    while (true)
    {
        unique_lock<mutex> lock(queueLock);

        waitingThreads++;
        condition.wait(lock, [this]() {return (!queue.empty() && !acceptFunctions) || stopped;});
        waitingThreads--;
        if (stopped)
            return;
        func = queue.front();
        queue.pop();

        lock.unlock();

        func();
    }
}

void Threads::addJob(function<void()> func) {
    unique_lock<mutex> lock(queueLock);
    queue.push(func);

    lock.unlock();
    condition.notify_one();
}

void Threads::waitUntilDone() {
    acceptFunctions = false;
    while (!queue.empty() || waitingThreads != threads.size()) {
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void Threads::shutdown() {
    unique_lock<mutex> lock(queueLock);
    acceptFunctions = false;
    stopped = true;
    lock.unlock();

    condition.notify_all();

    for (thread &th : threads)
    {
        th.join();
    }

    controlThread.join();
}

Threads::~Threads() {
    if (!stopped)
        shutdown();
}