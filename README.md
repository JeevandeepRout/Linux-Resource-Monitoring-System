# Linux-Resource-Monitoring-System

A lightweight C++17 client-server system for monitoring Linux machines. The client collects real-time system metrics (CPU, memory, disk, processes) and sends them to a central server via TCP (newline-delimited JSON) for live dashboarding, alerting, and CSV logging.

## ✨ Features
- **Client Daemon:** Tracks CPU, memory, disk, uptime, and kernel stats via `/proc`. Auto-reconnects on failure.
- **Central Server:** Manages multiple concurrent clients and logs historical data to CSV.
- **Alerts:** Flags clients as `NORMAL`, `WARNING`, `CRITICAL`, or `OFFLINE` based on performance thresholds.
- **Live TUI:** Terminal-based dashboard for real-time fleet monitoring.

## 🚀 Quick Start (Ubuntu/Debian)

**1. Install Dependencies:**
```bash
sudo apt update && sudo apt install build-essential nlohmann-json3-dev netcat-openbsd
```

**2. Build:**
```bash
git clone <repository-url> && cd Linux-Resource-Monitoring-System
make
```

**3. Run:**
Start the server (listens on port 5000):
```bash
./build/server
```
In another terminal, start the client:
```bash
./build/client
```
*(Press `Ctrl+C` to gracefully shut down the server or client)*

## 🧪 Testing
Run the automated metric and protocol test suite:
```bash
make test
```

## 📂 Project Structure

```text
Linux-Resource-Monitoring-System/
├── client/                     # Client daemon source files
│   ├── ClientIdentity.cpp      # Persistent hardware identity generator
│   ├── ClientIdentity.h
│   ├── NetworkClient.cpp       # POSIX TCP client socket implementation
│   ├── NetworkClient.h
│   ├── SystemMonitor.cpp       # Kernel telemetry collector (/proc, statvfs)
│   ├── SystemMonitor.h
│   └── main.cpp                # Client executable entry point
├── server/                     # Central telemetry server source files
│   ├── AlertManager.cpp        # Threshold classifier (NORMAL/WARN/CRIT)
│   ├── AlertManager.h
│   ├── ClientRegistry.cpp      # In-memory fleet tracking & heartbeat manager
│   ├── ClientRegistry.h
│   ├── ClientState.h           # Per-client state data structures
│   ├── CsvLogger.cpp           # Historical CSV metrics persistence
│   ├── CsvLogger.h
│   ├── Dashboard.cpp           # Real-time ANSI terminal dashboard (TUI)
│   ├── Dashboard.h
│   ├── Logger.cpp              # System event logging engine
│   ├── Logger.h
│   ├── Server.cpp              # TCP listener, NDJSON deframer & event loop
│   ├── Server.h
│   ├── main.cpp                # Server executable entry point
│   ├── test_csv.cpp            # CSV persistence test runner
│   ├── test_dashboard.cpp      # Dashboard rendering test harness
│   └── test_logger.cpp         # Event logger test harness
├── common/                     # Shared cross-boundary components
│   ├── Config.h                # Configuration data structures
│   ├── ConfigLoader.cpp        # JSON configuration parser
│   ├── ConfigLoader.h
│   ├── SystemData.h            # Telemetry model & JSON serialization
│   └── test_config.cpp         # ConfigLoader test harness
├── config/                     # Configuration definitions
│   └── config.json             # Fleet defaults & threshold rules
├── tests/                      # Automated test suite
│   ├── test_metrics.cpp        # Unit tests for kernel metric sampling
│   └── test_protocol.cpp       # Unit tests for NDJSON protocol integrity
├── logs/                       # Application runtime data & history
│   ├── metrics.csv             # Structured CSV time-series log
│   └── server.log              # Human-readable operational event log
├── build/                      # Build output artifacts directory
├── Makefile                    # Multi-target GNU build script
├── .gitignore                  # Git tracking exclusion rules
└── README.md                   # Comprehensive project documentation
```

## 🚧 Roadmap
- **Configuration:** Fully integrate `config/config.json` for hot-reloading alert thresholds.
- **Security:** Add TLS encryption and client authentication.
- **Storage & UI:** Migrate historical data to SQLite and build a web-based dashboard API.
