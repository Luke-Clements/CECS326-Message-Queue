ipcs -q:

lists all the queues that are qurrently allocated in your memory

ipcrm -q <qid>

removes the specified queue based upon the qid

Listing the allocated queues allows you to see the queues that are "in the way" of 
  running the program again, then you can remove them with the other command so 
  they are no longer "in the way."
