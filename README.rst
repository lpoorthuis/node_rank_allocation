##############################
Node rank determination in MPI
##############################

:Author: Lukas POORTHUIS(LPo)
:Contact: l.poorthuis@fz-juelich.de
:Website: http://www.hpsc-terrsys.de/simlab
:Version: 2016-01-17

**********************
Content and Motivation
**********************

This short piece of software provides the user with the rank of a MPI task
based on the actual hardware structure you are calculating on. This kind of
information,for example, is useful if you only want to open one I/O stream per
hardware node to avoid I/O performance bottlenecks.

The method relies on the `MPI_Comm_split <http://www.mpich.org/static/docs/latest/www3/MPI_Comm_split.html>`_
function which splits a communicator into sub-communicators based on a colour.
For this method we map the hostname via the
`Adler-32 <http://en.wikipedia.org/wiki/Adler-32>`_ hash function to an integer
and use it as our colour. We also supply the function with the ``MPI_rank`` of
the ``MPI_COMM_WORLD`` as a key which controls the rank assignment in the new
sub-communicator.

Because collisions of the hash can occur, we compare the hostnames of the MPI
tasks stored in each new sub-communicator with each other.

After this step we get the hardware related MPI rank.

*****
Usage
*****

This is exemplary output(run on hardware that resembles the resources required
by the slurm batch script).

.. code-block:: c

        slts08@jrl01:$ sbatch src/sub.sh
        slts08@jrl01:$ cat src/job.out
        Node(size/rank): 48/0
	World(size/rank): 384/96
	Node(size/rank): 48/0
	World(size/rank): 384/288
	Node(size/rank): 48/0
	World(size/rank): 384/0
	Node(size/rank): 48/0
	World(size/rank): 384/144
	Node(size/rank): 48/0
	World(size/rank): 384/336
	Node(size/rank): 48/0
	World(size/rank): 384/192
	Node(size/rank): 48/0
	World(size/rank): 384/240
	Node(size/rank): 48/0
	World(size/rank): 384/48

******
Status
******

Ready to use!
