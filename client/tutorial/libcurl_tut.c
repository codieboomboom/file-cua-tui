#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// This function gets called by libcurl when data arrives
// It's a callback function
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    
    // userp is a pointer to our string where we accumulate data
    char **response_ptr = (char **)userp;
    
    // Reallocate memory to fit new data
    *response_ptr = realloc(*response_ptr, strlen(*response_ptr) + realsize + 1);
    
    // Append new data to our string
    strncat(*response_ptr, (char *)contents, realsize);
    
    return realsize;
}

int main() {
    CURL *curl;
    CURLcode res;
    
    // Allocate memory for response
    char *response = malloc(1);
    response[0] = '\0';
    
    // Initialize libcurl
    curl = curl_easy_init();
    
    if(curl) {
        // Note: curl_easy_setopt sticks until you change options or until you resetjj
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8000/list"); // Damn this is painful to have to recompile again everytime...
        
        // Set callback function to receive data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        
        // Pass our response string to the callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        // Perform the request
        res = curl_easy_perform(curl);
        
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        } else {
            // Success! Print the response
            printf("Response:\n%s\n", response);
        }
        
        // Cleanup
        curl_easy_cleanup(curl);
    }
    
    free(response);
    return 0;
}