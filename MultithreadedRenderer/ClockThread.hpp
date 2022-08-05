#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;
class ClockThread {
public:
	ClockThread() : clockThread(&ClockThread::clockThreadInit, this), systemTickListener(&ClockThread::systemTickListenerInit, this) {
		tick = chrono::steady_clock::now().time_since_epoch().count();
	}
	~ClockThread() {
		terminateThreads();
		clockThread.join();
		systemTickListener.join();
	}
	
	/** @brief Call from any thread to decide wether to system-sleep or to busy-sleep based on last system tick
	* 
	*/
	void sleep() {

	}

private:
	//threads-------------------------------------------------------------------------
	thread clockThread;
	thread systemTickListener;//joined in destructor => no detach needed

	void clockThreadInit() {
		long long lastTick = getTick();
		while (getTerminate() == false) {
			long long tempTick = getTick();
			cout << averageTickTime << "\n";
		}
	}

	void systemTickListenerInit() {
		firstTick = chrono::steady_clock::now().time_since_epoch().count();
		while (getTerminate() == false) {
			setTick(chrono::steady_clock::now().time_since_epoch().count());
			this_thread::sleep_for(1ms);
		}
	}

	//data-------------------------------------------------------------------------------
	mutex termiantionMtx;
	bool terminate = false;
	mutex tickMtx;
	long long firstTick = 0L;
	long long tick = 0L;
	long tickCount = 0L;
	int averageTickTime = 0;
	void terminateThreads() {
		termiantionMtx.lock();
		terminate = true;
		termiantionMtx.unlock();
	}

	int getTerminate() {
		termiantionMtx.lock();
		bool temp = terminate;
		termiantionMtx.unlock();
		return temp;
	}
	
	void setTick(long long newTick) {
		tickMtx.lock();
		tick = newTick;
		averageTickTime = (int)(tick - firstTick) / ++tickCount;
		tickMtx.unlock();
	}

	long long getTick() {
		tickMtx.lock();
		long long temp = tick;
		tickMtx.unlock();
		return temp;
	}
};