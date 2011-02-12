import os
import platform
import subprocess
import shutil


def build_kernel(source, target, env, for_signature):
  return '%s -T %s -o %s %s' % (env['LD'], env['LDSCRIPT'], " ".join(map(str,
  target)) , " ".join(map(str, source)))

  

buildfiles = [ 'sys/SConstruct' ]
imagefiles = []

architecture = ARGUMENTS.get('arch')
debug = ARGUMENTS.get('debug')
toolsprefix = ARGUMENTS.get('prefix')

if architecture == None:
  architecture = "i686"
if toolsprefix == None:
  toolsprefix = '%s-elf' % architecture

blder = Builder(generator = build_kernel, suffix=".k")

env = Environment()
env.Append(ENV=os.environ)
env.Append(BUILDERS={'Kernel' : blder})
env.Replace(CC='%s-gcc' % toolsprefix)
env.Replace(LD='%s-ld' % toolsprefix)
env.Replace(AS='%s-as' % toolsprefix)
env.Append(CFLAGS="-nostdlib -nostartfiles -nodefaultlibs -std=c89 -Wall -Werror")
env.Replace(LDSCRIPT='sys/scripts/%s.ld' % architecture)
if debug:
  env.Append(CFLAGS="-g")

Export('architecture env debug toolsprefix')

SConscript(buildfiles)



