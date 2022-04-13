# Author: Rohan Tabish
# Description: This file tests the resctrl interface in Linux with VM launched with KVM
#!/bin/bash
umount resctrl
pqos -R
vm_ip=
#rm -rf  /root/.ssh/known_hosts
echo off > /sys/devices/system/cpu/smt/control
function get_vm_ip()
{
  local vm_name=$1
  local mac=$(virsh domiflist $vm_name | awk '{ print $5 }' | tail -2 | head -1)
  echo $mac
  local ip=$(arp -a | grep $mac | awk '{ print $2 }' | sed 's/[()]//g')
  vm_ip=$ip
  echo $vm_ip
  echo $vm_ip
}

for i in {56..111}
do
	echo 0 > /sys/devices/system/cpu/cpu$i/online
done

#mount -t resctrl resctrl /sys/fs/resctrl


echo "Running CO-RUN"

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


sleep 120

grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test0.xml | cut  -b 6-9
PP0=$(grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test0.xml | cut  -b 6-9)

get_vm_ip unit-test0
vm0_ip=$vm_ip
echo $vm0_ip

grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test1.xml | cut  -b 6-9

PP1=$(grep -o 'pid=[^ ,]\+' /var/run/libvirt/qemu/unit-test1.xml | cut  -b 6-9)

get_vm_ip unit-test1
vm1_ip=$vm_ip
echo $vm_ip

for i in {1..2}
do
    scp -oStrictHostKeyChecking=no mlc root@${vm0_ip}:/usr/local/bin/
    scp -oStrictHostKeyChecking=no run_mlc.sh root@${vm0_ip}:/root

    scp -oStrictHostKeyChecking=no mlc root@${vm1_ip}:/usr/local/bin/
    scp -oStrictHostKeyChecking=no run_mlc.sh root@${vm1_ip}:/root

    ssh -oStrictHostKeyChecking=no root@${vm0_ip} "/root/run_mlc.sh mlc_data0" &
    
    sleep 15
    
    ssh -oStrictHostKeyChecking=no root@${vm1_ip} "/root/run_mlc.sh mlc_data1" 
    
    sleep 100

    scp -oStrictHostKeyChecking=no root@${vm0_ip}:/root/mlc_data0 .
    scp -oStrictHostKeyChecking=no root@${vm1_ip}:/root/mlc_data1 .
 
    cat mlc_data0
    cat mlc_data1 
    
    cd hwdrc_postsi/scripts
    ./hwdrc_icx_2S_xcc_init_to_default_resctrl.sh
    cd -  
    
    for process in $PP0
    do
        echo $process > /sys/fs/resctrl/C07/tasks
    done
    
    for process in $PP1
    do
        echo $process > /sys/fs/resctrl/C04/tasks
    done	
done

