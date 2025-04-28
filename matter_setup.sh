#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <IC type> [optional argument]"
  echo "IC type: amebaz2 / amebaz2plus / amebad"
  echo "Project type (for amebaz2 & amebaz2plus): If provided, can be 'tz' or 'is'"
  exit 1
fi

AMEBA="$1"
PROJECT_ARG="$2"  # The second argument, optional

files_to_delete=(
  "$PWD/component/soc/realtek/8710c/misc/utilities/include/ctype.h"
)

AMEBAZPLUS_PROJECT="$PWD/project/realtek_amebaz2plus_v0_example"
AMEBAZ2_PROJECT="$PWD/project/realtek_amebaz2_v0_example"
MATTER_AMEBAZ2_DIR="$PWD/component/common/application/matter/project/amebaz2"
MATTER_AMEBAZ2PLUS_DIR="$PWD/component/common/application/matter/project/amebaz2plus"
MATTER_DIR="$PWD/component/common/application/matter"

DEFAULT_MATTER_VERSION="main"

echo "Available Matter Versions:"
echo "1) v1.4"
echo "2) main (default)"

read -p "Enter Matter Version (default: $DEFAULT_MATTER_VERSION): " MATTER_VERSION
MATTER_VERSION=${MATTER_VERSION:-$DEFAULT_MATTER_VERSION}

# Validate input
if [[ "$MATTER_VERSION" != "v1.4" && "$MATTER_VERSION" != "main" ]]; then
    echo "Invalid input of Matter Version"
    exit 1
fi

if [ "$MATTER_VERSION" != "main" ]; then
    MATTER_BRANCH="release/$MATTER_VERSION"
else
    MATTER_BRANCH="main"
fi

echo "Selected Matter version: $MATTER_VERSION ($MATTER_BRANCH)"


if [ ! -d third_party ];then
    mkdir third_party
else
    rm third_party/connectedhomeip
fi

cd third_party
rm -rf connectedhomeip
ln -s ../../connectedhomeip connectedhomeip

cd ../

# Clone or Update Matter Repository
if [ ! -d "$MATTER_DIR" ]; then
    mkdir -p "$MATTER_DIR"
    git clone -b $MATTER_BRANCH https://github.com/Ameba-AIoT/ameba-rtos-matter.git "$MATTER_DIR"
else
    cd "$MATTER_DIR" || exit 1
    if [ -d ".git" ] && git rev-parse --is-inside-work-tree > /dev/null 2>&1 && git rev-parse HEAD > /dev/null 2>&1; then
        current_branch=$(git rev-parse --abbrev-ref HEAD)
        if [ "$current_branch" != "$MATTER_BRANCH" ]; then
            echo "Switching to branch '$MATTER_BRANCH'"
            git checkout "$MATTER_BRANCH" || exit 1
        fi
    else
        echo "Something is wrong with Matter folder. Re-cloning..."
        cd - > /dev/null || exit 1
        rm -rf "$MATTER_DIR"
        git clone -b "$MATTER_BRANCH" https://github.com/Ameba-AIoT/ameba-rtos-matter.git "$MATTER_DIR"
    fi
    cd - > /dev/null || exit 1
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
  find "$MATTER_DIR" -type f -name "Makefile.include.matter" | while read -r FILE; do
    if [ "$PROJECT_ARG" == "tz" ]; then
      if grep -q "FREERTOS_VERSION   = freertos_v10.0.1" "$FILE"; then
        echo "Modifying $FILE for TZ project"
        sed -i 's/^FREERTOS_VERSION   = freertos_v10.0.1/FREERTOS_VERSION   = freertos_v10.2.0/' "$FILE" || {
          echo "Failed to modify $FILE" >&2
          exit 1
        }
      fi
    elif [ "$PROJECT_ARG" == "is" ]; then
      if grep -q "FREERTOS_VERSION   = freertos_v10.2.0" "$FILE"; then
        echo "Modifying $FILE for IS project"
        sed -i 's/^FREERTOS_VERSION   = freertos_v10.2.0/FREERTOS_VERSION   = freertos_v10.0.1/' "$FILE" || {
          echo "Failed to modify $FILE" >&2
          exit 1
        }
      fi
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
