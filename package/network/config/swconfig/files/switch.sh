#!/bin/sh
# Copyright (C) 2009 OpenWrt.org

setup_mt7628_port_mode() {
	# generate switch_port
	#setpvid
	
	# set mt7628 ethernet switch reg
	#reg s 0xB0110000
	switch reg w 98 0xff00
	
	#set port untag_en map, max vlan group (0--15)
	i=0
	
	while [ $i -le 15 ]
	do
		switch_vlan="network.@switch_vlan[$i]"
		#gid=$(uci get $switch_vlan.gid)
		gid=$i
	        vlan_id=$(uci get $switch_vlan.vlan)
		
		#if not set gid, skip
		#echo "vlan gid=$gid" | tee -a /tmp/switch.log	 
		#if [ -z $gid ]; then
		#	i=$(($i+1))		
		#	continue
		#fi
		 
		if [ -z $vlan_id ]; then
			i=$(($i+1))
			continue
		fi
		
		echo "vlan gid=$gid" | tee -a /tmp/switch.log
		echo "vlan_id=$vlan_id" | tee -a /tmp/switch.log	
		
		#get ports, eg. "0 1 2 6t"
		ports=$(uci get $switch_vlan.ports)
		echo "ports=$ports"	
		
		#生成port untag_en map
		portuntagmap=
		j=0
		while [ $j -le 6 ]
		do
		    #查找是否需要untag enable
		    untage_en=0
		    for port_untag in $ports
		    do
			#echo "port=$port_untag"
			tmp=`echo $port_untag | grep "t"` 
			if [ $tmp ];then
			   continue
			fi
		
			#CPU为端口6都需要tag则加上判断 -a "6" != "$port_untag"
			if [ "$j" == "$port_untag" ]; then
			   #第i个端口在表中找到需打开untag功能
			   echo "found untag port: $port_untag"
			   untage_en=1
			   break
			fi
		    done
		 
		    if [ "$untage_en" == "1" ]; then
			portuntagmap="${portuntagmap}1"
		    else 
			portuntagmap="${portuntagmap}0"
		    fi

		    j=$(($j+1))
		done

		echo "portuntagmap is [$portuntagmap]"

		#set vbus map
		echo "switch vub set $gid $vlan_id $portuntagmap" | tee -a /tmp/switch.log
		switch vub set $gid $vlan_id $portuntagmap

		i=$(($i+1))
	done
}

setup_switch_dev() {
	local name
	config_get name "$1" name
	name="${name:-$1}"
	[ -d "/sys/class/net/$name" ] && ifconfig "$name" up
	swconfig dev "$name" load network
	
	setup_mt7628_port_mode
}

setup_switch() {
	config_load network
	config_foreach setup_switch_dev switch
}
