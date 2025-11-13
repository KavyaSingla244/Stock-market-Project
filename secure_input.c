#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "secure_input.h"
#include "colors.h" 

// --- OS-DEPENDENT PASSWORD MASKING (POSIX Systems: Linux/macOS) ---

/**
 * @brief Reads password input from the terminal while masking input with '*'.
 */
void readPassword(char *password, int max_len) {
    struct termios oldt, newt;
    int i = 0;
    char ch;

    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        perror("tcgetattr failed");
        exit(EXIT_FAILURE);
    }
    
    newt = oldt;
    // Disable canonical mode (line buffering) and echo (displaying characters)
    newt.c_lflag &= ~(ICANON | ECHO);
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
        perror("tcsetattr failed");
        exit(EXIT_FAILURE);
    }

    // Read characters one by one
    while (i < max_len - 1 && (ch = getchar()) != '\n' && ch != EOF) {
        if (ch == 127 || ch == '\b') { // Handle backspace/delete
            if (i > 0) {
                i--;
                printf("\b \b"); // Move cursor back, overwrite with space, move back again
            }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0'; // Null terminate the string
    printf("\n"); 

    // Restore original terminal settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) != 0) {
        perror("tcsetattr failed");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Validates a password against complexity requirements (Min 8 chars, 
 * 1 uppercase, 1 lowercase, 1 digit, 1 special char).
 */
bool validatePassword(const char *password) {
    // 1. Minimum length check
    if (strlen(password) < 8) {
        printf(RED "  - FAILED: Password must be at least 8 characters long.\n" RESET);
        return false;
    }

    bool has_upper = false;
    bool has_lower = false;
    bool has_digit = false;
    bool has_special = false;
    
    // Set of valid special characters
    const char *special_chars = "!@#$%^&*()-_+=[]{}|;:,.<>?/~`";

    for (int i = 0; password[i] != '\0'; i++) {
        if (isupper((unsigned char)password[i])) {
            has_upper = true;
        } else if (islower((unsigned char)password[i])) {
            has_lower = true;
        } else if (isdigit((unsigned char)password[i])) {
            has_digit = true;
        } else if (strchr(special_chars, password[i]) != NULL) {
            has_special = true;
        }
    }

    // Check all requirements and print specific failure messages
    bool isValid = has_upper && has_lower && has_digit && has_special;

    if (!has_upper) printf(RED "  - FAILED: Must contain at least one uppercase letter (A-Z).\n" RESET);
    if (!has_lower) printf(RED "  - FAILED: Must contain at least one lowercase letter (a-z).\n" RESET);
    if (!has_digit) printf(RED "  - FAILED: Must contain at least one digit (0-9).\n" RESET);
    if (!has_special) printf(RED "  - FAILED: Must contain at least one special character (!@#...).\n" RESET);

    return isValid;
}