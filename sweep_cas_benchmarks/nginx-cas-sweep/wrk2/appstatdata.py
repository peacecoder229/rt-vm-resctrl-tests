#!/usr/bin/env python3
import subprocess
import sys
import time
import re
'''
arg1: cpus that needs to be moitored
arg2: ts in seconds for mpstat
arg3: no of iterations
arg4: outfile name
'''


def get_cores(c):
    cpu = list()
    seg = c.split(",")
    for s in seg:
        low,high = s.split("-")
        if low == None or high == None:
            cpu.append(s)
        else:
            for i in range(int(low), int(high)+1):
                cpu.append(i)

    return len(cpu)



def execute(cmd):
    cpustat = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    #print(cmd)
    cpustat.wait()

    for l in cpustat.stdout.readlines():
        yield l.decode('utf-8')
        #print(l.decode('utf-8'))



def get_cpu_stat(corestat):

    '''
    arg1: cpus that needs to be moitored
    arg2: ts in seconds for mpstat
    arg3: no of iterations
    arg4: outfile name
    '''
    for i in execute(corestat):
        cpuno = i.split()[1]
        usr = i.split()[3]
        ker = i.split()[5]
        #print("cpuno " + cpuno + " usr " + usr + " ker " + ker) 
        out.write("cpuno " + cpuno + " usr " + usr + " ker " + ker + "\n") 


#with open("metricfile", "r") as out:
#    for l in out:
#        print(l.split())

def get_wrk2_stat(cmd, dur):
    '''
    executes CMD and captures buffer
    and processes specific output
    numactl -C 64-71 ./wrk -t 8 -c 8000 -d 10s -L  http://localhost:80
    $1 cores $2 threads $3 con $4 duration $5 desired RPS/throughput per second $6 server
    app.py 0,2-35 70 16000 60s 120000 http://172.31.27.89:9001
    '''
    timeouts = 0
    for i in execute(cmd):
        match = re.search(r'(\d+) threads and (\d+) connections', i)
        if(match):
            thread = match.group(1)
            con = match.group(2)
            continue
        else:
            pass
            
        match = re.search(r'50.000%\s+(\d+.\d+)(\w+)', i)
        if(match):
            print(i)
            if match.group(2) == 'm':
                mult=60*1000
            elif match.group(2) == 's':
                mult=1000
            elif match.group(2) == 'us':
                mult=0.001
            else:
                mult=1
            p50 = mult*float(match.group(1))
            continue
        else:
            pass
        match = re.search(r'75.000%\s+(\d+.\d+)(\w+)', i)
        if(match):
            print(i)
            if match.group(2) == 'm':
                mult=60*1000
            elif match.group(2) == 's':
                mult=1000
            elif match.group(2) == 'us':
                mult=0.001
            else:
                mult=1
            p75 = mult*float(match.group(1))
            continue
        else:
            pass
        match = re.search(r'99.900%\s+(\d+.\d+)(\w+)', i)
        if(match):
            print(i)
            if match.group(2) == 'm':
                mult=60*1000
            elif match.group(2) == 's':
                mult=1000
            elif match.group(2) == 'us':
                mult=0.001
            else:
                mult=1
            p99 =  mult*float(match.group(1))
            continue
        else:
            pass
        match = re.search(r'(\d+) requests in', i)
        if(match):
            total =  match.group(1)
            continue
        else:
            pass
        match = re.search(r'timeout\s+(\d+)', i)
        if(match):
            timeouts = match.group(1)
        else:
            pass
    #print("Done")
    time = dur.replace("s", "")
    QPS = int(total)/int(time)

    res = "%s,%s,%s,%s,%s,%s,%s" %(thread, con, p50, p75, p99, QPS, timeouts)
    return res




def get_wrk_stat(cmd):
    '''
    executes CMD and captures buffer
    and processes specific output
    numactl -C 64-71 ./wrk -t 8 -c 8000 -d 10s -L  http://localhost:80
    $1 cores $2 threads $3 con $4 duration $5 server
    '''
    for i in execute(cmd):
        match = re.search(r'(\d+) threads and (\d+) connections', i)
        if(match):
            thread = match.group(1)
            con = match.group(2)
            continue
        else:
            pass
            
        match = re.search(r'50%\s+(\d+.\d+)ms', i)
        if(match):
            p50 = match.group(1)
            continue
        else:
            pass
        match = re.search(r'75%\s+(\d+.\d+)ms', i)
        if(match):
            p75 = match.group(1)
            continue
        else:
            pass
        match = re.search(r'99%\s+(\d+.\d+)ms', i)
        if(match):
            p99 =  match.group(1)
            continue
        else:
            pass
        match = re.search(r'(\d+) requests in', i)
        if(match):
            QPS =  match.group(1)
            continue
        else:
            pass
    #print("Done")
    res = ",".join((thread, con, p50, p75, p99, QPS))
    return res


def get_numa_stat(nstat):

    count = 1
    N0_prev = dict()
    N1_prev = dict()
    N0_cur = dict()
    N1_cur = dict()

    N0_prev['local'] =  0
    N1_prev['local'] =  0
    N0_prev['rem'] = 0
    N1_prev['rem'] =  0

    while(count < iteration):

        G = execute(nstat)
        local=next(G)
        remote=next(G)
        N0_cur['local'] = int(local.split()[1])
        N1_cur['local'] = int(local.split()[2])

        N0_cur['rem'] = int(remote.split()[1])
        N1_cur['rem'] = int(remote.split()[2])



    #print("Socket0 local =" + str(N0_cur['local'] - N0_prev['local']) + "Socket1 local =" + str(N1_cur['local'] - N1_prev['local']))
        out.write("Socket0 local =" + str(N0_cur['local'] - N0_prev['local']) + "  Socket1 local =" + str(N1_cur['local'] - N1_prev['local']) + "\n")
        out.write("Socket0 rem =" + str(N0_cur['rem'] - N0_prev['rem']) + "  Socket1 rem =" + str(N1_cur['rem'] - N1_prev['rem']) + "\n")

        N0_prev['local'] =  N0_cur['local']
        N1_prev['local'] =  N1_cur['local']
        N0_prev['rem'] = N0_cur['rem']
        N1_prev['rem'] =  N1_cur['rem']
    #print(next(G))
    #print(next(G))
        time.sleep(1)
        count+=1
    out.close()    

if __name__ == "__main__":

    if(len(sys.argv) < 6):
        print(get_wrk2_stat.__doc__)
    else:
        cores = sys.argv[1]
        threads = sys.argv[2]
        con = sys.argv[3]
        dur = sys.argv[4]
        rps = sys.argv[5]
        serv = sys.argv[6]

        cmd = "numactl --membind=1 --physcpubind=" + cores + " ./wrk -t " + threads + " -c " + con + " -d  " + dur + " -R " + rps +  " -L " + serv   
        #print(cmd)
#sudo taskset -c 0,2-35 ./wrk  -t 70  -c 16000 -d 60s  -R120000 -L  http://172.31.27.89:9001
#app.py 0,2-35 70 16000 60s 120000 http://172.31.27.89:9001
#cmd = "mpstat  -u -P 0-1,24-25 1 2 | tail -n 4 | awk \'{print $2 " " $3 + $4}\'"
#print(corestat)
#out = open("metricfile", "w")
#cpustat = subprocess.Popen(cmd, stdout=out, shell=True)
        #print("thread, con, p50, p75, p99, QPS, timeouts\n")
        #print(cmd)
        print("nginxscore=" + get_wrk2_stat(cmd, dur))
        #print(get_wrk2_stat(cmd))
