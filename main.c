#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

# define VERSION_MAJOR 0
# define VERSION_MINOR 0
# define VERSION_MICRO 1

void show_help(char *ex_name);


int parse_config(FILE *config,
                 char *reg_buf, unsigned long reg_buf_sz,
                 char *path_buf, unsigned long path_buf_sz);


/*
* Parse command line arguments. Depending on inputs, either add path to search
* path, show help text, show search paths, show version, or enter study mode.
*/
int main(int argc, char** argv) {
    int version_flag, help_flag, search_paths_flag;
    version_flag = help_flag = search_paths_flag = 0;
    char new_path[1024];
    int c;
    while ((c = getopt(argc, argv, "a:hpV")) != -1) {
        switch(c) {
            case 0:
                break;
            case 'a':
                strncpy(new_path, optarg, 1024);
                break;
            case 'h':
                help_flag = 1;
                break;
            case 'p':
                search_paths_flag = 1;
                break;
            case 'V':
                version_flag = 1;
                break;
            case '?':
                if (optopt == 'a') {
                    fprintf(stderr, "Option -%c requires an argument.", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c`.\n", optopt);
                } else {
                    fprintf(
                        stderr,
                        "Unkown option character `\\x%x`.\n",
                        optopt
                    );
                }
                show_help(argv[0]);
                return 1;
            default:
                abort();
        }
    }

    if (help_flag) {
        show_help(argv[0]);
        return 0;
    }

    if (version_flag) {
        printf(
            "%s: %d.%d.%d\n",
            argv[0],
            VERSION_MAJOR,
            VERSION_MINOR,
            VERSION_MICRO
        );
        return 0;
    }

    FILE *config;
    if ((config = fopen("./flashcards.conf", "r")) == NULL) {
        fprintf(stderr, "Could not open ./flashcards.conf\n");
        printf("Using default search path \"./\"\n");
    }

    if (config != NULL) {
        // Parse configuration file.
        char reg_buf[4096];
        memset(reg_buf, 0, 4096);
        char path_buf[4096];
        memset(path_buf, 0, 4096);
        int res;
        res = parse_config(config, reg_buf, 4096, path_buf, 4096);
        fclose(config);
        if (res < 0) {
            return 1;
        }
    }

    // Get search paths.
    //      Paths included are expected to be separated by ':'.
    // Do the showing of the search paths and the exiting, if desired.
    // Do the adding of search paths and the exiting, if desired.
    // Enter deck chooser.
    // Display a scrolling ncurses menu, where the items are the truncated
    //      deck names. Each item has a pointer to a function that starts study
    //      mode with the deck identifier passed in.
    // Enter study mode.
    return 0;
}


void show_help(char *ex_name) {
    printf(
        "Usage: %s [option...] [-a path]\n"
                "Options:\n"
                "   -a path     add path(s) to the search paths, :-separated\n"
                "   -h          show this help text\n"
                "   -p          show search paths\n"
                "   -V          show version\n",
        ex_name
    );
    return;
}


int parse_config(FILE *config,
                 char *reg_buf, unsigned long reg_buf_sz,
                 char *path_buf, unsigned long path_buf_sz) {
    regex_t regex;
    int reti;
    reti = regcomp(
        &regex,
        "^SEARCH_PATH=(.*)$",
        REG_EXTENDED | REG_NEWLINE
    );
    if (reti != 0) {
        fprintf(
            stderr,
            "parse_config: could not compile regex\n"
        );
        regerror(reti, &regex, reg_buf, reg_buf_sz);
        fprintf(stderr, "%s\n", reg_buf);
        regfree(&regex);
        return -1;
    }
    if (fgets(reg_buf, reg_buf_sz, config) == NULL) {
        fprintf(
            stderr,
            "parse_config: there was a problem reading the config file.\n"
        );
        regfree(&regex);
        return -1;
    }
    regmatch_t regmatch[2];
    reti = regexec(&regex, reg_buf, 2, regmatch, 0);
    while (reti == REG_NOMATCH
           && fgets(reg_buf, reg_buf_sz, config) != NULL) {
        reti = regexec(&regex, reg_buf, 2, regmatch, 0);
    }
    if (!reti) {
        int final = regmatch[1].rm_eo;
        int initial = regmatch[1].rm_so;
        strncpy(path_buf, reg_buf + initial, final - initial);
    } else {
        if (reti == REG_NOMATCH) {
            fprintf(
                stderr,
                "parse_config: no match was found in the file.\n"
            );
        } else {
            regerror(reti, &regex, reg_buf, reg_buf_sz);
            fprintf(
                stderr,
                "parse_config: regex match failed: %s\n",
                reg_buf
            );
        }
        regfree(&regex);
        return -1;
    }
    regfree(&regex);
    return 1;
}
