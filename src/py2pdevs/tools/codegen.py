#!/usr/bin/python2.7

import os
import re

from argparse import ArgumentParser


def parse_options():
    parser = ArgumentParser()

    parser.add_argument('-a', '--atomics-dir', dest='atomics_dir', action='store',
                        type=str, required=True,
                        help='root directory of atomic models')
    
    parser.add_argument('-i', '--input', dest='input', action='store',
                        type=str, required=True,
                        help='input file containing the boost::python module code template')
    
    parser.add_argument('-o', '--output', dest='output', action='store',
                        type=str, required=True,
                        help='output file where the complete boost::python module code will be written')
    
    return parser.parse_args()

def find_model_classes(atomics_dir):
    classes = list()
    class_regexp = re.compile('class\s+([a-zA-Z_\d]+)\s*:\s*public\s+(Simulator|BaseSimulator)')
    
    for root, subdirs, filenames in os.walk(atomics_dir):
        if 'modelica' in root:
            continue
        for filename in filenames:
            if not filename.endswith('.h'):
                continue
            # IMPORTANT: VectorialModel is a template class. For the moment any
            # new instance of this class should be manually added in
            # module.cpp.in.
            if 'ScilabSimulationsTracker' in filename or\
               'VectorialModel' in filename:
                continue
            with open('%s/%s' % (root, filename), 'r') as file:
                 content = file.read()
                 matches = class_regexp.findall(content)
                 matches = filter(lambda m: m[0] != 'BaseSimulator', matches)
                 index = root.find('atomics/')
                 path = root[index+8:] + '/' + filename
                 matches = map(lambda m: m + (path,), matches)
                 classes.extend(matches)
                 
    return classes
               
def generate_content(classes):
    includes = str()
    content = str()
    
    for classname, _, path in classes:
        classname_l = classname.lower()
        content += '        .def("create_%s", &Py2PDEVSModel::create_atomic<%s>)\n' % (classname_l, classname)
        includes += '#include "%s"\n' % path
        
    content += ';\n\n'
        
    for classname, basename, _ in classes:
        content += '    class_<%s, boost::shared_ptr<%s>, bases<%s> >("%s", init<char*>());\n' % (classname, classname, basename, classname)
        
    content += '}\n'
    
    return includes, content
                     
def update_module(input, output, classes):
    with open(input, 'r') as file:
        content = file.read()
        
    includes, new_content = generate_content(classes)
    
    index = content.find('// Atomic model includes follow')
    content = content[:index] + '\n' + includes + content[index+31:]
    
    index = content.find('; // Atomic model code follows')
    content = content[:index] + '\n' + new_content
        
    with open(output, 'w') as file:
        file.write(content)

def codegen():
    args = parse_options()
    model_classes = find_model_classes(args.atomics_dir)
    update_module(args.input, args.output, model_classes)


if __name__ == '__main__':
    codegen()
    