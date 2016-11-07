#include <stdlib.h>
#include <time.h>
#include "command.h"
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

struct timer;
struct _timer_vmtable {
    int (*start)(struct timer *timer);
    int (*stop) (struct timer *timer);
};

struct timer {
    const char *name;
    enum timer_type type;
    struct kvsrc *kv;
    struct word_trie *index_node;
    const struct _timer_vmtable *vmt;
};

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
standalone_start(struct timer *tm) {
    if(timer_is_running(tm)) {
        die_error("Timer '%s' already running", tm->name);
    }
    struct word_trie *timings = trie_get_path(tm->index_node, "timings");

    char max_str[24] = "";
    int max = trie_get_max_int_child(timings);
    sprintf(max_str, "%d", max + 1);

    char tbuf[16] = "";
    snprintf(tbuf, 16, "%lu", time(NULL));
    trie_insert_by_path(tm->index_node, build_path("timings", max_str, "start"), (void *)strdup(tbuf));
    return 1;
}

static int
standalone_stop(struct timer *tm) {
    if(!timer_is_running(tm)) {
        die_error("Timer '%s' not running", tm->name);
    }
    struct word_trie *timings = trie_get_path(tm->index_node, "timings");

    const char *max_str = trie_get_max_int_child_node(timings)->word;

    char tbuf[16] = "";
    snprintf(tbuf, 16, "%lu", time(NULL));
    trie_insert_by_path(tm->index_node, build_path("timings", max_str, "stop"), (void *)strdup(tbuf));
    return 1;
}

struct _timer_vmtable standalone_vmt = {
    standalone_start,
    standalone_stop,
};


static struct timer *get_timer_by_name(struct kvsrc *kv, const char *timer_name);
static void timer_bind_methods(struct timer *timer);
static int timer_start(struct timer *timer);
static int timer_stop(struct timer *timer);
static int cmd_start(int argc, const char **argv);
static int cmd_stop(int argc, const char **argv);
static int cmd_new(int argc, const char **argv);


static int
timer_start(struct timer *tm) {
    return tm->vmt->start(tm);
}


static int
timer_stop(struct timer *tm) {
    return tm->vmt->stop(tm);
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

static int cmd_new(int argc, const char **argv);
static const struct command commands[] = {
    {"new", cmd_new},
    {"start", cmd_start},
    {"stop", cmd_stop},
    {NULL, NULL},
};

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
init_timer(struct timer *tm) {
    tm->name = trie_get_value(trie_get_path(tm->index_node, "name"), 0);

    const char *type = trie_get_value(trie_get_path(tm->index_node, "type"), 0);

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
cmd_new(int argc, const char **argv) {
    if (argc < 1) return 0;

    struct kvsrc *kv = NULL;
    kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);

    struct timer *found = NULL;
    if( (found = get_timer_by_name(kv, argv[0])) != NULL) {
        die_error("Duplicate timer");
    }

    char max_str[24] = "";
    struct word_trie *index_node = kv_get(kv, "docket:timer");
    int max = trie_get_max_int_child(index_node);
    sprintf(max_str, "%d", max + 1);

    kv_add(kv, build_path("docket:timer", max_str, "name"), argv[0]);
    kv_add(kv, build_path("docket:timer", max_str, "type"), "regular");
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

    int rc = timer_start(tm);
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

    int rc = timer_stop(tm);
    if (rc != 1) {
        return 0;
    }

    kv_sync(kv);
    return 1;
}
