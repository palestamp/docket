#include <stdlib.h>
#include <time.h>

#include "command.h"
#include "options.h"
#include "kv.h"
#include "strbuf.h"
#include "docket.h"
#include "report.h"

#ifndef DOCKET_TIMER_STD_PATH
#error DOCKET_TIMER_STD_PATH must be specified
#endif

TAILQ_HEAD(,timer) timers_chain;


enum timer_op_st {
    TIMER_OP_OK,
    TIMER_OP_ERROR,
};


enum timer_type {
    UNKNOWN,
    ABSTRACT,
    CONCRETE,
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


static int concrete_start(struct timer *tm, int suppress_error, int flags);
static int concrete_stop(struct timer *tm, int suppress_error, int flags);
struct _timer_vmtable concrete_vmt = {
    concrete_start,
    concrete_stop,
};

static int abstract_start(struct timer *tm, int suppress_error, int flags);
static int abstract_stop(struct timer *tm, int suppress_error, int flags);
struct _timer_vmtable abstract_vmt = {
    abstract_start,
    abstract_stop,
};


struct timer {
    const char *name;
    struct timer *parent;
    enum timer_type type;
    struct kvsrc *kv;
    struct word_trie *index_node;
    struct word_trie *events;
    const struct _timer_vmtable *vmt;

    TAILQ_ENTRY(timer) shadow_chain;
};

static const char *
to_ics_time(unsigned long ts) {
    struct tm *info;
    info = localtime((const time_t *)&ts);
    char buffer[80] = "";
    strftime(buffer,80,"%Y%m%dT%H%M%SZ", info);
    return strdup(buffer);
}

static struct timer *init_timer(struct timer *tm);
static struct timer *get_timer_by_name(struct kvsrc *kv, const char *timer_name);
int timer_is_running(struct timer *tm);
static unsigned long timer_running_time(struct timer *tm, int mode);
static void timer_bind_methods(struct timer *timer);
static void timer_bind_event(struct timer *tm,
        const char *event_trigger,
        const char *event_name,
        const char *obj_name,
        int save);

// call timer-type defined operations
static int timer_start(struct timer *timer, int suppress_error, int flags);
static int timer_stop(struct timer *timer, int suppress_error, int flags);

// trie-level implementation, always stops timer
static void __docket_timer_stop(struct timer *tm);

// trie-level implementation, always stops timer
static void __docket_timer_start(struct timer *tm);

const char *duration_from_ul(char buf[], unsigned long t);
static struct timer * create_timer(struct kvsrc *kv,
        const char *timer_name,
        const char *parent,
        const char *timer_type,
        int suppress_duplicate);

static int timer_set_attr(struct timer *tm, const char *key, const char *value);

int timer_has_running_children(struct timer *tm);
// should return 0 on failure and 1 on success
typedef int(*timerfn)(struct timer *timer, int suppress_error, int flags);
int timer_children_apply(struct timer *tm, timerfn fn, int suppress_error, int flags);

// commands
static int cmd_start(int argc, const char **argv);
static int cmd_stop(int argc, const char **argv);
static int cmd_new(int argc, const char **argv);
static int cmd_list(int argc, const char **argv);
static int cmd_stat(int argc, const char **argv);
static int cmd_attr(int argc, const char **argv);


static const struct command commands[] = {
    {"new", cmd_new},
    {"start", cmd_start},
    {"stop", cmd_stop},
    {"list", cmd_list},
    {"stat", cmd_stat},
    {"set", cmd_attr},
    {NULL, NULL},
};


/* Entry point */
int
cmd_timer(int argc, const char **argv) {
    TAILQ_INIT(&timers_chain);
    if (argc < 1) {
        return 0;
    }

    struct command *cmd = get_command(argv[0], commands);
    if (cmd == NULL || cmd->cmd(--argc, ++argv) == 0) {
        return 0;
    }
    return 1;
}


static int
cmd_new(int argc, const char **argv) {
    char *parent = "*";
    char *abstract = NULL;
    struct option timer_new_options[] = {
        {"p", "parent", {.required = 0, .has_args = 1}, &parent},
        {"a", "abstract", {.required = 0, .has_args = 0}, &abstract},
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
    create_timer(kv,
            "*",        /* name   */
            NULL,       /* parent */
            "abstract", /* type   */
            1);         /* suppress_duplicate */
    create_timer(kv,
            timer_name,
            parent,
            (abstract != NULL ? "abstract" : "concrete"),
            0);
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

    int rc = timer_stop(tm, 0, USER_CALL);
    if (rc != 1) {
        return 0;
    }

    kv_sync(kv);
    return 1;
}


static int
cmd_list(int argc, const char **argv) {
    struct kvsrc *kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);

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
            fprintf(stdout, "%-15s %-10s %-10s\n",
                    tm.name,
                    "running",
                    duration_from_ul(buf, timer_running_time(&tm, TR_CURRENT)));
        } else {
            fprintf(stdout, "%-15s %-10s\n", tm.name, "stopped");
        }
    }
    return 1;
}


