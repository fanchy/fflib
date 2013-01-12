
import sys
from pylib.inc import *
from pylib.src_parser import src_parser_t
from pylib.code_generator import code_generator_t


if __name__ =='__main__':
    if len(sys.argv) < 3:
        print('Usage: %s idl_filename dest_file_name' % (sys.argv[0]))
        exit(0)
    idl_filename   = sys.argv[1]
    dest_file_name = sys.argv[2]

    p = src_parser_t(idl_filename)
    p.exe()
    mgr = p.get_struct_def_mgr()
    mgr.dump()
    cg = code_generator_t(mgr, dest_file_name)
    cg.exe()

