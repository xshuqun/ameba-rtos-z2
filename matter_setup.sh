#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <IC type> [project type]"
  echo "IC type: amebaz2 / amebaz2plus"
  echo "Project type (optional): tz or is (default: is)"
  exit 1
fi

AMEBA="$1"
PROJECT_ARG="${2:-is}"

if [ "$AMEBA" != "amebaz2" ] && [ "$AMEBA" != "amebaz2plus" ]; then
  echo "Invalid IC type. Expected: amebaz2 or amebaz2plus."
  exit 1
fi

REPO_NAME=$(basename -s .git "$(git remote get-url origin)")

CHIP_LINK="$PWD/third_party/connectedhomeip"
CHIP_SRC="../../connectedhomeip"

PROJECT_DIR="$PWD/project/realtek_${AMEBA}_v0_example"
MATTER_DIR="$PWD/component/common/application/matter"
MATTER_PROJECT_DIR="$MATTER_DIR/project/${AMEBA}"
MATTER_SCRIPT="$MATTER_DIR/tools/scripts/matter_version_selection.sh"

files_to_delete=(
  "$PWD/component/soc/realtek/8710c/misc/utilities/include/ctype.h"
)

# --- 1: Setup third_party softlink ---
mkdir -p third_party
rm -rf "$CHIP_LINK"
ln -s "$CHIP_SRC" "$CHIP_LINK"

# --- 2: Clone Ameba Matter repo if not present ---
if [ ! -d "$MATTER_DIR" ]; then
  echo "Cloning Matter repository..."
  git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git "$MATTER_DIR"
fi

# --- 3: Run matter version selection script ---
cd "$MATTER_DIR" || exit 1

if [ -f "$MATTER_SCRIPT" ]; then
  bash "$MATTER_SCRIPT" "$AMEBA" "$REPO_NAME"
else
  echo "Error: $MATTER_SCRIPT not found."
  exit 1
fi

# --- 4: Ensure correct Matter branch ---
MATTER_BRANCH=$(git rev-parse --abbrev-ref HEAD)

if git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
  current_branch=$(git rev-parse --abbrev-ref HEAD)
  if [ "$current_branch" != "$MATTER_BRANCH" ]; then
    echo "Switching to branch '$MATTER_BRANCH'"
    git checkout "$MATTER_BRANCH" || exit 1
  fi
else
  echo "Matter repo seems broken. Re-cloning..."
  cd - > /dev/null || exit 1
  rm -rf "$MATTER_DIR"
  git clone -b "$MATTER_BRANCH" https://github.com/Ameba-AIoT/ameba-rtos-matter.git "$MATTER_DIR"
fi

cd - > /dev/null || exit 1

# --- Helper Functions ---
delete_files() {
  for file_path in "${files_to_delete[@]}"; do
    if [ -e "$file_path" ]; then
      rm "$file_path"
      echo "Removed: $file_path"
    fi
  done
}

modify_makefiles() {
  find "$BASE_DIR" -type f -name "Makefile" | while read -r FILE; do
    if grep -q "ENABLE_MATTER = 0" "$FILE"; then
      echo "Enabling Matter in: $FILE"
      sed -i 's/^ENABLE_MATTER = 0/ENABLE_MATTER = 1/' "$FILE"
    fi
  done

  find "$MATTER_PROJECT_DIR" -type f -name "Makefile.include.matter" | while read -r FILE; do
    if [ "$PROJECT_ARG" == "tz" ]; then
      sed -i 's/^FREERTOS_VERSION   = freertos_v10.0.1/FREERTOS_VERSION   = freertos_v10.2.0/' "$FILE" && \
      echo "Updated FreeRTOS to v10.2.0 for "$PROJECT_ARG" project"
    elif [ "$PROJECT_ARG" == "is" ]; then
      sed -i 's/^FREERTOS_VERSION   = freertos_v10.2.0/FREERTOS_VERSION   = freertos_v10.0.1/' "$FILE" && \
      echo "Updated FreeRTOS to v10.0.1 for "$PROJECT_ARG" project"
    fi
  done
}

case "$AMEBA" in
  amebaz2|amebaz2plus)
    BASE_DIR="$PROJECT_DIR"
    echo "Configuring for $AMEBA..."
    delete_files
    modify_makefiles
    ;;
  *)
    echo "Invalid IC type. Expected: amebaz2 or amebaz2plus."
    exit 1
    ;;
esac

echo "Matter setup complete"
