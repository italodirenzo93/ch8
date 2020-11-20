#include <stdio.h>
#include "log.h"

static void _logging_function(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    const char fmt[] = "[DEBUG] %s\n";

    // Write to text file
    FILE *fp = NULL;
    fp = fopen("log.txt", "a");
    if (fp != NULL)
    {
        fprintf(fp, fmt, message);
        fclose(fp);
        fp = NULL;
    }
    
    // Write to stderr
    fprintf(stderr, fmt, message);
}

int log_init()
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
    //SDL_LogSetOutputFunction(_logging_function, NULL);
    //freopen("log.txt", "a", stderr);
    return 0;
}
