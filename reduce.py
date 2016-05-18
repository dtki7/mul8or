# reduce instructions doing the same changes ...

import filecmp
from os import listdir, chdir
from os.path import isfile, join

chdir("data/")
files = [f for f in listdir(".") if isfile(join(".", f))]

for i, f1 in enumerate(files):
    for f2 in files[i+1:]:
       if filecmp.cmp(f1, f2):
            print "Files \"" + f1 + "\" and \"" + f2 + "\" are equal!"
