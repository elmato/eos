#!/usr/bin/env python

import os
os.environ["CC"] = "clang-4.0"
os.environ["CXX"] = "clang++-4.0"
os.environ["CFLAGS"] = "-Wno-invalid-partial-specialization"
os.environ["LDFLAGS"] =  "-Wl,--no-as-needed"

from distutils.core import setup
from distutils.extension import Extension

dir_path = os.path.dirname(os.path.realpath(__file__))

iroot  = os.environ.get('EOS_ROOT_INCLUDE', '%s/../libraries' % dir_path)
lroot  = os.environ.get('EOS_ROOT_LIB', '%s/../build/libraries' % dir_path)
mt     = os.environ.get('EOS_BOOST_POSTFIX', '')
debug  = os.environ.get('EOS_DEBUG','')
boost  = os.environ.get('BOOST_ROOT','')

setup(name="EosHelper",
  ext_modules=[
    Extension("eoshelper", ["eoshelper.cpp"],
      extra_compile_args = ['-std=c++14'],
      libraries = ['secp256k1', 'eos_types', 'appbase', 'chainbase', 'eos_chain' , 'eos_utilities', 'fc%s'%debug , 'ssl' , 'boost_iostreams%s'%mt , 'boost_filesystem%s'%mt , 'boost_thread%s'%mt , 'boost_system%s'%mt , 'boost_chrono%s'%mt , 'crypto' , 'boost_coroutine%s'%mt , 'boost_context%s'%mt, 'boost_python%s'%mt, 'boost_date_time%s'%mt],
      include_dirs = [boost, '%s/appbase/include' % iroot, '%s/chainbase/include' % iroot, '%s/utilities/include' % iroot , '%s/chain/include' % iroot , '%s/types/include' % iroot ,'%s/fc/include' % iroot],
      library_dirs = ['%s/stage/lib/' % boost, '%s/types' % lroot, '%s/appbase' % lroot, '%s/chainbase' % lroot, '%s/utilities' % lroot, '%s/chain' % lroot , '%s/protocol' % lroot , '%s/fc' % lroot]
    )
  ]
)
