#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <IC type> [optional argument]"
  echo "IC type: amebaz2 / amebaz2plus / amebad"
  exit 1
fi

AMEBA="$1"

files_to_delete=(
  "$PWD/component/soc/realtek/8710c/misc/utilities/include/ctype.h"
)

AMEBAZPLUS_PROJECT="$PWD/project/realtek_amebaz2plus_v0_example"
AMEBAZ2_PROJECT="$PWD/project/realtek_amebaz2_v0_example"
MATTER_AMEBAZ2_DIR="$PWD/component/common/application/matter/project/amebaz2"
MATTER_AMEBAZ2PLUS_DIR="$PWD/component/common/application/matter/project/amebaz2plus"

if [ ! -d third_party ];then
    mkdir third_party
else
    rm third_party/connectedhomeip
fi

cd third_party
rm -rf connectedhomeip
ln -s ../../connectedhomeip connectedhomeip

cd ../

if [ ! -d component/common/application/matter ] || [ -z "$(find component/common/application/matter -mindepth 1)" ]; then
    mkdir -p component/common/application/matter
    git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/common/application/matter
fi

delete_files() {
  for file_path in "${files_to_delete[@]}"; do
    if [ -e "$file_path" ]; then
      rm "$file_path"
      echo "File $file_path removed."
    fi
  done
}

modify_makefiles() {
  find "$BASE_DIR" -type f -name "Makefile" | while read -r FILE; do
    if grep -q "ENABLE_MATTER = 0" "$FILE"; then
      echo "Modifying $FILE"
      sed -i 's/^ENABLE_MATTER = 0/ENABLE_MATTER = 1/' "$FILE"
    fi
  done
}

case "$AMEBA" in
  amebaz2)
    BASE_DIR="$AMEBAZ2_PROJECT"
    MATTER_DIR="$MATTER_AMEBAZ2_DIR"
    echo "Configuring for $AMEBA"
    delete_files
    modify_makefiles
    ;;
  amebaz2plus)
    BASE_DIR="$AMEBAZPLUS_PROJECT"
    MATTER_DIR="$MATTER_AMEBAZ2PLUS_DIR"
    echo "Configuring for $AMEBA"
    delete_files
    modify_makefiles
    ;;
  amebad)
    echo "Configuring for $AMEBA"
    ;;
  *)
    echo "Invalid argument. Expected 'amebaz2', 'amebaz2plus', or 'amebad'."
    exit 1
    ;;
esac

echo "Matter setup complete"
