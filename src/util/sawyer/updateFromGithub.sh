#!/bin/bash
# Update ROSE with the latest Sawyer source code
set -e

SAWYER_REPO=${1-github.com:matzke1/sawyer}

# Download the latest version of the source code
SAWYER_ROOT="sawyer-$(date +%Y%m%d)"
if [ -d "$SAWYER_ROOT" ]; then
    (
	cd "$SAWYER_ROOT"
	git fetch "$SAWYER_REPO" master
	git merge FETCH_HEAD
    )
else
    git clone "$SAWYER_REPO" "$SAWYER_ROOT"
fi
	
# Copy some of Sawyer's source files into the ROSE source tree. Don't worry about overwriting ROSE-local changes--they
# should have been contributed back to the Sawyer project by now (besides, that's what Git is for)!
for f in \
    Access Assert AddressMap AddressSegment AllocatingBuffer BitVector BitVectorSupport Buffer CommandLine Cached \
    Callbacks DefaultAllocator Graph GraphBoost GraphTraversal IndexedList Interval IntervalMap IntervalSet Map \
    MappedBuffer Markup MarkupPod Message NullBuffer Optional PoolAllocator ProgressBar Sawyer SharedPointer \
    SmallObject Stack StaticBuffer Stopwatch WarningsOff WarningsRestore
do
    srcbase="$SAWYER_ROOT/sawyer/$f";
    cp "$srcbase.h" .
    [ -f "$srcbase.C" ] && cp "$srcbase.C" .
done

