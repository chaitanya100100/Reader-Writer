# Reader Writer
This is modified version of famous consumer-producer problem with N readers and 1 writer threads with a fix sized buffer.

## Compile and Run
- Compile using simple make and then run the executable
- Give number of readers, buffer size and total number of items writer is going to generate.  
```
make
./start 8 9 50
```
- It will run 8 readers threads with buffer size of 9 and writer will write total 50 items.

## Implementation Details
- one mutex for each buffer position.
- Whenever someone try to access it (Reader or writer), it locks the mutex.
- Thus when writer is writing one position, readers can access other position.
- Reader generates a random index and try to read that buffer item.
- Writer sequentially check each item. It can know whether item is read by every reader or not in O(1). If yes then it will rewrite it.
