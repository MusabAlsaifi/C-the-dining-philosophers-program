# C-the-dining-philosophers-program

The Dining Philosophers

The Dining Philosophers problem is a classic multi-process synchronization
problem. The problem consists of five philosophers sitting at a table who do
nothing but think and eat. Between each philosopher, there is a single fork
In order to eat, a philosopher must have both forks. A problem can arise if
each philosopher grabs the fork on the right, then waits for the fork on the
left. In this case a deadlock has occurred, and all philosophers will starve.
Also, the philosophers should be fair. Each philosopher should be able to eat
as much as the rest.

Implement in the C language the dining philosophers program with the use of threads and mutexes from the
pthreads library.
