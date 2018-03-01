#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

void CreateWorkerThread(void *(*start_routine)(void *), void *arg);

#endif  /* THREAD_HELPER_H */