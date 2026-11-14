
# AccessWay API

C++ REST API for the AccessWay campus accessibility application.

## Requirements

- CMake 3.16 or newer
- A C++17 compiler
- Git
- Internet access during the first CMake configuration, to download dependencies

## Build

From the repository root, run:

```bash
cmake -S api -B build
cmake --build build
```

## Run

On Linux or macOS:

```bash
./build/accessway_api
```

On Windows, the executable may be located at:

```powershell
.\build\Debug\accessway_api.exe
```

The server listens on port `8080`.

## Health check

Open this URL after starting the server:

```text
http://localhost:8080/health
```

Expected response:

```json
{
  "status": "ok",
  "service": "AccessWay API"
}
```

## API endpoints

| Method | Endpoint | Purpose |
|---|---|---|
| GET | `/health` | Check whether the API is running |
| POST | `/building` | Load building locations and connections |
| GET | `/reviews` | Retrieve reviews |
| POST | `/reviews` | Submit a review |
| POST | `/reviews/verify` | Verify a review |
| GET | `/ranking` | Retrieve review priority rankings |
| GET | `/requests` | Retrieve help requests |
| POST | `/requests` | Create a help request |
| POST | `/requests/status` | Update a request status |
| GET | `/volunteers` | Retrieve volunteers |
| POST | `/volunteers` | Register a volunteer |
| POST | `/routes` | Find a route between two locations |

## Important integration notes

- The API uses the existing AccessWay C++ managers.
- Building, location, connection, volunteer, and request IDs are numeric.
- The frontend must map its location identifiers to backend numeric IDs before sending API requests.
- The server currently stores data in memory while it is running. Persistent database storage is not configured.
- Load building data using `POST /building` before using routes or submitting reviews.
- This API is an integration starting point and should be compiled and tested before deployment.
