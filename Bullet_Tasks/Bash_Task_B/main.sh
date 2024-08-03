#!/bin/bash

#__________________________________ Variables__________________________________

declare SELECTED_OPTION=""
declare OPTIONS=("Process Information" "Kill a Process" "Process Statistics" "Real-time Monitoring" "Search and Filter" "Resource Usage Alerts")

#____________________________ Functions sourcing_________________________________
if [ -f "Utils.sh" ]; then
    source Utils.sh
else
    echo "Utilities script not found or invalid. Using default values."
fi

#__________________________________   Main   __________________________________

function main() {

    # Read configuration from file
    READ_CONFIG_FILE

    #Read Log from file
    READ_LOG_FILE

    while true; do
        # Log start of monitoring
        LOG_MESSAGE "Process monitoring started"
        # Wait for user to terminate script
        trap 'LOG_MESSAGE "Process monitoring stopped"; exit' INT
        wait

        echo "=================================================================================="
        echo "=================================================================================="
        echo "=================================================================================="
        echo "=================================================================================="

        PS3="Select an option: " # special variable in bash for Setting the prompt for the select statement
        select SELECTED_OPTION in "${OPTIONS[@]}"; do
            echo "=================================================================================="
            echo "$SELECTED_OPTION:"
            echo "=================================================================================="

            case "${SELECTED_OPTION}" in
            "${OPTIONS[0]}") #Print all processes
                PRINT_ALL_PROCESSES
                ;;
            "${OPTIONS[1]}") # Kill a Process by PID
                KILL_PROCESS
                ;;

            "${OPTIONS[2]}") # Process Statistics
                PRINT_PROCESS_STAT
                ;;

            "${OPTIONS[3]}")
                REAL_TIME_MONITORING
                ;;

            "${OPTIONS[4]}")
                SEARCH_FOR_PROCESS
                ;;
            "${OPTIONS[5]}")
                SEARCH_FOR_ALERTS
                ;;

            *)
                echo "Please choose a valid option (number)"
                ;;
            esac

            break # Exiting the loop after selection
        done
    done
}

main ""
