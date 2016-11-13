rm ~/.docket_timer_test
set -o verbose
docket timer new work
grep -Ev 'timing' ~/.docket_timer_test
docket timer new work_child --parent work
docket timer new work_child_child --parent work_child
docket timer set work parent work_child_child
grep -Ev 'timing' ~/.docket_timer_test
