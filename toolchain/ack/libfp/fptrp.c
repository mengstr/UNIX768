/*
 * Convert ACK floating-point runtime traps into the V7 SIGFPE mechanism.
 */

#include <signal.h>
#include <unistd.h>

void
_fptrp(int trapno)
{
        (void)trapno;
        (void)kill(getpid(), SIGFPE);
}
