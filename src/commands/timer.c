#include <stdlib.h>
#include <time.h>

#include "command.h"
#include "options.h"
#include "kv.h"
#include "strbuf.h"
#include "docket.h"
#include "report.h"

#define DOCKET_TIMER_STD_PATH "/Users/stas/.timer"
enum timer_op_st {
    TIMER_OP_OK,
    TIMER_OP_ERROR,
};


enum timer_type {
    UNKNOWN,
    REGULAR,
};


enum timer_acc_mode {
    TR_ALL = 1 << 0,
    TR_CURRENT = 1 << 1,
};


enum timer_call_flag {
    USER_CALL =   1 << 0,
    CHILD_CALL =  1 << 1,
    PARENT_CALL = 1 << 2,
};


struct timer;
struct _timer_vmtable {
    int (*start)(struct timer *timer, int suppress_error, int flags);
    int (*stop) (struct timer *timer, int suppress_error, int flags);
};


static int regular_start(struct timer *tm, int suppress_error, int flags);
static int regular_stop(struct timer *tm, int suppress_error, int flags);
struct _timer_vmtable standalone_vmt = {
    regular_start,
    regular_stop,
};


struct timer {
    const char *name;
    struct timer *parent;
    enum timer_type type;
    struct kvsrc *kv;
    struct word_trie *index_node;
    const struct _timer_vmtable *vmt;
};


static struct timer *init_timer(struct timer *tm);
static struct timer *get_timer_by_name(struct kvsrc *kv, const char *timer_name);
int timer_is_running(struct timer *tm);
static unsigned long timer_running_time(struct timer *tm, int mode);
static void timer_bind_methods(struct timer *timer);
static int timer_start(struct timer *timer, int suppress_error, int flags);
static int timer_stop(struct timer *timer, int suppress_error, int flags);
const char *duration_from_ul(char buf[], unsigned long t);

static int cmd_start(int argc, const char **argv);
static int cmd_stop(int argc, const char **argv);
static int cmd_new(int argc, const char **argv);
static int cmd_list(int argc, const char **argv);
static int cmd_stat(int argc, const char **argv);


static const struct command commands[] = {
    {"new", cmd_new},
    {"start", cmd_start},
    {"stop", cmd_stop},
    {"list", cmd_list},
    {"stat", cmd_stat},
    {NULL, NULL},
};


/* Entry point */
int
cmd_timer(int argc, const char **argv) {
    if (argc < 1) {
        return 0;
    }

    struct command *cmd = get_command(argv[0], commands);
    if (cmd == NULL || cmd->cmd(--argc, ++argv) == 0) {
        return 0;
    }
    return 1;
}

static struct timer *
create_timer(struct kvsrc *kv,
             const char *timer_name,
             const char *parent,
             const char *timer_type,
             int suppress_duplicate) {
    struct timer *tm = malloc(sizeof(struct timer));
    tm->name = NULL;
    tm->parent = NULL;
    tm->kv = kv;

    struct timer *found = NULL;
    if((found = get_timer_by_name(kv, timer_name)) != NULL) {
        if (suppress_duplicate) {
            return NULL;
        }
        die_error("Duplicate timer");
    }
    tm->name = timer_name;

    char max_str[24] = "";
    struct word_trie *index_node = kv_get(kv, "docket:timer");
    int max = trie_get_max_int_child(index_node);
    sprintf(max_str, "%d", max + 1);

    kv_add(kv, build_path("docket:timer", max_str, "name"), timer_name);
    kv_add(kv, build_path("docket:timer", max_str, "type"), timer_type);

    if(parent) {
        if((found = get_timer_by_name(kv, parent)) == NULL) {
            die_error("No such timer '%s'", parent);
        }
        tm->parent = found;
        kv_add(kv, build_path("docket:timer", max_str, "parent"), parent);
    }

    return tm;
}

static int
cmd_new(int argc, const char **argv) {
    char *parent = "*";
    struct option timer_new_options[] = {
        {"p", "parent", {.required = 0, .has_args = 1}, &parent},
        {NULL, NULL, {0}, NULL},
    };

    if (argc < 1) return 0;

    const char *timer_name = argv[0];
    argc--, argv++;

    char err[1024] = "";
    int rv = options_populate(err, &argc, &argv, timer_new_options);
    if (rv != 0) {
        die_error("%s", err);
    }

    struct kvsrc *kv = NULL;
    kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);
    // root timer
    create_timer(kv, "*", NULL, "regular", 1);
    create_timer(kv, timer_name, parent,  "regular", 0);
    kv_sync(kv);
    return 1;
}


