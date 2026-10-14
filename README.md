# HTTP Server

A lightweight HTTP server and web framework written in **C++**, built from scratch to understand how modern web servers and frameworks work internally.

The framework follows a design similar to Go's `net/http`, where routes are registered with handler functions.

---

## Architecture
``` mermaid
flowchart TD
    C["Client"]
    E["Event Loop Thread<br/>epoll_wait / accept / recv / send"]
    CS["Connection State<br/>Input and Output Buffers"]
    Q["Thread-Safe Task Queue"]
    WA["Worker A"]
    WB["Worker B"]
    R["Completed Response Queue"]
    N["eventfd Notification"]

    C <-->|"Socket I/O"| E
    E <--> CS
    E -->|"Complete HTTP Request"| Q
    Q --> WA
    Q --> WB
    WA --> R
    WB --> R
    R --> N
    N -->|"Wake Event Loop"| E
```
---

## Routing

Routes are registered during server initialization.

Inspired by Go's `net/http`.

```cpp
Server app;

app.Get("/", HomeHandler);
app.Get("/users", ListUsers);
app.Post("/users", CreateUser);

app.Run(8080);
```

Internally, the router stores something conceptually similar to:

```cpp
{
    { GET, "/" }        -> HomeHandler,
    { GET, "/users" }   -> ListUsers,
    { POST, "/users" }  -> CreateUser
}
```

For every incoming request:

```text
TCP Connection
      |
      v
epoll_wait()
      |
      | EPOLLIN
      v
recv()
      |
      v
Input Buffer
      |
      v
HTTP Parser
      |
      | Complete request?
      v
Task Queue
      |
      v
Worker Thread
      |
      v
Router -> Handler
      |
      v
Completed Response Queue
      |
      v
eventfd -> Wake Event Loop
      |
      v
Output Buffer
      |
      | Socket writable
      v
send()
      |
      v
Client

```

---

## Request Lifecycle

```text
Client
    │
    ▼
TCP Connection
    │
    ▼
HTTP Parser
    │
    ▼
Router
    │
    ▼
Handler
    │
    ▼
Response Writer
    │
    ▼
Client
```

---



## Goals

- Learn how HTTP works internally
- Build a modular HTTP server in modern C++
- Keep the API minimal and easy to use
- Follow Go's routing philosophy while remaining idiomatic C++
