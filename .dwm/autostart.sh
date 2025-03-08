#!/usr/bin/env bash

black=#141b1e
gray=#232a2d
white=#dadada
red=#e57474
orange=#e5c76b
green=#8ccf7e
cyan=#67b0e8
blue=#67b0e8
purple=#c47fd5

function media() {
        status=`playerctl status`
  icon=""
  if [[ "$status" == "Playing" ]]; then
    icon=""
  elif [[ "$status" == "Paused" ]]; then
    icon=""
  fi
        if [[ -z "$status" ]]; then
                printf "^c$black^^b$blue^ ${icon} ^d^%s""^c$white^^b$gray^ No Players ^b$black^"
        else
                metadata=`playerctl metadata --format '{{artist}} • {{title}}' | sed 's/\(.\{25\}\).*/\1.../'`
                printf "^c$black^^b$blue^ ${icon} ^d^%s""^c$white^^b$gray^ ${metadata} ^b$black^"
        fi
}

function wifi() {
  wifi=`nmcli -t -f active,ssid dev wifi | grep -E '^yes' | cut -d\' -f2 | cut -b 5-`
  if [[ -z "$wifi" ]]; then
          printf "^c$black^^b$purple^  ^d^%s""^c$white^^b$gray^ Disconnected ^b$black^"
  else
          printf "^c$black^^b$purple^  ^d^%s""^c$white^^b$gray^ ${wifi} ^b$black^"
  fi
}

function cpu() {
  printf "CPU: $(grep -o '^[^ ]*' /proc/loadavg)%%"
}

function bri() {
  printf "^c$red^  $(xbacklight -get)%% "
}

function vol() {
  mute=`pamixer --get-mute`
  if [[ "$mute" == "true" ]]; then
    printf "^c$blue^ 婢Muted "
  else
   printf "^c$blue^ 墳$(pamixer --get-volume)%% "
  fi
}

function mem() {
  printf "MEM: $(free -h | awk '/^Mem/ { print $3"/"$2 }' | sed s/i//g)"
}

function dat() {
  printf "^c$black^^b$cyan^ 󰃰 ^d^%s""^c$white^^b$gray^ $(date '+%b %d, %I:%M %p') ^b$black^"
}

function bat() {
  bat=`cat /sys/class/power_supply/BAT0/status`
  icon="󰁹"
  if [[ "$bat" == "Charging" || "$bat" == "Full" ]]; then
    icon="󰚥"
  fi
  printf " ^c$green^${icon} $(cat /sys/class/power_supply/BAT0/capacity)%% "
}

while true; do
  xsetroot -name " $(bat) $(vol) $(bri) $(media) $(wifi) $(dat)"
  sleep 5
done
