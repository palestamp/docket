rm ~/.docket_timer_test
set -o verbose
docket timer new work --abstract
docket timer new work_child --parent work
docket timer new work_child2 --parent work
docket timer new work_child3 --parent work

docket timer start work_child
docket timer stat --status-only
docket timer start work_child2
docket timer stat --status-only
docket timer start work_child3
docket timer stat --status-only

docket timer stop work_child2
docket timer stat --status-only
docket timer stop work_child
docket timer stat --status-only
docket timer stop work_child3
docket timer stat --status-only
