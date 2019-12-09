#!/bin/sh

gcc -o tx2_comm_node tx2_comm_node.c CommController.c messages.c
gcc -o tx2_can_node  tx2_can_node.c CanController.c messages.c
gcc tx2_master.c messages.c

./a.out
