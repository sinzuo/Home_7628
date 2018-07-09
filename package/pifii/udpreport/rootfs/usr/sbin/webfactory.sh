#!/bin/sh

cp /overlay/upper/etc/config/pifii  /tmp/

rm -fr /overlay/upper/*


mkdir /overlay/upper/etc

mkdir /overlay/upper/etc/config


cp /tmp/pifii  /overlay/upper/etc/config/
reboot

