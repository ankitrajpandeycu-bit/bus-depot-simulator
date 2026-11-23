/* queue.c  -  see queue.h */
#include "models.h"
#include "queue.h"

static int items[MAX_BUSES];
static int front = 0, rear = 0, count = 0;

void enqueue(int busId)
{
    if (count == MAX_BUSES) return;
    items[rear] = busId;
    rear = (rear + 1) % MAX_BUSES;
    count++;
}

int dequeue(void)
{
    if (count == 0) return 0;
    int id = items[front];
    front = (front + 1) % MAX_BUSES;
    count--;
    return id;
}

int inQueue(int busId)
{
    for (int i = 0; i < count; i++)
        if (items[(front + i) % MAX_BUSES] == busId) return 1;
    return 0;
}

void removeFromQueue(int busId)
{
    int keep[MAX_BUSES], n = 0;
    while (count > 0) {
        int id = dequeue();
        if (id != busId) keep[n++] = id;
    }
    for (int i = 0; i < n; i++) enqueue(keep[i]);
}

int queueCount(void) { return count; }

int queueAt(int position)
{
    if (position < 0 || position >= count) return 0;
    return items[(front + position) % MAX_BUSES];
}
