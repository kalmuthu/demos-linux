// This example will create 10 timers. As soon that all of them are created
// the 5th timer is cancelled.
// We will see that all of them (except the 5th) are activated.
// ACE will activate the handle_timeout method when timer timeout arrives. 
//
#include <ace/Timer_Queue.h>
#include <ace/Reactor.h>

/*
 * EXTRA_CMDS=pkg-config --cflags --libs ACE
 */
const int NUMBER_TIMERS = 10;
volatile static bool done = false;
static int count = 0;
static int last_arrived = -1;
class MyTime_Handler : public ACE_Event_Handler {
public:
	//Method which is called back by the Reactor when timeout occurs.
	virtual int handle_timeout(const ACE_Time_Value& tv, const void *arg) {
		long current_count = long(arg);

		// assert that we are not getting the 5'th timer
		ACE_ASSERT(current_count != 5);
		// assert that we are getting them in order
		ACE_ASSERT(current_count > last_arrived);
		last_arrived = current_count;
		ACE_DEBUG((LM_DEBUG, "%t %d: Timer #%d timed out at %d!\n", count, current_count, tv.sec()));
		// If all timers done then set done flag
		count++;
		sleep(3);
		if (count == NUMBER_TIMERS - 1) {
			done = true;
		}
		// Keep yourself registered with the Reactor.
		return(0);
	}
};

int ACE_TMAIN(int, char **) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%t: main thread starting\n")));
	ACE_Reactor reactor;
	MyTime_Handler *th = new MyTime_Handler;
	int timer_id[NUMBER_TIMERS];
	for (int i = 0; i < NUMBER_TIMERS; i++) {
		timer_id[i] = reactor.schedule_timer(
		        th,
		        (const void *)i,// argument sent to handle_timeout()
		        ACE_Time_Value(2 * i + 1)// set timer to go off with delay
		        );
	}
	// Cancel the fifth timer before it goes off
	//Timer ID of timer to be removed
	reactor.cancel_timer(timer_id[5]);
	// In the following loop we wait for the event, except when done variable is set
	// this is done when the last one is activated.
	// This termination trick is only one option for termination out of many other possibilities.
	// and is not a good one at that because it makes us write the logic for the termination.
	// this is a busy free wait loop
	while (!done) {
		reactor.handle_events();
		ACE_DEBUG((LM_DEBUG, "%t in main thread\n"));
	}
	return(0);
}
