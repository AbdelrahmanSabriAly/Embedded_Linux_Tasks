#!/bin/bash

# __________________________________________________ Variables ___________________________________________________

declare LOG_FILE="$1"

declare -A log_counts

declare options=("Filter by INFO" "Filter by ERROR" "Filter by DEBUG" "Filter by WARN" "Summarize Logs" "Generate Report" "Exit")


# ________________________________________________ Exit codes ___________________________________________________

declare FILE_NOT_FOUND_ERROR=1
declare EXIT_SCRIPT=0


# __________________________________________________ Functions ___________________________________________________
# Function to print a separation line
function print_separation_lins(){
    echo "==================================================================="
}

# Function to filter log entries based on log type
function filter_logs() {
    local log_type="$1"
    grep "$log_type" "$LOG_FILE"
}

# Function to summarize log entries
function summarize_logs() {
    echo "Summary of Log Entries:"

    print_separation_lins
    

    log_counts["INFO"]=$(grep -c "INFO" "$LOG_FILE")
    log_counts["ERROR"]=$(grep -c "ERROR" "$LOG_FILE")
    log_counts["DEBUG"]=$(grep -c "DEBUG" "$LOG_FILE")
    log_counts["WARN"]=$(grep -c "WARN" "$LOG_FILE")

    for log_type in "${!log_counts[@]}"; do
        echo "${log_type} logs count: ${log_counts[$log_type]}"
    done
}

# Function to generate a report summarizing findings
function generate_report() {
    echo "Generating Report..."
    print_separation_lins
    
    echo "Trends in Error/Warning Logs:"

    #Sort alphabetically, count the occurrences of each log, sort them from more frequent to less frequent
    grep -e "ERROR" -e "WARN" "$LOG_FILE" | sort | uniq -c | sort -nr 
    
    echo "System Event Status:"
    grep -e "Startup" -e "Shutdown" -e "Backup" -e "Update" "$LOG_FILE"
}

# _______________________________________________ Main function ___________________________________________________
function main(){
    if ! [ -f "$LOG_FILE" ]; then
        echo "Error: File \"$LOG_FILE\" does not exist or is inaccessible."
        exit "$FILE_NOT_FOUND_ERROR"
        
    fi
    
    while true; do
        echo "Select an option:"
        select choice in "${options[@]}"; do
            case "$choice" in
                "Filter by INFO")
                    filter_logs "INFO"
                    print_separation_lins
                    break
                    ;;
                "Filter by ERROR")
                    filter_logs "ERROR"
                    print_separation_lins
                    break
                    ;;
                "Filter by DEBUG")
                    filter_logs "DEBUG"
                    print_separation_lins
                    break
                    ;;
                "Filter by WARN")
                    filter_logs "WARN"
                    print_separation_lins
                    break
                    ;;
                "Summarize Logs")
                    summarize_logs
                    print_separation_lins
                    break
                    ;;
                "Generate Report")
                    generate_report
                    print_separation_lins
                    break
                    ;;
                "Exit")
                    echo "Exiting..."
                    exit "$EXIT_SCRIPT"
                    ;;
                *)
                    echo "Invalid option. Please select a valid number."
                    print_separation_lins
                    ;;
            esac
        done
    done
}

main
