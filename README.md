# FILE-CUA-TUI (read as faai-kwa-tkwee)
An attempt to develop a TUI-based client with C programming to a simple fileserver

## Overview
This repo has two parts: server and client

### Server
For simplicity sake, server is using http.server on Python, supporting a few default routes. While other production-grade frameworks for HTTP server like Flask and FastAPI are available, these might be overkill. At the heart of the project, it is about learning the lower ideas that are not abstracted away

### Client
Client is developed in C as a TUI. There is a CLI interface version that serve as learning point in early stage.

Client make use of libcurl and cjson

# File Server API Documentation

**Base URL:** `http://localhost:8000`

**Response Format:** All endpoints return JSON

---

## Endpoints

### 1. List Files in Directory

**Endpoint:** `/list`

**Method:** `GET`

**Description:** Returns a list of all files and directories in the specified path.

**Query Parameters:**

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `path` | string | No | `.` (current directory) | The directory path to list |

**Success Response (200):**

```json
{
  "success": true,
  "path": "/absolute/path/to/directory",
  "items": [
    {
      "name": "folder1",
      "type": "directory",
      "size": null,
      "modified": "2025-01-14T10:30:45.123456"
    },
    {
      "name": "file.txt",
      "type": "file",
      "size": 1024,
      "modified": "2025-01-14T09:15:30.789012"
    }
  ]
}
```

**Response Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `success` | boolean | Always `true` on success |
| `path` | string | Absolute path of the listed directory |
| `items` | array | Array of file/directory objects |
| `items[].name` | string | Name of the file or directory |
| `items[].type` | string | Either `"file"` or `"directory"` |
| `items[].size` | integer or null | Size in bytes (null for directories) |
| `items[].modified` | string | ISO 8601 timestamp of last modification |

**Error Response (500):**

```json
{
  "success": false,
  "error": "No such file or directory"
}
```

**Examples:**

```bash
# List current directory
curl "http://localhost:8000/list?path=."

# List specific directory
curl "http://localhost:8000/list?path=/Users/username/Documents"

# List parent directory
curl "http://localhost:8000/list?path=.."
```

**Example Response:**

```json
{
  "success": true,
  "path": "/Users/username/project",
  "items": [
    {
      "name": "src",
      "type": "directory",
      "size": null,
      "modified": "2025-01-14T10:30:45"
    },
    {
      "name": "README.md",
      "type": "file",
      "size": 2048,
      "modified": "2025-01-13T15:20:10"
    },
    {
      "name": "main.c",
      "type": "file",
      "size": 4096,
      "modified": "2025-01-14T09:45:30"
    }
  ]
}
```

---

### 2. Read File Contents

**Endpoint:** `/read`

**Method:** `GET`

**Description:** Returns the text content of a file.

**Query Parameters:**

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `path` | string | Yes | - | The file path to read |

**Success Response (200):**

```json
{
  "success": true,
  "path": "path/to/file.txt",
  "content": "The entire file content as a string.\nIncluding newlines and all text."
}
```

**Response Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `success` | boolean | Always `true` on success |
| `path` | string | The path of the file that was read |
| `content` | string | The complete file contents |

**Error Response (500):**

```json
{
  "success": false,
  "error": "[Errno 2] No such file or directory: 'invalid.txt'"
}
```

**Common Errors:**

- File not found
- Permission denied
- Is a directory (trying to read a directory as a file)
- Binary file (will try to read as text and may fail)

**Examples:**

```bash
# Read a text file
curl "http://localhost:8000/read?path=README.md"

# Read file with spaces (URL encoded)
curl "http://localhost:8000/read?path=my%20file.txt"

# Read from subdirectory
curl "http://localhost:8000/read?path=docs/guide.txt"
```

**Example Response:**

```json
{
  "success": true,
  "path": "README.md",
  "content": "# My Project\n\nThis is a sample project.\n\n## Installation\n\nRun `make` to build."
}
```

**Notes:**

- This endpoint is designed for text files only
- Binary files may cause errors or return garbled content
- Large files will be returned in full (no pagination)
- Newlines are preserved as `\n` in the JSON string

---

### 3. Get File/Directory Information

**Endpoint:** `/info`

**Method:** `GET`

**Description:** Returns detailed metadata about a file or directory.

**Query Parameters:**

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `path` | string | Yes | - | The file or directory path |

**Success Response (200):**

```json
{
  "success": true,
  "path": "/absolute/path/to/file.txt",
  "name": "file.txt",
  "type": "file",
  "size": 4096,
  "modified": "2025-01-14T09:45:30.123456",
  "created": "2025-01-10T14:20:00.789012"
}
```

**Response Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `success` | boolean | Always `true` on success |
| `path` | string | Absolute path of the file/directory |
| `name` | string | Base name (filename without path) |
| `type` | string | Either `"file"` or `"directory"` |
| `size` | integer | Size in bytes |
| `modified` | string | ISO 8601 timestamp of last modification |
| `created` | string | ISO 8601 timestamp of creation (or last metadata change on Unix) |

**Error Response (500):**

```json
{
  "success": false,
  "error": "[Errno 2] No such file or directory: 'missing.txt'"
}
```

**Examples:**

