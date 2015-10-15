#!/bin/bash
# Hikariにまとめて接続するよースクリプト

IPADDR=$1

./KanaC/kanac $IPADDR &
read

./UreC/urec $IPADDR &
read

./IyaraC/iyarac $IPADDR &
read

./KibiC/kibic $IPADDR &
read

./KuruC/kuruc $IPADDR &
read

./MuzukaC/muzukac $IPADDR &
read

./OkaC/okac $IPADDR &
read

./SabiC/sabic $IPADDR &
read

./TanoC/tanoc $IPADDR &

echo "LAST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
read
killall -9 kanac urec iyarac kibic kuruc muzukac okac sabic tanoc

