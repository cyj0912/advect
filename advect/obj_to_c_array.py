#!/bin/env python3
# Copyright (c) 2020 Toby Chen All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.


import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input')
    parser.add_argument('--index', help='Use index buffer', action='store_true')
    args = parser.parse_args()
    with open(args.input, 'r') as f:
        text = f.read()
        lines = text.splitlines()
    
    vb = []
    nb = []
    tb = []
    ib = []
    unique_vertices = set()
    for line in lines:
        if len(line) == 0 or line.startswith('#'):
            continue
        l = line.split(' ')
        if l[0] == 'v':
            vb.append((l[1] + 'f', l[2] + 'f', l[3] + 'f'))
        elif l[0] == 'vn':
            nb.append((l[1] + 'f', l[2] + 'f', l[3] + 'f'))
        elif l[0] == 'vt':
            tb.append((l[1] + 'f', l[2] + 'f'))
        elif l[0] == 'f':
            if len(l) != 4:
                raise Exception('Must triangulate faces first')
            if args.index:
                for vert in l[1:]:
                    vert_tuple = tuple([int(s) - 1 for s in vert.split('/')])
                    unique_vertices.add(vert_tuple)
                    ib.append(vert_tuple)
            else:
                for vert in l[1:]:
                    vert_tuple = tuple([int(s) - 1 for s in vert.split('/')])
                    ib.append(vert_tuple)
        else:
            print('Ignoring line', l)
    
    pos_b = []
    tex_b = []
    nor_b = []
    if args.index:
        unique_list = sorted(list(unique_vertices))
        vtuple_to_vindex = {}
        for i, vert_tuple in enumerate(unique_list):
            vtuple_to_vindex[vert_tuple] = i
            pos_b.append(vb[vert_tuple[0]])
            tex_b.append(tb[vert_tuple[1]])
            nor_b.append(nb[vert_tuple[2]])
        real_vb = sum(sum(zip(pos_b, tex_b, nor_b), ()), ())
        real_ib = [vtuple_to_vindex[vtpl] for vtpl in ib]

        print('const float vertexData[] =')
        print('{' + ', '.join(real_vb) + '};')

        print('const uint16_t indexData[] =')
        print('{' + ', '.join([str(x) for x in real_ib]) + '};')
    else:
        for vert_tuple in ib:
            pos_b.append(vb[vert_tuple[0]])
            tex_b.append(tb[vert_tuple[1]])
            nor_b.append(nb[vert_tuple[2]])
        real_vb = sum(sum(zip(pos_b, tex_b, nor_b), ()), ())
        print('const float vertexData[] =')
        print('{' + ', '.join(real_vb) + '};')

if __name__ == "__main__":
    main()
