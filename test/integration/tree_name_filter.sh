ROOT=../../test/integration
CF=${ROOT}/out/.docket
TF_1=${ROOT}/out/test.docket
TF_2=${ROOT}/out/test1.docket
TF_3=${ROOT}/out/test2.docket
TF_4=${ROOT}/out/test3.docket
TF_5=${ROOT}/out/test4.docket

rm -f "${CF}"
rm -f "${TF_1}"
rm -f "${TF_2}"
rm -f "${TF_3}"
rm -f "${TF_4}"
rm -f "${TF_5}"

cp ${ROOT}/mockdata/mock.txt  "${TF_1}"
cp ${ROOT}/mockdata/mock.txt  "${TF_2}"
cp ${ROOT}/mockdata/mock.txt  "${TF_3}"
cp ${ROOT}/mockdata/mock.txt  "${TF_4}"
cp ${ROOT}/mockdata/mock.txt  "${TF_5}"

docket add --name test --use-config "${CF}" "${TF_1}"
docket add --name test1 --use-config "${CF}" "${TF_2}"
docket add --name test2 --use-config "${CF}" "${TF_3}"
docket add --name test3 --use-config "${CF}" "${TF_4}"
docket add --name test4 --use-config "${CF}" "${TF_5}"
docket tree --name test3 --use-config "${CF}"