static int
cmd_start(int argc, const char **argv) {
    if (argc < 1) return 0;

    struct kvsrc *kv = NULL;
    kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);

    struct timer *tm = NULL;
    if((tm = get_timer_by_name(kv, argv[0])) == NULL) {
        die_error("No such timer '%s'", argv[0]);
    }

    int rc = timer_start(tm, 0, USER_CALL);
    if (rc != 1) {
        return 0;
    }

    kv_sync(kv);
    return 1;
}


static int
cmd_stop(int argc, const char **argv) {
    if (argc < 1) return 0;

    struct kvsrc *kv = NULL;
    kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);

    struct timer *tm = NULL;
    if((tm = get_timer_by_name(kv, argv[0])) == NULL) {
        die_error("No such timer '%s'", argv[0]);
    }

    fprintf(stderr, "start");
    int rc = timer_stop(tm, 0, USER_CALL);
    fprintf(stderr, "stop");
    if (rc != 1) {
        return 0;
    }

    kv_sync(kv);
    return 1;
}


static int
cmd_list(int argc, const char **argv) {
    struct kvsrc *kv = NULL;
    kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);

    struct word_trie *root = kv_get(kv, "docket:timer");
    struct word_trie *index_node = NULL;
    struct timer tm = {0};

    char buf[24];
    while((index_node = trie_loop_children(index_node, root))) {
        tm.kv = kv;
        tm.index_node = index_node;
        init_timer(&tm);

        memset(buf, 0, 24);
        if (timer_is_running(&tm)) {
            fprintf(stdout, "%s: %s (%s)\n",
                    tm.name,
                    "running",
                    duration_from_ul(buf, timer_running_time(&tm, TR_CURRENT)));
        } else {
            fprintf(stdout, "%s: %s\n", tm.name, "stopped");
        }
    }
    return 1;
}


static int
cmd_stat(int argc, const char **argv) {
    struct kvsrc *kv = NULL;
    kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);

    struct word_trie *root = kv_get(kv, "docket:timer");
    struct word_trie *index_node = NULL;
    struct timer tm = {0};

    char buf[24];
    while((index_node = trie_loop_children(index_node, root))) {
        tm.kv = kv;
        tm.index_node = index_node;
        init_timer(&tm);

        memset(buf, 0, 24);
        fprintf(stdout, "%s: %s (%s)\n",
                tm.name,
                (timer_is_running(&tm) ? "running" : "stopped"),
                duration_from_ul(buf, timer_running_time(&tm, TR_ALL)));
    }
    return 1;
}


static struct timer *
init_timer(struct timer *tm) {
    tm->name = trie_get_value(trie_get_path(tm->index_node, "name"), 0);
    tm->parent = NULL;

    const char *type = trie_get_value(trie_get_path(tm->index_node, "type"), 0);
    const char *parent = trie_get_value(trie_get_path(tm->index_node, "parent"), 0);

    if (parent) {
        tm->parent = get_timer_by_name(tm->kv, parent);
    }

    if (strcmp(type, "regular") == 0) {
        tm->type = REGULAR;
    } else {
        die_error("Unknown timer type '%s'", type);
    }
    timer_bind_methods(tm);
    return tm;
}


static struct timer *
get_timer_by_name(struct kvsrc *kv, const char *name) {
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("docket:timer:*:name");
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(kv->trie, loop_ptr))) {
        if (trie_has_value(LOOP_ONSTACK_TRIE(loop_ptr), cmp_str, (void *)name)) {
            // get index node
            struct word_trie *index_node = TAILQ_PREV(LOOP_ONSTACK_TRIE(loop_ptr), loop_head, tries);

            struct timer *tm = malloc(sizeof(struct timer));
            tm->kv = kv;
            tm->index_node = index_node;
            return init_timer(tm);
        }
    }
    return NULL;
}


static int
timer_start(struct timer *tm, int suppress_error, int flags) {
    return tm->vmt->start(tm, suppress_error, flags);
}


static int
timer_stop(struct timer *tm, int suppress_error, int flags) {
    return tm->vmt->stop(tm, suppress_error, flags);
}


static void
timer_bind_methods(struct timer *tm) {
    switch (tm->type) {
        case REGULAR:
            tm->vmt = &standalone_vmt;
            break;
        default:
            die_fatal("BUG: trying to bind vmt on unknown timer type");
            break;
    }
}


