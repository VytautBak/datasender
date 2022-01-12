#include "sig_action_handler.h"

volatile sig_atomic_t deamonize = 1;

/* Create sigaction handler */
void setup_sig_action() {
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);
}

void term_proc(int sigterm) {
    deamonize = 0;
}