#pragma once
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <complex>
#include <fstream>
#include <iostream>
#include <vector>
#include <Windows.h> // SYSTEM_INFO
#include <thread>
#include <mutex>
#include <condition_variable>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::complex;
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::vector;
using std::thread;
using std::mutex;
using std::unique_lock;
using std::condition_variable;

typedef std::chrono::steady_clock the_clock;

class Creation
{
public:
	Creation();
	~Creation();

	static const int WIDTH = 480;
	static const int HEIGHT = 320;
	static const int MAX_ITERATIONS = 500;

	// -0.751085, -0.734975, 0.118378, 0.134488 --> Zoomed in
	//  -2.0, 1.0, 1.125, -1.125 --> Full pic
	
	void SetSliceSize(size_t);
	int ComputeThreads(size_t);
	void ComputeSlice(size_t);
	void WriteTGA(const char*);
	void WriteSlices(const char*, size_t);

protected:
	vector<thread> threadVec;

	vector<bool> threadComplete;
	vector<mutex*> threadMut;
	
	// Mutex's and CV's need to be accessable by all threads so must be 'global'

	uint32_t image[HEIGHT][WIDTH];

	double left = -2.0;
	double right = 1.0;
	double top = 1.125;
	double bottom = -1.125;

	int currentThreadNum = 1;
	size_t sliceSize;
};
