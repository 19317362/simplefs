This error typically indicates that the kernel module you're trying to insert is not signed or the signing key is not recognized by the kernel. Here are the steps to resolve this issue:

1. **Check Kernel Module Signing**: Ensure that your kernel module is signed. If not, you need to sign it.

2. **Generate a Signing Key**: If you don't have a signing key, you need to generate one.

3. **Sign the Kernel Module**: Use the signing key to sign your kernel module.

4. **Enroll the Key**: Enroll the signing key into the kernel's keyring.

Here is a step-by-step guide:

### Step 1: Generate a Signing Key

```sh
openssl req -new -x509 -newkey rsa:2048 -keyout MOK.priv -outform DER -out MOK.der -nodes -days 36500 -subj "/CN=Module Signing Key/"
```

### Step 2: Sign the Kernel Module

```sh
sudo /usr/src/linux-headers-$(uname -r)/scripts/sign-file sha256 MOK.priv MOK.der simplefs.ko
```

### Step 3: Enroll the Key

```sh
sudo mokutil --import MOK.der
```

You will be prompted to create a password. This password will be used in the next step.

### Step 4: Reboot and Enroll the Key

Reboot your system. During the boot process, you will be prompted to enroll the key. Follow the instructions and use the password you created earlier.

### Step 5: Insert the Module

After rebooting, try inserting the module again:

```sh
sudo insmod simplefs.ko
```

This should resolve the "Key was rejected by service" error.
