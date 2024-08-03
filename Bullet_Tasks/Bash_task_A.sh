#!/bin/bash
# change bash script to exe:        chmod +x ./main.sh
# run the script:                   ./main.sh ./test_dir

# Enable dotglob option to include hidden files
shopt -s dotglob

# ________________________________________ Variables ______________________________________

declare DIRECTORY
declare FILE_EXTENSION
declare FILE_BASE_NAME

# ________________________________________ Functions ______________________________________
function EXTRACT_FILE_EXTENSION() {

    declare FILE_BASE_NAME
    declare FILE_EXTENSION
    declare KNOWN_EXTENSIONS_FILE="Known_Types.txt"

    FILE_BASE_NAME=$1
    #check if file is hidden "begins with a dot"
    if [[ ${FILE_BASE_NAME} == .* ]]; then
        FILE_EXTENSION="Hidden"

    #check if file has no extensions"
    elif [[ ! "$FILE_BASE_NAME" =~ \. ]]; then
        FILE_EXTENSION="Misc"

    else

        FILE_EXTENSION=${FILE_BASE_NAME##*.}

        #if the extension is unknown (not included in the known_types txt file)
        if ! grep -q "^$FILE_EXTENSION$" "$KNOWN_EXTENSIONS_FILE"; then
            FILE_EXTENSION="Misc"

        fi
    fi

    echo "$FILE_EXTENSION" # Explicitly return the file extension
}

# ____________________________________ main_function ______________________________________

function main() {
    if (($# != 1)); then
        echo "Invalid number of arguments, please pass one argument"
        return 1
    fi

    DIRECTORY=$1

    #check if the passed directory exists
    if [ -d "$DIRECTORY" ]; then
        echo "directory \"$DIRECTORY\" exists"
        for file in "${DIRECTORY}/"*; do

            #check if the file exists
            if [ -a "$file" ]; then

                #Extract file's base name
                FILE_BASE_NAME=$(basename "$file")

                #Extract file's extension
                FILE_EXTENSION=$(EXTRACT_FILE_EXTENSION "$FILE_BASE_NAME")

                #check if there is a sub-directory for the file's extension
                if [ ! -d "$DIRECTORY/$FILE_EXTENSION" ]; then
                    mkdir "$DIRECTORY/$FILE_EXTENSION"
                fi

                #move the file to the corresponding sub-directory
                cp "$file" "$DIRECTORY/$FILE_EXTENSION"

            fi
        done

    else
        echo "DIRECTORY NOT FOUND"
        return 2
    fi

}

# Calling the main function
main "$1"
