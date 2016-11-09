rm ~/.timer
set -o verbose
docket timer new work
grep -Ev 'timing' ~/.timer
docket timer new work_child --parent work
docket timer new work_child_child --parent work_child
docket timer set work parent work_child_child
grep -Ev 'timing' ~/.timer
