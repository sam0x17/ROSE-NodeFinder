#!/bin/bash

#-------------------------------------------------------------------------------------------------------------------------------
# Looks for pairs of specimens compiled with different optimation levels and creates a database for each pair.  The argument
# for this script is the name of a directory, and that directory is searched recursively for target directories with names
# like <TARGET>/<OPTIM> where <TARGET> is the name of the package (e.g., "nginx") and <OPTIM> is the optimization level (one of
# O0, O1, O2, or O3).  Each <TARGET>/<OPTIM> directory should contain exactly one specimen (executable file or library; see
# the find_specimens function).
#-------------------------------------------------------------------------------------------------------------------------------


# String to prepend to all database names
DB_PREFIX=robb_

# Try to drop the database ('yes' or 'no')?
DROP_DB=yes

# Create the database ('yes' or 'no')?  If 'no' then the analysis is skipped, which is a convenient way to drop a whole bunch
# of databases that you no longer want.  If 'yes' and the creation fails, then the analysis is skipped--either the database
# already exists and we don't need to re-run, or the database could not be created and running would be pointless.
CREATE_DB=yes


# Settings for running the analysis (see $(srcdir)/run-analysis.sh for documentation)
generate_inputs_flags='--ngroups=1 --memhash arguments:redirect=memhash locals:random=1000 locals:pad=inf,0 globals:random=1000 globals:pad=inf,0 functions:random=1000 functions:pad=inf,0 integers:redirect=memhash'
add_functions_flags='--signature-components=total_for_variant,by_category,apply_log'
get_pending_tests_flags='--size=20'
run_tests_nprocs=''
run_tests_job_multiplier='1'
run_tests_flags='--follow-calls=builtin --timeout=1000000 --coverage=save --call-graph=save --path-syntactic=function --signature-components=total_for_variant,by_category,apply_log'
func_similarity_worklist_flags=''
func_similarity_flags=''


###############################################################################################################################

mydir=${0%/*}

die() {
    echo "$@" >&1
    exit 1
}

usage() {
    die "usage: $0 [TARGETS_DIR]"
}

find_specimens() {
    local dir="$1"
    find $dir -maxdepth 1 -type f \( -perm /ugo+x -o -name '*.a' -o -name '*.so' \)
}

generate_config() {
    local filename="$1" db_name="$2"
    (
	echo "dbname='postgresql:///$db_name'"
	echo "recreate=yes"
	echo "generate_inputs_flags='$generate_inputs_flags'"
	echo "add_functions_flags='$add_functions_flags'"
	echo "get_pending_tests_flags='$get_pending_tests_flags'"
	echo "run_tests_flags='$run_tests_flags'"
	echo "func_similarity_worklist_flags='$func_similarity_worklist_flags'"
	echo "func_similarity_flags='$func_similarity_flags'"
    ) >$filename
}

# Find the names of the directories containing target directories. Target directories are O0, O1, O2, or O3
SEARCH_DIR="${1-.}"
TARGET_DIRS=$(find "$SEARCH_DIR" -type d -name 'O[0123]' |perl -p -e 's((.*)/O[0123]$)($1)' |sort |uniq)
[ -n "$TARGET_DIRS" ] || die "no target directories found in $SEARCH_DIR"

for TARGET_DIR in $TARGET_DIRS; do
    TARGET_NAME=$(basename $TARGET_DIR)

    # What optimizations are present?
    OPTIM_DIRS=$(find "$TARGET_DIR" -type d -name 'O[0123]')
    if [ ! "$OPTIM_DIRS" ]; then
	echo "no optimization directories (O0, O1, O2, O3) in $TARGET_DIR"
	continue
    fi

    # Cross product of the optimization levels with itself, avoiding the case where both are the same
    for DIR1 in $OPTIM_DIRS; do
	OPTIM1=$(basename $DIR1)
	DIR1_SPECIMENS=($(find_specimens $DIR1))
	if [ "${#DIR1_SPECIMENS[*]}" -ne 1 ]; then
	    echo "directory should have exactly one specimen (has ${#DIR1_SPECIMENS[*]}): $DIR1"
	    continue
	fi
	SPECIMEN1=${DIR1_SPECIMENS[0]}

	for DIR2 in $OPTIM_DIRS; do

	    # Don't run if DIR1 is lexicographically greater than DIR2 
	    cmp=$(perl -e 'print $ARGV[0] cmp $ARGV[1]' $DIR1 $DIR2)
	    [ "$cmp" -gt 0 ] && continue;

	    OPTIM2=$(basename $DIR2)
	    DIR2_SPECIMENS=($(find_specimens $DIR2))
	    if [ "${#DIR2_SPECIMENS[*]}" -ne 1 ]; then
		echo "directory should have exactly one specimen (has ${#DIR2_SPECIMENS[*]}): $DIR2"
		continue
	    fi
	    SPECIMEN2=${DIR2_SPECIMENS[0]}

	    # If SPECIMEN1==SPECIMEN2 then copy SPECIMEN2 to a new name and use that instead. Don't copy it into the
	    # same directory it was in or else we violate the one-specimen-per-directory rule described at the top of this
	    # script. Also, don't delete the file after copying it, or else it will exist in the database but not in the file
	    # system, which would cause problems when 25-run-tests tries to analyze it.
	    if [ "$SPECIMEN1" = "$SPECIMEN2" ]; then
		SPECIMEN2=$(tempfile)
		rm -f "$SPECIMEN2"
		cp "$SPECIMEN1" "$SPECIMEN2"
	    fi

	    DB_NAME=${DB_PREFIX}${TARGET_NAME}_${OPTIM1}_${OPTIM2}
	    echo
	    echo "==============================================================================================="
	    banner $TARGET_NAME
	    echo
	    echo "database  = $DB_NAME"
	    echo "specimen1 = $SPECIMEN1"
	    echo "specimen2 = $SPECIMEN2"
	    echo

	    # Only run if we don't already have this database and we can create it
	    [ "$DROP_DB" = yes ] && dropdb $DB_NAME
	    [ "$CREATE_DB" = yes ] || continue
	    if ! createdb $DB_NAME >/dev/null; then
		echo "database '$DB_NAME' exists or could not be created; skipping $TARGET_DIR"
		continue
	    fi

	    # Run the analysis
	    CONFIG_FILE=$(tempfile)
	    generate_config $CONFIG_FILE $DB_NAME
	    $mydir/run-analysis.sh --batch --config=$CONFIG_FILE $SPECIMEN1 $SPECIMEN2 || exit 1

	done
    done
done
