#!/usr/bin/env bash

# Configures udev rules for ST-LINK and USB-to-Serial devices.

set -o errexit -o nounset -o pipefail

# Configures udev rules by writing the specified content to the specified rules file.
configure_udev_rules() {
  local rules_file=$1
  local rules_content=$2

  echo -e "${rules_content}" | sudo tee "/etc/udev/rules.d/${rules_file}" >/dev/null
  echo "'${rules_file}' configured."
}

# Configures ST-LINK udev rules.
configure_st_link_udev_rules() {
  local st_link_rules
  st_link_rules=$(
    cat <<EOF
# ST-LINK V2
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666", TAG+="uaccess"
# ST-LINK V2.1
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666", TAG+="uaccess"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3752", MODE="0666", TAG+="uaccess"
# ST-LINK V3
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374d", MODE="0666", TAG+="uaccess"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666", TAG+="uaccess"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="0666", TAG+="uaccess"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3753", MODE="0666", TAG+="uaccess"
EOF
  )

  configure_udev_rules "99-st-link.rules" "${st_link_rules}"
}

# Configures USB-to-Serial udev rules.
configure_usb_to_serial_udev_rules() {
  local usb_to_serial_rules
  usb_to_serial_rules=$(
    cat <<EOF
# CP2101 - CP2104
SUBSYSTEMS=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666", TAG+="uaccess"
# ATEN UC-232A
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0557", ATTRS{idProduct}=="2008", MODE="0666", TAG+="uaccess"
EOF
  )

  configure_udev_rules "99-usb-to-serial.rules" "${usb_to_serial_rules}"
}

# Refreshes udev rules to apply the new configurations.
refresh_udev_rules() {
  sudo udevadm control --reload-rules
  sudo udevadm trigger
}

# Main function to coordinate the configuration process.
main() {
  echo "Starting udev configuration..."

  configure_st_link_udev_rules
  configure_usb_to_serial_udev_rules
  refresh_udev_rules

  echo "udev configuration completed."
}

main
