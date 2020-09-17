// The worker thread that calls the gosia calculations
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 29/04/2020

#ifndef __worker_hh__
#define __worker_hh__

#include <iostream>
#include <sstream>
#include <future>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <thread>
using namespace std;

class workers {
	
public:
	
	workers();
	~workers();
	void loop();
	
	template< class Func, class ... Args >
	bool add_job( Func f, Args ... a );
	
private:

	std::thread t; // The thread
	std::queue<std::packaged_task<void()>> jobs; // The job queue
	std::condition_variable cv; // The condition variable to wait for threads
	std::mutex m; // Mutex used for avoiding data races
	bool stop = true; // Change to false to exit from the job
	
};

#endif