```bash
# Get info for a file
curl "http://localhost:8000/info?path=main.c"

# Get info for a directory
curl "http://localhost:8000/info?path=src"

# Get info for current directory
curl "http://localhost:8000/info?path=."
```

**Example Response (File):**

```json
{
  "success": true,
  "path": "/Users/username/project/main.c",
  "name": "main.c",
  "type": "file",
  "size": 4096,
  "modified": "2025-01-14T09:45:30",
  "created": "2025-01-10T14:20:00"
}
```

**Example Response (Directory):**

```json
{
  "success": true,
  "path": "/Users/username/project/src",
  "name": "src",
  "type": "directory",
  "size": 4096,
  "modified": "2025-01-14T10:30:45",
  "created": "2025-01-08T11:00:00"
}
```

**Notes:**

- Works for both files and directories
- `size` for directories is the directory entry size, not the total size of contents
- On macOS/Unix, `created` is actually the "ctime" (last metadata change time)
- True creation time requires additional platform-specific code

---

## Common Response Patterns

### Success Response Structure

All successful responses include:
```json
{
  "success": true,
  // ... endpoint-specific data
}
```

### Error Response Structure

All error responses include:
```json
{
  "success": false,
  "error": "Error message description"
}
```

And return HTTP status code `500 Internal Server Error`.

---

## Error Codes

| HTTP Status | Meaning | When it occurs |
|-------------|---------|----------------|
| 200 | OK | Request succeeded |
| 404 | Not Found | Invalid endpoint (not `/list`, `/read`, or `/info`) |
| 500 | Internal Server Error | File system error (permissions, not found, etc.) |

---

## URL Encoding

File paths with special characters must be URL encoded:

| Character | Encoded |
|-----------|---------|
| Space | `%20` |
| `/` | `%2F` (in query params only) |
| `?` | `%3F` |
| `&` | `%26` |
| `#` | `%23` |

**Example:**

```bash
# Path: "my documents/file (1).txt"
# Encoded: "my%20documents%2Ffile%20(1).txt"

curl "http://localhost:8000/read?path=my%20documents%2Ffile%20(1).txt"
```

**In C, use libcurl's encoding:**

```c
char *encoded = curl_easy_escape(curl, path, 0);
snprintf(url, sizeof(url), "http://localhost:8000/read?path=%s", encoded);
curl_free(encoded);
```

---

## Testing the API

### Using curl:

```bash
# Start server
python3 fileserver.py

# In another terminal, test endpoints
curl "http://localhost:8000/list?path=."
curl "http://localhost:8000/info?path=fileserver.py"
curl "http://localhost:8000/read?path=fileserver.py"
```

### Using a web browser:

Just open these URLs:
- http://localhost:8000/list?path=.
- http://localhost:8000/info?path=fileserver.py
- http://localhost:8000/read?path=fileserver.py

The JSON will be displayed (or downloaded, depending on browser).

---

## Example Client Usage in C

### List Files

```c
char url[512];
snprintf(url, sizeof(url), "http://localhost:8000/list?path=.");
char *response = http_get(url);

cJSON *root = cJSON_Parse(response);
cJSON *items = cJSON_GetObjectItem(root, "items");
int count = cJSON_GetArraySize(items);

for (int i = 0; i < count; i++) {
    cJSON *item = cJSON_GetArrayItem(items, i);
    char *name = cJSON_GetObjectItem(item, "name")->valuestring;
    char *type = cJSON_GetObjectItem(item, "type")->valuestring;
    printf("%s (%s)\n", name, type);
}
```

### Read File

```c
char url[512];
snprintf(url, sizeof(url), "http://localhost:8000/read?path=README.md");
char *response = http_get(url);

cJSON *root = cJSON_Parse(response);
cJSON *success = cJSON_GetObjectItem(root, "success");

if (cJSON_IsTrue(success)) {
    cJSON *content = cJSON_GetObjectItem(root, "content");
    printf("File contents:\n%s\n", content->valuestring);
} else {
    cJSON *error = cJSON_GetObjectItem(root, "error");
    printf("Error: %s\n", error->valuestring);
}
```

### Get Info

```c
char url[512];
snprintf(url, sizeof(url), "http://localhost:8000/info?path=main.c");
char *response = http_get(url);

cJSON *root = cJSON_Parse(response);
char *name = cJSON_GetObjectItem(root, "name")->valuestring;
char *type = cJSON_GetObjectItem(root, "type")->valuestring;
int size = cJSON_GetObjectItem(root, "size")->valueint;

printf("Name: %s\n", name);
printf("Type: %s\n", type);
printf("Size: %d bytes\n", size);
```

---

## Rate Limits

None - this is a local development server.

---

## Authentication

None - this server has no authentication. **Do not expose to the internet!**

---

## Security Notes

⚠️ **WARNING:** This server has **no security**:
- No authentication
- No path validation (can access any file the Python process can read)
- No rate limiting
- Intended for **local development only**

**Never run this on a public network or expose to the internet!**

---

## Quick Reference

| Endpoint | Purpose | Required Param | Returns |
|----------|---------|----------------|---------|
| `/list` | List directory contents | `path` (optional) | Array of items with name, type, size |
| `/read` | Read file contents | `path` (required) | File content as string |
| `/info` | Get file/directory metadata | `path` (required) | Detailed file information |

---

**Server Version:** 1.0  
**Last Updated:** January 14, 2026