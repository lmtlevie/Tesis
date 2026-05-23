#!/bin/bash

# Parameters

# Remove temporary files
rm /tmp/file*
rm *performance.log


# run simulation logging performance times
./RunMultipleSimulations.sh


# Check file sizes for tmp files
du -cah /tmp/file* | wc -l 
du -cah /tmp/file* | grep total


