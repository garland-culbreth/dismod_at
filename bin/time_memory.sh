# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ $0 != 'bin/time_memory.sh' ]
then
	echo 'bin/time_memory.sh: must be executed from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
python3 speed/simulated.py | tee build/speed/time.out
cd build/speed
../devel/dismod_at example.db start
rm memory.out.*
valgrind --tool=massif ../devel/dismod_at example.db fit
ms_print massif.out.* > memory.out
new.sh time.out memory.out
cd ../..
ls -R build/speed/new


