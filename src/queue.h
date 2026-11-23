/*
 * queue.h  -  WAITING QUEUE (circular queue)
 * ------------------------------------------
 * When every parking bay is full, a bus that wants to park joins this
 * first-in-first-out queue. When a bay frees up, the bus at the front
 * is parked automatically (see depot.c).
 */
#ifndef QUEUE_H
#define QUEUE_H

void enqueue(int busId);
int  dequeue(void);                 /* returns the bus id, or 0 if empty */
int  inQueue(int busId);
void removeFromQueue(int busId);    /* removes one bus, keeps the order of the rest */
int  queueCount(void);
int  queueAt(int position);         /* bus id at position 0 (front) .. count-1 */

#endif
