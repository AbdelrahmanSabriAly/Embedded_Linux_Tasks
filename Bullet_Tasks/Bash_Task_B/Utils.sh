#!/bin/bash

# Log file path
declare LOG_FILE="process_monitor.log"
declare PID
declare total_processes
declare memory_usage
declare cpu_load
declare PROCESS_NAME

#__________________________________ Functions__________________________________

# Function to read configuration from file
function READ_CONFIG_FILE() {
    if [[ -f process_monitor.conf ]]; then
        source process_monitor.conf
    else
        LOG_MESSAGE "Configuration file not found or invalid. Using default values."
    fi
}

# Function to read configuration from file
function READ_LOG_FILE() {
    if [[ -f process_monitor.log ]]; then
        source process_monitor.log
    else
        echo "Log file not found or invalid. Using default values."
    fi
}

function PRINT_ALL_PROCESSES() {
    # Header
    printf "%-5s %-10s %-10s %-10s %-10s %-20s\n" "PID" "TTY" "STAT" "CPU %" "MEM %" "COMMAND"

    # Loop through processes in /proc
    for pid in $(ls /proc | grep -E '^[0-9]+$'); do
        if [ -d "/proc/$pid" ]; then
            tty=$(readlink /proc/"$pid"/fd/0 | cut -d/ -f3 | sed 's/\(.*\)/\1/')
            if [ -z "$tty" ]; then
                tty="? "
            fi
            stat=$(awk '/State:/ {print $2}' /proc/"$pid"/status)
            if [ -z "$stat" ]; then
                stat="?"
            fi
            # Retrieve CPU and memory usage directly from /proc/$pid/stat
            cpu=$(awk '{print $14 + $15}' /proc/"$pid"/stat)
            mem=$(awk '{print $24}' /proc/"$pid"/stat)
            if [ -z "$cpu" ]; then
                cpu="?"
            fi
            if [ -z "$mem" ]; then
                mem="?"
            fi
            cmd=$(tr -d '\0' </proc/"$pid"/comm | cut -d' ' -f1)
            if [ -z "$cmd" ]; then
                cmd="[???]"
            fi
            printf "%-5s %-10s %-10s %-10s %-10s %-20s\n" "$pid" "$tty" "$stat" "$cpu" "$mem" "$cmd"
        fi
    done
}

function KILL_PROCESS() {
    read -r -p "Enter the process ID to kill: " PID
    if [[ -z $PID ]]; then
        LOG_MESSAGE "No process ID provided. Exiting."
    else
        kill "$PID"
        if [[ $? -eq 0 ]]; then
            LOG_MESSAGE "Process with PID $PID killed successfully."
        else
            LOG_MESSAGE "Failed to kill process with PID $PID."
        fi
    fi
}

function PRINT_PROCESS_STAT() {
    # Display total number of processes
    total_processes=$(pgrep -c "")

    # Display memory usage
    mem_info=$(awk '/MemTotal/{total=$2}/MemAvailable/{available=$2}END{printf("%.2f MB used out of %.2f MB\n", (total-available)/1024, total/1024)}' /proc/meminfo)

    # Display CPU load
    cpu_load=$(uptime)

    # Display the collected statistics
    echo "Total Number of Processes: $total_processes"
    echo "Memory Usage: $mem_info"
    echo "CPU Load: $cpu_load"
}

function REAL_TIME_MONITORING() {
    trap 'return' SIGINT # Catch Ctrl+C and return from the function
    while true; do
        clear # Clear the screen before displaying updated information
        echo "Real-time Monitoring - Press Ctrl+C to exit"

        # Display latest process information
        PRINT_ALL_PROCESSES

        # Sleep for 2 seconds before updating the display again
        sleep "$UPDATE_INTERVAL"
    done
}

function SEARCH_FOR_PROCESS() {

    read -r -p "Enter search criteria (process name, user, etc.): " PROCESS_NAME

    # Search and filter processes based on the user input
    pgrep -fl "$PROCESS_NAME"
}

function CHECK_CPU_USAGE() {
    local pid=$1
    local cpu_usage
    local cpu_threshold=${CPU_THRESHOLD:-90} # Default CPU threshold set to 90%

    if [ -d "/proc/$pid" ]; then
        # Read process CPU usage from /proc
        cpu_usage=$(awk '/^cpu /{print $2+$4}' "/proc/$pid/stat")

        if [ -n "$cpu_usage" ]; then
            if [ "$(echo "$cpu_usage > $cpu_threshold" | bc)" -eq 1 ]; then
                LOG_MESSAGE "Process $pid has exceeded CPU threshold ($cpu_threshold%): CPU usage is $cpu_usage%"
            fi
        else
            LOG_MESSAGE "Warning: CPU usage not available for process $pid"
        fi
    else
        LOG_MESSAGE "Error: Process $pid not found."
    fi
}

function CHECK_MEMORY_USAGE() {
    local pid=$1
    local memory_usage
    local memory_threshold=${MEMORY_THRESHOLD:-90} # Default memory threshold set to 90%

    if [ -d "/proc/$pid" ]; then
        # Read process memory usage from /proc
        memory_usage=$(awk '/VmRSS/{print $2}' "/proc/$pid/status")

        if [ -n "$memory_usage" ]; then
            if [ "$memory_usage" -gt "$memory_threshold" ]; then
                LOG_MESSAGE "Process $pid has exceeded memory threshold ($memory_threshold KB): Memory usage is $memory_usage KB"
            fi
        else
            LOG_MESSAGE "Warning: Memory usage not available for process $pid"
        fi
    else
        LOG_MESSAGE "Error: Process $pid not found."
    fi
}

# Main function to monitor processes
function SEARCH_FOR_ALERTS() {
    local update_interval=${UPDATE_INTERVAL:-5} # Default update interval set to 5 seconds

    while true; do
        # Get list of all process IDs
        pids=$(ls -1 /proc/ | grep -E "^[0-9]+$")

        # Iterate through each process and check CPU and memory usage
        for pid in $pids; do
            CHECK_CPU_USAGE "$pid"
            CHECK_MEMORY_USAGE "$pid"
        done

        # Sleep for specified interval before checking again
        sleep "$update_interval"
    done
}

# Function to log messages
function LOG_MESSAGE() {
    local message=$1
    echo "$(date +'%Y-%m-%d %H:%M:%S') - $message" >>"$LOG_FILE"
}
