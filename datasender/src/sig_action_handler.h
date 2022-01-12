
#include <stddef.h>
#include <signal.h>

#ifndef SIG_ACTION_HANDLER_H
#define SIG_ACTION_HANDLER_H

void setup_sig_action();
void term_proc(int sigterm);

#endif