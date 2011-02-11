
openbsd: ../sys/kernel.k
	cp source.img local.img
	vnconfig svnd0 local.img
	mount -t ext2fs /dev/svnd0c mountpoint/
	cp ../sys/kernel.k mountpoint/
	umount mountpoint/
	vnconfig -u svnd0
