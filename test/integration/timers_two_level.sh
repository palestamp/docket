

rm -f ~/.docket_timer_test > /dev/null
set -v
docket timer new home
docket timer new work --abstract
docket timer new coding --parent work
docket timer new robocop --parent coding
docket timer new projects --parent home --abstract
docket timer new docket --parent projects
docket timer new hf --parent projects

docket timer set home event:onstart:stop work
docket timer set work event:onstart:stop home

docket timer start robocop
docket timer stat --status-only

docket timer start hf
docket timer stat --status-only

