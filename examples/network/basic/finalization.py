import os
from py2pdevs import core # /powerdevs/build/lib should be in the $PYTHONPATH

core.run_scilab_script('%s/../examples/network/basic/network_basic.sce' % os.getcwd())