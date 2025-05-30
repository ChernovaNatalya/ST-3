// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor&) :door(door) {}

TimedDoor::TimedDoor(int timeout) :iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}
bool TimedDoor::isDoorOpened() {
    return isOpened;
}
void TimedDoor::unlock() {
    if (isOpened) {
        throw std::logic_error("Door is already open!");
    }
    isOpened = true;
}
void TimedDoor::lock() {
    if (!isOpened) {
        throw std::logic_error("Door is already close!");
    }
    isOpened = false;
}
int  TimedDoor::getTimeOut() const {
    return iTimeout;
}
void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Time is up!");
    }
}

void DoorTimerAdapter::Timeout() {
    door.throwState();
}

void Timer::sleep(int time) {
    std::this_thread::sleep_for(std::chrono::seconds(time));
}

void Timer::tregister(int timeout, TimerClient* client) {
    sleep(timeout);
    client->Timeout();
}