int
timer_is_running(struct timer *tm) {
    struct word_trie *timings = trie_get_path(tm->index_node, "timings");
    struct word_trie *max_node = trie_get_max_int_child_node(timings);

    if(trie_get_path(max_node, "start") == NULL) {
        return 0;
    }

    if(trie_get_path(max_node, "stop") != NULL) {
        return 0;
    }

    return 1;
}


static int
regular_start(struct timer *tm, int suppress_error, int flags) {
    if(timer_is_running(tm)) {
        if (suppress_error) {
            return 0;
        }
        die_error("Timer '%s' already running", tm->name);
    }
    struct word_trie *timings = trie_get_path(tm->index_node, "timings");

    char max_str[24] = "";
    int max = trie_get_max_int_child(timings);
    sprintf(max_str, "%d", max + 1);

    char tbuf[16] = "";
    snprintf(tbuf, 16, "%lu", time(NULL));
    trie_insert_by_path(tm->index_node, build_path("timings", max_str, "start"), (void *)strdup(tbuf));

    if(tm->parent) {
        timer_start(tm->parent, 1, CHILD_CALL);
    }
    return 1;
}



int timer_has_running_children(struct timer *tm) {
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("docket:timer:*:parent");
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(tm->kv->trie, loop_ptr))) {
        if (trie_has_value(LOOP_ONSTACK_TRIE(loop_ptr), cmp_str, (void *)tm->name)) {
            return 1;
        }
    }
    return 0;
}

typedef int(*timerfn)(struct timer *timer, int suppress_error, int flags);

void
timer_children_apply(struct timer *tm, timerfn fn, int suppress_error, int flags) {
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("docket:timer:*:parent");
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    
    while((loop_ptr = trie_filter_branch(tm->kv->trie, loop_ptr))) {
        if (trie_has_value(LOOP_ONSTACK_TRIE(loop_ptr), cmp_str, (void *)tm->name)) {
            struct word_trie *index_node = TAILQ_PREV(LOOP_ONSTACK_TRIE(loop_ptr), loop_head, tries);

            struct timer tmc = {0};
            tmc.kv = tm->kv;
            tmc.index_node = index_node;
            init_timer(&tmc);
            fn(&tmc, suppress_error, CHILD_CALL);
        }
        fprintf(stderr, "%p\n", tm->kv->trie);
    }

}

static int
regular_stop(struct timer *tm, int suppress_error, int flags) {
    fprintf(stderr, "asdasdasdasd");
    if(!timer_is_running(tm)) {
        if (suppress_error) {
            return 0;
        }
        die_error("Timer '%s' not running", tm->name);
    }
    if(timer_has_running_children(tm)) {
        if((CHILD_CALL & flags) != 0) {
            return 0;
        } else {
            timer_children_apply(tm, timer_stop, 1, PARENT_CALL);
        }
    }
    struct word_trie *timings = trie_get_path(tm->index_node, "timings");

    const char *max_str = trie_get_max_int_child_node(timings)->word;

    char tbuf[16] = "";
    snprintf(tbuf, 16, "%lu", time(NULL));
    trie_insert_by_path(tm->index_node, build_path("timings", max_str, "stop"), (void *)strdup(tbuf));

    return 1;
}


static unsigned long
timer_running_time(struct timer *tm, int mode) {
    if ((mode & TR_ALL) != 0) {
        struct word_trie *timings = trie_get_path(tm->index_node, "timings");
        struct word_trie *chunk = NULL;
        unsigned long sum = 0;
        while((chunk = trie_loop_children(chunk, timings))) {
            int left = 0, right = 0;
            struct word_trie *start = trie_get_path(chunk, "start");
            struct word_trie *stop = trie_get_path(chunk, "stop");

            left = strtol(trie_get_value(start, 0), NULL, 10);
            if(stop) {
                right = strtol(trie_get_value(stop, 0), NULL, 10);
            } else {
                right = time(NULL);
            }
            sum += right - left;
        }
        return sum;
    } else if ((mode & TR_CURRENT) != 0 && timer_is_running(tm)) {
        struct word_trie *timing = trie_get_max_int_child_node(
                trie_get_path(tm->index_node, "timings"));
        if (timing) {
            struct word_trie *start = trie_get_path(timing, "start");
            if (start) {
                return time(NULL) - strtol(trie_get_value(start, 0), NULL, 10);
            }
        }
    }
    return 0;
}


const char *
duration_from_ul(char buf[], unsigned long t) {
    int hours = t / 3600;
    int minutes = t / 60 % 60;
    int seconds = t % 60;
    sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);
    return buf;
}
