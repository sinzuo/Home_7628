#!/bin/sh

dev=$(iwconfig 2>/dev/null | grep "tozed-zhongji" | awk '{print $1}')
if [ "$dev" == "" ]; then
    dev="ra1"    
fi
dbm=$(iwinfo $dev a 2>/dev/null | grep dBm | head -n 1 | awk '{print $2}')

if [ "$dbm" == "" ]; then 
    exit 1
fi
th=$(uci -q get relayroam.roam.sendWifiNameTh)
if [ "$th" == "" ]; then
   th="-65"
fi
if [ "$dbm" -le "$th" ] ;then
    exit 1
fi

for index in 0 1 2 3 ;do
    type=$(uci -q get wireless.@wifi-iface[$index])
    if [ "$type" != "wifi-iface" ] ;then
        break
    fi
    ssid=$(uci -q get wireless.@wifi-iface[$index].ssid) 
    key=$(uci -q get wireless.@wifi-iface[$index].key) 
    enc=$(uci -q get wireless.@wifi-iface[$index].encryption) 
    hidden=$(uci -q get wireless.@wifi-iface[$index].hidden) 
    if [ "$hidden" == "" -o "$hidden" == "0" ];then
        echo "ssid:$ssid"
        echo "enc:$enc"
        echo "key:$key"
    fi
done


