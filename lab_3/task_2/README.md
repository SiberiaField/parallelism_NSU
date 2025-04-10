# Matrix Mul using std::thread

## Description

There are files to build two programs:

* One of them is client-server application based on std::thread. Clients send tasks to server which then solves them, after that clients save res to __logs.txt__.
* Second is testing program which checks correctness of calculations.

## Usage

### Build

To build programs use:

```shell
make TYPE=<type-of-vars-in-client's-tasks>
```

You can also use ```make``` without variables. It's the same as ```make TYPE=double```.

After building there are two programs - __main__ and __test__.

### Launch

To launch client-server app use:

```shell
./main <Num-of-tasks>
```

* Num-of-tasks - a number of tasks for each client.

To launch test just use ```./test```.

### Note

You can use ```make clean``` to delete object files.
