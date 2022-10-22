#!/bin/bash

#fix the name of device by typing ibv_device
ib_send_bw  -d rocep56s0  -s 16384 -q 8  -R
