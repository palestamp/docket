ROOT=../../test/integration
CF=${ROOT}/out/.docket
TF_1=${ROOT}/out/test.docket

rm -f "${CF}"
rm -f "${TF_1}"

cp ${ROOT}/mockdata/mock.txt  "${TF_1}"

docket add --name test --use-config "${CF}" "${TF_1}"
docket tree --use-config "${CF}"
