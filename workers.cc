
// The worker thread that calls the gosia calculations
// Liam Gaffney (liam.gaffney@liverpool.ac.uk) - 29/04/2020

#ifndef __workers_cc__
#define __workers_cc__

#include "workers.hh"

// Constructor to create the thread
workers::workers() : stop(false) {

	t = std::thread( &workers::loop, this );
	
}

// The destructor joins all threads
workers::~workers() {
	
    {
        std::unique_lock<std::mutex> lock(m);
        stop = true;
    }
	
    cv.notify_all();
	t.join();
	
}


void workers::loop() {

	while( !stop ) {
		
		std::unique_lock<std::mutex> lock(m);
		cv.wait( lock, [this] { return !jobs.empty(); } );
		
		(jobs.front())();
		jobs.pop();
		
	}
	
}

template< class Func, class ... Args >
bool workers::add_job( Func f, Args ... a ) {

	std::unique_lock<std::mutex> lock(m);
	
	std::packaged_task<void()> task( std::bind(f,a...) );
	jobs.emplace( std::move(task) );
	
	// Notify the thread that there is a new job
	cv.notify_one();
	
	return true;
	
}

#endif
