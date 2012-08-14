import os
import platform
import subprocess
import shutil


def build_kernel(source, target, env, for_signature):
  return '%s -ggdb -T %s -o %s %s' % (env['LD'], env['LDSCRIPT'], " ".join(map(str,
  target)) , " ".join(map(str, source)))

def build_loader(source, target, env, for_signature):
  return '%s -ggdb -T %s -o %s %s' % (env['LD'], "sys/scripts/loader-%s.ld" %
  architecture, " ".join(map(str,
  target)) , " ".join(map(str, source)))
  

buildfiles = [ 'sys/SConstruct' ]
imagefiles = []

architecture = ARGUMENTS.get('arch')
debug = ARGUMENTS.get('debug')
toolsprefix = ARGUMENTS.get('prefix')

if architecture == None:
  architecture = "i686"
  debug="true"
if architecture == "test":
  toolsprefix = ''
  debug="true"
if toolsprefix == None:
  toolsprefix = '%s-elf-' % architecture

blder = Builder(generator = build_kernel, suffix=".k")
loader_blder = Builder(generator = build_loader, suffix=".elf")

env = Environment()
env.Append(ENV=os.environ)
env.Append(BUILDERS={'Kernel' : blder, 'Loader' : loader_blder})
env.Replace(CC='%sgcc' % toolsprefix)
env.Replace(LD='%sld' % toolsprefix)
env.Replace(AS='%sas' % toolsprefix)
env.Append(CFLAGS="-nostdlib -nostartfiles -nodefaultlibs -std=c99 -Wall -Werror -O0")

env.Replace(LDSCRIPT='sys/scripts/%s.ld' % architecture)
if debug:
  env.Append(CFLAGS="-ggdb")

Export('architecture env debug toolsprefix')

SConscript(buildfiles)



