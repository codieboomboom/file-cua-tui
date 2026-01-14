#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
// #include <cjson/cJSON.h>

# define CHOICE_BUFFER_SIZE 10
# define CLIENT_SUCCESS 0
# define BASEURL "http://localhost:8000/"

// Define a memory structure to be used with receiving callbacks data
typedef struct memory {
    char *resp_buffer;
    size_t size;
} ReceivedMemory_t;

// Callback function to process the response
// This is from https://curl.se/libcurl/c/getinmemory.html
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size *nmemb;
    ReceivedMemory_t *mem = (ReceivedMemory_t *)userp; // Cast the user pointer into pointer to memory
    // Reallocate the memory to "receive" more response
    char *ptr = realloc(mem->resp_buffer, mem->size + realsize + 1);
    if (!ptr) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0; // Out of memory, reallocate failed
    }

    mem->resp_buffer = ptr; // Assign the updated memory (more space)
    // Put more contents just received into the memory (updated)
    memcpy(&(mem->resp_buffer[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->resp_buffer[mem->size] = 0; // Null terminate

    return realsize;
}

// HTTP Stuffs
CURLcode http_get(char* url){
    CURL *curl;
    CURLcode result = CURLE_OK;
    ReceivedMemory_t chunk;

    chunk.resp_buffer = malloc(1); // can be grown as needed
    chunk.size = 0;

    // Init the curl session for this request
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

        // perform request
        result = curl_easy_perform(curl);

        // error checking
        if (result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        } else {
            // success
            printf("Response:\n%s\n", chunk.resp_buffer);
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    // Free up the malloc before return
    // no need cleanup curl as should be done inside the guard branch
    free(chunk.resp_buffer);

    return result;
}


// process handling based on menu selections
int do_list() {
    CURLcode ret;
    char url[256];
    const char* path = "list";
    snprintf(url, sizeof(url), "%s%s", BASEURL, path);
    ret = http_get(url);
    if (ret != CURLE_OK) {
        printf("GET failed: %s", curl_easy_strerror(ret));
    }
    return CLIENT_SUCCESS;
}

int do_read() {
    return CLIENT_SUCCESS;
}

int do_info() {
    return CLIENT_SUCCESS;
}

// Display menu and get input
int display_menu_and_get_choice() {
    int choice;
    int return_code;
    while (1) {
        printf("\n");
        printf("╔════════════════════════════════════════╗\n");
        printf("║      File Server Client Menu           ║\n");
        printf("╠════════════════════════════════════════╣\n");
        printf("║  1. List files in directory            ║\n");
        printf("║  2. Read file contents                 ║\n");
        printf("║  3. Get file info                      ║\n");
        printf("║  4. Quit                               ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("\nEnter choice: ");

        return_code = scanf("%d", &choice);
        // Successfully read and parse the choice digit
        if (return_code == 1) {
            return choice;
        } else {
            printf("Invalid input! Please enter a valid choice (in digit).\n");
            // Clear the buffer, otherwise the buffer still have something which leads to
            // the next scanf to go haywire
            scanf("%*[^\n]"); // match and discard
        }
    }
}

int main() {
    // Some global setup
    CURLcode curl_rc = curl_global_init(CURL_GLOBAL_ALL);
    if (curl_rc) {
        printf("Global setup for curl failed!");
        return (int)curl_rc;
    }

    // Program main loop
    while (1) {
        int menu_opt_selected = display_menu_and_get_choice();
        switch(menu_opt_selected) {
            case 1:
                if (do_list() != CLIENT_SUCCESS) {
                    printf("Error Occured while handling request to list all files on server! Please Retry!");
                }
                break;
            case 2:
                if (do_read() != CLIENT_SUCCESS) {
                    printf("Error Occured while handling request to read file on server! Please Retry!");
                }
                break;
            case 3:
                if (do_info() != CLIENT_SUCCESS) {
                    printf("Error Occured while handing request to get file metadata! Please retry!");
                }
                break;
            case 4:
                printf("Quitting the program!\n");
                exit(0); // Exit the program
            default:
                printf("The request item is not on menu! Please retry");
                break;
        }
    }

    curl_global_cleanup();

    return 0;
}