#!/bin/bash

# Bash Script to Analyze Network Traffic

# Input: Path to the Wireshark pcap file
pcap_file=$1

# Function to extract information from the pcap file
analyze_traffic() {
    if [ -z "$pcap_file" ]; then
        echo "Error: Please provide a path to the pcap file."
        exit 1
    fi

    if [ ! -f "$pcap_file" ]; then
        echo "Error: File '$pcap_file' not found."
        exit 1
    fi

    echo "Analyzing '$pcap_file'..."

    # Use sudo with tshark commands
    total_packets=$(sudo tshark -r "$pcap_file" -T fields -e frame.number 2>/dev/null | wc -l)
    http_packets=$(sudo tshark -r "$pcap_file" -Y "http" -T fields -e frame.number 2>/dev/null | wc -l)
    https_packets=$(sudo tshark -r "$pcap_file" -Y "ssl" -T fields -e frame.number 2>/dev/null | wc -l)
    source_ips=$(sudo tshark -r "$pcap_file" -T fields -e ip.src 2>/dev/null | sort | uniq -c | sort -rn | head -n 5)
    dest_ips=$(sudo tshark -r "$pcap_file" -T fields -e ip.dst 2>/dev/null | sort | uniq -c | sort -rn | head -n 5)

    # Output analysis summary
    echo "----- Network Traffic Analysis Report -----"
    echo "1. Total Packets: $total_packets"
    echo "2. Protocols:"
    echo "   - HTTP: $http_packets packets"
    echo "   - HTTPS/TLS: $https_packets packets"
    echo ""
    echo "3. Top 5 Source IP Addresses:"
    echo "$source_ips"
    echo ""
    echo "4. Top 5 Destination IP Addresses:"
    echo "$dest_ips"
    echo ""
    echo "----- End of Report -----"
}

# Run the analysis function
analyze_traffic
