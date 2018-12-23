#!/bin/bash

# Run beewatch and restart it if a crash occurs
# Courtesy: https://stackoverflow.com/a/697064
until beewatch; do
    echo "Program crashed with exit code $?. Respawning..." >&2
    sleep 1
done
