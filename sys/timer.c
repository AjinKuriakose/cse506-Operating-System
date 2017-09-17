#include <sys/timer.h>
#include <sys/kprintf.h>


void print_timer() {

    static int count;
    static int sec;
    count += 1;

    if (count == 18) {
        sec += 1;
	kprintf("%d second\n", sec);
	count = 0;
    }
}
