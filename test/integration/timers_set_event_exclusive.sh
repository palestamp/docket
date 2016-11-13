rm ~/.timer
set -o verbose

docket timer new home
docket timer new work

docket timer set home event:onstart:stop work
docket timer set work event:onstart:stop home

docket timer start work
docket timer stat --status-only
docket timer start home
docket timer stat --status-only
