#ifndef SECURE_INPUT_H
#define SECURE_INPUT_H

#include <stdbool.h>
#include <termios.h> // Required for POSIX terminal control
#include <unistd.h>  // Required for STDIN_FILENO

// Max length matches the password field in struct User (50)
#define MAX_PASS_LENGTH 50 

/**
 * @brief Reads password input from the terminal while masking input with '*'.
 * * NOTE: This function is written for POSIX (Linux/macOS) systems using termios.h.
 * * If running on Windows, this function's implementation will need to be adapted 
 * * (e.g., using conio.h's getch()).
 * @param password The buffer to store the read password.
 * @param max_len The maximum length of the buffer.
 */
void readPassword(char *password, int max_len);

/**
 * @brief Validates a password against complexity requirements.
 * @param password The password string to validate.
 * @return true if the password meets all requirements, false otherwise.
 */
bool validatePassword(const char *password);

#endif