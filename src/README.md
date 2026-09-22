# HTTP Server

A lightweight HTTP server and web framework written in **C++**, built from scratch to understand how modern web servers and frameworks work internally.

The framework follows a design similar to Go's `net/http`, where routes are registered with handler functions.

---

## Architecture

```text
                           Client
                              │
                              │ HTTP Request
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│                          HTTP Server                             │
│                                                                  │
│   TCP Connection                                                 │
│        │                                                         │
│        ▼                                                         │
│   HTTP Parser                                                    │
│        │                                                         │
│        ▼                                                         │
│   Router                                                         │
│        │                                                         │
│        ▼                                                         │
│   Handler Function                                               │
│        │                                                         │
│        ▼                                                         │
│   Response Writer                                                │
└──────────────────────────────────────────────────────────────────┘
                              │
                              │ HTTP Response
                              ▼
                           Client
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
HTTP Request
        │
        ▼
HTTP Parser
        │
        ▼
Router
        │
        ▼
Find matching handler
        │
        ▼
Execute handler
        │
        ▼
Return Response
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

## Design Philosophy

Each component has a single responsibility.

| Component | Responsibility |
|-----------|----------------|
| TCP Connection | Accept client connections and transfer bytes |
| HTTP Parser | Parse raw HTTP requests |
| Router | Match `(Method, Path)` to a handler |
| Handler | Execute application logic |
| Response Writer | Serialize and send HTTP responses |

The Router never performs business logic—it only resolves which handler should process the request.

---

## Goals

- Learn how HTTP works internally
- Build a modular HTTP server in modern C++
- Keep the API minimal and easy to use
- Follow Go's routing philosophy while remaining idiomatic C++
