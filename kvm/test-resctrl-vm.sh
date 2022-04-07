# Author: Rohan Tabish
# Description: This file tests the resctrl interface in Linux with VM launched with KVM
#!/bin/bash
umount resctrl
pqos -R
vm_ip=
rm -rf  /root/.ssh/known_hosts
function get_vm_ip()
{
  local vm_name=$1
  local mac=$(virsh domiflist $vm_name | awk '{ print $5 }' | tail -2 | head -1)
  #echo $mac
  local ip=$(arp -a | grep $mac | awk '{ print $2 }' | sed 's/[()]//g')
  vm_ip=$ip
  echo $ip
}

for i in {56..111}
do
	echo 0 > /sys/devices/system/cpu/cpu$i/online
done

mount -t resctrl resctrl /sys/fs/resctrl

mkdir /sys/fs/resctrl/p0
mkdir /sys/fs/resctrl/p1

echo "MB:0=10" > /sys/fs/resctrl/p0/schemata
echo "MB:0=100" > /sys/fs/resctrl/p1/schemata

cat /sys/fs/resctrl/p0/schemata

echo "Running SOLO"

rm -rf /home/vmimages/unit*
rm -rf /home/vmimages/unit*

virsh destroy unit-test0
virsh undefine unit-test0
virsh destroy unit-test1
virsh undefine unit-test1


cp /home/vmimages/spec-golden-image.qcow2 /home/vmimages/unit0_golden.qcow2
cp /home/vmimages/spec-golden-image.qcow2 /home/vmimages/unit1_golden.qcow2

#Launch the first VM
virt-install --import -n unit-test0 -r 40960 --vcpus=30 --os-type=linux --os-variant=centos7.0 --accelerate --disk path=/home/vmimages/unit0_golden.qcow2,format=raw,bus=virtio,cache=writeback --disk path=/home/vmimages/unit0_golden.iso,device=cdrom,size=8 --network bridge=virbr0 --noautoconsole --cpu host-passthrough,cache.mode=passthrough --nographics

#Launch the secon VM
virt-install --import -n unit-test1 -r 40960 --vcpus=30 --os-type=linux --os-variant=centos7.0 --accelerate --disk path=/home/vmimages/unit1_golden.qcow2,format=raw,bus=virtio,cache=writeback --disk path=/home/vmimages/unit1_golden.iso,device=cdrom,size=8 --network bridge=virbr0 --noautoconsole --cpu host-passthrough,cache.mode=passthrough --nographics


sleep 60

grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test0.xml | cut  -b 6-11
PP0=$(grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test0.xml | cut  -b 6-11)

get_vm_ip unit-test0
vm0_ip=$vm_ip

for i in {1..2}
do
    scp -oStrictHostKeyChecking=no mlc root@${vm0_ip}:/usr/local/bin/
    scp -oStrictHostKeyChecking=no run_mlc.sh root@${vm0_ip}:/root

    ssh -oStrictHostKeyChecking=no root@${vm0_ip} "/root/run_mlc.sh mlc_data"

    sleep 100

    scp -oStrictHostKeyChecking=no root@${vm0_ip}:/root/mlc_data .
    cat mlc_data

    for process in $PP0
    do
        echo $process > /sys/fs/resctrl/p0/tasks
    done
done

echo "===================================="
echo "Co-Runs"
echo "===================================="

umount resctrl
mount -t resctrl resctrl /sys/fs/resctrl

mkdir /sys/fs/resctrl/p0
mkdir /sys/fs/resctrl/p1

echo "MB:0=10" > /sys/fs/resctrl/p0/schemata
echo "MB:0=100" > /sys/fs/resctrl/p1/schemata

cat /sys/fs/resctrl/p0/schemata



grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test1.xml | cut  -b 6-11

PP1=$(grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test1.xml | cut  -b 6-11)

get_vm_ip unit-test1
vm1_ip=$vm_ip

for i in {1..2}
do
    scp -oStrictHostKeyChecking=no mlc root@${vm0_ip}:/usr/local/bin/
    scp -oStrictHostKeyChecking=no run_mlc.sh root@${vm0_ip}:/root

    scp -oStrictHostKeyChecking=no mlc root@${vm1_ip}:/usr/local/bin/
    scp -oStrictHostKeyChecking=no run_mlc.sh root@${vm1_ip}:/root

    ssh -oStrictHostKeyChecking=no root@${vm0_ip} "/root/run_mlc.sh mlc_data0" &
    
    sleep 10
    
    ssh -oStrictHostKeyChecking=no root@${vm1_ip} "/root/run_mlc.sh mlc_data1" &
    
    sleep 100

    scp -oStrictHostKeyChecking=no root@${vm0_ip}:/root/mlc_data0 .
    scp -oStrictHostKeyChecking=no root@${vm1_ip}:/root/mlc_data1 .
 
    cat mlc_data0
    cat mlc_data1 
   
    
    for process in $PP0
    do
        echo $process > /sys/fs/resctrl/p0/tasks
    done
    
    for process in $PP1
    do
        echo $process > /sys/fs/resctrl/p1/tasks
    done
	

done


#virsh destroy unit-test0
#virsh undefine unit-test0

#rm -rf /home/vmimages/unit_golden.iso
#rm -rf /home/vmimages/unit_golden.qcow2


