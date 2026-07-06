#define _GNU_SOURCE /* Required for setresuid, setresgid and clearenv profiles */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <crypt.h>
#include <time.h>
#include <syslog.h>
#include <fnmatch.h>
#include "local.h"

#define MAX_ARGS 256
#define MAX_LINE_LEN 512
#define CONFIG_PATH "/etc/froot.conf"
#define TIMESTAMP_DIR "/run/froot"
#define TIMESTAMP_LIFETIME 300
#define MAX_PASSWORD_ATTEMPTS 3

#define RULE_PERMIT  1
#define RULE_DENY    2
#define OPT_KEEPENV  4
#define OPT_NOPASS   8

typedef struct {
    int action_flags;
    char identity[64];
    char target_user[64];
    char target_group[64];
    char command_filter[128];
    char chroot_jail[256];
    long max_mem_bytes;
} FrootRule;

void sanitize_environment(int keepenv) {
    const char *bad_env_vars[] = {
        "LD_PRELOAD", "LD_LIBRARY_PATH", "LD_AUDIT", "LD_DEBUG",
        "IFS", "ENV", "BASH_ENV", "XAUTHORITY", "DISPLAY"
    };
    int num_bad_vars = sizeof(bad_env_vars) / sizeof(bad_env_vars[0]);

    if (!keepenv) {
        clearenv();
        setenv("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 1);
        setenv("USER", "root", 1);
        setenv("HOME", "/root", 1);
        setenv("TERM", "xterm-256color", 1);
    } else {
        for (int i = 0; i < num_bad_vars; i++) {
            unsetenv(bad_env_vars[i]);
        }
    }
}

int check_timestamp_session(const char *username) {
    char session_path[256];
    snprintf(session_path, sizeof(session_path), "%s/%s", TIMESTAMP_DIR, username);

    struct stat st;
    if (stat(session_path, &st) == -1) return 0;

    time_t current_time = time(NULL);
    if ((current_time - st.st_mtime) <= TIMESTAMP_LIFETIME) return 1;

    unlink(session_path);
    return 0;
}

void write_timestamp_session(const char *username) {
    char session_path[256];
    snprintf(session_path, sizeof(session_path), "%s/%s", TIMESTAMP_DIR, username);
    mkdir(TIMESTAMP_DIR, 0700);
    FILE *fp = fopen(session_path, "w");
    if (fp) {
        fprintf(fp, "%ld\n", (long)time(NULL));
        fclose(fp);
        chmod(session_path, 0600);
    }
}

int verify_user_password_native(const char *username) {
    struct spwd *shadow_entry = getspnam(username);
    if (!shadow_entry) {
        syslog(LOG_AUTH | LOG_ALERT, "froot: failure reading shadow record for %s", username);
        return 0;
    }

    int attempts = 0;
    while (attempts < MAX_PASSWORD_ATTEMPTS) {
        char *input_password = getpass(MSG_PASS_PROMPT);
        if (!input_password) return 0;

        char *hashed_input = crypt(input_password, shadow_entry->sp_pwdp);
        if (hashed_input && strcmp(hashed_input, shadow_entry->sp_pwdp) == 0) {
            return 1;
        }

        attempts++;
        syslog(LOG_AUTH | LOG_WARNING, "froot: password mismatch for %s (Attempt %d/%d)", username, attempts, MAX_PASSWORD_ATTEMPTS);
        
        if (attempts < MAX_PASSWORD_ATTEMPTS) {
            fprintf(stderr, "%s", MSG_PASS_RETRY);
        }
    }

    /* Brute force lock mitigation trigger phase */
    syslog(LOG_AUTH | LOG_CRIT, "froot: Brute force detected on TTY for identity: %s. Locking execution context.", username);
    fprintf(stderr, "%s", MSG_BRUTE_FORCE);
    sleep(10); 
    return 0;
}

int user_in_group(const char *username, const char *groupname) {
    struct group *grp = getgrnam(groupname);
    if (!grp) return 0;
    int i = 0;
    while (grp->gr_mem[i] != NULL) {
        if (strcmp(grp->gr_mem[i], username) == 0) return 1;
        i++;
    }
    return 0;
}

int parse_config_and_authorize(const char *username, const char *requested_cmd, int *keepenv, FrootRule *matched_rule) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) {
        syslog(LOG_AUTH | LOG_ERR, "froot: missing configuration file at %s", CONFIG_PATH);
        return RULE_DENY;
    }

    char line[MAX_LINE_LEN];
    int authorized_state = RULE_DENY;

    while (fgets(line, sizeof(line), fp)) {
        char *comment = strchr(line, '#');
        if (comment) *comment = '\0';
        
        char *token = strtok(line, " \t\r\n");
        if (!token) continue;

        FrootRule rule = {0, "", "", "", "", "", 0};

        if (strcmp(token, "permit") == 0) rule.action_flags |= RULE_PERMIT;
        else if (strcmp(token, "deny") == 0) rule.action_flags |= RULE_DENY;
        else continue;

        token = strtok(NULL, " \t\r\n");
        while (token) {
            if (strcmp(token, "keepenv") == 0) {
                rule.action_flags |= OPT_KEEPENV;
                token = strtok(NULL, " \t\r\n");
            } else if (strcmp(token, "nopass") == 0) {
                rule.action_flags |= OPT_NOPASS;
                token = strtok(NULL, " \t\r\n");
            } else break;
        }

        if (!token) continue;
        strncpy(rule.identity, token, sizeof(rule.identity) - 1);

        token = strtok(NULL, " \t\r\n");
        while (token) {
            if (strcmp(token, "as") == 0) {
                token = strtok(NULL, " \t\r\n");
                if (token) {
                    if (token[0] == ':') strncpy(rule.target_group, token + 1, sizeof(rule.target_group) - 1);
                    else strncpy(rule.target_user, token, sizeof(rule.target_user) - 1);
                }
            } else if (strcmp(token, "chroot") == 0) {
                token = strtok(NULL, " \t\r\n");
                if (token) strncpy(rule.chroot_jail, token, sizeof(rule.chroot_jail) - 1);
            } else if (strcmp(token, "maxmem") == 0) {
                token = strtok(NULL, " \t\r\n");
                if (token) {
                    long val = atol(token);
                    char unit = token[strlen(token) - 1];
                    if (unit == 'M' || unit == 'm') rule.max_mem_bytes = val * 1024 * 1024;
                    else if (unit == 'G' || unit == 'g') rule.max_mem_bytes = val * 1024 * 1024 * 1024;
                    else rule.max_mem_bytes = val;
                }
            } else if (strcmp(token, "cmd") == 0) {
                token = strtok(NULL, " \t\r\n");
                if (token) strncpy(rule.command_filter, token, sizeof(rule.command_filter) - 1);
            }
            token = strtok(NULL, " \t\r\n");
        }

        int identity_match = 0;
        if (rule.identity[0] == ':') {
            if (user_in_group(username, rule.identity + 1)) identity_match = 1;
        } else if (strcmp(rule.identity, username) == 0) identity_match = 1;

        if (identity_match && strlen(rule.command_filter) > 0) {
            if (fnmatch(rule.command_filter, requested_cmd, 0) != 0) {
                identity_match = 0;
            }
        }

        if (identity_match) {
            authorized_state = (rule.action_flags & RULE_DENY) ? RULE_DENY : RULE_PERMIT;
            if (authorized_state == RULE_PERMIT) {
                *keepenv = (rule.action_flags & OPT_KEEPENV) ? 1 : 0;
                *matched_rule = rule;
            }
        }
    }

    fclose(fp);
    return authorized_state;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, MSG_USAGE, argv[0]);
        return EXIT_FAILURE;
    }

    openlog("froot", LOG_PID, LOG_AUTH);

    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (!pw) {
        closelog();
        return EXIT_FAILURE;
    }

    int keepenv = 0;
    FrootRule rule = {0, "", "", "", "", "", 0};
    int auth_status = parse_config_and_authorize(pw->pw_name, argv[1], &keepenv, &rule);
    
    if (auth_status != RULE_PERMIT) {
        syslog(LOG_AUTH | LOG_NOTICE, "froot: unauthorized attempt by %s", pw->pw_name);
        fprintf(stderr, "%s", MSG_UNAUTHORIZED);
        closelog();
        return EXIT_FAILURE;
    }

    if (!(rule.action_flags & OPT_NOPASS)) {
        if (!check_timestamp_session(pw->pw_name)) {
            if (!verify_user_password_native(pw->pw_name)) {
                fprintf(stderr, "%s", MSG_AUTH_FAIL);
                closelog();
                return EXIT_FAILURE;
            }
            write_timestamp_session(pw->pw_name);
        }
    }

    syslog(LOG_AUTH | LOG_INFO, "froot: user %s authorized for command: %s", pw->pw_name, argv[1]);

    if (strlen(rule.chroot_jail) > 0) {
        if (chroot(rule.chroot_jail) == -1 || chdir("/") == -1) {
            perror(MSG_JAIL_FAIL);
            closelog();
            return EXIT_FAILURE;
        }
    }

    if (rule.max_mem_bytes > 0) {
        struct rlimit rl;
        rl.rlim_cur = rule.max_mem_bytes;
        rl.rlim_max = rule.max_mem_bytes;
        if (setrlimit(RLIMIT_AS, &rl) == -1) {
            /* Handled non-fatally to let process continue under legacy footprints */
        }
    }

    uid_t target_uid = 0;
    gid_t target_gid = 0;

    if (strlen(rule.target_user) > 0) {
        struct passwd *tpw = getpwnam(rule.target_user);
        if (tpw) { target_uid = tpw->pw_uid; target_gid = tpw->pw_gid; }
    } else if (strlen(rule.target_group) > 0) {
        struct group *tgr = getgrnam(rule.target_group);
        if (tgr) { target_uid = uid; target_gid = tgr->gr_gid; }
    }

    if (setresgid(target_gid, target_gid, target_gid) == -1 || 
        setresuid(target_uid, target_uid, target_uid) == -1) {
        closelog();
        return EXIT_FAILURE;
    }

    sanitize_environment(keepenv);
    closelog();

    char *cmd_args[MAX_ARGS];
    int i;
    for (i = 1; i < argc && i < (MAX_ARGS - 1); i++) cmd_args[i - 1] = argv[i];
    cmd_args[i - 1] = NULL;

    if (execvp(cmd_args[0], cmd_args) == -1) {
        perror(MSG_EXEC_FAIL);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
