rm ~/.timer
set -v
docket timer new work --abstract
docket timer new home
docket timer new work_child
docket timer new work_child2 --parent work
docket timer set work_child parent work
docket timer set work_child event:onstart:stop work_child2

docket timer new home_child --parent home
docket timer set home_child event:onstop:start work_child

docket timer start work_child2
docket timer stat --status-only
docket timer start home_child
docket timer stat --status-only
docket timer stop home_child
docket timer stat --status-only
