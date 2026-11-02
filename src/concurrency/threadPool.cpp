#include "concurrency/threadPool.hpp"
#include "application/http_request.hpp"
#include "application/http_response.hpp"
#include "concurrency/queue.hpp"
#include <atomic>
#include <cstdint>
#include "application/http_response.hpp"
#include "application/router.hpp"
#include <cstdint>
#include <unistd.h>
#include <atomic>

void processTask(TaskQueue<Task> &requestQueue, ReadyQueue& readyQueue, Router& router, int efd, std::atomic<bool>& running){
    while (running.load()){
        Task task;
        if (!requestQueue.tryPop(task)){
            continue;
        }
        HttpRequest request = HttpRequest::parse(task.requestString);
        Handler httpHandler = router.find(request.method, request.path); 
        if (httpHandler == nullptr){
            // return error
        }
        HttpResponse response = httpHandler(request);
        std::string httpResp = HttpResponse::toHttpMessage(response);

        readyQueue.push(ReadyTask{task.fd, std::move(httpResp)});
        
        uint64_t signal = 1;
        ssize_t n = write(efd, &signal, sizeof(signal));
        if (n != sizeof(signal)){
            perror("eventfd write");
        }
    }
}

void ThreadPool::start(size_t numOfWorker, TaskQueue<Task>& requestQueue, ReadyQueue& readyQueue, Router& router, int efd){
    running.store(true);

    for (size_t i = 0; i < numOfWorker; i++){
        workers.emplace_back(processTask, std::ref(requestQueue), std::ref(readyQueue), std::ref(router), efd, std::ref(running));
    }
    
}
void ThreadPool::stop() {
    running.store(false);

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    workers.clear();
}
