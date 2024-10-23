### Step 1: keep-watch-ing

```
sudo dmesg -w | grep simplefs
```

### Step 2: mount test.img

```
sudo mkdir /test
sudo chown wps:wps /test
sudo mount -o loop -t simplefs test.img /test
echo "Hello World" > /test/hello
```