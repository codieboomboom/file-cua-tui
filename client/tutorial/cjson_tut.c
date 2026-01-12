#include <stdio.h>
#include <cjson/cJSON.h>

int main() {
    // Example JSON response from server
    const char *json_string = 
        "{"
        "  \"success\": true,"
        "  \"path\": \"/home/user\","
        "  \"items\": ["
        "    {\"name\": \"folder1\", \"type\": \"directory\", \"size\": null},"
        "    {\"name\": \"file.txt\", \"type\": \"file\", \"size\": 1024}"
        "  ]"
        "}";
    
    // Parse the JSON string
    cJSON *root = cJSON_Parse(json_string);
    
    if (root == NULL) {
        printf("Error parsing JSON\n");
        return 1;
    }
    
    // Get the "success" field (boolean)
    cJSON *success = cJSON_GetObjectItem(root, "success");
    if (cJSON_IsBool(success)) {
        printf("Success: %s\n", cJSON_IsTrue(success) ? "true" : "false");
    }
    
    // Get the "path" field (string)
    cJSON *path = cJSON_GetObjectItem(root, "path");
    if (cJSON_IsString(path)) {
        printf("Path: %s\n", path->valuestring);
    }
    
    // Get the "items" array
    cJSON *items = cJSON_GetObjectItem(root, "items");
    if (cJSON_IsArray(items)) {
        int item_count = cJSON_GetArraySize(items);
        printf("Found %d items:\n", item_count);
        
        // Loop through each item in array
        for (int i = 0; i < item_count; i++) {
            cJSON *item = cJSON_GetArrayItem(items, i);
            
            // Get fields from each item
            cJSON *name = cJSON_GetObjectItem(item, "name");
            cJSON *type = cJSON_GetObjectItem(item, "type");
            cJSON *size = cJSON_GetObjectItem(item, "size");
            
            printf("  [%d] %s (%s)", i, 
                   name->valuestring, 
                   type->valuestring);
            
            // Size might be null for directories
            if (cJSON_IsNumber(size)) {
                printf(" - %d bytes", size->valueint);
            }
            printf("\n");
        }
    }
    
    // IMPORTANT: Always free the cJSON object when done!
    cJSON_Delete(root);
    
    return 0;
}