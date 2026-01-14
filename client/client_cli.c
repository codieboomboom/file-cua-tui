#include <stdio.h>
#include <stdlib.h>
// #include <curl/curl.h>
// #include <cjson/cJSON.h>

# define CHOICE_BUFFER_SIZE 10

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
    // Program main loop
    while (1) {
        int menu_opt_selected = display_menu_and_get_choice();
        switch(menu_opt_selected) {
            case 1:
                printf("%s\n", "Case 1");
                break;
            case 2:
                printf("%s\n", "Case 2");
                break;
            case 3:
                printf("%s\n", "Case 3");
                break;
            case 4:
                printf("Quitting the program!\n");
                exit(0); // Exit the program
            default:
                printf("%s\n", " Default");
                break;
        }
    }

    return 0;
}