rm ~/.docket_timer_test
set -o verbose
docket timer new work --abstract
docket timer new work_child --parent work
docket timer new work_child2 --parent work
docket timer new work_child3 --parent work

docket timer set work event:onstart:stop work_child2