static int
cmd_attr(int argc, const char **argv) {
    // args layout should look like
    // <timer_name> <key_1> <value_1> <key_2> <value_2> ...

    struct kvsrc *kv = kv_load(DOCKET_TIMER_STD_PATH, kv_parse);

    if (argc < 3) {
        die_error("Minimum 3 argument required");
    }

    // check key value pairs correctnes
    if ((argc - 1) % 2 != 0) {
        die_error("Invalid arguments count");
    }

    int pairs_number = (argc - 1) / 2;

    struct timer *tm = NULL;
    if((tm = get_timer_by_name(kv, argv[0])) == NULL) {
        die_error("No such timer '%s'", argv[0]);
    }
    argc--; argv++;

    int i;
    for (i = 0; i < pairs_number; i+=2) {
        timer_set_attr(tm, argv[i], argv[i+1]);
    }
    kv_sync(kv);
    return 1;
}


static int
cmd_stat(int argc, const char **argv) {
    char *status_only = NULL;
    struct option timer_stat_options[] = {
        {"s", "status-only", {.required = 0, .has_args = 0}, &status_only},
        {NULL, NULL, {0}, NULL},
    };

    char err[1024] = "";
    int rv = options_populate(err, &argc, &argv, timer_stat_options);
    if (rv != 0) {
        die_error("%s", err);
    }

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
        fprintf(stdout, "%-15s %-10s",
                tm.name,
                (timer_is_running(&tm) ? "running" : "stopped"));
        if(status_only == NULL) {
            fprintf(stdout, " %-10s", duration_from_ul(buf, timer_running_time(&tm, TR_ALL)));
        }
        fputc('\n', stdout);
    }
    return 1;

}


static struct timer *
get_child_by_name(struct timer *tm, const char *name, int depth) {
    if (strcmp(tm->name, name) == 0) {
        return tm;
    }

    struct word_trie *host = kv_get(tm->kv, "docket:timer");
    struct word_trie *swap = NULL;
    while((swap = trie_loop_children(swap, host))) {
        struct word_trie *parent_node = trie_get_path(swap, "parent");
        // if child found
        if (parent_node && trie_has_value(parent_node, cmp_str, (void *)tm->name)) {
            struct word_trie *name_node = trie_get_path(swap, "name");

            struct timer *tmc = get_timer_by_name(tm->kv, trie_get_value(name_node, 0));

            if (depth != 0) {
                tmc = get_child_by_name(tmc, name, (depth < 0 ? depth : depth - 1));
                if(tmc) {
                    return tmc;
                }
            }
        }
    }

    return NULL;
}


static int
timer_set_parent(struct timer *tm, const char *parent_name) {
    if(parent_name == NULL) {
        die_fatal("BUG: parent name is NULL");
    }
    struct timer *found = NULL;
    if((found = get_child_by_name(tm, parent_name, -1))) {
        die_error("Cycle found");
    }
    if((found = get_timer_by_name(tm->kv, parent_name)) == NULL) {
        die_error("No such timer '%s'", parent_name);
    }


    if (tm->parent) {
        struct word_trie *tmp = trie_get_path(tm->index_node, "parent");
        if (tmp == NULL) {
            die_fatal("BUG: trie has no parent but timer has");
        }
        struct data_leaf *v = trie_get_value_node(tmp, 0);
        if (v == NULL) {
            die_fatal("BUG: parent has no value");
        }
        free((char *)v->data);
        v->data = (void *)strdup(parent_name);
    } else {
        trie_insert_by_path(tm->index_node, "parent", (void*)strdup(parent_name));
    }
    tm->parent = found;
    return 1;
}


