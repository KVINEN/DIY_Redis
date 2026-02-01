# DIY_Redis

A lightweight, event-driven redis prototype built from scratch in C as part of my "1 New Project Every Month of 2026" challange.

## 📅 The 2026 Challange

This project is Part 1 of 12 in my year-long journey to build different projects in 2026. The focus is on creating functional prototypes or MVPs (Minimum Viable Products) rather than 100% feature-complete applications. If future months allow, I may return to earlier projects to add more advanced features.

_Note: Since this first project was started late in January, this version serves as a core networking prototype._

## 🚀 Overview

DIY_Redis is a minimal implementation of a key-value store server. It uses a single-threaded event loop with non-blocking I/O to handle multiple concurrent client connections efficently.

### Features
* **Event-Driven Architecture:** Uses the ```poll()``` system call to monitor multiple file descriptors simultaneously without the overhead of multi-threading.
* **Modular C Design:** Organized into logical components:
  * ```server.h```: Define the core ```RedisServer``` structure.
  * ```networking.c```: Handles socket initialization, binding, listening, and client management.
  * ```main.c```: Contains the central event loop.
  * ```commands.c```: Placeholder for the command parsing logic.
* **RESP-ish Communication:** Currently supports a classic "PING" command, responding with "+PONG\r\n" to verify connection health.

## 🛠️ Technical Details
* **Port:** 6380 (Adjusted from default 6379 to avoid conflicts during development).
* **Max Connections:** Supports up tp 1000 simultaneous file descriptors.
* **Socket Options:** Implements ```SO_REUSEADDR``` to allow immediate server restarts without "Address already in use" errors.

## 🏗️ Project Structure
```
.
├── server.h      # Core data structures & function prototypes
├── main.c        # Event loop & program entry point
├── networking.c  # Socket & connection logic
├── commands.c    # Command handling (In-progress)
└── README.md     # Project documentation
```

## 🚦 Getting started

### prerequisites
* A C compiler (e.g., ```gcc```)
* Linux environment (tested on Arch Linux)

### Compilation
To compile the server, run the following commands in your terminal:
```
gcc main.c networking.c -o redis_server
```

### Running the Server
```
./redis_server
```

###
You can test the server using ```netcat```:
```
# In a separate terminal
nc localhost 6380
PING
# Expected response: +PONG
```

## 📝License
This project is licensed under the MIT License.


 
