import os
import sys
#from nose.util import src

# Make py2pdevs package visible. An alternative is to export the
# environment variable PYTHONPATH appeding the absolute path to
# build/lib and then running the script without the following
# line.
sys.path.append('../../../build/lib')


# implements common method for all extender routers (Router, REDRouter, etc)
class TwoWayRouter(object):
    def __init__(self):
        self._connections = dict()
    
    # called when connected to other model
    def connected(self, src, src_port, dst, dst_port):
        # sort params: params[0]=self params[1]=my_port, params[2]=other, params[3]=other_port
        if self == src:
            params = (src, src_port, dst, dst_port)            
        elif self==dst:
            params = (dst, dst_port, src, src_port)           
        else:
           raise RuntimeError('This should never happen') 
        
#         print "%s(%d) connected to %s(%d)" % (src.name, src_port, dst.name, dst_port )
        # validate: router can not use same port twice
        if params[1] in self._connections and (params[2], params[3]) != self._connections[params[1]] :
            old, old_port = self._connections[params[1]]
            raise RuntimeError('%s: attempting to connect port %d to (%s, %d), but port %d is already connected to (%s, %d)' 
                                     % (self.name, params[1], params[2].name, params[3] , params[1], old.name, old_port ))  

        # add ports if necesary
        if params[1]+1 > self.in_ports:
            self.in_ports = params[1]+1            
        if params[1]+1 > self.out_ports:    
            self.out_ports = params[1]+1
            self.bufferSizes += [self.bufferSizes[-1]]*(self.out_ports-len(self.bufferSizes))
            self.bandwidths += [self.bandwidths[-1]]*(self.out_ports-len(self.bandwidths))
                     
        # add the reverse connection 
        self._connections[params[1]] = (params[2], params[3])     
        self.parent.connect((dst, dst_port), (src, src_port)) # reverse the connection

    def add_flow(self, flow):
        if hasattr(self, 'n_flows'):
            self.n_flows += 1

