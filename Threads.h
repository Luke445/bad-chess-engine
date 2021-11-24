#pragma once
#include <thread>
#include <vector>
#include <queue>
#include "Board.h"

class Threads {
public:
    std::vector<std::thread> threads;
    std::thread controlThread;
    int waitingThreads;

    std::queue<std::function<void()>> queue;
    bool stopped = false;
    std::mutex queueLock;
    std::condition_variable condition;
    bool acceptFunctions;


    Threads(std::function<void(Move *)> func, Move *sharedMove);

    void threadWaitLoop();

    void waitUntilDone();

    void addJob(std::function<void()> func);

    void shutdown();

    ~Threads();
};
