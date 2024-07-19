# webserv
 Hypertext Transfer Protocol and web server.
 
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv_main.jpg">
  
## Introduction
* This project is to implement a simple HTTP server in C++ 98
* The server operates using IO multiplexing.
* The execution environment is designed for Mac(Kqueue) and may not be supported on other platforms.
* Most of the operations have been implemented based on Nginx.

## Process
* The program operates with two loops: one loop fetches events from the kernel, and the other loop processes these events.   
  In case an exception occurs during the event processing, it is caught and a response message is composed to be sent to the client.
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv1.jpg">

* The configuration file contains rules that the server can use to process requests. These rules are parsed according to predefined classes.
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv2.jpg">

* Here's the description of the configuration file.
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv3.jpg">

* Once parsing is complete, sockets are created and set to listen based on the parsed server information.   
  Below is an illustration of the program's socket communication method:
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv4.jpg">

* Kqueue is a function that supports IO multiplexing.   
  Through Kqueue, you can receive events to send or receive data to/from the buffers of file descriptors.
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv5.jpg">

* Events are branched and processed according to their respective conditions.
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv6.jpg">

* Since the program must continuously process events, it handles all file descriptors (FDs) in a non-blocking, asynchronous manner. Therefore, because the order of operations is not guaranteed, it assigns state values to each object to facilitate performing actions based on their respective states.
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv7.jpg">

* Below is a detailed flowchart.
  <img alt="main" src="https://github.com/leebo155/webserv/blob/main/img/webserv_flow.jpg">

## Makefile
 
| Rules | Description |
| ----- | ----------- |
| all | Compile a program webserv. |
| clean | Remove all the temporary generated files. |
| fclean | Remove all the generated files. |
| re | Remove all the generated files and compile a webserv. |

## Usage

Run the configuration file as an argument.
```shell
$> ./webserv [configuration file]
```

Doesn't necessarily require an argument.
```shell
$> ./webserv
```
  In this case, the file in the default path(conf/defualt.conf) is configured.