static int
timer_set_type(struct timer *tm, const char *type) {
    if(tm->type) {
        struct word_trie *tmp = trie_get_path(tm->index_node, "type");
        if (tmp == NULL) {
            die_fatal("BUG: trie has no type but timer has");
        }
        struct data_leaf *v = trie_get_value_node(tmp, 0);
        if (v == NULL) {
            die_fatal("BUG: type has no value");
        }
        free((char *)v->data);
        v->data = (void *)strdup(type);
    } else {
        trie_insert_by_path(tm->index_node, "type", (void*)strdup(type));
    }
    timer_bind_methods(tm);
    return 1;
}


static int
timer_set_attr(struct timer *tm, const char *key, const char *value) {
    if(strcmp(key, "parent") == 0) {
        timer_set_parent(tm, value);
    } else if(strcmp(key, "type") == 0) {
        timer_set_type(tm, value);
    } else if(startswith("event", key)) {
        const char *trigger_start = strchr(key, ':');
        if (trigger_start != NULL && *(trigger_start + 1) != '\0') {
            const char *action_start = strchr((trigger_start + 1), ':');
            if (action_start != NULL && *(action_start + 1) != '\0') {
                const char *trigger = copy_slice((trigger_start + 1), action_start - (trigger_start + 1));
                const char *action = copy_slice((action_start + 1), (key + strlen(key)) - (action_start + 1));
                timer_bind_event(tm, trigger, action, value, 1);
                return 1;
            }
        }
        die_error("Wrong event syntax");
    } else {
        die_error("Unrecognized timer attribute '%s'", key);
    }
    return 1;
}


static struct timer *
create_timer(struct kvsrc *kv,
        const char *timer_name,
        const char *parent,
        const char *timer_type,
        int suppress_duplicate) {
    struct timer *tm = NULL;
    tm = malloc(sizeof(struct timer));

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


static void
timer_bind_event(struct timer *tm, const char *event_trigger, const char *event_name, const char *obj_name, int save) {
    if(strcmp(event_trigger, "onstart") != 0 &&
            strcmp(event_trigger, "onstop") != 0) {
        die_error("Unrecognized event trigger '%s'", event_trigger);
    }

    if (strcmp(event_name, "start") != 0 &&
            strcmp(event_name, "stop") != 0) {
        die_error("Unrecognized event '%s'", event_name);
    }
    if (strcmp(tm->name, obj_name) == 0) {
        die_error("Event cycle found: %s -> %s(%s)",
                tm->name, event_name, obj_name);
    }

    struct timer *evtimer = get_timer_by_name(tm->kv, obj_name);
    if(evtimer == NULL) {
        die_error("Timer '%s' not found", obj_name);
    }

    // if current timer is parent of eventor - die
    if(get_child_by_name(tm, obj_name, -1) != NULL) {
        die_error("Event can not affect children (%s child of %s)",
                tm->name, obj_name);
    } else if(get_child_by_name(evtimer, tm->name, -1) != NULL) {
        die_error("Event can not affect parents (%s parent of %s)",
                tm->name, obj_name);
    }
    char *evdesc = build_path(event_trigger, event_name);

    if (save) {
        trie_insert_by_path(tm->index_node, build_path("event", evdesc), (void *)evtimer->name);
    }
    trie_insert_by_path(tm->events, evdesc, (void *)evtimer);
}


void
timer_bind_events_node(struct timer *tm, struct word_trie *event_node) {
    struct word_trie *event_node_child = NULL;
    while((event_node_child = trie_loop_children(event_node_child, event_node))) {

        struct word_trie *onevent_node = NULL;
        while((onevent_node = trie_loop_children(onevent_node, event_node_child))) {
            struct data_leaf *dl = NULL;
            TAILQ_FOREACH(dl, &onevent_node->leafs, leaf) {
                timer_bind_event(tm, event_node_child->word, onevent_node->word, (const char *)dl->data, 0);
            }
        }
    }
}


static struct timer *
init_timer(struct timer *tm) {
    tm->events = trie_new();
    if(tm->events == NULL) {
        die_fatal("Memory allocation failed");
    }
    tm->name = trie_get_value(trie_get_path(tm->index_node, "name"), 0);

    // parent start
    const char *parent = trie_get_value(trie_get_path(tm->index_node, "parent"), 0);
    tm->parent = NULL;
    if (parent) {
        tm->parent = get_timer_by_name(tm->kv, parent);
    }
    // parent end

    // type start
    const char *type = trie_get_value(trie_get_path(tm->index_node, "type"), 0);
    if (strcmp(type, "concrete") == 0) {
        tm->type = CONCRETE;
    } else if (strcmp(type, "abstract") == 0) {
        tm->type = ABSTRACT;
    } else {
        die_error("Unknown timer type '%s'", type);
    }
    // type end

    timer_bind_methods(tm);
    TAILQ_INSERT_TAIL(&timers_chain, tm, shadow_chain);

    // Bind events after timer caching to prevent infinite recursion of timer
    // initializations

    // events start
    struct word_trie *events_node = trie_get_path(tm->index_node, "event");
    if (events_node) {
        timer_bind_events_node(tm, events_node);
    }
    // events end
    return tm;
}


static struct timer *
get_timer_by_name(struct kvsrc *kv, const char *name) {
    struct timer *tm = NULL;
    TAILQ_FOREACH(tm, &timers_chain, shadow_chain) {
        if (strcmp(tm->name, name) == 0) {
            return tm;
        }
    }

    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("docket:timer:*:name");
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(kv->trie, loop_ptr))) {
        if (trie_has_value(LOOP_ONSTACK_TRIE(loop_ptr), cmp_str, (void *)name)) {
            // get index node
            struct word_trie *index_node = TAILQ_PREV(LOOP_ONSTACK_TRIE(loop_ptr), loop_head, tries);

            tm = malloc(sizeof(struct timer));
            tm->kv = kv;
            tm->index_node = index_node;
            return init_timer(tm);
        }
    }
    return NULL;
}


