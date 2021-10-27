# Framework

The C message framework is a system for inter-task communication.

The Zephyr port was created from the FreeRTOS port. It is a wrapper around sending and receiving messages using queues. Although two out of three reasons for the framework don't apply to Zephyr (command line interface and deferred logging), the third still remains. The framework is a method for partitioning a design. It is an alternative to callbacks.

Message buffers are allocated from the buffer pool. In Zephyr, the buffer pool is a statically defined heap. The buffer pool also contains optional statistics.

## IDs

Each task or queue has an ID (prefixed with FWK_ID). Each message also has a unique ID (prefixed with FMC). IDs are used to route messages. The IDs must be configured for each project. The templates are found in the config folder. Some IDs are reserved for use by the framework.

Messages can be routed to individual tasks based on IDs. They can also be broadcast. It is also possible to route a message after searching each task for a handler (unicast). When sending a unicast message, there should only be one handler for that message code.

## Message Task

Message tasks are based on Zephyr's threads. They contain an ID, message dispatcher, message queue, default block amount, and a timer. The ID is used for message routing. The dispatcher contains handlers for each type of message that the task can process. The message queue is used to hold messages. The size of the queue is a compile time constant. A message task's timer can be used to schedule periodic events. On expiration of the timer the predefined message FMC_PERIODIC will be put on the task's queue.

The default block amount determines how long a task waits for a message in a queue. This is often used when a task controls a transport and must periodically service a receive buffer.

A message queue is an integral part of a framework message task but can also be used stand-alone.

## Design Details

### Macros

The framework macros enable easier porting from FreeRTOS to Zephyr. Each macro wraps commonly used operations.

### Buffer Pool Shell

The optional shell can be used to display buffer pool statistics. The statistics can be used to determine if the pool is too large or small or if there is a memory leak.

```
bp stats
```

```
Buffer Pool 0
stats initialized     1
space available       8192
min space available   7968
min alloced size      4
max alloced size      112
total allocs          4816
current allocations   0
max allocations       2
take failures         0
last fail size        0
```

## Design Considerations

For a simple project, the overhead of the framework may not be desired. However, even a single task sending messages to itself can divide the design into smaller pieces.

The default thread model for FreeRTOS and Zephyr are different. Zephyr defaults to cooperative threads that run until they yield, whereas Laird Connectivity's default configuration of FreeRTOS was fully preemptive with time-slicing enabled.
