# DLT Log Analyzer

This Bash script allows you to analyze and filter log files based on predefined log types and track specific system events. It provides a menu-driven interface for interacting with log data and generating a comprehensive analysis report.

## Features

- **Log Analysis**:
  - Parses log files based on configured log types (`info`, `error`, `warning`, `debug`).
  - Filters and displays logs by selected log types.

- **Event Tracking**:
  - Tracks specific system events within log files (e.g., "System Startup Sequence Initiated", "System Shutdown").

- **Report Generation**:
  - Generates a detailed report summarizing log analysis findings.
  - Reports the status of tracked system events.

## Prerequisites 
- Ensure you have Bash installed on your system.

## Usage

   
1. **Run the Script**:
   - Make the script executable:
     ```bash
     chmod +x main.sh
     ```
   - Execute the script with a log file as an argument:
     ```bash
     ./main.sh path/to/your_log_file.log
     ```

2. **Interact with the Menu**:
   - Choose options from the interactive menu:
     - Filter logs by type (`info`, `error`, `warning`, `debug`).
     - Generate a comprehensive report summarizing log analysis.
     - Quit the script.




## Example

```bash
./main.sh ./syslog
```

## Output samples:
![Screenshot from 2024-04-19 15-04-54](https://github.com/AbdelrahmanSabriAly/Embedded_Linux_Tasks_Group1/assets/137514155/3c1e8f1e-8dc6-4c9f-9a89-619dbe1cd28f)

-----------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------
![Screenshot from 2024-04-19 15-05-21](https://github.com/AbdelrahmanSabriAly/Embedded_Linux_Tasks_Group1/assets/137514155/0e21e235-4781-4b01-93f6-9e2cd0f41802)


-----------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------
![Screenshot from 2024-04-19 15-05-41](https://github.com/AbdelrahmanSabriAly/Embedded_Linux_Tasks_Group1/assets/137514155/f78d4d2b-5d6f-4b5a-9571-bcb879899bb3)


-----------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------
![Screenshot from 2024-04-19 15-05-59](https://github.com/AbdelrahmanSabriAly/Embedded_Linux_Tasks_Group1/assets/137514155/88d5fd4e-f9a6-4ebb-93c7-eeec9295f2f5)


-----------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------
![Screenshot from 2024-04-19 15-06-13](https://github.com/AbdelrahmanSabriAly/Embedded_Linux_Tasks_Group1/assets/137514155/ce76460d-bd46-4ea8-b3fb-4f46c479f4dc)

-----------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------
![Screenshot from 2024-04-19 15-06-28](https://github.com/AbdelrahmanSabriAly/Embedded_Linux_Tasks_Group1/assets/137514155/ef33382d-0db1-4eb9-a370-b074ded1e938)


-----------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------