void
trigger_timer_events(struct timer *tm, const char *event_trigger) {
    if (tm->events) {
        struct word_trie *event_trigger_node = trie_get_path(tm->events, event_trigger);
        if (event_trigger_node) {
            struct word_trie *event_node = NULL;
            while((event_node = trie_loop_children(event_node, event_trigger_node))){
                struct timer *st = NULL;
                struct data_leaf *dl = NULL;
                int(*tmfn)(struct timer *, int, int) = NULL;

                if(strcmp(event_node->word, "start") == 0) {
                    tmfn = &timer_start;
                } else if(strcmp(event_node->word, "stop") == 0) {
                    tmfn = &timer_stop;
                } else {
                    die_fatal("BUG: Error event '%s'", event_node->word);
                }
                TAILQ_FOREACH(dl, &event_node->leafs, leaf) {
                    st = (struct timer *)dl->data;
                    tmfn(st, 1, USER_CALL);
                }
            }
        }
    }
}


static int
timer_start(struct timer *tm, int suppress_error, int flags) {

    int rc =  tm->vmt->start(tm, suppress_error, flags);
    trigger_timer_events(tm, "onstart");
    return rc;
}


static int
timer_stop(struct timer *tm, int suppress_error, int flags) {

    int rc =  tm->vmt->stop(tm, suppress_error, flags);
    trigger_timer_events(tm, "onstop");
    return rc;
}


