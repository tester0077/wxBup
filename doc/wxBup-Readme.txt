wxBup is a simple minded file splitterupper which takes
an input directory, a list of file name patters to exclude,
a target directory and a size specification for the output
chunks which are to receive the zipped up data from the 
source directory.
The program uses an algorithm very losely based on a 'greedy'
version of the knapsack stuffing problem.

It's main application is to break up data files from a 
large source directory into 'bags' or chunks which are
filled so as to fill the seelcted media as much as possible.

Many of the current crop of zip utilities include a similar
'disk spanning' facility. All of these seem to have several 
drawbacks. 

All media must not only be available and readable, but the 
user must have enough disk space to effectively combine
all of the parts on disk before even a single file can 
be extracted or restored.

wxBup avoids all those disadvantages, at a small cost 
of an extra DVD or CD, by zipping all files individually
and stuffing each of the disks as much as possible.

Thus, if a single file needs to be found and possibly
be restored, all you need to do is to find which disk 
contains the file.

To help with this, each disk carries with it a list of 
all files on the disk. This is useful in cases where 
some of the files may exist several layers down in 
the directory tree.
