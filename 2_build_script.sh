#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <is/tz>"
    exit 1
fi

PROJECT_ARG="$1"

projects=(
    "$PWD/project/realtek_amebaz2_v0_example/GCC-RELEASE"
    "$PWD/project/realtek_amebaz2plus_v0_example/GCC-RELEASE"
)

config_opt=(
    "CONFIG_EXAMPLE_MATTER_CHIPTEST"
    "CONFIG_EXAMPLE_MATTER_CHIPTEST"
    "CONFIG_EXAMPLE_MATTER_CHIPTEST"
    "CONFIG_EXAMPLE_MATTER_CHIPTEST"
    "CONFIG_EXAMPLE_MATTER_BRIDGE"
    "CONFIG_EXAMPLE_MATTER_DISHWASHER"
    "CONFIG_EXAMPLE_MATTER_FAN"
    "CONFIG_EXAMPLE_MATTER_GENERIC_SWITCH"
    "CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER"
    "CONFIG_EXAMPLE_MATTER_LIGHT"
    "CONFIG_EXAMPLE_MATTER_LIGHT"
    "CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN"
    "CONFIG_EXAMPLE_MATTER_REFRIGERATOR"
    "CONFIG_EXAMPLE_MATTER_ROOM_AIR_CONDITIONER"
    "CONFIG_EXAMPLE_MATTER_TEMPERATURE_SENSOR"
    "CONFIG_EXAMPLE_MATTER_THERMOSTAT"
)

examples=(
    "all_clusters"
    "air_purifier"
    "light"
    "light_switch"
    "bridge_dm"
    "dishwasher_port"
    "fan_port"
    "generic_switch_port"
    "laundry_washer_port"
    "light_dm"
    "light_port"
    "microwave_oven_port"
    "refrigerator_port"
    "room_air_conditioner_port"
    "temperature_sensor_port"
    "thermostat_port"
)

platform_opts="$PWD/component/common/application/matter/common/include/platform_opts_matter.h"

update_opt() {
    local selected="$1"

    if [ ! -f "$platform_opts" ]; then
        echo "Error: file not found: $platform_opts"
        return 1
    fi

    for opt in "${config_opt[@]}"; do
        if [ "$opt" == "$selected" ]; then
            # If already set to 1, skip
            if grep -q "^#define[[:space:]]*$opt[[:space:]]*1" "$platform_opts"; then
                continue
            fi
            sed -i "s/^#define[[:space:]]*$opt[[:space:]]*[01]/#define $opt 1/" "$platform_opts"
        else
            # Disable all other options if they are 1
            if grep -q "^#define[[:space:]]*$opt[[:space:]]*1" "$platform_opts"; then
                sed -i "s/^#define[[:space:]]*$opt[[:space:]]*[01]/#define $opt 0/" "$platform_opts"
            fi
        fi
    done
}

for project_dir in "${projects[@]}"; do
    # navigate to project directory
    cd "$project_dir" || { echo "Failed to enter $project_dir"; continue; }

    # loop through all the examples
    for ((i=0; i<${#examples[@]}; i++)); do
        opt="${config_opt[$i]}"
        target="${examples[$i]}"

        # Determine the expected output binary for this PROJECT_ARG
        if [ "$PROJECT_ARG" == "is" ]; then
            out_bin="$project_dir/$target/flash_is.bin"
        elif [ "$PROJECT_ARG" == "tz" ]; then
            out_bin="$project_dir/$target/flash_tz.bin"
        else
            echo "Usage: $0 <is/tz>"
            exit 1
        fi

        # Skip if this target was already successfully built
        if [ -f "$out_bin" ]; then
            echo "Skipping $target: output already exists ($out_bin)"
            continue
        fi

        update_opt "$opt"

        mkdir -p "$project_dir/$target"

        echo "Building example: $target"

        make "$target"
        if [ $? -eq 0 ]; then
            echo "Build of $target successful"
        else
            echo "=========> [Error] Build library failed: ${target}"
            exit 1
        fi

        if [ "$PROJECT_ARG" == "is" ]; then
            make is_matter

            if [ $? -eq 0 ]; then
                echo "Build of $target is_matter successful"
            else
                echo "=========> [Error] Build is firmware failed: ${target}"
                exit 1
            fi
            cp "$project_dir/application_is/Debug/bin/flash_is.bin" "$project_dir/$target"
            cp "$project_dir/application_is/Debug/bin/application_is.map" "$project_dir/$target"
        elif [ "$PROJECT_ARG" == "tz" ]; then
            make tz_matter
            if [ $? -eq 0 ]; then
                echo "Build of $target tz_matter successful"
            else
                echo "=========> [Error] Build tz firmware failed: ${target}"
                exit 1
            fi
            cp "$project_dir/application_tz/flash_tz.bin" "$project_dir/$target"
            cp "$project_dir/application_ns/Debug/bin/application_ns.map" "$project_dir/$target"
            cp "$project_dir/application_s/Debug/bin/application_s.map" "$project_dir/$target"
        fi

        make clean_matter_libs
        make clean_matter
    done
done

echo "Build process completed successfully."

