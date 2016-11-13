rm ~/.docket_timer_test
set -o verbose
docket timer new work --abstract
docket timer new work_child --parent work
docket timer start work
