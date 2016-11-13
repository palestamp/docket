
rm ~/.docket_timer_test > /dev/null
set -o verbose
docket timer new workall
docket timer new coding --parent workall
docket timer new meetings --parent workall
docket timer new workuseless --parent workall --abstract
docket timer new interrupt --parent workuseless
docket timer new on_the_way --parent workuseless
docket timer start coding
docket timer stop coding
grep -v 'timings' ~/.docket_timer_test
docket timer start on_the_way
docket timer stop on_the_way
docket timer set workuseless type concrete
docket timer start on_the_way
docket timer stop on_the_way
grep -v 'timings' ~/.docket_timer_test

