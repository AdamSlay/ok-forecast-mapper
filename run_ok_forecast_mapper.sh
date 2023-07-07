#!/bin/bash

# Export the current user's USER_ID and GROUP_ID as environment variables
export USER_ID=$(id -u)

# Check the command line arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 {prod|test}"
    exit 1
fi

# Run docker-compose with the appropriate profile based on the argument
if [ "$1" = "prod" ]; then
    docker-compose --profile prod up
elif [ "$1" = "test" ]; then
    docker-compose --profile test up
else
    echo "Invalid argument. Use either 'prod' or 'test'."
    exit 1
fi

