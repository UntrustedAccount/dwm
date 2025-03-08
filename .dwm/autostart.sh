#!/bin/bash
black=#020203
gray=#1c252c
white=#e3e6eb
red=#e05f65
orange=#f1cf8a
green=#78dba9
cyan0=#74bee9
cyan1=#69add4
blue=#70a5eb
purple=#c68aee

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
  printf "^c$black^^b$cyan0^ 󰃰 ^d^%s""^c$black^^b$cyan1^ $(date '+%b %d, %I:%M %p') ^b$black^"
}

function bat() {
  bat=`cat /sys/class/power_supply/BAT0/status`
  icon="󰁹"
  if [[ "$bat" == "Charging" || "$bat" == "Full" ]]; then
    icon="󰚥"
  fi
  printf " ^c$green^${icon} $(cat /sys/class/power_supply/BAT0/capacity)%% "
}

~/.fehbg &
dunst &
xfsettingsd &
picom &

while true; do
  xsetroot -name " $(bat) $(vol) $(bri) $(media) $(wifi) $(dat)"
  sleep 5
done