static void
timer_bind_methods(struct timer *tm) {
    switch (tm->type) {
        case CONCRETE:
            tm->vmt = &concrete_vmt;
            break;
        case ABSTRACT:
            tm->vmt = &abstract_vmt;
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
concrete_start(struct timer *tm, int suppress_error, int flags) {
    if(timer_is_running(tm)) {
        if (suppress_error) {
            return 0;
        }
        die_error("Timer '%s' already running", tm->name);
    }

    __docket_timer_start(tm);

    if(tm->parent) {
        timer_start(tm->parent, 1, CHILD_CALL);
    }
    return 1;
}


static int
abstract_start(struct timer *tm, int suppress_error, int flags) {
    if(timer_is_running(tm)) {
        if (suppress_error) {
            return 0;
        }
        die_error("Timer '%s' already running", tm->name);
    }

    if((USER_CALL & flags) != 0) {
        die_error("Abstract timer can not be manually started");
    }

    __docket_timer_start(tm);

    if(tm->parent) {
        timer_start(tm->parent, 1, CHILD_CALL);
    }
    return 1;
}


static int
abstract_stop(struct timer *tm, int suppress_error, int flags) {
    if(!timer_is_running(tm)) {
        if (suppress_error) {
            return 0;
        }
        die_error("Timer '%s' not running", tm->name);
    }

    int has_running_children = 0;
    if((has_running_children = timer_has_running_children(tm))) {
        if((CHILD_CALL & flags) != 0) {
            return 0;
        } else {
            if(timer_children_apply(tm, timer_stop, 1, PARENT_CALL) == 0) {
                die_error("unrecognized error");
            }
        }
    }
    __docket_timer_stop(tm);

    if(has_running_children == 0 && tm->parent) {
        timer_stop(tm->parent, 1, CHILD_CALL);
    }

    return 1;
}


static int
concrete_stop(struct timer *tm, int suppress_error, int flags) {
    if(!timer_is_running(tm)) {
        if (suppress_error) {
            return 0;
        }
        die_error("Timer '%s' not running", tm->name);
    }
    if((CHILD_CALL & flags) != 0) {
        return 0;
    } else {
        if(timer_has_running_children(tm)) {
            if (timer_children_apply(tm, timer_stop, 1, PARENT_CALL) == 0) {
                die_error("unrecognized error");
            }
        }
    }

    __docket_timer_stop(tm);

    if(tm->parent) {
        timer_stop(tm->parent, 1, CHILD_CALL);
    }

    return 1;
}


int
timer_has_running_children(struct timer *tm) {
    struct word_trie *host = kv_get(tm->kv, "docket:timer");
    struct word_trie *swap = NULL;
    while((swap = trie_loop_children(swap, host))) {
        struct word_trie *parent_node = trie_get_path(swap, "parent");
        if (parent_node && trie_has_value(parent_node, cmp_str, (void *)tm->name)) {
            struct word_trie *name_node = trie_get_path(swap, "name");
            struct timer *tmc = get_timer_by_name(tm->kv, trie_get_value(name_node, 0));

            if(timer_is_running(tmc)) {
                return 1;
            }
        }
    }
    return 0;
}


int
timer_children_apply(struct timer *tm, timerfn fn, int suppress_error, int flags) {
    struct word_trie *host = kv_get(tm->kv, "docket:timer");
    struct word_trie *swap = NULL;
    while((swap = trie_loop_children(swap, host))) {
        struct word_trie *parent_node = trie_get_path(swap, "parent");
        if (parent_node && trie_has_value(parent_node, cmp_str, (void *)tm->name)) {
            struct word_trie *name_node = trie_get_path(swap, "name");

            struct timer *tmc = get_timer_by_name(tm->kv, trie_get_value(name_node, 0));
            if(fn(tmc, suppress_error, PARENT_CALL) == 0) {
                return 0;
            }
        }
    }
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


static void
__docket_timer_start(struct timer *tm) {
    struct word_trie *timings = trie_get_path(tm->index_node, "timings");

    char max_str[24] = "";
    int max = trie_get_max_int_child(timings);
    sprintf(max_str, "%d", max + 1);

    char tbuf[16] = "";
    snprintf(tbuf, 16, "%lu", time(NULL));
    trie_insert_by_path(tm->index_node, build_path("timings", max_str, "start"), (void *)strdup(tbuf));
}


static void
__docket_timer_stop(struct timer *tm) {
    struct word_trie *timings = trie_get_path(tm->index_node, "timings");

    const char *max_str = trie_get_max_int_child_node(timings)->word;
    char tbuf[16] = "";
    snprintf(tbuf, 16, "%lu", time(NULL));
    trie_insert_by_path(tm->index_node, build_path("timings", max_str, "stop"), (void *)strdup(tbuf));
}
