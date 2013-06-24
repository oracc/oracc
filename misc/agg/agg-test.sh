#!/bin/sh
if [ "$ORACC_MODE" = "single" ]; then
    exit 0
fi
if [ echo `id -Gn` | grep -q oracc ]; then
    exit 0
fi
echo agg: user `whoami` is not permitted to run aggregation scripts. Stop.

