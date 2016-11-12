rm ~/.timer
set -o verbose
docket timer new work --abstract
docket timer new work_child --parent work
docket timer new work_child2 --parent work
docket timer new work_child3 --parent work

docket timer set work_child event:onstart:stop work
