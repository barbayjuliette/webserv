# Webserv

In this project, we have to create our own webserver, using C++98.
We handled GET, POST (with cgi) and DELETE requests.
Thanks to this project we learned in detail about how web servers work, and were able to hone our C++ skills since this was our first big project using C++.

## Requirements

Some of the requirements we had to follow:

- The program has to take a configuration file as argument, or use a default path.

- It must be non-blocking and use only 1 poll() (or equivalent) for all the I/O
operations between the client and the server (listen included).

- poll() (or equivalent) must check read and write at the same time.

- You must never do a read or a write operation without going through poll() (or
equivalent).

## How to run
```
make
```

```
./webserv [configuration file]
```

Then go to Chrome and discover our beautiful webserver.

You will find examples of config files in /config_files
