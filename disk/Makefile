
linux: ../sys/kernel.k
	cp source.img local.img
	mount -o loop -t ext2 local.img mountpoint
	cp ../sys/kernel.k mountpoint/
	umount mountpoint/

openbsd: ../sys/kernel.k
	cp source.img local.img
	vnconfig svnd0 local.img
	mount -t ext2fs /dev/svnd0c mountpoint/
	cp ../sys/kernel.k mountpoint/
	umount mountpoint/
	vnconfig -u svnd0

freebsd:
	cp source.img local.img
	mdconfig -a -t vnode -f local.img  -u 0
	mount -t ext2fs /dev/md0 mountpoint/
	cp ../sys/kernel.k mountpoint/
	umount mountpoint/
	mdconfig -d -u 0
