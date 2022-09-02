#/bin/bash
# KLOCWORK_SERVER needs to be set in the environment
# KLOCWORK_PATH needs to be set in the environment
# Adding them for now into this script due to lack of admin rights to set the CI/CD environment variables
KLOCWORK_SERVER=https://klocwork-ir3.devtools.intel.com:8085
KLOCWORK_PATH=/home/sys_opcm/kw10/bin
make clean > /dev/null
make version
# inject fetches all gcc and ld calls
$KLOCWORK_PATH/kwinject -w -o mlc.txt make > /dev/null
# buildproject builds the projects (duh) using the mlc.txt and writes scan output into mlctables
$KLOCWORK_PATH/kwbuildproject --force --url $KLOCWORK_SERVER/MLC mlc.txt --tables-directory mlctables
# upload the project for viewing on the klocwork server
$KLOCWORK_PATH/kwadmin --url $KLOCWORK_SERVER load MLC mlctables
