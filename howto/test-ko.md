### Step 1: keep-watch-ing

```
sudo /usr/src/linux-headers-$(uname -r)/scripts/sign-file sha256 MOK.priv MOK.der dxrfs.ko

sudo insmod dxrfs.ko
sudo dmesg -w | grep dxrfs
```

### Step 2: mount test.img

```
sudo mkdir /test
sudo chown wps:wps /test
sudo mount -o loop -t dxrfs test.img /test
echo "Hello World" > /test/hello

sudo umount /test
sudo rmmod dxrfs
```