#!/bin/bash

# Put here the program (maybe with path)
if [ $# -ne 3 ]; then
    GETF0="get_pitch --umaxnorm=0.42 --ur1norm=0.4 --upot=-50"
else
    GETF0="get_pitch --umaxnorm=$1 --ur1norm=$2 --upot=$3"
fi

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done

exit 0
