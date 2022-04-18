#!/bin/bash


#example for creating hpjob name cgroup with cpu resource type and setting  cpu.share paremeter
#./create_cgroup.sh "cpu" "hpjob" "shares:512"

if ! command -v cgcreate &> /dev/null
then
	echo "Install libcgroups"
	yum  -y install libcgroup libcgroup-tools

	exit
else
	echo "libcgroups and tools already installed"

fi


cgrpname=$1
grp_hierarchy=$2
grp_para=$3
pname=$(echo "$grp_para" | cut -d ":" -f 1 )
pval=$(echo "$grp_para" | cut -d ":" -f 2 )

if [ "$#" -eq 4 ]
then
	echo "setting both cpu.share and cpu.cfs_quota_us"
	grp_para1=$4
	pname1=$(echo "$grp_para1" | cut -d ":" -f 1 )
	pval1=$(echo "$grp_para1" | cut -d ":" -f 2 )
fi



if [ -d "/sys/fs/cgroup/${cgrpname}/${grp_hierarchy}" ]
then
	echo "group name already exists; deleting it"
	cgdelete "${cgrpname}:/${grp_hierarchy}"
	sleep 1
	echo "Creating cgroup ${grp_hierarchy}"
	cgcreate -g "${cgrpname}:/${grp_hierarchy}"
	sleep 2
	echo "setting parameter ${cgrpname}.${pname}=${pval} ${grp_hierarchy}"
        echo "cgset -r ${cgrpname}.${pname}=${pval} ${grp_hierarchy}"	
        cgset -r "${cgrpname}.${pname}=${pval}" "${grp_hierarchy}"

	if [ "$#" -eq 4 ]
	then
		cgset -r "${cgrpname}.${pname1}=${pval1}" "${grp_hierarchy}"
	fi


	
else
	echo "cgcreate -g ${cgrpname}:/${grp_hierarchy}"
	cgcreate -g "${cgrpname}:/${grp_hierarchy}"
	sleep 2
	echo "cgset -r ${cgrpname}.${pname}=${pval} ${grp_hierarchy}"
	cgset -r "${cgrpname}.${pname}=${pval}" "${grp_hierarchy}"

	if [ "$#" -eq 4 ]
	then
		cgset -r "${cgrpname}.${pname1}=${pval1}" "${grp_hierarchy}"
	fi
fi








