#!/bin/bash
# executes pdppt QT application forcing a display. Some display must be available on the system
# if it fails (usually message) :
#  - try connecting to the server with ssh -X <server> to share X session
#  - check X session in the server with 'ls /tmp/.X11-unix/'. If is empty on the server after connecting with "ssh -X" the pdppt_noGUI.sh workaround will not work.
#      - check if X server is running: ps ax | grep -i "/usr/bin/X"  . If not running you can start it manually with "sudo /usr/bin/X :0 & ". Afterward it should appear in 'ls /tmp/.X11-unix/'
#  - might be good to have a full desktop install in the server if possible. For example KDE: sudo yum groupinstall -y "KDE Plasma Workspaces"
#
# NOTE: this script is copied during make to <powerdevs>/bin directory. DO NOT change the copy in <powerdevs>/bin directory as it will be overwritten

# Script paths
SCRIPT_PATH="${BASH_SOURCE}"
while [ -L "${SCRIPT_PATH}" ]; do
  SCRIPT_DIR="$(cd -P "$(dirname "${SCRIPT_PATH}")" >/dev/null 2>&1 && pwd)"
  SCRIPT_PATH="$(readlink "${SCRIPT_PATH}")"
  [[ ${SCRIPT_PATH} != /* ]] && SCRIPT_PATH="${SCRIPT_DIR}/${SCRIPT_PATH}"
done
SCRIPT_PATH="$(readlink -f "${SCRIPT_PATH}")"
SCRIPT_DIR="$(cd -P "$(dirname -- "${SCRIPT_PATH}")" >/dev/null 2>&1 && pwd)"


# search for available displays
displays=( $(cd /tmp/.X11-unix && for x in X*; do echo ":${x#X}"; done)   )
# displays=()

qt_display_param=""
if (( ${#displays[@]} )); then
    # there is at least 1 display
	# qt_display_param="-display $displays"
	  qt_display_param="-display $DISPLAY"
else
	echo "Warning: no displays found in tmp/.X11-unix. Attempting to run with GUI"
fi

${SCRIPT_DIR}/pdppt $qt_display_param $@
