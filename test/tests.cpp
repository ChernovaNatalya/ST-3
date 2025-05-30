// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono> // NOLINT [build/c++11]
#include <thread> // NOLINT [build/c++11]
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Invoke;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door;
    DoorTimerAdapter* adapter;
    Timer* timer;

    void SetUp() override {
        door = new TimedDoor(3);
        adapter = new DoorTimerAdapter(*door);
        timer = new Timer();
    }

    void TearDown() override {
        delete door;
        delete adapter;
        delete timer;
    }
};

TEST_F(TimedDoorTest, DoorIsOpen) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorIsClose) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorOpensAndCloses) {
    EXPECT_FALSE(door->isDoorOpened());
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorIsAlreadyOpen) {
    door->unlock();
    EXPECT_THROW(door->unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, DoorIsAlreadyClose) {
    EXPECT_THROW(door->lock(), std::logic_error);
}

TEST_F(TimedDoorTest, DoorUnlocksAndLoksFewTimes) {
    for (int i = 0; i < 5; i++) {
        door->unlock();
        EXPECT_TRUE(door->isDoorOpened());
        door->lock();
        EXPECT_FALSE(door->isDoorOpened());
    }
}

TEST_F(TimedDoorTest, DoorStateThrowOnUnlocked) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoorStateThrowOnLocked) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoorNoThrowTimeout) {
    std::this_thread::sleep_for(std::chrono::seconds(door->getTimeOut() + 1));
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, DoorThrowTimeout) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(door->getTimeOut()));
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoorTimeoutWhenOpened) {
    door->unlock();
    DoorTimerAdapter adapter(*door);
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}
