import os
import platform
import subprocess
import shutil


def build_kernel(source, target, env, for_signature):
  return '%s -T %s -o %s %s' % (env['LD'], env['LDSCRIPT'], " ".join(map(str,
  target)) , " ".join(map(str, source)))

def build_image(source, target, env, for_signature):
  if platform.system() == "OpenBSD":
    shutil.copyfile('disk/source.img', 'disk/local.img')
    subprocess.call(['vnconfig', 'svnd0', 'disk/local.img'])
    subprocess.call(['mount', '-t', 'ext2fs', '/dev/svnd0c', 'disk/mountpoint'])
    shutil.copyfile('sys/kernel.k', 'disk/mountpoint/kernel.k')
    subprocess.call(['umount', 'disk/mountpoint'])
    subprocess.call(['vnconfig', '-u', 'svnd0'])


  

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
imager = Builder(action = build_image )

env = Environment()
env.Append(ENV=os.environ)
env.Append(BUILDERS={'Kernel' : blder, 'Disk' : imager})
env.Replace(CC='%s-gcc' % toolsprefix)
env.Replace(LD='%s-ld' % toolsprefix)
env.Replace(AS='%s-as' % toolsprefix)
env.Append(CFLAGS="-nostdlib -nostartfiles -nodefaultlibs -std=c89")
env.Replace(LDSCRIPT='sys/scripts/%s.ld' % architecture)
if debug:
  env.Append(CFLAGS="-g")

Export('architecture env debug toolsprefix')

SConscript(buildfiles)

