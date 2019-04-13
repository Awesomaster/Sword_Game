#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define BUFF_SIZE 1000
#define MAX_INPUT 1000

#define OPTIN "t:h:m:n:d:"

long int power(int num, int index) {
    long int returnNum = num;
    for(int i = 0; i < (index-1); i++) {
        returnNum *= num;
    }
    return returnNum;
}

char *stringCat(char *str1, char *str2) {
    char *tempStr2 = strdup(str2);
    char *finalString = calloc(strlen(str1)+strlen(str2)+1, sizeof(char));
    sprintf(finalString, "%s%s", str1, str2);
    return finalString;
}

// Replaces all copies of key with value in line
char *replaceAll(char *line, char *name, char *value)
{
    char *pos = strstr(line, name);
    char *finalLine = calloc(1000, 1000);
    while (pos != NULL)
    {
        if (pos != NULL)
        {
            memcpy(finalLine, line, (pos - line));
            memcpy(finalLine + (pos - line), value, strlen(value));
            strcpy(finalLine + (pos - line) + strlen(value), pos + strlen(name));
            line = calloc(strlen(finalLine), strlen(finalLine));
            if (line == NULL)
            {
                fprintf(stderr, "Could not allocate space for word in 'replaceAll(%s, %s, %s)'", line, name, value);
                exit(EXIT_FAILURE);
            }
            strcpy(line, finalLine);
        }
        pos = strstr(line, name);
    }
    return line;
}

int main(int argc, char *argv[]) {
    FILE *fileRead;
    FILE *fileWrite;

    char *fileName = "default.txt";
    char *fileAttributes = "";

    long int minutesAgo = 0;
    long int hoursAgo = 0;
    long int daysAgo = 0;

    bool timeSet = false;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    long int dateNum = (tm.tm_year + 1900) * power(10, 8) + (tm.tm_mon + 1) * power(10, 6) + (tm.tm_mday) * power(10, 4) + (tm.tm_hour) * power(10, 2) + (tm.tm_min);

    // Take in optional inputs such as time since it was created, and others

    // #define OPTIN "t:h:m:n:d:"
    int opt;
    while ((opt = getopt(argc, argv, OPTIN)) != -1)
    {
        switch (opt)
        {
        case 't':
            // To set to current date & time
            dateNum = strtol(optarg, NULL, 10);
            printf("Date number: \"%lu\"\n", dateNum);
            break;
        case 'h':
            // Set the amount of hours created ago
            hoursAgo = strtol(optarg, NULL, 10);
            printf("Hours ago: \"%d\"\n", hoursAgo);
            break;
        case 'm':
            // Set the amount of minutes created ago
            minutesAgo = strtol(optarg, NULL, 10);
            printf("Minutes delayed: \"%d\"\n", minutesAgo);
            break;
        case 'n':
            // Set a file name
            printf("File name: \"%s\"\n", optarg);
            fileName = calloc(sizeof(char), strlen(optarg));
            strcpy(fileName, optarg);
            break;
        case 'd':
            // Set a file description seperated by _'s
            printf("Description: \"%s\"\n", optarg);
            fileAttributes = replaceAll(optarg, "_", " ");
            break;
        }
    }

    // Reading and copying contents of the file
    printf("name: %s\n", fileName);
    char buff[BUFF_SIZE];

    // Time Analysis
    int minutes = (dateNum % power(10, 2));
    int hours = (dateNum % power(10, 4)) / power(10, 2);
    int days = (dateNum % power(10, 6)) / power(10, 4);
    int months = (dateNum % power(10, 8)) / power(10, 6);
    int years = (dateNum % power(10, 12)) / power(10, 8);

    printf("mins: %d, hours: %d, days: %d, months: %d, years: %d\n", minutes, hours, days, months, years);
    while(minutesAgo > minutes) {
        hoursAgo += 1;
        minutesAgo -= 60; 
    } 
    while(hoursAgo > hours) {
        daysAgo += 1;
        hoursAgo -= 24;
    }
    dateNum -= minutesAgo;
    dateNum -= hoursAgo*power(10,2);
    dateNum -= daysAgo*power(10,4);

    // Change in time
    int deltaMins = tm.tm_min - minutes;
    int deltaHours = tm.tm_hour - hours;
    int deltaDays = tm.tm_mday - days;
    int deltaMonths = (tm.tm_mon + 1) - months;
    int deltaYears = (tm.tm_year + 1900) - years;

    if (deltaYears >= 1) {
        printf("done\n");
        fileAttributes = realloc(fileAttributes, strlen(fileAttributes)+9); //8 of which are for the word Ancient and the space, the last for the null byte
        fileAttributes = stringCat("Ancient ", fileAttributes);
    }

    // Date we want to set as creation date
    int nStrLen = (int)((ceil(log10(dateNum)) + 1) * sizeof(char));
    char dateStr[nStrLen];
    sprintf(dateStr, "%lu", dateNum);

    // Open the file for writting (which may also make it) and type something into it
    fileWrite = fopen(fileName, "a");
    printf("attr: %s\n", fileAttributes);
    fprintf(fileWrite, fileAttributes);
    fclose(fileWrite);

    // Touch the file and set its creation date to the number set
    char *updateFileStr = stringCat(stringCat(stringCat("touch -t ", dateStr), " "), fileName);
    printf("%s", updateFileStr);
    system(updateFileStr);
    
    return 0;
}