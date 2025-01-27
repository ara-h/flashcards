#include <stdio.h>
#include <string.h>
#include <regex.h>

# define VERSION 1.0
// MY_PATH_MAX is provisional. I would like to read up on PATH_MAX further.
# define MY_PATH_MAX 512


FILE *find_config() {
    char *config_search_paths[] = {
        "./" //,
        // getenv("HOME"),
        // "/etc/flashcards/"
    };
    int len = sizeof(config_search_paths)/sizeof(config_search_paths[0]);
    char config_file_name[] = "flashcards.conf";
    char full_config_path[MY_PATH_MAX];
    for (int i = 0; i < len; i++) {
        strncpy(full_config_path, config_search_paths[i], MY_PATH_MAX);
        // Assume the path has a final '/' (and that '/' is the separator).
        strncat(full_config_path,
                config_file_name,
                MY_PATH_MAX - strlen(config_file_name) - 1);
        FILE *file = fopen(full_config_path, "r");
        if (file != NULL) {
            printf("Found configuration file: %s\n", full_config_path);
            return file;
        }
    }
    fprintf(stderr, "Configuration file was not found.\n");
    return NULL;
}


/*
* Parse command line arguments. Depending on inputs, either add path to search
* path, show help text, show search paths, show version, or enter study mode.
*/
int main(int argc, char** argv) {
    // Find configuration file.
    FILE *config;
    if ((config = find_config()) == NULL) {
        fclose(config);
        return 1;
    }

    // Parse configuration file.
    char buf[256];
    regex_t regex;
    int reti = regcomp(
        &regex,
        "^SEARCH_PATH=(.*)$",
        REG_EXTENDED | REG_NEWLINE
    );
    if (reti != 0) {
        fprintf(stderr, "Could not compile regex.\n");
        regerror(reti, &regex, buf, sizeof(buf));
        fprintf(stderr, "%s\n", buf);
        fclose(config);
        regfree(&regex);
        return 1;
    }
    if (fgets(buf, sizeof(buf), config) == NULL) {
        fprintf(stderr, "There was a problem reading the config file.\n");
        fclose(config);
        regfree(&regex);
        return 1;
    }
    regmatch_t regmatch[2];
    reti = regexec(&regex, buf, 2, regmatch, 0);
    while (reti == REG_NOMATCH && fgets(buf, sizeof(buf), config) != NULL) {
        reti = regexec(&regex, buf, 2, regmatch, 0);
    }
    char search_path[MY_PATH_MAX];
    memset(search_path, 0, sizeof(search_path));
    if (!reti) {
        int final = regmatch[1].rm_eo;
        int initial = regmatch[1].rm_so;
        strncpy(search_path, buf + initial, final - initial);
    } else {
        if (reti == REG_NOMATCH) {
            fprintf(stderr, "No match was found in the file.\n");
        } else {
            regerror(reti, &regex, buf, sizeof(buf));
            fprintf(stderr, "Regex match failed: %s\n", buf);
        }
        fclose(config);
        regfree(&regex);
        return 1;
    }

    fclose(config);
    regfree(&regex);

    // Parse arguments.
    // If desired, show help and exit.
    // If desired, show search paths and exit.
    // If desired, show version and exit.
    // Enter study mode.
    return 0;
}
