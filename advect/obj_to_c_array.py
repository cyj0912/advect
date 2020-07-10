#!/bin/env python3

import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input')
    args = parser.parse_args()
    with open(args.input, 'r') as f:
        text = f.read()
        lines = text.splitlines()
    
    vb = []
    nb = []
    ib = []
    for line in lines:
        if len(line) == 0 or line.startswith('#'):
            continue
        l = line.split(' ')
        if l[0] == 'v':
            vb.append((l[1] + 'f', l[2] + 'f', l[3] + 'f'))
        elif l[0] == 'vn':
            pass
        elif l[0] == 'f':
            # index mode
            assert(len(l) == 4)
            def get_vertex_index(vert):
                vert = vert.split('/')
                return str(int(vert[0]) - 1)
            ib += [get_vertex_index(part) for part in l[1:]]
        else:
            print('Ignoring line', l)
    
    print('const float vertexData[] =')
    print('{' + ', '.join([', '.join(tpl) for tpl in vb]) + '};')

    print('const uint16_t indexData[] =')
    print('{' + ', '.join(ib) + '};')

if __name__ == "__main__":
    main()
